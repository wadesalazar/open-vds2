/****************************************************************************
** Copyright 2019 The Open Group
** Copyright 2019 Bluware, Inc.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
****************************************************************************/

#include "File.h"

#ifndef _XOPEN_SOURCE
  #define _XOPEN_SOURCE 500
#endif

#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace OpenVDS
{

static std::string error_to_string(int error)
{
  return std::string(strerror(error));
}

static void set_io_error(int error, IOError &io_error)
{
  io_error.code = error;
  io_error.string = error_to_string(error);
}

static void set_io_error(int error, std::string &error_string_prefix, IOError &io_error)
{
  io_error.code = error;
  io_error.string = error_string_prefix + error_to_string(error);
}

template<size_t N>
static void set_io_error(int error, const char (&error_string_prefix)[N], IOError &io_error)
{
  io_error.code = error;
  io_error.string = std::string(error_string_prefix, N) + error_to_string(error);
}

template<size_t N>
static void set_io_error(const char (&error_string_prefix)[N], IOError &io_error)
{
  io_error.code = -1;
  io_error.string = std::string(error_string_prefix, N);
}

static __thread sigjmp_buf * SystemFileView_pSigjmpEnv;

class SystemFileView : public FileView
{
private:
	static void SIGBUSHandler(int sig)
  {
    if(SystemFileView_pSigjmpEnv)
    {
      siglongjmp (*SystemFileView_pSigjmpEnv, 1);
    }
  }

  class SignalHandlerInstaller
  {
  public:
    static struct sigaction
                        m_SigactionOld,
                        m_SigactionNew;

    SignalHandlerInstaller()
    {
      m_SigactionNew.sa_handler = SystemFileView::SIGBUSHandler;

      int iRetval = sigaction(SIGBUS, &m_SigactionNew, &m_SigactionOld);

      assert(iRetval == 0);
    }

    ~SignalHandlerInstaller()
    {
      int iRetval = sigaction(SIGBUS, &m_SigactionOld, NULL);

      assert(iRetval == 0);
    }
  };

  static SignalHandlerInstaller
                        m_cSignalHandlerInstaller;

  void*                 m_pxBaseAddress;
  size_t                m_nNumberOfBytes;

  static size_t         s_nPageSize;

public:
	SystemFileView(SystemFileMappingObject *pFileMappingObject, int64_t nPos, int64_t nSize, bool isPopulate, IOError &error)
		: m_pxBaseAddress(NULL)
  {
    (void)&m_cSignalHandlerInstaller;

    int64_t nDelta = nPos % s_nPageSize;

    m_nNumberOfBytes = nSize + nDelta;

    off_t nAdjustedOffset = nPos - nDelta;

    int iFD = *reinterpret_cast<int*>(&pFileMappingObject);

    int iFlags = MAP_PRIVATE;

    if (isPopulate)
    {
      iFlags |= MAP_POPULATE;
    }

    m_pxBaseAddress = mmap(NULL, m_nNumberOfBytes, PROT_READ, iFlags, iFD, nAdjustedOffset);

    m_pData = m_pxBaseAddress != MAP_FAILED ? reinterpret_cast<const unsigned char *>(m_pxBaseAddress) + nDelta : NULL;
    m_nPos = nPos;
    m_nSize = nSize;

    if (m_pxBaseAddress == MAP_FAILED)
    {
			set_io_error(errno, "FileView failed ", error);
      return;
    }

    if (isPopulate)
    {
			int32_t iRetval = madvise((void*)m_pxBaseAddress, m_nNumberOfBytes, MADV_WILLNEED);

      if (iRetval != 0)
      {
				set_io_error(errno, "FileView failed ", error);
      }
    }
  }

  ~SystemFileView()
  {
    int iRetval = munmap(m_pxBaseAddress, m_nNumberOfBytes);

    if(iRetval != 0)
    {
      int32_t iErrno = errno;
      (void)iErrno;
    }
  }

  bool prefetch(const void *pData, int64_t nSize, IOError &error) const override
  {
    int64_t nDelta = (int64_t)((uintptr_t)pData % s_nPageSize);

    pData = static_cast<const uint8_t*>(pData) - nDelta;
    nSize = ((nSize + nDelta + s_nPageSize - 1) / s_nPageSize) * s_nPageSize;

    int32_t iRetval = madvise(const_cast<void*>((const void*)pData), nSize, MADV_WILLNEED);

    if (iRetval != 0)
    {
			set_io_error(errno, "FileView::prefetch ", error);
			return false;
    }
		return true;
  }
};

bool File::exists(const std::string& filename)
{
  struct stat buf;
  return (lstat(filename.c_str(), &buf) == 0) && S_ISREG(buf.st_mode);
}

bool File::open(const std::string& filename, bool isCreate, bool isDestroyExisting, bool isWriteAccess, IOError &error)
{
  assert(!isDestroyExisting || isCreate);
  assert(!isCreate || isWriteAccess || !"it is meaningless to demand creation with RO access");
  assert(!_pxPlatformHandle || ("RawFileAccess_c::Open: file already open"));

  _cFileName = filename;

  int flags = (isCreate || isWriteAccess) ? O_RDWR : O_RDONLY;

  if (isCreate)
    flags |= O_CREAT | (isDestroyExisting ? O_TRUNC : O_EXCL);

  int fd = ::open(_cFileName.c_str(), flags, 0666);

  if (fd < 0)
  {
    _pxPlatformHandle = 0;
    set_io_error(errno, "File::open ", error);
    return false;
  }

  _pxPlatformHandle = reinterpret_cast<void*>(fd);
  _isWriteable = isWriteAccess;
  return true;
}

void File::close()
{
  assert(isOpen());

  int fd  = (int)(intptr_t)_pxPlatformHandle;
  bool isOK = ::close(fd) == 0;

  _pxPlatformHandle = 0;
  _cFileName.clear();
  //return isOK;
}

int64_t File::size(IOError& error) const
{
  int fd  = (int)(intptr_t)_pxPlatformHandle;
  int64_t nLength = lseek(fd, 0, SEEK_END);

  if (nLength < 0)
  {
    set_io_error(errno, "File::size ", error);
    return -1;
  }
  return nLength;
}

bool File::read(void* pxData, int64_t nOffset, int32_t nLength, IOError& error) const
{
	assert(nOffset >= 0);
	int fd    = (int)(intptr_t)_pxPlatformHandle;
	ssize_t nread;

	while (nLength > 0)
	{
		nread = pread(fd, pxData, nLength, nOffset);
		assert(nread <= nLength);

		if (nread < 0)
		{
			set_io_error(errno, "File::read ", error);
			return false;
		}

		if (nread == 0)
		{
			set_io_error(errno, "File::read (zero-length read) ", error);
			return false;
		}

		nLength -= nread;
		pxData = (char*)pxData + nread;
		nOffset += nread;
	}
	return true;
}

bool File::write(const void* pxData, int64_t nOffset, int32_t nLength, IOError & error)
{
  assert(nOffset >= 0);

  if (!_isWriteable)
	{
		set_io_error("File::write: file not writeable", error);
		return false;
	}

  int fd  = (int)(intptr_t)_pxPlatformHandle;
  ssize_t nwritten;

  while (nLength > 0)
  {
    nwritten = pwrite(fd, pxData, nLength, nOffset);
    assert(nwritten <= nLength);

    if (nwritten < 0)
    {
      set_io_error(errno, "File::write ", error);
      return false;
    }
    if (nwritten == 0)
    {
      set_io_error(errno, "File::write (zero-length write)", error);
      return false;
    }

    nLength -= nwritten;
    pxData = (char*)pxData + nwritten;
    nOffset += nwritten;
  }
  return true;
}

bool File::flush()
{
	int fd  = (int)(intptr_t)_pxPlatformHandle;
	return syncfs(fd) == 0;
}

FileView *File::createFileView(int64_t nPos, int64_t nSize, bool isPopulate, IOError &error)
{
  if(!m_pFileMappingObject)
  {
    if (!FileView::SystemFileMappingObject::open(&m_pFileMappingObject, *this, error))
      return nullptr;
  }
  FileView *ret = new SystemFileView(m_pFileMappingObject, nPos, nSize, isPopulate, error);
  if (error.code)
  {
    bool deleted = FileView::removeReference(ret);
    assert(deleted);
    ret = nullptr;
  }
  return ret;
}

void
SystemFileView_SetSigBusJmpEnv(sigjmp_buf* pSigjmpEnv)
{
  SystemFileView_pSigjmpEnv = pSigjmpEnv;
}

bool FileView::SystemFileMappingObject::open(SystemFileMappingObject** ppcFileMappingObject, File& file, IOError& error)
{
  assert(ppcFileMappingObject && !*ppcFileMappingObject);
  int iFile = (int)(intptr_t)file.handle();

  *ppcFileMappingObject = reinterpret_cast<SystemFileMappingObject *>(iFile);
  return true;
}

void FileView::SystemFileMappingObject::close(SystemFileMappingObject** ppcFileMappingObject)
{
  assert(ppcFileMappingObject && *ppcFileMappingObject);
}

SystemFileView::SignalHandlerInstaller SystemFileView::m_cSignalHandlerInstaller; 
struct sigaction SystemFileView::SignalHandlerInstaller::m_SigactionOld;
struct sigaction SystemFileView::SignalHandlerInstaller::m_SigactionNew;
size_t SystemFileView::s_nPageSize = (size_t)sysconf(_SC_PAGESIZE);

} //namespace
macro(BuildZlib)
  if (WIN32)
    list(APPEND ZLIB_LIBS_LIST_RELEASE "lib/zlib.lib")
    list(APPEND ZLIB_LIBS_LIST_DEBUG   "lib/zlibd.lib")
    list(APPEND ZLIB_DLLS_LIST_RELEASE "bin/zlib.dll")
    list(APPEND ZLIB_DLLS_LIST_DEBUG   "bin/zlibd.dll")
  elseif (APPLE)
    list(APPEND ZLIB_LIBS_LIST_RELEASE "lib${LIBSUFFIX}/libz.dylib.1.2.11")
  else()
    list(APPEND ZLIB_LIBS_LIST_RELEASE "lib${LIBSUFFIX}/libz.so.1.2.11")
  endif()

  GetRootInstallDir(ZLIB_ROOT_INSTALL zlib ${zlib_VERSION})
  get_property(_isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
  if (_isMultiConfig)
    set(ZLIB_INST "${ZLIB_ROOT_INSTALL}/$<CONFIG>")
  else()
    set(ZLIB_INST "${ZLIB_ROOT_INSTALL}/${CMAKE_BUILD_TYPE}")
  endif()

  set(EXTRA_CMAKE_ARGS "-DINSTALL_BIN_DIR=${ZLIB_INST}/bin;-DINSTALL_LIB_DIR=${ZLIB_INST}/lib;-DINSTALL_INC_DIR=${ZLIB_INST}/include")
  BuildExternal(zlib ${zlib_VERSION} "" ${zlib_SOURCE_DIR} "${ZLIB_LIBS_LIST_RELEASE}" "${ZLIB_DLLS_LIST_RELEASE}" "${ZLIB_LIBS_LIST_DEBUG}" "${ZLIB_DLLS_LIST_DEBUG}" "${EXTRA_CMAKE_ARGS}")
endmacro()



function(BuildAzure)
  if (WIN32)
    list(APPEND AZURE_LIBS_LIST "lib/wastorage.lib")

    list(APPEND AZURE_DLLS_LIST "bin/wastorage.dll")
  elseif (APPLE)
    list(APPEND AZURE_DLLS_LIST "lib/libwastorage.dylib")
  else()
    list(APPEND AZURE_DLLS_LIST "lib/libazurestorage.so.7.0")
    list(APPEND AZURE_DLLS_LIST "lib/libazurestorage.so.7")
    list(APPEND AZURE_DLLS_LIST "lib/libazurestorage.so")
  endif()
  set(AzureCmakeDir "${azure-storage-cpp_SOURCE_DIR}/Microsoft.WindowsAzure.Storage")
  if (WIN32)
    set(cmake_arg -Dcpprestsdk_DIR=${cpp-rest-api_INSTALL_INT_CONFIG}/lib/cpprestsdk)
  else()
    set(cmake_arg -DCASABLANCA_DIR=${cpp-rest-api_INSTALL_INT_CONFIG})
  endif()
  if (BOOST_ROOT)
    set(BOOST_FLAGS ";-DBOOST_ROOT=${BOOST_ROOT};")
  elseif(BOOST_INCLUDEDIR)
    set(BOOST_FLAGS ";-DBOOST_INCLUDEDIR=${BOOST_INCLUDEDIR};-DBOOST_LIBRARYDIR=${BOOST_LIBRARYDIR};")
  endif()
  BuildExternal(azure-storage ${azure-storage-cpp_VERSION} cpp-rest-api ${AzureCmakeDir} "${AZURE_LIBS_LIST}" "${AZURE_DLLS_LIST}" "" "" "${cmake_arg}${BOOST_FLAGS}")


endfunction()

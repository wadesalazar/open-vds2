function(BuildGoogleClient)
  if (WIN32)
    list(APPEND GOOGLE_CLOUD_LIBS_LIST "lib/google_cloud_cpp_common.lib")
    list(APPEND GOOGLE_CLOUD_LIBS_LIST "lib/storage_client.lib")
  elseif (APPLE)
  else()
    list(APPEND GOOGLE_CLOUD_LIBS_LIST "lib${LIBSUFFIX}/libgoogle_cloud_cpp_common.a")
    list(APPEND GOOGLE_CLOUD_LIBS_LIST "lib${LIBSUFFIX}/libstorage_client.a")
  endif()

  list(APPEND GOOGLE_CLOUD_CMAKE_ARGS "-DCMAKE_POSITION_INDEPENDENT_CODE=ON")
  list(APPEND GOOGLE_CLOUD_CMAKE_ARGS "-DBUILD_TESTING=OFF")
  list(APPEND GOOGLE_CLOUD_CMAKE_ARGS "-DGOOGLE_CLOUD_CPP_ENABLE=storage")
  list(APPEND GOOGLE_CLOUD_CMAKE_ARGS "-DGOOGLE_CLOUD_CPP_ENABLE_GRPC=OFF")

  list(APPEND GOOGLE_CLOUD_DEPENDS absl)
  list(APPEND GOOGLE_CLOUD_CMAKE_ARGS "-Dabsl_DIR=${absl_INSTALL_INT_CONFIG}/lib/cmake/absl")
  
  list(APPEND GOOGLE_CLOUD_DEPENDS crc32c)
  list(APPEND GOOGLE_CLOUD_CMAKE_ARGS "-DCrc32c_DIR=${crc32c_INSTALL_INT_CONFIG}/lib/cmake/Crc32c")

  list(APPEND GOOGLE_CLOUD_CMAKE_ARGS "-DGOOGLE_CLOUD_CPP_NLOHMANN_JSON_HEADER=${google_nlohmann_SOURCE_DIR}")
  
  if (BUILD_CURL)
    list(APPEND GOOGLE_CLOUD_DEPENDS curl)
    if (WIN32)
      list(APPEND GOOGLE_CLOUD_CMAKE_ARGS "-DCURL_DIR=${curl_INSTALL_INT_CONFIG}/lib/cmake/CURL")
    endif()
  endif()

  if (BUILD_ZLIB)
    list(APPEND GOOGLE_CLOUD_DEPENDS zlib)
    if (WIN32)
      list(APPEND GOOGLE_CLOUD_CMAKE_ARGS "-DZLIB_INCLUDE_DIR=${ZLIB_INSTALL_INC_DIR}")
      list(APPEND GOOGLE_CLOUD_CMAKE_ARGS "-DZLIB_LIBRARY=${ZLIB_INSTALL_LIB_DIR}")
    endif()
  endif()

  BuildExternal(google-cloud-cpp ${google-cloud-cpp_VERSION} "${GOOGLE_CLOUD_DEPENDS}" ${google-cloud-cpp_SOURCE_DIR} "${GOOGLE_CLOUD_LIBS_LIST}" "" "" "" "${GOOGLE_CLOUD_CMAKE_ARGS}")
endfunction()

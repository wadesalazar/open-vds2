# Based on https://devblogs.microsoft.com/cppblog/clear-functional-c-documentation-with-sphinx-breathe-doxygen-cmake/
find_package(Doxygen REQUIRED)
find_package(Sphinx REQUIRED)

set(PYTHON_DEP_STRING "Please install Python package breathe (at least version 4.18.0), recommonmark sphinx_markdown_tables and sphinx_rtd_theme.")

execute_process(COMMAND ${Python3_EXECUTABLE} -c "from packaging import version; import sys; import breathe; sys.exit(version.parse(\"4.18.0\") > version.parse(breathe.__version__))" RESULT_VARIABLE ret)
if (ret)
  message(FATAL_ERROR "Missing Documentation dependency breathe. ${PYTHON_DEP_STRING}")
endif()
execute_process(COMMAND ${Python3_EXECUTABLE} -c "import recommonmark" RESULT_VARIABLE ret)
if (ret)
  message(FATAL_ERROR "Missing Documentation dependency recommonmark. ${PYTHON_DEP_STRING}")
endif()
execute_process(COMMAND ${Python3_EXECUTABLE} -c "import sphinx_markdown_tables" RESULT_VARIABLE ret)
if (ret)
  message(FATAL_ERROR "Missing Documentation dependency sphinx_markdown_tables. ${PYTHON_DEP_STRING}")
endif()
execute_process(COMMAND ${Python3_EXECUTABLE} -c "import sphinx_rtd_theme" RESULT_VARIABLE ret)
if (ret)
  message(FATAL_ERROR "Missing Documentation dependency sphinx_rtd_theme. ${PYTHON_DEP_STRING}")
endif()

# Find all the public headers
get_target_property(OPENVDS_PUBLIC_HEADER_DIR openvds  INTERFACE_INCLUDE_DIRECTORIES)
file(GLOB_RECURSE OPENVDS_PUBLIC_HEADERS ${OPENVDS_PUBLIC_HEADER_DIR}/*.h)

set(DOXYGEN_INPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/src/OpenVDS/OpenVDS)
set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
set(DOXYGEN_HTML_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/html/cppdoc/doxygen)
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIRECTORY}/xml/index.xml)
set(DOXYGEN_GENERATE_HTML YES)
set(DOXYGEN_GENERATE_XML YES)
set(DOXYGEN_ENABLE_PREPROCESSING YES)
set(DOXYGEN_MACRO_EXPANSION YES)
set(DOXYGEN_EXPAND_ONLY_PREDEF YES)
set(DOXYGEN_INTERNAL_DOCS NO)
list(APPEND DOXYGEN_PREDEFINED "OPENVDS_EXPORT=")
list(APPEND DOXYGEN_PREDEFINED "DOXYGEN_SHOULD_SKIP_THIS")

make_directory(${DOXYGEN_HTML_OUTPUT})
doxygen_add_docs(Doxygen
    ${DOXYGEN_INPUT_DIRECTORY}
    COMMENT "Generating docs")

#set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
#set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# Replace variables inside @@ with the current values
#configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

# Doxygen won't create this for us
#file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIRECTORY})

# Only regenerate Doxygen when the Doxyfile or public headers change
#add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
#                   DEPENDS ${OPENVDS_PUBLIC_HEADERS}
#                   COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
#                   MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
#                   COMMENT "Generating docs"
#                   VERBATIM)

# Nice named target so we can run the job easily
#add_custom_target(Doxygen DEPENDS ${DOXYGEN_INDEX_FILE})

set(SPHINX_SOURCE ${CMAKE_CURRENT_BINARY_DIR})
set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/html)
set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)

if (UNIX)
  set(separator ":")
else ()
  set(separator ";")
endif()

# Nice named target so we can run the job easily
# Only regenerate Sphinx when:
# - Doxygen has rerun
# - Our doc files have been updated
# - The Sphinx config has been updated

set(RST_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/connection.rst
  ${CMAKE_CURRENT_SOURCE_DIR}/getting-started.rst
  ${CMAKE_CURRENT_SOURCE_DIR}/index.rst
  ${CMAKE_CURRENT_SOURCE_DIR}/python-api.rst
  )
set(RST_CPP_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/cppdoc/cpp-api.rst
  ${CMAKE_CURRENT_SOURCE_DIR}/cppdoc/cpp-important-types-and-functions.rst
  )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in ${CMAKE_CURRENT_BINARY_DIR}/conf.py)

make_directory(${CMAKE_CURRENT_BINARY_DIR}/cppdoc)

add_custom_command(OUTPUT ${SPHINX_INDEX_FILE}
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different ${RST_FILES} ${CMAKE_CURRENT_BINARY_DIR}
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different ${RST_CPP_FILES} ${CMAKE_CURRENT_BINARY_DIR}/cppdoc/
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/../tools/SEGYImport/README.md ${CMAKE_CURRENT_BINARY_DIR}/tools/SEGYImport/README.md
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/../tools/SEGYExport/README.md ${CMAKE_CURRENT_BINARY_DIR}/tools/SEGYExport/README.md
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/../tools/VDSInfo/README.md    ${CMAKE_CURRENT_BINARY_DIR}/tools/VDSInfo/README.md
                   COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/apidoc.py -f -o cppdoc ${CMAKE_CURRENT_BINARY_DIR}/doxygen/xml/
                   COMMAND ${CMAKE_COMMAND} -E env "PYTHONPATH=$<TARGET_FILE_DIR:core>/.." "PATH=${separator}${CMAKE_CURRENT_BINARY_DIR}"
                     ${Python3_EXECUTABLE} -m sphinx -b html
                   ${SPHINX_SOURCE} ${SPHINX_BUILD}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   DEPENDS
                   # Other docs files you want to track should go here (or in some variable)
                   ${RST_FILES}
                   ${RST_CPP_FILES}
                   ${CMAKE_CURRENT_SOURCE_DIR}/../tools/SEGYImport/README.md
                   ${CMAKE_CURRENT_SOURCE_DIR}/../tools/SEGYExport/README.md
                   ${CMAKE_CURRENT_SOURCE_DIR}/../tools/VDSInfo/README.md
                   ${CMAKE_CURRENT_SOURCE_DIR}/apidoc.py
                   Doxygen
                   openvds-javadoc
                   MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in
                   COMMENT "Generating documentation with Sphinx")

add_custom_target(Sphinx ALL DEPENDS ${SPHINX_INDEX_FILE})

# Add an install target to install the docs
install(DIRECTORY ${SPHINX_BUILD} DESTINATION ${CMAKE_INSTALL_DOCDIR})

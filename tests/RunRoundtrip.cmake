function(check_variable variable)
  if (NOT ${variable})
    message(FATAL_ERROR "Missing ${variable} variable")
  endif()
endfunction()

check_variable(SEGYImport)
check_variable(SEGYExport)
check_variable(TEST_URL)
check_variable(TEST_SEGY_FILE)

set(connection_parameter "--url ${TEST_URL}")
if (TEST_CONNECTION)
  set(CONNECTION "--connection")
endif()



execute_process(COMMAND ${SEGYImport} --persistentID roundtrip_test --url "${TEST_URL}" ${CONNECTION} ${TEST_CONNECTION} ${TEST_SEGY_FILE} RESULT_VARIABLE CMD_RESULT)
if (CMD_RESULT)
  message(FATAL_ERROR "Failed to run SEGYImport")
endif()

execute_process(COMMAND ${SEGYExport} --persistentID roundtrip_test --url "${TEST_URL}" ${CONNECTION} ${TEST_CONNECTION} roundtrip_test.segy RESULT_VARIABLE CMD_RESULT)
if (CMD_RESULT)
  message(FATAL_ERROR "Failed to run SEGYExport")
endif()

file(SHA256 ${TEST_SEGY_FILE} INPUT_SHA)
file(SHA256 roundtrip_test.segy OUTPUT_SHA)

if (INPUT_SHA STREQUAL OUTPUT_SHA)
  message("Input segy is equal to output segy. Compared hash is ${INPUT_SHA}")
else()
  message(FATAL_ERROR "The generated SEGY file is not equal to the input SEGY file")
endif()

# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\JupiterClient_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\JupiterClient_autogen.dir\\ParseCache.txt"
  "JupiterClient_autogen"
  "tests\\integration\\CMakeFiles\\integration_tests_autogen.dir\\AutogenUsed.txt"
  "tests\\integration\\CMakeFiles\\integration_tests_autogen.dir\\ParseCache.txt"
  "tests\\integration\\integration_tests_autogen"
  "tests\\ui\\CMakeFiles\\ui_tests_autogen.dir\\AutogenUsed.txt"
  "tests\\ui\\CMakeFiles\\ui_tests_autogen.dir\\ParseCache.txt"
  "tests\\ui\\ui_tests_autogen"
  "tests\\unit\\CMakeFiles\\unit_tests_autogen.dir\\AutogenUsed.txt"
  "tests\\unit\\CMakeFiles\\unit_tests_autogen.dir\\ParseCache.txt"
  "tests\\unit\\unit_tests_autogen"
  )
endif()

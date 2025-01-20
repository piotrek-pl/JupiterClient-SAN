# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\JupiterClient_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\JupiterClient_autogen.dir\\ParseCache.txt"
  "JupiterClient_autogen"
  )
endif()

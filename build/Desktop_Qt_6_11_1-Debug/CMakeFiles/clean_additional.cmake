# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/web_scrapper_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/web_scrapper_autogen.dir/ParseCache.txt"
  "web_scrapper_autogen"
  )
endif()

project(stb)

file(GLOB STB_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB STB_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp*")

message("files in thrid_party/stb directory \n source files ${STB_SOURCE}\n header files ${STB_HEADER}\n")
add_library(stb ${STB_SOURCE})
target_include_directories(stb PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
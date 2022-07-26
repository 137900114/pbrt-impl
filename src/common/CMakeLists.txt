#common only contains hpp files
file(GLOB COMMON_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB COMMON_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

message("files in common directory \n header files ${COMMON_HEADER}\n source files ${COMMON_SOURCE}")
target_sources(Alex_lib PRIVATE ${COMMON_SOURCE} ${COMMON_HEADER})
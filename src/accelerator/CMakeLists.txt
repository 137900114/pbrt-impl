file(GLOB ACCEL_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB ACCEL_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

message("files in acclerator directory \n source files ${ACCEL_SOURCE}\n header files ${ACCEL_HEADER}")
target_sources(Alex_lib PRIVATE ${ACCEL_SOURCE} ${ACCEL_HEADER})
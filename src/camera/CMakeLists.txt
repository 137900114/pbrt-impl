file(GLOB CAMERA_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB CAMERA_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

message("files in camera directory \n source files ${CAMERA_SOURCE} \n header files ${CAMERA_HEADER}\n")
target_sources(Alex_lib PRIVATE ${CAMERA_SOURCE} ${CAMERA_HEADER})
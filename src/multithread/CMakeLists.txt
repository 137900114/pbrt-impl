file(GLOB MTHREAD_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB MTHREAD_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

message("files in multithread directory \n source files ${MTHREAD_SOURCE} \n header files ${MTHREAD_HEADER}\n")
target_sources(Alex_lib PRIVATE ${MTHREAD_SOURCE} ${MTHREAD_HEADER})
file(GLOB INTEG_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB INTEG_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

message("files in integrater directory \n source files ${INTEG_SOURCE} \n header files ${INTEG_HEADER}\n")
target_sources(Alex_lib PRIVATE ${INTEG_SOURCE} ${INTEG_HEADER})
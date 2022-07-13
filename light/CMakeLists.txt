file(GLOB LIGHT_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB LIGHT_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

message("files in light directory \n header files ${LIGHT_HEADER}\n source files ${LIGHT_SOURCE}")
target_sources(Alex PUBLIC ${LIGHT_SOURCE} ${LIGHT_HEADER})
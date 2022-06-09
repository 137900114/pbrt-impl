#common only contains hpp files
file(GLOB COMMON_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

message("files in common directory \n source files ${COMMON_SOURCE}\n")
target_sources(Alex PUBLIC ${COMMON_SOURCE})
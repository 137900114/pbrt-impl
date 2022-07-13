file(GLOB SAMPLER_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB SAMPLER_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

message("files in sampler directory \n source files ${SAMPLER_SOURCE}\n header files ${SAMPLER_HEADER}")
target_sources(Alex PUBLIC ${SAMPLER_SOURCE} ${SAMPLER_HEADER})
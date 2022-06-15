#math only contains hpp files
file(GLOB MATH_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB MATH_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

message("files in math directory \n source files ${MATH_SOURCE}\nheader files ${MATH_HEADER}")
target_sources(Alex PUBLIC ${MATH_SOURCE} ${MATH_HEADER})
file(GLOB MMD_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB MMD_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/mmd/*.h")

message("files in thrid_party/mmd directory \n source files ${MMD_SOURCE}\n")
add_library(mmd ${MMD_SOURCE} ${MMD_HEADER})
target_include_directories(mmd PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/mmd)

#Without this cmake will complain lack of linker language but I don't know why.(other libraries are fine)
set_target_properties(mmd PROPERTIES LINKER_LANGUAGE C++)


file(GLOB SCENE_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB SCENE_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

message("files in scene directory \n source files ${SCENE_SOURCE} \n header files ${SCENE_HEADER}\n")
target_sources(Alex_lib PRIVATE ${SCENE_SOURCE} ${SCENE_HEADER})
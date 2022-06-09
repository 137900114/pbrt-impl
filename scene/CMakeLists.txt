file(GLOB SCENE_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB SCENE_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

message("files in scene directory \n source files ${SCENE_SOURCE} \n header files ${SCENE_HEADER}\n")
target_sources(Alex PUBLIC ${SCENE_SOURCE} ${SCENE_HEADER})
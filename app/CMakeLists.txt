#add main file to excutable
file(GLOB APP_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB APP_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

message("files in app directory \n source files ${APP_SOURCE} \n header files ${APP_HEADER}\n")
target_sources(Alex PUBLIC ${APP_SOURCE} ${APP_HEADER})
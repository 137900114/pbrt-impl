cmake_minimum_required(VERSION 3.21)
set (CMAKE_CXX_STANDARD 17)

project(Alex)

add_executable(Alex "")
target_include_directories(Alex PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

#macro that marks some debug only code
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -DENABLE_PROFILE")

#macro that marks some release only code
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DRELEASE")

add_subdirectory(thrid_party)
add_subdirectory(app)
add_subdirectory(accelerator)
add_subdirectory(math)
add_subdirectory(common)
add_subdirectory(scene)

#copy the dll file
add_custom_command(TARGET Alex POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:assimp>  $<TARGET_FILE_DIR:Alex>
  COMMAND_EXPAND_LISTS
  )
  
add_dependencies(Alex spdlog stb)
target_link_libraries(Alex spdlog stb assimp)

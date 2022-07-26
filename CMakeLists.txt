cmake_minimum_required(VERSION 3.21)
set (CMAKE_CXX_STANDARD 17)

project(Alex)

add_library(Alex_lib "")
add_executable(Alex_exe "")

#macro that marks some debug only code
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -DENABLE_PROFILE")

#macro that marks some release only code
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DRELEASE")

add_subdirectory(thrid_party)
add_subdirectory(src)

#copy the dll file
add_custom_command(TARGET Alex_exe POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:assimp>  $<TARGET_FILE_DIR:Alex_exe>
  COMMAND_EXPAND_LISTS
  )

add_custom_command(TARGET Alex_exe POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:optick>  $<TARGET_FILE_DIR:Alex_exe>
  COMMAND_EXPAND_LISTS
  )
  
add_dependencies(Alex_lib spdlog stb)
add_dependencies(Alex_exe Alex_lib)
target_link_libraries(Alex_lib spdlog stb assimp optick)
target_link_libraries(Alex_exe Alex_lib)

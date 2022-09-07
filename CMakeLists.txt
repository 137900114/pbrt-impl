cmake_minimum_required(VERSION 3.21)
set (CMAKE_CXX_STANDARD 17)

project(Alex)

add_library(Alex_lib STATIC "")
add_executable(Alex "")

option(ENABLE_TEST "enable unit test cases" ON)

#macro that marks some debug only code
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -DENABLE_PROFILE")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DRELEASE")

if(ENABLE_TEST)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DENABLE_TEST")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DENABLE_TEST")
endif()

#print compile flags
set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        )
foreach(CompilerFlag ${CompilerFlags})
  message("MSVC flags: ${CompilerFlag}:${${CompilerFlag}}")
endforeach()

#macro that marks some release only code
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DRELEASE")

add_subdirectory(third_party)
add_subdirectory(src)

#copy the dll file
add_custom_command(TARGET Alex POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:assimp>  $<TARGET_FILE_DIR:Alex>
   COMMAND_EXPAND_LISTS
   )

 add_custom_command(TARGET Alex POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:OptickCore>  $<TARGET_FILE_DIR:Alex>
   COMMAND_EXPAND_LISTS
   )
   add_custom_command(TARGET Alex POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:spdlog>  $<TARGET_FILE_DIR:Alex>
   COMMAND_EXPAND_LISTS
   )

 add_custom_command(TARGET Alex POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:tinyxml2>  $<TARGET_FILE_DIR:Alex>
   COMMAND_EXPAND_LISTS
   )
  
add_dependencies(Alex_lib spdlog stb mmd assimp OptickCore tinyxml2)
add_dependencies(Alex Alex_lib)
target_link_libraries(Alex_lib spdlog stb OptickCore mmd assimp tinyxml2)
target_link_libraries(Alex Alex_lib)

if(ENABLE_TEST)
  message("testing is enabled")
  enable_testing()
  add_subdirectory(test)
endif()

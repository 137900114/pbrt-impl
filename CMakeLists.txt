cmake_minimum_required(VERSION 3.21)
set (CMAKE_CXX_STANDARD 17)

project(Alex)

add_library(Alex_lib "")
add_executable(Alex_exe "")

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

if(ENABLE_TEST)
  message("testing is enabled")
  enable_testing()
  add_subdirectory(test)
endif()

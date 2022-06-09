add_subdirectory(spdlog)
add_subdirectory(stb)
add_subdirectory(assimp)

message("add ${CMAKE_CURRENT_SOURCE_DIR}/spdlog/include to include directory")
target_include_directories(Alex PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/spdlog/include)
message("add ${CMAKE_CURRENT_SOURCE_DIR}/stb/include to include directory")
target_include_directories(Alex PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/stb/include)
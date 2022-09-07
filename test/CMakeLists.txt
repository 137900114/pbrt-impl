# Now simply link against gtest or gtest_main as needed. Eg

set(test_cases math_test  multithread_test bvh_test)

message("***testing include directory : ${GTEST_INCLUDE}")

foreach(test_case ${test_cases})
  message("adding test case ${test_case}...")

  add_executable(
      ${test_case}
      ${test_case}.cpp
    )
  target_link_libraries(
      ${test_case}
      gtest
      Alex_lib
    )

  target_include_directories(
      ${test_case}
      PUBLIC
      ${CMAKE_SOURCE_DIR}/src
      ${GTEST_INCLUDE}
    )

  add_test(
    NAME
     ${test_case}
    COMMAND
      $<TARGET_FILE_DIR:${test_case}>/${test_case}
    )

add_custom_command(TARGET ${test_case} POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:gtest>  $<TARGET_FILE_DIR:${test_case}>
   COMMAND_EXPAND_LISTS
   )

   add_custom_command(TARGET ${test_case} POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:assimp>  $<TARGET_FILE_DIR:${test_case}>
   COMMAND_EXPAND_LISTS
   )

 add_custom_command(TARGET ${test_case} POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:OptickCore>  $<TARGET_FILE_DIR:${test_case}>
   COMMAND_EXPAND_LISTS
   )

   add_custom_command(TARGET ${test_case} POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:spdlog>  $<TARGET_FILE_DIR:${test_case}>
   COMMAND_EXPAND_LISTS
   )

  message("test case ${test_case} is added")
endforeach()

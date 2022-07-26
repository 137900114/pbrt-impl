# Now simply link against gtest or gtest_main as needed. Eg
add_executable(
    math_test
    math_test.cpp
    )

target_link_libraries(
    math_test
    ${gtest}
    Alex_lib
    )

target_include_directories(
    math_test
    PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${GTEST_INCLUDE}
)

if(MSVC)
  set_target_properties(math_test PROPERTIES COMPILE_FLAGS "/MTd")
endif()


add_test(
  NAME
    math_test
  COMMAND
    $<TARGET_FILE_DIR:math_test>/math_test
  )


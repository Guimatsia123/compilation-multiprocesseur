add_library(testutils testutils.cpp wrapper.c)
target_include_directories(testutils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(test_justmake test_justmake.cpp)
target_link_libraries(test_justmake PRIVATE jml testutils Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(test_justmake)

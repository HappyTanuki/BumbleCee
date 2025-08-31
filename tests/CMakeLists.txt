file(GLOB_RECURSE TEST_SOURCES "*.cpp" "*.cxx" "*.cc")

foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} PRIVATE ${BOT_NAME}_lib)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
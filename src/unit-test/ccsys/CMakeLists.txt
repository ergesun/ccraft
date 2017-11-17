aux_source_directory(. SRCS)

add_executable(ccsys_test ${SRCS})
target_link_libraries(ccsys_test ccsys gtest pthread)

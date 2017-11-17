aux_source_directory(. SRCS)

add_executable(fsio_test ${SRCS})
target_link_libraries(fsio_test fsio gtest pthread)

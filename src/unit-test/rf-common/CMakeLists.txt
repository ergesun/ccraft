aux_source_directory(. SRCS)

add_executable(rfcommon_test ${SRCS})
target_link_libraries(rfcommon_test rfcommon common protobuf glog gflags gtest stdc++ pthread)

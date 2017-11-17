aux_source_directory(. SRCS)
aux_source_directory(./tcp TCP_SRCS)
aux_source_directory(./tcp/server TCP_SRV_SRCS)

add_executable(net_test ${SRCS} ${TCP_SRCS} ${TCP_SRV_SRCS})

add_dependencies(net_test net common)
target_link_libraries(net_test net common ccsys protobuf glog gflags gtest pthread)



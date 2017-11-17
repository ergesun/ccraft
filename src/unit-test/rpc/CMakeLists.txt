aux_source_directory(. SRCS)
aux_source_directory(../../server/node/rpc CLIENT_SRCS)
aux_source_directory(./server SERVER_SRCS)

add_executable(rpc_test ${SRCS} ${CLIENT_SRCS} ${SERVER_SRCS})
target_link_libraries(rpc_test rpc protobufgen protobuf net common ccsys glog gflags gtest pthread)

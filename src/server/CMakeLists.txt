aux_source_directory(. SOURCE_FILES)
aux_source_directory(./node ND_SRCS)
aux_source_directory(./node/rpc ND_RPC_SRCS)

add_executable(ccraftd ${SOURCE_FILES} ${ND_SRCS} ${ND_RPC_SRCS})

set(LINK_LIBS
        protobufgen
        protobuf
        rpc
        net
        common
        glog
        gflags
        pthread
        )

set(DEPEN_LIBS
        protobufgen
        net
        common
        )

add_dependencies(ccraftd ${DEPEN_LIBS})
target_link_libraries(ccraftd ${LINK_LIBS})

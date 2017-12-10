aux_source_directory(.                 SOURCE_FILES)
aux_source_directory(./node            ND_SRCS)
aux_source_directory(./node/rpc        ND_RPC_SRCS)
aux_source_directory(./rflog           RFLOG_SRCS)

add_executable(ccraftd ${SOURCE_FILES} ${ND_SRCS} ${ND_RPC_SRCS} ${RFLOG_SRCS}
        ./rflog/irflogger.h node/iservice-rpc-callback-handler.h node/rpc/common-def.h)

set(LINK_LIBS
        protobufgen
        protobuf
        rpc
        net
        fsio
        common
        ccsys
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

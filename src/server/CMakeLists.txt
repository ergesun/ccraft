aux_source_directory(. SOURCE_FILES)
add_executable(ccraftd ${SOURCE_FILES})

set(LINK_LIBS
        net
        common
        glog
        gflags
        pthread
        )

set(DEPEN_LIBS
        net
        common
        )

add_dependencies(ccraftd ${DEPEN_LIBS})
target_link_libraries(ccraftd ${LINK_LIBS})

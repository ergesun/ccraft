aux_source_directory(. SRCS)
add_library(common SHARED ${SRCS}
                            thread-pool.h
                            blocking-queue.h
                            buffer.h
                            cctime.h
                            errors.h
                            resource-pool.h
                            )

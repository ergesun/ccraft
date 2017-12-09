aux_source_directory(. SRCS)
aux_source_directory(./client-bases CB_SRCS)
add_library(rpc SHARED ${SRCS} ${CB_SRCS})

aux_source_directory(. SRCS)
add_library(rpc SHARED ${SRCS} common-def.h exceptions.h)

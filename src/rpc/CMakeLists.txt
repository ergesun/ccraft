aux_source_directory(. SRCS)
add_library(rpc SHARED ${SRCS} common-def.h framework-response.h)

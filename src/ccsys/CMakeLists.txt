aux_source_directory(. SRCS)
add_library(ccsys SHARED ${SRCS} cctime.h)

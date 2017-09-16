/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_GFLAGS_CONFIG_H
#define CCRAFT_GFLAGS_CONFIG_H

#include <gflags/gflags.h>

/**
 * glog
 */
DECLARE_string(glog_dir);
DECLARE_bool(glog_prefix);
DECLARE_int32(max_glog_size);
DECLARE_int32(glogbufsecs);
DECLARE_int32(mingloglevel);
DECLARE_bool(stop_glogging_if_full_disk);
DECLARE_bool(glogtostderr);
DECLARE_bool(alsoglogtostderr);
DECLARE_bool(colorglogtostderr);

/**
 * net common
 */
DECLARE_int32(net_server_handshake_threads_cnt);


/**
 * internal rpc server
 */
DECLARE_int32(internal_rpc_client_threads_cnt);
DECLARE_int32(internal_rpc_client_wait_timeout_secs);
DECLARE_int64(internal_rpc_client_wait_timeout_nsecs);
DECLARE_int32(internal_rpc_server_threads_cnt);
DECLARE_int32(internal_rpc_messenger_threads_cnt);
DECLARE_int32(internal_rpc_io_threads_cnt);

/**
 * rf node
 */
DECLARE_int32(server_id);

#endif //CCRAFT_GFLAGS_CONFIG_H

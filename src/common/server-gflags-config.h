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
DECLARE_int32(net_server_connect_timeout);

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
 * rf server
 */
DECLARE_int32(server_id);
DECLARE_string(data_dir);
DECLARE_string(rflogger_type);
DECLARE_int32(raft_election_interval);
DECLARE_double(raft_election_timeout_percent);
DECLARE_string(servers_conf_path);
DECLARE_int32(leader_hb_timeout_duration);
DECLARE_int32(start_election_rand_latency_low);
DECLARE_int32(start_election_rand_latency_high);

#endif //CCRAFT_GFLAGS_CONFIG_H

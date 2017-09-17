/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <gflags/gflags.h>

/**
 * glog配置
 */
DEFINE_string(glog_dir, "/tmp/ccraft/logs", "glog的保存路径");
DEFINE_bool(glog_prefix, true, "设置日志前缀是否应该添加到每行输出");
DEFINE_int32(max_glog_size, 100, "设置最大日志文件大小（以MB为单位）");
DEFINE_int32(glogbufsecs, 0, "设置可以缓冲日志的最大秒数，0指实时输出");
DEFINE_int32(mingloglevel, 0, "设置log输出级别(包含)：INFO = 0, WARNING = 1, ERROR = 2,  FATAL = 3");
DEFINE_bool(stop_glogging_if_full_disk, true, "设置是否在磁盘已满时避免日志记录到磁盘");
DEFINE_bool(glogtostderr, false, "设置日志消息是否转到标准输出而不是日志文件");
DEFINE_bool(alsoglogtostderr, false, "设置日志消息除了日志文件之外是否去标准输出");
DEFINE_bool(colorglogtostderr, true, "设置记录到标准输出的颜色消息（如果终端支持）");

/**
 * net common
 */
DEFINE_int32(net_server_handshake_threads_cnt, 16, "网络框架服务端处理握手的线程个数");

/**
 * rpc server common
 */
DEFINE_int32(internal_rpc_client_threads_cnt, 0, "设置用于rpc client处理任务的线程池线程个数。默认0为cpu逻辑核数的2倍。");
DEFINE_int32(internal_rpc_client_wait_timeout_secs, 10, "设置用rpc client等待请求的最大时长的秒数部分。");
DEFINE_int64(internal_rpc_client_wait_timeout_nsecs, 0, "设置用于rpc client等待请求的最大时长的纳秒数部分，与秒数共同组成等待时长。");
DEFINE_int32(internal_rpc_server_threads_cnt, 0, "设置用于rpc server处理任务的线程池线程个数。默认0为cpu逻辑核数的2倍。");
DEFINE_int32(internal_rpc_messenger_threads_cnt, 0, "设置用于internal rpc处理的信使的消息分发的线程数目。默认0为cpu逻辑核数的2倍。");
DEFINE_int32(internal_rpc_io_threads_cnt, 0, "设置用于node间通信的rpc服务的网络io处理线程个数。默认0为cpu逻辑核数的一半。");

/**
 * rf node
 */
DEFINE_int32(server_id, 0, "设置server id，从0开始。");
DEFINE_string(data_dir, "/tmp/ccraft", "数据存放目录");
DEFINE_string(rflogger_type, "realtime-disk", "rflogger类型");

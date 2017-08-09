/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <gflags/gflags.h>

/**
 * glog配置
 */
DEFINE_string(glog_dir, "/tmp/logs/jcloud-acc", "glog的保存路径");
DEFINE_bool(glog_prefix, true, "设置日志前缀是否应该添加到每行输出");
DEFINE_int32(max_glog_size, 100, "设置最大日志文件大小（以MB为单位）");
DEFINE_int32(glogbufsecs, 0, "设置可以缓冲日志的最大秒数，0指实时输出");
DEFINE_int32(mingloglevel, 0, "设置log输出级别(包含)：INFO = 0, WARNING = 1, ERROR = 2,  FATAL = 3");
DEFINE_bool(stop_glogging_if_full_disk, true, "设置是否在磁盘已满时避免日志记录到磁盘");
DEFINE_bool(glogtostderr, false, "设置日志消息是否转到标准输出而不是日志文件");
DEFINE_bool(alsoglogtostderr, false, "设置日志消息除了日志文件之外是否去标准输出");
DEFINE_bool(colorglogtostderr, true, "设置记录到标准输出的颜色消息（如果终端支持）");

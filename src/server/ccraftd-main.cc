/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <exception>
#include <sstream>
#include <map>

#include <sys/stat.h>
#include <csignal>
#include <condition_variable>

#include "../common/common-def.h"
#include "../common/errors.h"
#include "../common/file-utils.h"
#include "../common/gflags-config.h"

using std::string;
using std::stringstream;
using std::map;

using ccraft::common::FileUtils;

//RpcService                     *m_pRpcInst = nullptr;
//HttpServerService              *m_pHttpServerInst = nullptr;
//jcloud::acc::ACC               *m_pAccInst = nullptr;

bool                            m_bAllStopped = false;
std::mutex                      m_mtxRpc;
std::condition_variable         m_cvRpc;

void init_gflags_glog(int *argc, char ***argv) {
    gflags::ParseCommandLineFlags(argc, argv, true);
    google::InitGoogleLogging((*argv)[0]);
    stringstream ss;
    //ss << FLAGS_glog_dir << "/instance-" << FLAGS_thriftserver_port;
    FLAGS_log_dir = ss.str();
    if (-1 == FileUtils::CreateDirPath(FLAGS_log_dir.c_str(), 0775)) {
        THROW_CREATE_DIR_ERR();
    }

    FLAGS_log_prefix = FLAGS_glog_prefix;
    FLAGS_max_log_size = FLAGS_max_glog_size;
    FLAGS_logbufsecs = FLAGS_glogbufsecs;
    FLAGS_minloglevel = FLAGS_mingloglevel;
    FLAGS_stop_logging_if_full_disk = FLAGS_stop_glogging_if_full_disk;
    FLAGS_logtostderr = FLAGS_glogtostderr;
    FLAGS_alsologtostderr = FLAGS_alsoglogtostderr;
    FLAGS_colorlogtostderr = FLAGS_colorglogtostderr;
}

void uninit_gflags_glog() {
    google::ShutdownGoogleLogging();
    gflags::ShutDownCommandLineFlags();
}

void signal_handler(int sig) {
    static const map<int, string> what_sig = map<int, string>{
        {SIGHUP, "SIGHUP"},
        {SIGINT, "SIGINT"},
        {SIGTERM, "SIGTERM"},
        {SIGPIPE, "SIGPIPE"},
        {SIGALRM, "SIGALRM"}
    };

    auto iter = what_sig.find(sig);
    if (iter != what_sig.end()) {
        LOGWFUN << "catch " << iter->second << " signal";
    } else {
        LOGWFUN << "default catch " << sig << " signal";
    }

    switch (sig) {
        case SIGHUP: {
            //std::string bizConfPath = FLAGS_acc_biz_conf_path.c_str();
            //m_pAccInst->Reload(bizConfPath);
            break;
        }
        case SIGINT:
        case SIGTERM: {
            // TODO(sunchao): 抽象一个ServiceManager和一个IService接口?
            std::unique_lock<std::mutex> l(m_mtxRpc);
            //m_pRpcInst->Stop();
            //m_pHttpServerInst->Stop();

            m_bAllStopped = true;
            m_cvRpc.notify_one();
            break;
        }
        default: {
            break;
        }
    }
}

void register_signal() {
    struct sigaction newSigaction;
    newSigaction.sa_handler = signal_handler;
    sigemptyset(&newSigaction.sa_mask);
    newSigaction.sa_flags = 0;
    sigaction(SIGHUP, &newSigaction, nullptr); /* catch hangup signal */
    sigaction(SIGINT, &newSigaction, nullptr); /* catch term signal */
    sigaction(SIGTERM, &newSigaction, nullptr); /* catch interrupt signal */
    sigaction(SIGPIPE, &newSigaction, nullptr); /* catch pipe signal */
    sigaction(SIGALRM, &newSigaction, nullptr); /* catch alarm signal */
}

void wait_all_stopped() {
    std::unique_lock<std::mutex> l(m_mtxRpc);
    while (!m_bAllStopped) {
        m_cvRpc.wait(l);
    }
}

int
main(int argc, char *argv[])
try {
    if (argc != 2) {
        LOGEFUN << "you must just start app with --flagfile=\"xxx\" args";
        return -1;
    }

    /**
     * 不要删除这个。
     */
    umask(0);
    init_gflags_glog(&argc, &argv);
    register_signal();
    ccraft::common::initialize();
    // create acc
    //m_pAccInst = new jcloud::acc::ACC();

    //std::string bizConfPath = FLAGS_acc_biz_conf_path.c_str();
    ////m_pAccInst->Init(bizConfPath);
//
    //// start rpc service
    //m_pRpcInst = new RpcService(m_pAccInst, FLAGS_thriftserver_port,
    //                            (size_t)(FLAGS_thriftserver_resident_threads_cnt),
    //                            (size_t)(FLAGS_thriftserver_pending_threads_cnt));
    //LOGIFUN << "start thrift server port = " << FLAGS_thriftserver_port;
    //m_pRpcInst->Start();

    //m_pHttpServerInst = new HttpServerService(m_pAccInst);
    //m_pHttpServerInst->Start();
    LOGIFUN << "access control service is running!";
    wait_all_stopped();

    //DELETE_PTR(m_pRpcInst);
    //DELETE_PTR(m_pAccInst);
    //DELETE_PTR(m_pHttpServerInst);

    uninit_gflags_glog();

    return 0;
} catch (std::exception &ex) {
    LOG(FATAL) << ex.what();
    uninit_gflags_glog();
} catch (std::exception *ex) {
    LOG(FATAL) << ex->what();
    uninit_gflags_glog();
} catch (...) {
    LOG(FATAL) << "Unknown exception occur!";
    uninit_gflags_glog();
}

/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_SERVER_SERVICE_MANAGER_H
#define CCRAFT_SERVER_SERVICE_MANAGER_H

#include <map>
#include <mutex>
#include <condition_variable>

#include "../../common/spin-lock.h"
#include "common-def.h"

namespace ccraft {
class IService;
namespace server {
class ServiceManager {
public:
    /**
     * services引导加载程序。
     */
    class ServiceBootstrap {
    public:
        /**
         * 引导所有services启动。
         * 只需要调用一次，不过已经有了安全保障，随你调，我自固若金汤;-)
         */
        static void Run();

    private:
        static void create_services();
        static void start_services();

    private:
        static common::spin_lock_t s_boot_mtx;
        static bool s_bServsBooted;
    };

    class ServiceDestroyer {
    public:
        /**
         * 停止所有services
         */
        static void Run();
        /**
         * 阻塞调用。当services都停止时该函数返回。
         */
        static void WaitAllServicesStopped();

    private:
        static std::mutex s_mtxServsLiving;
        static bool s_bAllServsStopped;
        static std::condition_variable s_cvServsLiving;
    };

    /**
     * 根据类型获取service实例。
     * @param st
     * @return
     */
    static IService* GetService(ServiceType st);

private:
    //friend class ServiceBootstrap;
    static std::map<ServiceType, IService*> s_mapServices;
};
}
}

#endif //CCRAFT_SERVER_SERVICE_MANAGER_H

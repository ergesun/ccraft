/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <iostream>

#include "factory.h"
#include "../network-api/posix/local/simple-read-event-handler.h"
#include "../../../../../ccsys/utils.h"

#include "event-worker.h"

namespace ccraft {
namespace net {
EventWorker::EventWorker(uint32_t maxEvents, NonBlockingEventModel m) {
    m_vDriverInternalEvents.resize(maxEvents);
    m_pEventDriver = EventDriverFactory::GetNewDriver(m);
    m_pEventDriver->Init(maxEvents);
    int fds[2];
    if (pipe(fds) < 0) {
        auto err = errno;
        LOGEFUN << " can't create notify pipe with errmsg = " << strerror(err);
        throw std::runtime_error("Create notify pipe failed!");
    }

    m_notifyRecvFd = fds[0];
    m_notifySendFd = fds[1];
    auto r = ccsys::Utils::SetNonBlocking(m_notifyRecvFd);
    if (r < 0) {
        auto err = errno;
        LOGEFUN << " can't set notify pipe non-blocking with errmsg = " << strerror(err);
        throw std::runtime_error("Set notify pipe non-blocking failed!");
    }

    r = ccsys::Utils::SetNonBlocking(m_notifySendFd);
    if (r < 0) {
        auto err = errno;
        LOGEFUN << " can't set notify pipe non-blocking with errmsg = " << strerror(err);
        throw std::runtime_error("Set notify pipe non-blocking failed!");
    }

    m_pLocalReadEventHandler = new PosixLocalReadEventHandler(m_notifyRecvFd);
    if (!m_pLocalReadEventHandler->Initialize()) {
        throw std::runtime_error("cannot initialize PosixLocalReadEventHandler.");
    }
    m_pEventDriver->AddEvent(m_pLocalReadEventHandler, EVENT_NONE, EVENT_READ);
}

EventWorker::~EventWorker() {
    DELETE_PTR(m_pEventDriver);
    DELETE_PTR(m_pLocalReadEventHandler);
    close(m_notifySendFd);
}

void EventWorker::Wakeup() {
    char buf = 'w';
    // wake up "event_wait"
    auto n = ::write(m_notifySendFd, &buf, sizeof(buf));
    if (n < 0) {
        LOGEFUN << " write notify pipe failed: " << strerror(errno);
        throw std::runtime_error("Write notify pipe failed!");
    }

    fsync(m_notifySendFd);
}
}  // namespace net
}  // namespace ccraft

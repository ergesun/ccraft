//
// Created by sunchao31 on 17-8-9.
//

#ifndef CCRAFT_ISERVICE_H
#define CCRAFT_ISERVICE_H

namespace ccraft {
/**
 * 所有服务的统一接口类。
 */
class IService {
public:
    virtual bool Start() = 0;
    virtual bool Stop() = 0;
    virtual ~IService() {};
};
} // namespace ccraft

#endif //CCRAFT_ISERVICE_H

/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HC_STARTUP_CFG_GEN_H
#define HC_STARTUP_CFG_GEN_H

#include <fstream>
#include <map>
#include "generator.h"

namespace OHOS {
namespace Hardware {
enum EXPECT_TYPE {
    EXPECT_HOST_NAME,
    EXPECT_HOST_PRIORITY,
    EXPECT_HOST_CAPS,
    EXPECT_DEVICE_PROLOAD,
    EXPECT_TYPE_BUTT,
};
class StartupCfgGen : public Generator {
public:
    explicit StartupCfgGen(std::shared_ptr<Ast> ast);

    ~StartupCfgGen() override = default;

    bool Output() override;

private:
    bool Initialize();

    bool TemplateNodeSeparate();

    bool NodeWalk();

    void HostInfoOutput(const std::string &name, bool end);

    void HostInfoOutputMusl(const std::string &name, bool end);

    bool CheckDeepestNode(std::shared_ptr<AstObject> &current);

    void SetExpectType(std::shared_ptr<AstObject> &current, uint32_t type);

    void GetExpectVaule(std::shared_ptr<AstObject> &current, uint32_t type);

    void HeaderTopOutput();

    void HeaderBottomOutput();

    void SaveHostInfo();

    void HostInfosOutput();

    std::ofstream ofs_;
    std::string outFileName_;
    bool isDeviceInfo = false;
    bool preload = true;
    bool isFirstHost = true;
    std::string serviceName;
    std::string hostCaps;
    uint32_t hostPriority = 0;
    uint32_t hostId = 0;
    bool isDeepestNode = false;
    uint32_t expectType = EXPECT_TYPE_BUTT;
    uint16_t capArraySize = 0;
    bool useMusl = false;
    std::map<std::string, int> hostPriMap_;
    std::map<std::string, int> hostIdMap_;
    std::map<std::string, int> hostPreloadMap_;
    std::map<std::string, std::string> hostCapsMap_;
};
} // namespace Hardware
} // namespace OHOS
#endif // HC_STARTUP_CFG_GEN_H

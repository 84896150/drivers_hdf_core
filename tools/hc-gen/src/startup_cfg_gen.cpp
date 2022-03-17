/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <algorithm>
#include <string>
#include "file.h"
#include "logger.h"
#include "startup_cfg_gen.h"

using namespace OHOS::Hardware;

constexpr static const char *BOOT_CONFIG_TOP =
    "{\n"
    "    \"jobs\" : [{\n"
    "            \"name\" : \"post-fs-data\",\n"
    "            \"cmds\" : [\n"
    "                \"start hdf_devhost\"\n"
    "            ]\n"
    "        }\n"
    "    ],\n"
    "    \"services\" : [\n";
constexpr static const char *BOOT_CONFIG_BOTTOM =
    "    ]\n"
    "}\n";
constexpr static const char *SERVICE_TOP =
    "        {\n"
    "            \"name\" : ";
constexpr static const char *SERVICE_BOTTOM =
    "            \"path\" : [\"/vendor/bin/hdf_devhost\"],\n"
    "            \"uid\" : \"root\",\n"
    "            \"gid\" : [\"system\"]\n"
    "        }";
constexpr static const char *PATH_INFO = "            \"path\" : [\"/vendor/bin/hdf_devhost\", ";
constexpr static const char *UID_INFO_ALL =
    "            \"uid\" : \"root\",\n"
    "            \"gid\" : [\"system\"]\n"
    "        }";
constexpr static const char *UID_INFO = "            \"uid\" : ";
constexpr static const char *GID_INFO = "            \"gid\" : [";
constexpr static const char *CAPS_INFO = "            \"caps\" : [";
constexpr static const char *DYNAMIC_PART = "            \"dynamic\" : true,\n";

StartupCfgGen::StartupCfgGen(std::shared_ptr<Ast> ast) : Generator(ast) {}

void StartupCfgGen::HeaderTopOutput()
{
    ofs_ << BOOT_CONFIG_TOP;
}

void StartupCfgGen::HeaderBottomOutput()
{
    ofs_ << BOOT_CONFIG_BOTTOM;
    ofs_.close();
}

bool StartupCfgGen::Output()
{
    if (!Initialize()) {
        return false;
    }
    if (!TemplateNodeSeparate()) {
        return false;
    }
    HeaderTopOutput();

    if (!NodeWalk()) {
        return false;
    }

    HeaderBottomOutput();

    return true;
}

bool StartupCfgGen::Initialize()
{
    std::string outFileName = Option::Instance().GetOutputName();
    if (outFileName.empty()) {
        outFileName = Option::Instance().GetSourceNameBase();
    }
    outFileName = Util::File::StripSuffix(outFileName).append(".cfg");
    outFileName_ = Util::File::FileNameBase(outFileName);
    if (outFileName_.find("musl") != std::string::npos) {
        useMusl = true;
    }

    ofs_.open(outFileName, std::ofstream::out | std::ofstream::binary);
    if (!ofs_.is_open()) {
        Logger().Error() << "failed to open output file: " << outFileName;
        return false;
    }

    Logger().Debug() << "output: " << outFileName << outFileName_ << '\n';

    return true;
}

void StartupCfgGen::HostInfoOutputMusl(const std::string &name, bool end)
{
    std::string out = SERVICE_TOP;
    out.append("\"").append(name).append("\",\n");
    if (hostPreloadMap_[name]) {
        out.append(DYNAMIC_PART);
    }
    out.append(PATH_INFO).append("\"").append(std::to_string(hostIdMap_[name])).append("\"").append(", ");
    out.append("\"").append(name).append("\"").append("],\n");
    out.append(UID_INFO).append("\"").append(name).append("\"").append(",\n");
    out.append(GID_INFO).append("\"").append(name).append("\"").append("]");
    if (hostCapsMap_[name] != "") {
        out.append(",\n");
        out.append(CAPS_INFO).append(hostCapsMap_[name]).append("]");
    }
    out.append("\n").append(TAB).append(TAB).append("}");
    if (!end) {
        out.append(",");
    }
    ofs_ << out << '\n';
}

void StartupCfgGen::HostInfoOutput(const std::string &name, bool end)
{
    std::string out = SERVICE_TOP;
    out.append("\"").append(name).append("\",\n");
    if (hostPreloadMap_[name]) {
        out.append(DYNAMIC_PART);
    }
    out.append(PATH_INFO).append("\"").append(std::to_string(hostIdMap_[name])).append("\"").append(", ");
    out.append("\"").append(name).append("\"").append("],\n");
    out.append(UID_INFO_ALL);
    if (!end) {
        out.append(",");
    }
    ofs_ << out << '\n';
}

bool StartupCfgGen::TemplateNodeSeparate()
{
    return ast_->WalkBackward([this](std::shared_ptr<AstObject> &object, int32_t depth) {
        if (object->IsNode() && ConfigNode::CastFrom(object)->GetNodeType() == NODE_TEMPLATE) {
            object->Separate();
            return NOERR;
        }
        return NOERR;
    });
}

bool StartupCfgGen::CheckDeepestNode(std::shared_ptr<AstObject> &current)
{
    auto child = current->Child();
    while (child != nullptr) {
        if (child->IsNode()) {
            return false;
        }
        child = child->Next();
    }
    return true;
}

void StartupCfgGen::SaveHostInfo()
{
    hostPriMap_.insert(make_pair(serviceName, hostPriority));
    hostIdMap_.insert(make_pair(serviceName, hostId));
    hostPreloadMap_.insert(make_pair(serviceName, preload));
}

struct compare {
    bool operator() (const std::pair<std::string, int> &p1, const std::pair<std::string, int> &p2)
    {
        return p1.second < p2.second;
    }
};

void StartupCfgGen::HostInfosOutput()
{
    bool end = false;
    uint32_t cnt = 1;
    uint32_t size = hostPriMap_.size();
    std::vector<std::pair<std::string, int>> vect(hostPriMap_.begin(), hostPriMap_.end());
    sort(vect.begin(), vect.end(), compare());
    for (std::vector<std::pair<std::string, int>>::iterator it = vect.begin(); it != vect.end(); it++, cnt++) {
        if (cnt == size) {
            end = true;
        }
        if (useMusl) {
            HostInfoOutputMusl(it->first, end);
        } else {
            HostInfoOutput(it->first, end);
        }
    }
}

void StartupCfgGen::SetExpectType(std::shared_ptr<AstObject> &current, uint32_t type)
{
    if (type != PARSEROP_CONFTERM) {
        return;
    }
    if (current->Name() == "hostName") {
        if (!isFirstHost) {
            SaveHostInfo();
            hostId++;
        }
        if (isFirstHost) {
            isFirstHost = false;
        }

        preload = true;
        expectType = EXPECT_HOST_NAME;
        capArraySize = 0;
        hostCaps = "";
    } else if (isDeepestNode && current->Name() == "preload") {
        expectType = EXPECT_DEVICE_PROLOAD;
    } else if (!isDeepestNode && current->Name() == "priority") {
        expectType = EXPECT_HOST_PRIORITY;
    } else if (!isDeepestNode && current->Name() == "caps") {
        expectType = EXPECT_HOST_CAPS;
    } else {
        expectType = EXPECT_TYPE_BUTT;
    }
}

void StartupCfgGen::GetExpectVaule(std::shared_ptr<AstObject> &current, uint32_t type)
{
    if (expectType == EXPECT_TYPE_BUTT) {
        return;
    }
    if (type == PARSEROP_STRING && expectType == EXPECT_HOST_NAME) {
        serviceName = current->StringValue();
    } else if (type == PARSEROP_UINT64 || type == PARSEROP_UINT8 ||
        type == PARSEROP_UINT16 || type == PARSEROP_UINT32) {
        if (expectType == EXPECT_DEVICE_PROLOAD) {
            if (current->IntegerValue() == 0 || current->IntegerValue() == 1) {
                preload = false;
            }
        } else if (expectType == EXPECT_HOST_PRIORITY) {
            hostPriority = current->IntegerValue();
        }
    } else if (type == PARSEROP_ARRAY && expectType == EXPECT_HOST_CAPS) {
        capArraySize = ConfigArray::CastFrom(current)->ArraySize();
    } else if (type == PARSEROP_STRING && expectType == EXPECT_HOST_CAPS && capArraySize != 0) {
        hostCaps.append("\"").append(current->StringValue()).append("\"");
        if (capArraySize == 1) {
            hostCapsMap_.insert(make_pair(serviceName, hostCaps));
        } else {
            hostCaps.append(", ");
        }
        capArraySize--;
    }
}

bool StartupCfgGen::NodeWalk()
{
    return ast_->WalkForward([this](std::shared_ptr<AstObject> &current, int32_t depth) {
        auto type = current->Type();

        Logger().Debug() << "name,type:[" << current->Name() << "," << type << "] depth:" << depth << '\n';

        if (isDeviceInfo && depth == 1) {
            isDeviceInfo = false;
            SaveHostInfo();
            HostInfosOutput();
        }
        if (type == PARSEROP_CONFNODE && current->Name() == "device_info") {
            isDeviceInfo = true;
        }
        if (isDeviceInfo) {
            if (type == PARSEROP_CONFNODE) {
                if (CheckDeepestNode(current)) {
                    isDeepestNode = true;
                } else {
                    isDeepestNode = false;
                }
            }
            SetExpectType(current, type);
            GetExpectVaule(current, type);
        }

        return NOERR;
    });
}

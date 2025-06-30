/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HC_GEN_FILE_H
#define HC_GEN_FILE_H

#include <cstring>
#include <climits>
#include <string>

namespace OHOS {
namespace Hardware {
namespace Util {
class File {
public:
    static std::string AbsPath(const std::string &path);
    static std::string StripSuffix(std::string path);
    static std::string GetDir(std::string path);
    static std::string FileNameBase(const std::string &path);
};
} // namespace Util


#define MAX_PATH_LENGTH PATH_MAX
inline bool IsValidPath(const char *realPath)
{
    return !(realPath == nullptr || realPath[0] != '/' || strlen(realPath) > MAX_PATH_LENGTH);
}

} // namespace Hardware
} // namespace OHOS
#endif // HC_GEN_FILE_H

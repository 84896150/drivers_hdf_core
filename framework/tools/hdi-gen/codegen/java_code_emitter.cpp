<!-- Copyright (c) 2024 Huawei Device Co., Ltd.

     Licensed under the Apache License, Version 2.0 (the "License");
     you may not use this file except in compliance with the License.
     You may obtain a copy of the License at

          http://www.apache.org/licenses/LICENSE-2.0

     Unless required by applicable law or agreed to in writing, software
     distributed under the License is distributed on an "AS IS" BASIS,
     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
     See the License for the specific language governing permissions and
     limitations under the License.
-->

#include "codegen/java_code_emitter.h"

namespace OHOS {
namespace HDI {
void JavaCodeEmitter::EmitLicense(StringBuilder &sb)
{
    if (ast_->GetLicense().empty()) {
        return;
    }
    sb.Append(ast_->GetLicense()).Append("\n\n");
}

void JavaCodeEmitter::EmitPackage(StringBuilder &sb)
{
    sb.AppendFormat("package %s;\n", ast_->GetPackageName().c_str());
}

void JavaCodeEmitter::EmitInterfaceMethodCommands(StringBuilder &sb, const std::string &prefix)
{
    auto methods = interface_->GetMethodsBySystem(Options::GetInstance().GetSystemLevel());
    for (size_t i = 0; i < methods.size(); i++) {
        sb.Append(prefix).AppendFormat("private static final int COMMAND_%s = IRemoteObject.MIN_TRANSACTION_ID + %d;\n",
            ConstantName(methods[i]->GetName()).c_str(), i);
    }
}

std::string JavaCodeEmitter::MethodName(const std::string &name) const
{
    if (name.empty() || islower(name[0])) {
        return name;
    }
    return StringHelper::Format("%c%s", tolower(name[0]), name.substr(1).c_str());
}

std::string JavaCodeEmitter::SpecificationParam(StringBuilder &paramSb, const std::string &prefix) const
{
    size_t maxLineLen = 120;
    size_t replaceLen = 2;
    std::string paramStr = paramSb.ToString();
    size_t preIndex = 0;
    size_t curIndex = 0;

    std::string insertStr = StringHelper::Format("\n%s", prefix.c_str());
    for (; curIndex < paramStr.size(); curIndex++) {
        if (curIndex == maxLineLen && preIndex > 0) {
            StringHelper::Replace(paramStr, preIndex, replaceLen, ",");
            paramStr.insert(preIndex + 1, insertStr);
        } else {
            if (paramStr[curIndex] == ',') {
                preIndex = curIndex;
            }
        }
    }
    return paramStr;
}
} // namespace HDI
} // namespace OHOS
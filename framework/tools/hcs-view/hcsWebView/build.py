#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd. 
# Licensed under the Apache License, Version 2.0 (the "License"); 
# you may not use this file except in compliance with the License. 
# You may obtain a copy of the License at 
#
# http://www.apache.org/licenses/LICENSE-2.0 
#
# Unless required by applicable law or agreed to in writing, software 
# distributed under the License is distributed on an "AS IS" BASIS, 
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
# See the License for the specific language governing permissions and 
# limitations under the License. 

from asyncio import subprocess

if __name__ == "__main__":
    # development production
    subprocess.run(["npx", "webpack", "--mode=development"])
    with open(r".\..\hcsVSCode\editor.html", "r", encoding="utf8") as file:
        ss = file.read()
    i1 = ss.index("// update js code begin") + len("// update js code begin") + 1
    i2 = ss.index("// update js code end") - 1
    with open(r".\dist\main.js", "r", encoding="utf8") as file:
        destss = file.read()
    ss = ss[:i1] + destss + ss[i2:]
    with open(r".\..\hcsVSCode\editor.html", "w", encoding="utf8") as file:
        file.write(ss)
    print("replaced")

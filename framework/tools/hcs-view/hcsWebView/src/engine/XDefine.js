/*
 * Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

const { NapiLog } = require("./../hcs/NapiLog");
export class Scr {
    constructor() {

    }
    static ReSize(w, h) {
        Scr.width = w;
        Scr.height = h;
        if (Scr.keeplogicworh == "width")
            Scr.logich = Scr.logicw * h / w
        else
            Scr.logicw = Scr.logich * w / h
    }
    static setLogicScreenSize(w, h) {
        if (Scr.logicw == w && Scr.width == w && Scr.logich == h && Scr.height == h) return;
        Scr.logicw = w
        Scr.logich = h
        Scr.width = w
        Scr.height = h
        NapiLog.logError("setLogicScreenSize")
        if ("undefined" != typeof wx) {
            var info = wx.getSystemInfoSync();
            Scr.width = info.windowWidth;
            Scr.height = info.windowHeight;
        }
    }
}

Scr.width = 320
Scr.height = 240
Scr.keeplogicworh = "height"
Scr.logicw = 320
Scr.logich = 240
Scr.fps = 60





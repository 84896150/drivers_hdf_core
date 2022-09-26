/*
 * Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

import { Scr } from "./engine/XDefine.js"
import { GLFrame } from "./engine/GLFrame.js"
import { X2DFast } from "./engine/graphics/X2DFast.js"
import { RandInt } from "./engine/XTools.js"
import { XTexture } from "./engine/graphics/XTexture.js";
const { NapiLog } = require("./hcs/NapiLog");
const { MainEditor } = require("./MainEditor");

let canvas = document.getElementById("visual_area");
canvas.width = window.innerWidth - 200
canvas.height = window.innerHeight

function myDraw() {
    if (canvas.width != window.innerWidth - 200 ||
        canvas.height != window.innerHeight) {

        canvas.width = window.innerWidth - 200
        canvas.height = window.innerHeight

        Scr.setLogicScreenSize(canvas.width, canvas.height)
        GLFrame.gi().resize();
    }

    let pm2f = X2DFast.gi()
    pm2f.swapMode2D();
    pm2f.clearBuffer();

    MainEditor.gi().draw(pm2f);

    pm2f.freshBuffer();
}

function myTouch(msg, x, y) {
    MainEditor.gi().onTouch(msg, x, y)
}

function myKey(type, code) {
    NapiLog.logError(type + code);
    MainEditor.gi().onKey(code)
}

Scr.setLogicScreenSize(canvas.width, canvas.height)
GLFrame.gi().go(canvas, myDraw, myTouch, myKey)
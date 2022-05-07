/*
* Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd. 
* Licensed under the Apache License, Version 2.0 (the "License"); 
* you may not use this file except in compliance with the License. 
* You may obtain a copy of the License at 
*
* http://www.apache.org/licenses/LICENSE-2.0 
*
* Unless required by applicable law or agreed to in writing, software 
* distributed under the License is distributed on an "AS IS" BASIS, 
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
* See the License for the specific language governing permissions and 
* limitations under the License. 
*/

import { X2DFast } from "./graphics/X2DFast.js";
import { Scr } from "./XDefine.js";

export var gl;

function touch_Start(e) { 
    e.preventDefault(); 
    GLFrame.pinstance_.callback_proctouch(1, e.touches[0].clientX, e.touches[0].clientY);
}
function touch_Move(e) { 
    e.preventDefault(); 
    GLFrame.pinstance_.callback_proctouch(2, e.touches[0].clientX, e.touches[0].clientY);
}
function touch_End(e) { 
    e.preventDefault(); 
    GLFrame.pinstance_.callback_proctouch(3, e.changedTouches[0].clientX, e.changedTouches[0].clientY);
}

function mouse_Down(e) { 
    e.preventDefault(); 
    GLFrame.pinstance_.callback_proctouch(1, e.offsetX, e.offsetY) 
}
function mouse_Move(e) { 
    e.preventDefault(); 
    GLFrame.pinstance_.callback_proctouch(2, e.offsetX, e.offsetY) 
}
function mouse_Up(e) { 
    e.preventDefault(); 
    GLFrame.pinstance_.callback_proctouch(3, e.offsetX, e.offsetY) 
}

function key_Down(e) {
    let ret = ""
    if (e.ctrlKey) {
        if (ret.length > 0) ret += "+"
        ret += "ctrl"
    }
    if (e.shiftKey) {
        if (ret.length > 0) ret += "+"
        ret += "shift"
    }
    if (e.altKey) {
        if (ret.length > 0) ret += "+"
        ret += "alt"
    }
    if (ret.length > 0) ret += "+"
    ret += e.key
    GLFrame.pinstance_.callback_Key(1, ret)
}

function main_Loop() {
    GLFrame.pinstance_.callback_Draw();
    window.requestAnimationFrame(main_Loop)
}

export class GLFrame {
    static gi() {
        return GLFrame.pinstance_;
    }
    constructor() {
    }

    go(cvs, _draw = null, _touch = null, _key = null, _logic = null) {
        gl = cvs.getContext('webgl', { premultipliedAlpha: false });

        this.pCallbackDraw = _draw
        this.pCallbackTouch = _touch
        this.pCallbackKey = _key
        this.pCallbackLogic = _logic

        cvs.addEventListener("touchstart", touch_Start);
        cvs.addEventListener("touchmove", touch_Move);
        cvs.addEventListener("touchend", touch_End);

        cvs.addEventListener("mousedown", mouse_Down);
        cvs.addEventListener("mousemove", mouse_Move);
        cvs.addEventListener("mouseup", mouse_Up);

        window.addEventListener('keydown', key_Down);
        window.requestAnimationFrame(main_Loop)
    }
    callback_Key(type, code) {
        if (this.pCallbackKey != null) {
            this.pCallbackKey(type, code)
        }
    }
    callback_Draw() {
        gl.clearColor(0.0, 0.0, 0.0, 1.0);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        if (this.pCallbackDraw != null) {
            this.pCallbackDraw();
        }
    }
    callback_proctouch(msg, x, y) {
        if (this.pCallbackTouch != null) {
            x = x * Scr.logicw / Scr.width
            y = y * Scr.logich / Scr.height
            this.pCallbackTouch(msg, x, y);
        }
    }
    resize() {
        gl.viewport(0, 0, Scr.logicw, Scr.logich);
        X2DFast.gi().resetMat();
    }
}

GLFrame.pinstance_ = new GLFrame()
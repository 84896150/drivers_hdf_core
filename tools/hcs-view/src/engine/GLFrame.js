//import { canvas } from "../libs/weapp-adapter";

import { X2DFast } from "./graphics/X2DFast.js";
import { Scr } from "./XDefine.js";

export var gl;

function touch_start(e) { e.preventDefault(); GLFrame.pinstance_.callback_proctouch(1, e.touches[0].clientX, e.touches[0].clientY); }
function touch_move(e) { e.preventDefault(); GLFrame.pinstance_.callback_proctouch(2, e.touches[0].clientX, e.touches[0].clientY); }
function touch_end(e) { e.preventDefault(); GLFrame.pinstance_.callback_proctouch(3, e.changedTouches[0].clientX, e.changedTouches[0].clientY); }

function mouse_down(e) { e.preventDefault(); GLFrame.pinstance_.callback_proctouch(1, e.offsetX, e.offsetY) }
function mouse_move(e) { e.preventDefault(); GLFrame.pinstance_.callback_proctouch(2, e.offsetX, e.offsetY) }
function mouse_up(e) { e.preventDefault(); GLFrame.pinstance_.callback_proctouch(3, e.offsetX, e.offsetY) }

// function key_down(e) { GLFrame.pinstance_.callback_key(1, e.keyCode ? e.keyCode : e.which) }
// function key_up(e) { GLFrame.pinstance_.callback_key(2, e.keyCode ? e.keyCode : e.which) }
// function key_press(e) { GLFrame.pinstance_.callback_key(0, e.keyCode ? e.keyCode : e.which) }
function key_down(e) {
    // if(e.keyCode>47 && e.keyCode<91)
    // {
    //     GLFrame.pinstance_.callback_key(1, e.key.charCodeAt(0))
    // }
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
    GLFrame.pinstance_.callback_key(1, ret)
}

function main_loop() {
    GLFrame.pinstance_.callback_draw();
    window.requestAnimationFrame(main_loop)
}

export class GLFrame {
    static gi() {
        return GLFrame.pinstance_;
    }
    constructor() {
    }

    Go(cvs, _draw = null, _touch = null, _key = null, _logic = null) {
        // let canvas=wx.createCanvas()
        gl = cvs.getContext('webgl', { premultipliedAlpha: false });

        this.pCallbackDraw = _draw
        this.pCallbackTouch = _touch
        this.pCallbackKey = _key
        this.pCallbackLogic = _logic

        cvs.addEventListener("touchstart", touch_start);
        cvs.addEventListener("touchmove", touch_move);
        cvs.addEventListener("touchend", touch_end);

        cvs.addEventListener("mousedown", mouse_down);
        cvs.addEventListener("mousemove", mouse_move);
        cvs.addEventListener("mouseup", mouse_up);

        window.addEventListener('keydown', key_down);
        // cvs.onkeydown=key_down;
        // window.addEventListener('keyup', key_up);
        // window.addEventListener('keypress', key_press);
        // cvs.focus();

        window.requestAnimationFrame(main_loop)
    }
    callback_key(type, code) {
        if (this.pCallbackKey != null) {
            this.pCallbackKey(type, code)
        }
    }
    callback_draw() {
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
        X2DFast.gi().ResetMat();
    }
}

GLFrame.pinstance_ = new GLFrame()
/*
 * Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

const { MockMessage } = require("./mock")
const { NapiLog } = require("./../hcs/NapiLog");

class XMessage {
    constructor() {
        this.messageType = XMessage.TYPE_NONE
        try {
            this.vscode = acquireVsCodeApi();
            window.addEventListener('message', this.onRecv);
            this.messageType = XMessage.TYPE_VSCODE
        } catch (err) {
            NapiLog.logError("Error:"+err);
        }
        this.callbackFunc = null
        NapiLog.logInfo("Init XMessage : " + this.messageType)
    }
    registRecvCallback(func) {
        this.callbackFunc = func
    }
    onRecv(event) {
        if (XMessage.gi().callbackFunc != null) {
            XMessage.gi().callbackFunc(event.data.type, event.data.data)
        }
    }
    send(msgtype, msgdata) {
        if (this.messageType == XMessage.TYPE_VSCODE) {
            this.vscode.postMessage({
                type: msgtype,
                data: msgdata
            })
        }
        else if (this.messageType == XMessage.TYPE_NONE) {
            MockMessage.gi().processMessage({
                type: msgtype,
                data: msgdata
            })
        }
    }
}

XMessage.TYPE_NONE = 0
XMessage.TYPE_VSCODE = 1

XMessage.pinstance_ = null;
XMessage.gi = function () {
    if (XMessage.pinstance_ == null) {
        XMessage.pinstance_ = new XMessage();
    }
    return XMessage.pinstance_;
}

module.exports = {
    XMessage
}

const { MockMessage } = require("./mock")

class XMessage {
    constructor() {
        this.message_type = XMessage.TYPE_NONE
        try {
            this.vscode = acquireVsCodeApi();
            window.addEventListener('message', this.OnRecv);
            this.message_type = XMessage.TYPE_VSCODE
        } catch (err) {
        }
        this.callbackFunc = null
        console.log("Init XMessage : " + this.message_type)
    }
    registRecvCallback(func) {
        this.callbackFunc = func
    }
    OnRecv(event) {
        if (XMessage.gi().callbackFunc != null) {
            XMessage.gi().callbackFunc(event.data.type, event.data.data)
        }
    }
    send(msgtype, msgdata) {
        if (this.message_type == XMessage.TYPE_VSCODE) {
            this.vscode.postMessage({
                type: msgtype,
                data: msgdata
            })
        }
        else if (this.message_type == XMessage.TYPE_NONE) {
            MockMessage.gi().ProcessMessage({
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

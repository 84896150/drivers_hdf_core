/*
 * Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

const { NapiLog } = require("./../hcs/NapiLog");
let mockTest = [{
    fn: "D:\\ceshi\\d.hcs",
    data: `
	#include "D:\\ceshi\\b.hcs"
root {
    nodeaa {
		childnode {
		}
		childatt = "";
    }
	nodebb {
	}
	extint = 0;	
	extstring = "";
 }`
},
{
    fn: "D:\\ceshi\\b.hcs",
    data: `
root {
	 nodeaa {
    }
	nodebb {
	}
 }`
}
]

function getArray(fn) {
    for (let i in mockTest) {
        if (mockTest[i].fn == fn) {
            let ret = []
            for (let j in mockTest[i].data) {
                ret.push(mockTest[i].data.charCodeAt(j))
            }
            return ret
        }
    }
    return null;
}

class MockMessage {
    constructor() {
    }
    send(type, data) {
        setTimeout(() => {
            const { XMessage } = require("./XMessage");
            XMessage.gi().onRecv({
                data: {
                    type: type,
                    data: data
                }
            })
        }, 100);
    }
    processMessage(msg) {
        NapiLog.logInfo("---MockMessage start---")
        NapiLog.logInfo(msg.type)
        NapiLog.logInfo(msg.data)
        if (msg.type == "inited") {//After initialization, send the file name for resolution
            this.send("parse", mockTest[0].fn);
        }
        else if (msg.type == "getfiledata") {//Get file data
            this.send("filedata", {
                fn: msg.data,
                data: getArray(msg.data)
            })
        }
        NapiLog.logInfo("---MockMessage end---")
    }
}

MockMessage.pInstance_ = null;
MockMessage.gi = function () {
    if (MockMessage.pInstance_ == null) MockMessage.pInstance_ = new MockMessage();
    return MockMessage.pInstance_;
}

module.exports = {
    MockMessage
}
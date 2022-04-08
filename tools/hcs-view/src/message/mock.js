
let mock_test = [{
    fn: "D:\\hci\\js_ast\\a.hcs",
    data: `root {
    node0 {//节点0 -- 数据类，不继承
        att0 = 1;
    }
    node1 : node0 {//节点1 -- 复制类节点
        att1 = 2;
    }
    node2 : &node0 {//节点2 -- 引用修改类节点
        att0 = 3;
    }
    node3 : delete {//节点3 -- 删除类节点
    }
    template node4 {//节点4 -- 模板类
        att3 = 4;
        att4 = 5;
    }
    node5 :: node4 {//节点5 -- 数据类，继承
        att3 = 6;
    }

    att0 = 0;         //属性0 -- 定义类 -- 整形
    att1 = "abc";     //属性0 -- 定义类 -- 字符串
    att2 = [1,2,3];   //属性0 -- 定义类 -- 数组<整形>
    att3 = ["a","b"]; //属性0 -- 定义类 -- 数组<字符串>
    att4 = true;      //属性0 -- 定义类 -- 布尔

    // att0 = delete;    //属性1 -- 删除类
    // att4 = &att0;     //属性2 -- 引用类

    testnode1 {
        bb=5;
        testnode2 {
            testnode3 {
                a="1";
            }
        }
    }

    node3b {
        fordelete = "asdf";
    }
}`},
{
    fn: "D:\\hci\\b.hcs",
    data: `
#include "js_ast/a.hcs"
root {
    testnode2 {
    }
    testnode1 {
        testnode2 {
            testnode3 {
                a="2";
            }
            aaa=1;
        }
        ff : testnode2 {
            testnode3 : delete {}
            aaa=2;
        }
    }
    zz : testnode2 {
        
    }
    dd : root.testnode1.testnode2 {
        dd=1;
        testnode3 : delete {}
    }
    ee : &root.testnode1.testnode2 {
        ee=1;
        yy {}
    }

    node1b : node0 {//节点1 -- 复制类节点
        att1 = 2;
    }
    node2b : &node0 {//节点2 -- 引用修改类节点
        att0 = 3;
    }
    node3b : delete {//节点3 -- 删除类节点
    }
    node5b :: node4 {//节点5 -- 数据类，继承
        att3 = 6;
    }

    att0b = delete;    //属性1 -- 删除类
    att4b = &node1b;    //属性2 -- 引用类
}`
}]

function get_array(fn) {
    for (let i in mock_test) {
        if (mock_test[i].fn == fn) {
            let ret = []
            for (let j in mock_test[i].data) {
                ret.push(mock_test[i].data.charCodeAt(j))
            }
            return ret
        }
    }
    return null;
}

class MockMessage {
    constructor() {
    }
    Send(type, data) {
        setTimeout(() => {
            const { XMessage } = require("./XMessage");
            XMessage.gi().OnRecv({
                data: {
                    type: type,
                    data: data
                }
            })
        }, 100);
    }
    ProcessMessage(msg) {
        console.log("---MockMessage start---")
        console.log(msg.type)
        console.log(msg.data)
        if (msg.type == "inited") {//初始化完成，发送文件名进行解析
            this.Send("parse", mock_test[1].fn);
        }
        else if (msg.type == "getfiledata") {//获取文件数据
            this.Send("filedata", {
                fn: msg.data,
                data: get_array(msg.data)
            })
        }
        console.log("---MockMessage end---")
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
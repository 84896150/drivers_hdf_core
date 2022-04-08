const { NapiLog } = require("./NapiLog");
const re = require("./re")

var DataType = {
    INT8: 1,
    INT16: 2,
    INT32: 3,
    INT64: 4,
    STRING: 5,
    NODE: 6,
    ATTR: 7,//属性定义
    ARRAY: 8,
    REFERENCE: 9,//引用 a = &b
    DELETE: 10,//属性删除 a = delete
    BOOL: 11,
}
var NodeType = {
    DATA: 0,
    COPY: 1,//复制 a : b {}
    REFERENCE: 2,//引用 a : &b {}
    DELETE: 3,//删除节点 a : delete {}
    TEMPLETE: 4,//模板节点 templete a {}
    INHERIT: 5,//继承类数据节点 a :: b {}
}

class NodeTools {
}

function GetRoot(node) {
    while (node.parent_ != undefined)
        node = node.parent_
    return node;
}

NodeTools.IsElders = function (node, elders) {
    while (node != undefined) {
        if (node == elders) {
            return true;
        }
        node = node.parent_
    }
    return false;
}

NodeTools.GetPathByNode = function (node) {
    if (node == null) return "";
    ret = node.name_;
    while (node.parent_ != undefined) {
        node = node.parent_;
        ret = node.name_ + "." + ret;
    }
    return ret;
}
NodeTools.GetNodeByPath = function (node, path) {
    let ps = path.split(".")
    if (ps[0] == "root") ps.shift()
    for (let p in ps) {
        node = NodeTools.FindChildByName(node, ps[p])
        if (node == null) return null;
    }
    return node;
}

NodeTools.Lookup = function (node) {//copy,reference,inherit查找其引用节点
    let refname;
    if (node.type_ == DataType.NODE &&
        (node.nodeType_ == NodeType.COPY ||
            node.nodeType_ == NodeType.REFERENCE ||
            node.nodeType_ == NodeType.INHERIT))
        refname = node.ref_;
    else if (node.type_ == DataType.ATTR &&
        node.value_.type_ == DataType.REFERENCE)
        refname = node.value_.value_;
    else return null;
    if (refname.indexOf(".") >= 0) return NodeTools.GetNodeByPath(GetRoot(node), refname);
    return NodeTools.FindChildByName(node.parent_, refname);
}

NodeTools.RecursionNode = function (node, callback) {//找到所有节点
    if (node.type_ == DataType.NODE) {
        callback(node)
        for (let n in node.value_) {
            NodeTools.RecursionNode(node.value_[n], callback)
        }
    }
}
NodeTools.RecursionAll = function (node, callback, isForward) {
    let ret = false;
    if (isForward) ret = callback(node)
    if (node.type_ == DataType.NODE) {
        for (let i = 0; i < node.value_.length; i++) {
            if (NodeTools.RecursionAll(node.value_[i], callback, isForward))
                i--;
        }
    }
    if (!isForward) ret = callback(node)
    return ret;
}
NodeTools.RedefineCheck = function (node) {
    let ret = true;
    NodeTools.RecursionNode(node, (pn) => {
        let nameMap = [];
        for (let i in pn.value_) {
            if (nameMap.indexOf(pn.value_[i].name_) >= 0) {
                pn.value_[i].errMsg_ = "重名"
                ret = false;
            }
            else {
                nameMap.push(pn.value_[i].name_)
                pn.value_[i].errMsg_ = null;
            }
        }
    })
    return ret;
}
function Separate(node) {
    let pn = node.parent_;
    if (pn == null)
        return;
    for (let i in pn.value_) {
        if (pn.value_[i] == node) {
            pn.value_.splice(i, 1);
        }
    }
}
NodeTools.FindChildByName = function (node, name) {
    for (let i in node.value_) {
        if (node.value_[i].name_ == name) {
            return node.value_[i]
        }
    }
    return null;
}
NodeTools.CopyNode = function (node, parent) {
    let ret = {
        type_: node.type_,
        name_: node.name_,
        parent_: parent
    }
    switch (node.type_) {
        case DataType.INT8://uin8
        case DataType.INT16://uin16
        case DataType.INT32://uin32
        case DataType.INT64://uin64
            ret.value_ = node.value_
            ret.jinzhi_ = node.jinzhi_
            break;
        case 5://string
            ret.value_ = node.value_
            break;
        case 6://ConfigNode
            ret.nodeType_ = node.nodeType_
            switch (node.nodeType_) {
                case NodeType.INHERIT://数据类节点，继承
                case NodeType.COPY://复制类节点
                case NodeType.REFERENCE://引用修改类节点
                    ret.ref_ = node.ref_;
                    break;
                case NodeType.DATA:
                case NodeType.TEMPLETE:
                case NodeType.DELETE:
                    break;
                default://
                    NapiLog.logError("unknow node type");
                    break;
            }
            ret.value_ = []
            for (let i in node.value_) {
                ret.value_.push(NodeTools.CopyNode(node.value_[i], ret))
            }
            break;
        case 7://ConfigTerm
            ret.value_ = NodeTools.CopyNode(node.value_, ret)
            break;
        case 8://array 数组类属性值
            ret.value_ = []
            for (let i in node.value_) {
                ret.value_.push(NodeTools.CopyNode(node.value_[i], ret))
            }
            break;
        case 9://属性等于叶子
            ret.value_ = node.value_;
            break;
        case 10://删除属性
            ret.value_ = null;
            break;
        case 11://bool
            ret.value_ = node.value_
            break;
        default:
            console.log("unknow", node.type_)
            break;
    }
    return ret;
}

function MakeError(node, errStr) {
    if (node.raw_ != undefined) {
        node.raw_.errMsg_ = errStr;
    }
}

NodeTools.NodeExpand = function (node) {
    NodeTools.RecursionAll(node, (pn) => {
        if (pn.type_ == DataType.NODE) {
            if (pn.nodeType_ == NodeType.DELETE) {
                Separate(pn);
                return true;
            }
            if (pn.nodeType_ == NodeType.COPY || pn.nodeType_ == NodeType.REFERENCE) {
                let ref = NodeTools.Lookup(pn);
                if (ref == null) {
                    NapiLog.logError("reference not exist" + NodeTools.GetPathByNode(pn) + ":" + pn.ref_);
                    if (pn.nodeType_ == NodeType.COPY) MakeError(pn, "复制目标没找到")
                    else MakeError(pn, "引用目标没找到")
                }
                else if (NodeTools.IsElders(pn, ref)) {
                    NapiLog.logError("circular reference" + NodeTools.GetPathByNode(pn) + ":" + pn.ref_);
                    if (pn.nodeType_ == NodeType.COPY) MakeError(pn, "循环复制")
                    else MakeError(pn, "循环引用")
                }
                else {
                    if (pn.nodeType_ == NodeType.COPY) {
                        //todo 判断拷贝的目标必须是 数据类继承，数据类不继承，引用，模板
                        pn.nodeType_ = NodeType.DATA;//当前节点转换为数据类节点
                        let tref = NodeTools.CopyNode(ref, pn.parent_);//复制一个ref出来
                        NodeTools.Merge(tref, pn)//把当前节点的内容合并进去
                        pn.value_ = tref.value_//更新当前节点的子孙(todo 后续遍历，子孙已经更新过?)
                        NodeTools.NodeExpand(pn);
                    }
                    else if (pn.nodeType_ == NodeType.REFERENCE) {
                        pn.nodeType_ = ref.nodeType_;
                        NodeTools.Merge(ref, pn)//将当前节点内容合并到ref里面去
                        Separate(pn);
                        return true;
                    }
                }
            }
        }
        else if (pn.type_ == DataType.ATTR) {
            if (pn.value_.type_ == DataType.DELETE) {
                Separate(pn);
                return true;
            }
            if (pn.value_.type_ == DataType.REFERENCE) {
                let ref = NodeTools.Lookup(pn);
                if (ref == null || ref.type_ != DataType.NODE ||
                    ref.nodeType_ == NodeType.REFERENCE ||
                    ref.nodeType_ == NodeType.TEMPLETE ||
                    ref.nodeType_ == NodeType.DELETE) {
                    NapiLog.logError("reference invalid node" + NodeTools.GetPathByNode(pn) + ":" + pn.value_.value_);
                    if (ref == null) MakeError(pn, "找不到引用目标")
                    else if (ref.type_ != DataType.NODE) MakeError(pn, "不能引用属性")
                    else if (ref.nodeType == NodeType.REFERENCE) MakeError(pn, "不能引用引用类节点")
                    else if (ref.nodeType == NodeType.TEMPLETE) MakeError(pn, "不能引用模板类节点")
                    else if (ref.nodeType == NodeType.DELETE) MakeError(pn, "不能引用删除类节点")
                }
                else pn.refNode_ = ref;
            }
        }
        return false;
    }, false);
}
NodeTools.InheritExpand = function (node) {
    NodeTools.RecursionAll(node, (pn) => {
        let tt = re.match("^[a-zA-Z_]{1}[a-zA-Z_0-9]*$", pn.name_);
        if (tt == null) {
            MakeError(pn, "名字不合规范")
        }
        if (pn.type_ != DataType.NODE) return false;
        if (pn.nodeType_ != NodeType.INHERIT) return false;
        let inherit = NodeTools.Lookup(pn);
        if (inherit == null) {
            NapiLog.logError("inherit invalid node: " + NodeTools.GetPathByNode(pn) + ":" + pn.ref_);
            MakeError(pn, "找不到继承目标")
            return false;
        }
        //todo Compare
        pn.nodeType_ = NodeType.DATA;//当前节点转换为数据类节点
        let tinherit = NodeTools.CopyNode(inherit, pn.parent_);//复制一个ref出来
        NodeTools.Merge(tinherit, pn)//把当前节点的内容合并进去
        pn.value_ = tinherit.value_//更新当前节点的子孙(todo 后续遍历，子孙已经更新过?)
        return false;
    }, true);
}
NodeTools.Merge = function (node1, node2) {
    if (node2 == null) {
        return true;
    }
    if (node2.raw_ == undefined)
        node1.raw_ = node2
    else
        node1.raw_ = node2.raw_
    if (node1.type_ == DataType.NODE) {

        if (node1.name_ != node2.name_) {
            return false;
        }
        // if (node2.nodeType_ == NodeType.DELETE) {
        //     Separate(node1);
        //     return true;
        // }
        node1.nodeType_ = node2.nodeType_;
        if (node2.nodeType_ == NodeType.INHERIT || node2.nodeType_ == NodeType.REFERENCE || node2.nodeType_ == NodeType.COPY) node1.ref_ = node2.ref_
        if (node1.value_ == undefined) node1.value_ = [];

        for (let i in node2.value_) {
            let child2 = node2.value_[i];
            let child1 = NodeTools.FindChildByName(node1, child2.name_)
            if (child1 == null) {
                child1 = {
                    type_: child2.type_,
                    name_: child2.name_,
                    parent_: node1
                }
                node1.value_.push(child1)
            }
            NodeTools.Merge(child1, child2);
        }
    }
    else if (node1.type_ == DataType.ATTR) {
        node1.value_ = NodeTools.CopyNode(node2.value_, node1)
    }

    return true;
}
// NodeTools.CheckError = function (files, node) {
//     node.errMsg_ = null;
//     if (node.parent_ == undefined)
//         return false;
//     let root = GetRoot(node);
//     //同级重名
//     if (node.parent_.type_ == DataType.NODE) {
//         for (let i in node.parent_.value_) {
//             if (node.parent_.value_[i].name_ == undefined || node.name_ == undefined) {
//                 console.log(123)
//             }
//             if (node.parent_.value_[i] != node && node.parent_.value_[i].name_ == node.name_) {
//                 node.errMsg_ = "重名";
//                 return true;
//             }
//         }
//     }

//     if (node.type_ == DataType.NODE) {
//         switch (node.nodeType_) {
//             case NodeType.DATA:
//                 break;
//             case NodeType.COPY://是否有源
//                 break;
//             case NodeType.REFERENCE://是否有源
//                 break;
//             case NodeType.DELETE://是否有源
//                 break;
//             case NodeType.TEMPLETE://
//                 break;
//             case NodeType.INHERIT://是否有源
//                 break;
//         }
//     }
// }

NodeTools.JinZhi_10_to_x = function (num, jinzhi) {
    let ret;
    switch (jinzhi) {
        case 2:
            ret = "0b"
            break;
        case 8:
            ret = "0"
            break;
        case 10:
            ret = ""
            break;
        case 16:
            ret = "0x"
            break;
        default:
            NapiLog.logError(jinzhi + "进制转换失败")
            break;
    }
    return ret + num.toString(jinzhi);
}
NodeTools.JinZhi_x_to_10 = function (s) {
    if (s == null || s.length == 0)
        return [0, 10]
    if (s[0] == '0') {
        if (s.length == 1) {
            return [parseInt(s, 10), 10];
        }
        else if (s[1] == 'b') {
            return [parseInt(s.substring(2), 2), 2];
        }
        else if (s[1] == 'x' || s[1] == 'X') {
            return [parseInt(s.substring(2), 16), 16];
        }
        else {
            return [parseInt(s.substring(1), 8), 8];
        }
    }
    else {
        return [parseInt(s, 10), 10];
    }
}

NodeTools.CreateNewNode = function (type, name, value, nodetype) {
    let ret = new Object();
    ret.type_ = type;
    ret.name_ = name;
    ret.value_ = value;
    if (type < DataType.STRING) ret.jinzhi_ = 10;
    if (type == DataType.NODE) ret.nodeType_ = nodetype;
    return ret;
}
NodeTools.ArrayToString = function (node, maxw) {
    let ret = ""
    let type = DataType.INT8;
    for (let d in node.value_) {
        if (type < node.value_[d].type_) type = node.value_[d].type_;
    }
    let line = ""
    for (let d in node.value_) {
        if (d > 0) {
            line += ","
            if (maxw != undefined) {
                if (line.length >= maxw) {
                    ret += line + "\n";
                    line = "";
                }
            }
        }
        if (type == DataType.STRING)
            line += '"' + node.value_[d].value_ + '"';
        else
            line += NodeTools.JinZhi_10_to_x(node.value_[d].value_, node.value_[d].jinzhi_);
    }
    ret += line;
    return ret;
}
NodeTools.StringToArray = function (s) {
    let type = DataType.INT8;
    let ret = [];
    s = s.replace(/\n/g, "")
    if (s.length <= 0)
        return ret;
    if (s.indexOf('"') >= 0) {
        type = DataType.STRING;
        let p = 0;
        let stat = 0;
        let v
        while (p < s.length && stat < 100) {
            switch (stat) {
                case 0:
                    if (s[p] == '"') {
                        stat = 1;
                        v = ""
                    }
                    else if (s[p] != ' ') stat = 100;
                    break;
                case 1:
                    if (s[p] == '"') {
                        stat = 2;
                        ret.push(NodeTools.CreateNewNode(type, "", v))
                    }
                    else v += s[p]
                    break;
                case 2:
                    if (s[p] == ',') stat = 0;
                    else if (s[p] != ' ') stat = 100;
                    break;
            }
            p += 1;
        }
    }
    else {
        let arr = s.split(",")
        for (let i in arr) {
            let num = NodeTools.JinZhi_x_to_10(arr[i])
            if (isNaN(num[0])) num[0] = 0;
            let attr = NodeTools.CreateNewNode(type, "", num[0]);
            attr.jinzhi_ = num[1]
            ret.push(attr)
            if (num[0] <= 0xff) {
                if (type < DataType.INT8) type = DataType.INT8;
            }
            else if (num[0] <= 0xffff) {
                if (type < DataType.INT16) type = DataType.INT16;
            }
            else if (num[0] <= 0xffffffff) {
                if (type < DataType.INT32) type = DataType.INT32;
            }
            else {
                type = DataType.INT64;
            }
        }
        if (type != DataType.INT8) {
            for (let i in ret) {
                ret[i].type_ = type;
            }
        }
    }
    return ret;
}

module.exports = {
    NodeTools,
    DataType,
    NodeType
}
const { NapiLog } = require("./NapiLog");
const { NodeTools, DataType, NodeType } = require("./NodeTools");
const re = require("./re")

class ModifyNode {
}

function GetParent(node, dest, parent) {
    return dest.parent_;
    // if (node == dest) return parent;
    // if (node.type_ == 6) {
    //     for (let i in node.value_) {
    //         let t = GetParent(node.value_[i], dest, node)
    //         if (t) return t;
    //     }
    // }
    // return null;
}

function IsNameRepeat(node, name) {
    for (let i in node.value_) {
        if (node.value_[i].name_ == name) return true;
    }
    return false;
}

function GetListByNode(node) {
    ret = [node];
    while (node.parent_ != undefined) {
        node = node.parent_;
        ret.splice(0, 0, node);
    }
    return ret;
}

function RecursionNodeByReference(root, refNode, callback) {//在root中找到所有引用refNode的节点
    let name = refNode.name_;
    let absPath = NodeTools.GetPathByNode(refNode);
    let list = GetListByNode(refNode);
    NodeTools.RecursionNode(root, (pn) => {
        if (pn.nodeType_ == NodeType.REFERENCE) {
            if (pn.ref_.length >= absPath.length && pn.ref_.substring(0, absPath.length) == absPath) {//绝对路径匹配
                // pn.reference_ = newPath + pn.reference_.substring(absPath.length)
            }
        }
    })
}

function IsNameDefined(node, name) {
    for (let p in node.parent_.value_) {
        if (node.parent_.value_[p].name_ == name) {
            NapiLog.logError("同级已有同名节点");
            return true;
        }
    }
    return false;
}

ModifyNode.ModifyName = function (files, root, node, name) {
    let parent = GetParent(root, node, null);
    if (parent == null) {
        NapiLog.logError("不能改变root节点名称");
        return false;
    }
    // if (IsNameDefined(node, name)) {
    //     return false;
    // }
    // if (IsNameDefined(files, parent, name)) {
    //     return false;
    // }
    // let oldName = node.name_;
    // let oldPath = NodeTools.GetPathByNode(node)
    // if (node.type_ == DataType.NODE) {//节点
    //     if (node.nodeType_ == NodeType.DATA) {//数据类
    //         //获取parent path
    //         let parentPath = NodeTools.GetPathByNode(parent)
    //         //所有files中枚举parent path的child，数据不继承，复制，引用，删除类节点，同步更新名称。模板类，数据继承节点与此无关
    //         for (let fn in files) {
    //             let anyParent = GetNodeByPath(files[fn], parentPath)
    //             if (anyParent == null) continue;
    //             for (let p in anyParent.value_) {
    //                 let pn = anyParent.value_[p]
    //                 if (pn.type_ == DataType.NODE) {
    //                     switch (pn.nodeType_) {
    //                         case NodeType.DATA:
    //                         case NodeType.DELETE:
    //                             if (pn.name_ == oldName) pn.name_ = name
    //                             break;
    //                         case NodeType.COPY:
    //                             if (pn.copy_ == oldName) pn.copy_ = name
    //                             break;
    //                         case NodeType.REFERENCE:
    //                             if (pn.reference_ == oldName) pn.reference_ = name
    //                             break;
    //                     }
    //                 }
    //             }
    //         }

    //         //获取path
    //         let newPath = NodeTools.GetPathByNode(node)
    //         // let nodePath = NodeTools.GetPathByNode(node)
    //         //所有files中检查复制，引用类节点，同步更新名称
    //         for (let fn in files) {
    //             RecursionNode(files[fn], (pn) => {
    //                 switch (pn.nodeType_) {
    //                     case NodeType.COPY://全路径匹配
    //                         if (pn.copy_.length >= oldPath.length && pn.copy_.substring(0, oldPath.length) == oldPath) {
    //                             pn.copy_ = newPath + pn.copy_.substring(oldPath.length)
    //                         }
    //                         break;
    //                     case NodeType.REFERENCE://全路径匹配
    //                         if (pn.reference_.length >= oldPath.length && pn.reference_.substring(0, oldPath.length) == oldPath) {
    //                             pn.reference_ = newPath + pn.reference_.substring(oldPath.length)
    //                         }
    //                         break;
    //                 }
    //                 // if (pn.name_ == oldName) {//节点名字相同，判断路径
    //                 //     // let parentFrom = pn.parent_.name_;
    //                 //     // switch (pn.parent_.nodeType_) {
    //                 //     //     case NodeType.DATA: break;
    //                 //     //     case NodeType.COPY: parentFrom = pn.parent_.copy_; break;
    //                 //     //     case NodeType.REFERENCE: parentFrom = pn.parent_.reference_; break;
    //                 //     //     case NodeType.DELETE: break;
    //                 //     //     case NodeType.TEMPLETE: break;
    //                 //     //     case NodeType.INHERIT: break;
    //                 //     // }
    //                 //     // if (parentFrom.indexOf(".") >= 0) {//绝对路径判断
    //                 //     //     if (parentFrom + "." + pn.name_ == oldPath) {
    //                 //     //         pn.name_ = name
    //                 //     //     }
    //                 //     // }
    //                 //     // else if()
    //                 //     if (pn.parent_.nodeType_ == NodeType.COPY) {
    //                 //         if (pn.parent_.copy_.indexOf(".") >= 0) {
    //                 //             if (pn.parent_.copy_ + "." + pn.name_ == oldPath) {
    //                 //                 pn.name_ = name
    //                 //             }
    //                 //         }
    //                 //         else if (pn.parent_ != root && FindChildByName(pn.parent_.parent_, node.parent_.name_) != null) {
    //                 //             pn.name_ = name
    //                 //         }
    //                 //     }
    //                 //     else if (pn.parent_.nodeType_ == NodeType.REFERENCE) {
    //                 //         if (pn.parent_.refrence_.indexOf(".") >= 0) {
    //                 //             if (pn.parent_.refrence_ + "." + pn.name_ == oldPath) {
    //                 //                 pn.name_ = name
    //                 //             }
    //                 //         }
    //                 //         else if (pn.parent_ != root && FindChildByName(pn.parent_.parent_, node.parent_.name_) != null) {
    //                 //             pn.name_ = name
    //                 //         }
    //                 //     }
    //                 // }
    //             })
    //         }
    //     }
    // }
    // if (node.type_ == DataType.NODE && node.nodeType_ == NodeType.TEMPLETE) {//模板类节点，同时更新继承者
    //     for (let i in parent.value_) {
    //         let pn = parent.value_[i];
    //         if (pn.type_ == DataType.NODE && pn.nodeType_ == NodeType.INHERIT && pn.inherit_ == node.name_) {
    //             pn.inherit_ = name;
    //         }
    //     }
    // }
    node.name_ = name;
    return true;
}

ModifyNode.ModifyNodeType = function (files, root, node, type) {//节点类型改变
    let parent = GetParent(root, node, null);
    if (parent == null) {
        NapiLog.logError("不能改变root节点类型");
        return false;
    }
    // if (node.type_ == DataType.NODE && node.nodeType_ == NodeType.TEMPLETE) {//模板类节点
    //     for (let i in parent.value_) {//当前层级枚举
    //         let pn = parent.value_[i];
    //         if (pn.type_ == DataType.NODE && pn.nodeType_ == NodeType.INHERIT && pn.inherit_ == node.name_) {//如果有继承者，不能修改
    //             NapiLog.logError(pn.name_ + "继承于此模板，所以不能把模板节点类型改为其他");
    //             return false;
    //         }
    //     }
    // }
    // if (type == NodeType.INHERIT) {
    //     let inheritList = ModifyNode.GetInheritList(root, node);//可继承列表
    //     if (inheritList.length <= 0) {
    //         NapiLog.logError(node.name_ + "节点所在位置没有可继承的模板节点，所以改变类型失败");
    //         return false;
    //     }
    //     node.inherit_ = inheritList[0].name_;
    //     node.value_=[]//todo 不在模板内的value要删除，这里直接清空
    // }

    // COPY: 1,//复制 a : b {}
    // REFERENCE: 2,//引用 a : &b {}
    // DELETE: 3,//删除节点 a : delete {}
    // TEMPLETE: 4,//模板节点 templete a {}
    // INHERIT: 5,//继承类数据节点 a :: b {}
    switch (type) {
        case NodeType.COPY:
        case NodeType.INHERIT:
        case NodeType.REFERENCE:
            node.ref_ = "_unknow_";
            break;
        case NodeType.DELETE:
            break;
        case NodeType.TEMPLETE:
            break;
    }
    node.nodeType_ = type;
    return true;
}

ModifyNode.AddChildAttr = function (root, node) {//添加子属性，默认为int8
    for (let i = 1; i < 256; i++) {
        let name = "attr_" + i;
        if (IsNameRepeat(node, name)) continue;
        let newAttr = NodeTools.CreateNewNode(DataType.ATTR, name, NodeTools.CreateNewNode(DataType.INT8, "", 0))
        node.value_.push(newAttr);
        newAttr.parent_ = node;
        break;
    }
}

ModifyNode.AddChildNode = function (root, node) {//添加子节点，默认为数据类节点(不继承)
    for (let i = 1; i < 256; i++) {
        let name = "node_" + i;
        if (IsNameRepeat(node, name)) continue;
        let newNode = NodeTools.CreateNewNode(DataType.NODE, name, [], NodeType.DATA);
        node.value_.push(newNode);
        newNode.parent_ = node;
        break;
    }
}

ModifyNode.DeleteNode = function (node) {//删除子节点
    let parent = node.parent_;
    if (parent == null) {
        NapiLog.logError("不能删除root节点");
        return false;
    }
    for (let i in parent.value_) {
        if (parent.value_[i] == node) {
            parent.value_.splice(i, 1);
            return;
        }
    }
}

ModifyNode.GetInheritList = function (root, node) {//获取可继承列表（模板列表）
    let ret = []
    let parent = GetParent(root, node, null);
    if (parent == null) return ret;
    for (let i in parent.value_) {//获取同级模板节点
        let pn = parent.value_[i]
        if (pn.type_ == DataType.NODE && pn.nodeType_ == NodeType.TEMPLETE) {
            ret.push(pn);
        }
    }
    let ps = [parent]
    while (true) {
        let pp = GetParent(root, parent, null);
        if (pp == null) break;
        ps.splice(0, 0, pp)
    }
    let ptemp = null;
    for (let i in ps) {
        if (ps[i].nodeType_ == NodeType.INHERIT) {
            ptemp = NodeTools.FindChildByName(ptemp, ps[i].ref_)
        }
        else if (ptemp == null) {
            ptemp = ps[i]
        }
        else {
            ptemp = NodeTools.FindChildByName(ptemp, ps[i].name_)
        }
        if (ptemp == null) break;
    }
    if (ptemp != null && ptemp != parent) {
        for (let i in ptemp.value_) {//获取其他同级模板节点
            let pn = ptemp.value_[i]
            if (pn.type_ == DataType.NODE && pn.nodeType_ == NodeType.TEMPLETE) {
                ret.push(pn);
            }
        }
    }

    return ret;
}
ModifyNode.GetInheritNameList = function (root, node) {
    let nodes = ModifyNode.GetInheritList(root, node);
    let ret = []
    for (i in nodes) {
        ret.push(nodes[i].name_)
    }
    return ret;
}
ModifyNode.GetInheritTemplete = function (root, node) {
    let parent = GetParent(root, node, null);
    if (parent == null)
        return null;
    let ilist = ModifyNode.GetInheritList(root, node);
    for (let i in ilist) {
        if (parent.nodeType_ == NodeType.TEMPLETE) {
            if (parent.ref_ == ilist[i].name_)
                return ilist[i];
        }
        else if (parent.name_ == ilist[i].name_) {
            return ilist[i]
        }
    }
    return null;
}

// ModifyNode.pInstance_ = null;
// ModifyNode.gi = function () {
//     if (ModifyNode.pInstance_ == null) ModifyNode.pInstance_ = new ModifyNode();
//     return ModifyNode.pInstance_;
// }

module.exports = {
    ModifyNode
}
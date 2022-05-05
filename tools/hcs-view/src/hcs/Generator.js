const { NapiLog } = require("./NapiLog");
const { NodeTools } = require("./NodeTools");
const { Parser } = require("./parser");
const { DataType } = require("./NodeTools")

class Generator {
    constructor() {
        this.parser = new Parser();
    }
    HscToAst(fn) {
        // let ret = parser.Parse("D:\\hci\\hdf_config\\hdf.hcs")
        return this.parser.Parse(fn)
    }
    AstToObj(node, parent) {
        let child;
        let ret = new Object();
        ret.type_ = node.type_
        ret.name_ = node.name_;
        ret.parent_ = parent;
        switch (node.type_) {
            case 1://uin8
            case 2://uin16
            case 3://uin32
            case 4://uin64
                ret.value_ = node.integerValue_;
                ret.jinzhi_ = node.jinzhi_;
                break;
            case 5://string
                ret.value_ = node.stringValue_;
                break;
            case 6://ConfigNode，节点
                ret.value_ = []
                child = node.child_;
                while (child != null) {
                    ret.value_.push(this.AstToObj(child, ret));
                    child = child.next_;
                }
                ret.nodeType_ = node.nodeType_
                switch (node.nodeType_) {
                    case 0://数据类节点，不继承
                    case 3://删除类节点
                    case 4://模板类节点
                        break;
                    case 5://数据类节点，继承
                    case 1://复制类节点
                    case 2://引用修改类节点
                        ret.ref_ = node.refNodePath_;
                        break;
                    default://
                        NapiLog.logError("unknow node type");
                        break;
                }
                break;
            case 7://ConfigTerm，属性名称
                ret.value_ = this.AstToObj(node.child_, ret)
                break;
            case 8://array 数组类属性值
                ret.value_ = [];
                child = node.child_;
                while (child != null) {
                    ret.value_.push(this.AstToObj(child, ret))
                    child = child.next_;
                }
                ret.arraySize_ = node.arraySize_;
                ret.arrayType_ = node.arrayType_;
                break;
            case 9://属性等于叶子
                ret.value_ = node.stringValue_
                break;
            case 10://删除属性
                ret.value_ = null;
                break;
            case 11://bool
                if (node.integerValue_ == 0) ret.value_ = false;
                else ret.value_ = true;
                break;
            default:
                NapiLog.logError("unknow type");
                break;
        }
        return ret;
    }
    MergeObj(files) {
        let objList = []
        for (let i in files) {
            objList.push(files[i])
        }
        let err = true;
        for (let i in objList) {
            err = err && NodeTools.RedefineCheck(objList[i])
        }
        if (!err) {
            return false;
        }
        objList.push(objList.shift());
        let ret = {
            type_: DataType.NODE,
            name_: "root",
            value_: []
        }
        for (let i in objList) {
            NodeTools.Merge(ret, objList[i])
        }
        // console.log(this.ObjToHcs(ret, 0))
        return ret;
    }
    ExpandObj(node) {
        // console.log(this.ObjToHcs(node, 0))
        NodeTools.NodeExpand(node)
        // console.log(this.ObjToHcs(node, 0))
        NodeTools.InheritExpand(node)
        // console.log(this.ObjToHcs(node, 0))
        return node;
    }
    MakeSpace(deep) {
        let ret = ""
        for (let i = 0; i < deep; i++)
            ret += "    "
        return ret;
    }
    MakeHcs(fn, node) {
        let body = this.ObjToHcs(node, 0);
        let head = "";

        let fpath = fn.substring(0, fn.lastIndexOf("\\") + 1);
        for (let i in this.parser.astList[fn].include) {
            head += '#include "./' + this.parser.astList[fn].include[i].substring(fpath.length) + '"\n';
        }
        return head + body;
    }
    ObjToHcs(node, deep) {
        let ret = ""
        let child;
        switch (node.type_) {
            case 1://uin8
            case 2://uin16
            case 3://uin32
            case 4://uin64
                ret = NodeTools.JinZhi_10_to_x(node.value_, node.jinzhi_);
                break;
            case 5://string
                ret = '"' + node.value_ + '"';
                break;
            case 6://ConfigNode
                switch (node.nodeType_) {
                    case 0://数据类节点，不继承
                        ret = this.MakeSpace(deep) + node.name_ + ' {\n';
                        break;
                    case 3://删除类节点
                        ret = this.MakeSpace(deep) + node.name_ + ' : delete {\n';
                        break;
                    case 4://模板类节点
                        ret = this.MakeSpace(deep) + 'template ' + node.name_ + ' {\n';
                        break;
                    case 5://数据类节点，继承
                        ret = this.MakeSpace(deep) + node.name_ + ' :: ' + node.ref_ + ' {\n';
                        break;
                    case 1://复制类节点
                        ret = this.MakeSpace(deep) + node.name_ + ' : ' + node.ref_ + ' {\n';
                        break;
                    case 2://引用修改类节点
                        ret = this.MakeSpace(deep) + node.name_ + ' : &' + node.ref_ + ' {\n';
                        break;
                    default://
                        NapiLog.logError("unknow node type");
                        break;
                }

                for (let i in node.value_) {
                    ret += this.ObjToHcs(node.value_[i], deep + 1)
                }
                ret += this.MakeSpace(deep) + '}\n'
                break;
            case 7://ConfigTerm
                ret = this.MakeSpace(deep) + node.name_ + ' = '
                ret += this.ObjToHcs(node.value_, 0);
                ret += ";\n"
                break;
            case 8://array 数组类属性值
                ret = '[';
                for (let i in node.value_) {
                    let ss = this.ObjToHcs(node.value_[i], 0);
                    if (i > 0) ret += ", ";
                    ret += ss;
                }
                ret += "]";
                break;
            case 9://属性等于叶子
                ret = "&" + node.value_;
                break;
            case 10://删除属性
                ret = "delete";
                break;
            case 11://bool
                if (node.value_) ret = "true";
                else ret = "false";
                break;
            default:
                console.log("unknow", node.type_)
                break;
        }
        return ret;
    }
    // ObjToJson(node, deep) {
    //     let ret = ""
    //     let child;
    //     switch (node.type_) {
    //         case 1://uin8
    //         case 2://uin16
    //         case 3://uin32
    //         case 4://uin64
    //             ret = '"' + node.integerValue_ + '"';
    //             break;
    //         case 5://string
    //             ret = '"' + node.stringValue_ + '"';
    //             break;
    //         case 6://ConfigNode
    //             ret = this.MakeSpace(deep) + '"' + node.name_ + '":{\n'
    //             child = node.child_;
    //             while (child != null) {
    //                 ret += this.ObjToJson(child, deep + 1)
    //                 child = child.next_
    //             }
    //             ret += this.MakeSpace(deep) + '}\n'
    //             break;
    //         case 7://ConfigTerm
    //             ret = this.MakeSpace(deep) + '"' + node.name_ + '":'
    //             child = node.child_;
    //             while (child != null) {
    //                 ret += this.ObjToJson(child, deep + 1)
    //                 child = child.next_
    //             }
    //             ret += ";\n"
    //             break;
    //         default:
    //             console.log("unknow")
    //             break;
    //     }
    //     return ret;
    // }
}
Generator.pInstance_ = null;
Generator.gi = function () {
    if (Generator.pInstance_ == null) Generator.pInstance_ = new Generator();
    return Generator.pInstance_;
}

module.exports = {
    Generator
}
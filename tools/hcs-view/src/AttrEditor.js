const { AttributeArea } = require("./attr/AttributeArea");
const { ModifyNode } = require("./hcs/ModifyNode");
const { DataType, NodeType } = require("./hcs/NodeTools");
const { NodeTools } = require("./hcs/NodeTools");

class AttrEditor {
    constructor() {
        AttributeArea.gi().RegistRecvCallback(this.OnChange);
        this.files_ = []
        this.callback_ = null;
    }
    SetFiles(files) {
        this.files_ = files;
    }
    FreshEditor(fn, node) {
        this.filePoint_ = fn;
        this.node_ = node;
        if (fn in this.files_) {
            this.root_ = this.files_[fn];
        }

        AttributeArea.gi().Clear();
        if (this.node_ != null) {
            AttributeArea.gi().AddTitle(this.node_.name_)
            switch (this.node_.type_) {
                case 6://节点
                    switch (this.node_.nodeType_) {
                        case 0://数据类节点，不继承
                            this.FreshDataNodeNotInheritEditor(this.node_);
                            break;
                        case 1://复制类节点
                        case 2://引用修改类节点
                        case 5://数据类节点，继承
                            this.FreshCopyNodeEditor(this.node_);
                            break;
                        case 3://删除类节点
                            this.FreshDeleteNodeEditor(this.node_);
                            break;
                        case 4://模板类节点
                            this.FreshTempletNodeEditor(this.node_);
                            break;
                        // return "templete " + data.name_;
                        default://
                            break;
                    }
                    break;
                case 7://属性
                    this.FreshDefineAttributeEditor(this.node_);
                    break;
            }
        }
        else AttributeArea.gi().AddTitle("属性编辑区")

        AttributeArea.gi().Flush();
    }
    //节点0 -- 数据类  不继承
    FreshDataNodeNotInheritEditor(node) {//编辑数据节点
        AttributeArea.gi().AddSelect("node_type", "节点类型", AttrEditor.NODE_TYPE_STR, AttrEditor.NODE_TYPE_STR[node.nodeType_], this.root_ == this.node_);
        AttributeArea.gi().AddGap(0);

        AttributeArea.gi().AddInput("name", "名称", node.name_, this.root_ == this.node_);
        AttributeArea.gi().AddGap(0);
        AttributeArea.gi().AddButton("add_child_node", "添加子节点");
        AttributeArea.gi().AddGap(0);
        AttributeArea.gi().AddButton("add_child_attr", "添加子属性");
        AttributeArea.gi().AddGap(0);
        AttributeArea.gi().AddButton("delete", "删除");
    }
    ChangeDataNodeNotInherit(searchId, type, value) {
        if (searchId == "node_type") {
            ModifyNode.ModifyNodeType(this.files_, this.root_, this.node_, AttrEditor.NODE_TYPE_STR.indexOf(value));
            this.FreshEditor(this.filePoint_, this.node_);
        }
        if (searchId == "name" && this.node_.name_ != value) ModifyNode.ModifyName(this.files_, this.root_, this.node_, value);
        if (searchId == "add_child_node") ModifyNode.AddChildNode(this.root_, this.node_);
        if (searchId == "add_child_attr") ModifyNode.AddChildAttr(this.root_, this.node_);
        if (searchId == "delete") {
            ModifyNode.DeleteNode(this.node_);
            this.FreshEditor();
        }
    }
    //节点1 -- 复制类节点
    //节点2 -- 引用修改类节点
    //节点5 -- 数据节点 继承
    FreshCopyNodeEditor(node) {
        AttributeArea.gi().AddInput("node_type", "节点类型", AttrEditor.NODE_TYPE_STR[node.nodeType_], true);
        AttributeArea.gi().AddGap(0);
        AttributeArea.gi().AddInput("name", "名称", node.name_);
        AttributeArea.gi().AddGap(0);
        AttributeArea.gi().AddButton("change_target", node.ref_);
        AttributeArea.gi().AddGap(0);
        AttributeArea.gi().AddButton("add_child_node", "添加子节点");
        AttributeArea.gi().AddGap(0);
        AttributeArea.gi().AddButton("add_child_attr", "添加子属性");
        AttributeArea.gi().AddGap(0);
        AttributeArea.gi().AddButton("delete", "删除");
    }
    ChangeCopyNode(searchId, type, value) {
        if (searchId == "name" && this.node_.name_ != value) ModifyNode.ModifyName(this.files_, this.root_, this.node_, value);
        if (searchId == "change_target") {
            this.CallCallback("change_target", this.node_);
        }
        if (searchId == "add_child_node") ModifyNode.AddChildNode(this.root_, this.node_);
        if (searchId == "add_child_attr") ModifyNode.AddChildAttr(this.root_, this.node_);
        if (searchId == "delete") {
            ModifyNode.DeleteNode(this.node_);
            this.FreshEditor();
        }
    }
    //节点3 -- 删除类节点
    FreshDeleteNodeEditor(node) {
        AttributeArea.gi().AddInput("node_type", "节点类型", AttrEditor.NODE_TYPE_STR[node.nodeType_], true);
        AttributeArea.gi().AddGap(0);
        AttributeArea.gi().AddInput("name", "名称", node.name_);
        AttributeArea.gi().AddGap(0);
        AttributeArea.gi().AddButton("delete", "删除");
    }
    ChangeDeleteNode(searchId, type, value) {
        if (searchId == "name" && this.node_.name_ != value) ModifyNode.ModifyName(this.files_, this.root_, this.node_, value);
        if (searchId == "delete") {
            ModifyNode.DeleteNode(this.node_);
            this.FreshEditor();
        }
    }
    //节点4 -- 模板类
    FreshTempletNodeEditor(node) {
        AttributeArea.gi().AddInput("node_type", "节点类型", AttrEditor.NODE_TYPE_STR[node.nodeType_], true);
        AttributeArea.gi().AddGap(0);
        AttributeArea.gi().AddInput("name", "名称", node.name_);
        AttributeArea.gi().AddGap(0);
        AttributeArea.gi().AddButton("add_child_node", "添加子节点");
        AttributeArea.gi().AddGap(0);
        AttributeArea.gi().AddButton("add_child_attr", "添加子属性");
        AttributeArea.gi().AddGap(0);
        AttributeArea.gi().AddButton("delete", "删除");
    }
    ChangeTempletNode(searchId, type, value) {
        if (searchId == "name") ModifyNode.ModifyName(this.files_, this.root_, this.node_, value);
        if (searchId == "add_child_node") ModifyNode.AddChildNode(this.root_, this.node_);
        if (searchId == "add_child_attr") ModifyNode.AddChildAttr(this.root_, this.node_);
        if (searchId == "delete") {
            ModifyNode.DeleteNode(this.node_);
            this.FreshEditor();
        }
    }


    //属性0 定义类
    FreshDefineAttributeEditor(node) {//定义类属性
        let v = node.value_;
        AttributeArea.gi().AddInput("name", "名称", node.name_);
        AttributeArea.gi().AddGap(0);

        if (v.type_ == DataType.INT8 ||
            v.type_ == DataType.INT16 ||
            v.type_ == DataType.INT32 ||
            v.type_ == DataType.INT64) {
            AttributeArea.gi().AddSelect("value_type", "类型", AttrEditor.ATTR_TYPE_STR, AttrEditor.ATTR_TYPE_STR[0]);
            AttributeArea.gi().AddGap(0);
            // AttributeArea.gi().AddInput("jinzhi", "进制", v.jinzhi_);
            AttributeArea.gi().AddInput("value", "属性值", NodeTools.JinZhi_10_to_x(v.value_, v.jinzhi_));
            AttributeArea.gi().AddGap(0);
        }
        else if (v.type_ == DataType.STRING) {
            AttributeArea.gi().AddSelect("value_type", "类型", AttrEditor.ATTR_TYPE_STR, AttrEditor.ATTR_TYPE_STR[1]);
            AttributeArea.gi().AddGap(0);
            AttributeArea.gi().AddInput("value", "属性值", v.value_);
            AttributeArea.gi().AddGap(0);
        }
        else if (v.type_ == DataType.ARRAY) {
            AttributeArea.gi().AddSelect("value_type", "类型", AttrEditor.ATTR_TYPE_STR, AttrEditor.ATTR_TYPE_STR[2]);
            AttributeArea.gi().AddGap(0);
            AttributeArea.gi().AddTextArea("value", "属性值", NodeTools.ArrayToString(v, 20));
            AttributeArea.gi().AddGap(0);
        }
        else if (v.type_ == DataType.BOOL) {
            AttributeArea.gi().AddSelect("value_type", "类型", AttrEditor.ATTR_TYPE_STR, AttrEditor.ATTR_TYPE_STR[3]);
            AttributeArea.gi().AddGap(0);
            AttributeArea.gi().AddSelect("value", "属性值", [true, false], v.value_ == 1);
            AttributeArea.gi().AddGap(0);
        }
        else if (v.type_ == DataType.REFERENCE) {
            AttributeArea.gi().AddSelect("value_type", "类型", AttrEditor.ATTR_TYPE_STR, AttrEditor.ATTR_TYPE_STR[4]);
            AttributeArea.gi().AddGap(0);
            AttributeArea.gi().AddButton("change_target", v.value_);
            AttributeArea.gi().AddGap(0);
        }
        else if (v.type_ == DataType.DELETE) {
            AttributeArea.gi().AddSelect("value_type", "类型", AttrEditor.ATTR_TYPE_STR, AttrEditor.ATTR_TYPE_STR[5]);
            AttributeArea.gi().AddGap(0);
        }

        AttributeArea.gi().AddButton("delete", "删除");
    }
    ChangeDefineAttribute(searchId, type, value) {
        let v = this.node_.value_;
        if (searchId == "name" && value != this.node_.name_) ModifyNode.ModifyName(this.files_, this.root_, this.node_, value);
        if (searchId == "value_type") {
            switch (AttrEditor.ATTR_TYPE_STR.indexOf(value)) {
                case 0://整形
                    v.type_ = DataType.INT8;
                    v.value_ = 0;
                    v.jinzhi_ = 10;
                    break;
                case 1://str
                    v.type_ = DataType.STRING;
                    v.value_ = "";
                    break;
                case 2://array
                    v.type_ = DataType.ARRAY;
                    v.value_ = [];
                    break;
                case 3://bool
                    v.type_ = DataType.BOOL;
                    v.value_ = 1;
                    break;
                case 4://ref
                    v.type_ = DataType.REFERENCE;
                    v.value_ = "_unknow_";
                    break;
                case 5://delete
                    v.type_ = DataType.DELETE;
                    break;
            }
            this.FreshEditor(this.filePoint_, this.node_);
        }
        if (searchId == "change_target") {
            this.CallCallback("change_target", v);
        }
        if (searchId == "value") {
            if (v.type_ == DataType.INT8 ||
                v.type_ == DataType.INT16 ||
                v.type_ == DataType.INT32 ||
                v.type_ == DataType.INT64) {
                let ret = NodeTools.JinZhi_x_to_10(value)
                if (isNaN(ret[0])) {
                    // this.node_.errMsg_="整数格式错误";
                }
                else {
                    v.value_ = ret[0]
                    v.jinzhi_ = ret[1]
                }
            }
            else if (v.type_ == DataType.STRING) {
                v.value_ = value;
            }
            else if (v.type_ == DataType.ARRAY) {
                v.value_ = NodeTools.StringToArray(value);
            }
            else if (v.type_ == DataType.BOOL) {
                v.value_ = value == "true" ? 1 : 0;
            }
        }
        if (searchId == "delete") {
            ModifyNode.DeleteNode(this.node_);
            this.FreshEditor();
        }
    }
    //属性1 删除类
    //属性2 引用类
    OnChange(searchId, type, value) {
        let pattr = AttrEditor.gi();
        if (pattr.node_ != null) {
            AttributeArea.gi().AddTitle(pattr.node_.name_)
            switch (pattr.node_.type_) {
                case 6://节点
                    switch (pattr.node_.nodeType_) {
                        case 0://数据类节点，不继承
                            pattr.ChangeDataNodeNotInherit(searchId, type, value);
                            break;
                        case 1://复制类节点
                        case 2://引用修改类节点
                        case 5://数据类节点，继承
                            pattr.ChangeCopyNode(searchId, type, value);
                            break;
                        case 3://删除类节点
                            pattr.ChangeDeleteNode(searchId, type, value);
                            break;
                        case 4://模板类节点
                            pattr.ChangeTempletNode(searchId, type, value);
                            break;
                        // return "templete " + data.name_;
                        default://
                            break;
                    }
                    break;
                case 7://属性
                    pattr.ChangeDefineAttribute(searchId, type, value);
                    break;
            }
            pattr.CallCallback("writefile")
        }
        // console.log(searchId, type, value)
    }
    CallCallback(type, value) {
        if (this.callback_ != null) {
            this.callback_(type, value);
        }
    }
    RegistCallback(func) {
        this.callback_ = func;
    }
}

AttrEditor.NODE_TYPE_STR = ["数据类不继承", "复制类", "引用修改类", "删除类", "模板类", "数据类继承"];
AttrEditor.ATTR_CLASS = ["定义类属性", "删除类属性", "引用类属性"];

AttrEditor.ATTR_TYPE_STR = ["整数", "字符串", "数组", "布尔", "引用", "删除"]

AttrEditor.pInstance_ = null;
AttrEditor.gi = function () {
    if (AttrEditor.pInstance_ == null) AttrEditor.pInstance_ = new AttrEditor();
    return AttrEditor.pInstance_;
}

module.exports = {
    AttrEditor
}

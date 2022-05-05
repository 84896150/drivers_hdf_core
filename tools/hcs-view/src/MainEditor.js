const { XMessage } = require("./message/XMessage");
const { Lexer } = require("./hcs/lexer");
const { Generator } = require("./hcs/Generator");
const { Scr } = require("./engine/XDefine");
const { XButton } = require("./engine/control/XButton");
const { AttrEditor } = require("./AttrEditor");
const { NapiLog } = require("./hcs/NapiLog");
const { XSelect } = require("./engine/control/XSelect");
const { NodeTools, DataType, NodeType } = require("./hcs/NodeTools");
const { ModifyNode } = require("./hcs/ModifyNode");

function RGBA(r, g, b, a) {
    if (a == undefined) a = 255
    return a << 24 | r << 16 | g << 8 | b
}
class MainEditor {
    constructor() {
        this.files_ = {}
        this.filePoint_ = null;
        this.rootPoint_ = null;
        this.nodePoint_ = null;
        this.offX_ = 100;
        this.offY_ = 100;
        this.touchQueue_ = [];
        this.keyQueue_ = [];
        this.dropAll_ = {
            locked: false,
            oldx: -1,
            oldy: -1
        }
        this.nodeBtns = [];
        this.nodeBtnPoint_ = 0;
        XMessage.gi().RegistRecvCallback(this.OnReceive);
        XMessage.gi().Send("inited", "");
        AttrEditor.gi().FreshEditor();

        this.btnSelectTemp = new XButton(0, 0, 0, 0, "选择模板")
        this.sltInclude = new XSelect(["a", "b", "c"], "b");
        this.sltInclude.RegistCallback(this.OnSelectInclude)
        NapiLog.registError(this.OnError);
        this.errorMsg_ = []
        this.delay_ = 0;

        this.selectNode_ = {
            type: null,
            pnode: null
        };
        this.btnCancelSelect_ = new XButton();

        AttrEditor.gi().RegistCallback(this.OnAttributeChange);

        this.mousePos_ = {
            x: 0,
            y: 0
        }
    }

    CalcPostionY(data, y) {
        data.posY = y;
        let ty = y;
        switch (data.type_) {
            case 1://uin8
            case 2://uin16
            case 3://uin32
            case 4://uin64
                y += MainEditor.LINE_HEIGHT;
                break;
            case 5://string
                y += MainEditor.LINE_HEIGHT;
                break;
            case 6://ConfigNode
                for (let i in data.value_) {
                    y = this.CalcPostionY(data.value_[i], y)
                }
                break;
            case 7://ConfigTerm
                y = this.CalcPostionY(data.value_, y)
                break;
            case 8://array 数组类属性值
                y += MainEditor.LINE_HEIGHT;
                // for (let d in data.value_) {
                //     y = this.CalcPostionY(data.value_[d], y)
                // }
                break;
            case 9://属性等于叶子
                y += MainEditor.LINE_HEIGHT
                break;
            case 10://删除属性
                y += MainEditor.LINE_HEIGHT
                break;
            case 11://bool
                y += MainEditor.LINE_HEIGHT
                break;
            default:
                console.log("unknow", data.type_)
                break;
        }
        if (y > ty) {
            data.posY = (ty + y - MainEditor.LINE_HEIGHT) / 2
        }
        return y > (ty + MainEditor.LINE_HEIGHT) ? y : (ty + MainEditor.LINE_HEIGHT);
    }
    GetNodeText(data) {
        switch (data.nodeType_) {
            case 0://数据类节点，不继承
                return data.name_;
            case 3://删除类节点
                return data.name_ + " : delete";
            case 4://模板类节点
                return "templete " + data.name_;
            case 5://数据类节点，继承
                return data.name_ + ' :: ' + data.ref_;
            case 1://复制类节点
                return data.name_ + " : " + data.ref_;
            case 2://引用修改类节点
                return data.name_ + " : &" + data.ref_;
            default://
                return "unknow node type";
        }
        return "unknow node type";
    }
    DrawNode(pm2f, s, size, x, y, color, bkcolor) {
        let w = pm2f.GetTextWidth(s, size);
        pm2f.FillRect(x - 3, y - 3, w + 6, 20 + 6, bkcolor);
        pm2f.DrawText(s, size, x, y, 1, 1, 0, -1, -1, color);
        return w;
    }
    DrawObj(pm2f, data, offx, offy, path) {
        let w;
        path += data.name_;
        data.posX = offx;

        switch (data.type_) {
            case 1://uin8
            case 2://uin16
            case 3://uin32
            case 4://uin64
                w = pm2f.DrawText(NodeTools.JinZhi_10_to_x(data.value_, data.jinzhi_), 18, offx, offy + data.posY, 1, 1, 0, -1, -1, 0xffff0000);
                break;
            case 5://string
                w = pm2f.DrawText('"' + data.value_ + '"', 18, offx, offy + data.posY, 1, 1, 0, -1, -1, 0xffff0000);
                break;
            case 6://ConfigNode
                w = this.DrawNode(pm2f, this.GetNodeText(data), 18, offx, offy + data.posY, 0xffffffff, RGBA(67, 95, 188));
                this.SetNodeButton(pm2f, offx, offy + data.posY, w, 20, path, data);

                for (let i in data.value_) {
                    this.DrawObj(pm2f, data.value_[i], offx + w + 50, offy, path + ".");
                    pm2f.DrawLine(data.posX + w, offy + data.posY + 10,
                        data.value_[i].posX, offy + data.value_[i].posY + 10, 0xffffffff, 2)
                }
                break;
            case 7://ConfigTerm
                w = this.DrawNode(pm2f, data.name_ + "=", 18, offx, offy + data.posY, 0xffff0000, RGBA(153, 217, 234));
                this.SetNodeButton(pm2f, offx, offy + data.posY, w, 20, path, data);
                this.DrawObj(pm2f, data.value_, offx + w, offy, path);
                break;
            case 8://array 数组类属性值
                let ss = "[" + data.value_.length + "]" + NodeTools.ArrayToString(data)
                if (ss.length > 32) {
                    ss = ss.substring(0, 29) + "..."
                }
                w = pm2f.DrawText(ss, 18, offx, offy + data.posY, 1, 1, 0, -1, -1, 0xffffffff);
                break;
            case 9://属性等于叶子
                w = pm2f.DrawText("&" + data.value_, 18, offx, offy + data.posY, 1, 1, 0, -1, -1, 0xffff0000);
                break;
            case 10://删除属性
                w = pm2f.DrawText("delete", 18, offx, offy + data.posY, 1, 1, 0, -1, -1, 0xffff0000);
                break;
            case 11://bool
                if (data.value_) w = pm2f.DrawText("true", 18, offx, offy + data.posY, 1, 1, 0, -1, -1, 0xffff0000);
                else w = pm2f.DrawText("false", 18, offx, offy + data.posY, 1, 1, 0, -1, -1, 0xffff0000);
                break;
            default:
                console.log("unknow", data.type_)
                break;
        }
        // NodeTools.CheckError(this.files_, data);
        if (data.errMsg_ != null) {
            if (parseInt(this.delay_ / 10) % 2 == 0) {
                pm2f.DrawRect(offx - 5, offy + data.posY - 5, w + 10, 20 + 10, 0xffff0000, 3);
            }
            pm2f.DrawText(data.errMsg_, 18, offx + w - 5, offy + data.posY + 5, 1, 1, 0, -1, -3, 0xffff0000);
        }
    }
    SetNodeButton(pm2f, x, y, w, h, path, node) {
        if (this.nodePoint_ == node) {
            pm2f.DrawRect(x - 3, y - 3, w + 6, h + 6, 0xff00ff00, 2);
        }
        if (this.nodeBtnPoint_ >= this.nodeBtns.length) {
            this.nodeBtns.push(new XButton());
        }
        let pbtn = this.nodeBtns[this.nodeBtnPoint_];
        pbtn.Move(x - 3, y - 3, w + 6, h + 6);
        pbtn.name_ = path;
        pbtn.node_ = node;
        this.nodeBtnPoint_ += 1;
    }
    Draw(pm2f) {
        pm2f.FillRect(0, 0, Scr.logicw, Scr.logich, 0xff303030);

        if (this.filePoint_ != null && this.filePoint_ in this.files_) {
            let data = this.files_[this.filePoint_];
            this.CalcPostionY(data, 0);

            this.nodeBtnPoint_ = 0;
            this.DrawObj(pm2f, data, this.offX_, this.offY_, "");
        }
        // for (let i = 0; i < this.nodeBtnPoint_; i++) {
        //     this.nodeBtns[i].Draw();
        // }
        this.btnSelectTemp.Move(Scr.logicw - 120, 20, 100, 25).Draw();
        this.sltInclude.Move(10, 10, 200, 20).Draw();

        if (this.selectNode_.type != null) {
            if (this.selectNode_.type == "change_target") {
                pm2f.DrawText("点击选择目标", 18, this.mousePos_.x, this.mousePos_.y, 1, 1, 0, -3, -3, 0xff00ff00);
                this.btnCancelSelect_.name_ = "取消选择";
            }
            else if (this.selectNode_.type == "copy_node") {
                pm2f.DrawText("已复制" + this.selectNode_.pnode.name_, 18, this.mousePos_.x, this.mousePos_.y, 1, 1, 0, -3, -3, 0xff00ff00);
                this.btnCancelSelect_.name_ = "取消复制";
            }
            else if (this.selectNode_.type == "cut_node") {
                pm2f.DrawText("已剪切" + this.selectNode_.pnode.name_, 18, this.mousePos_.x, this.mousePos_.y, 1, 1, 0, -3, -3, 0xff00ff00);
                this.btnCancelSelect_.name_ = "取消剪切";
            }

            this.btnCancelSelect_.Move(Scr.logicw - 120, Scr.logich - 30, 100, 20).Draw();
        }

        if (this.errorMsg_.length > 0) {
            let ts = (new Date()).getTime()
            while (this.errorMsg_.length > 0 && this.errorMsg_[0][0] < ts) {
                this.errorMsg_.shift();
            }
            for (let i in this.errorMsg_) {
                let y = Scr.logich / 2 - this.errorMsg_.length * 20 + i * 20
                let a = parseInt((this.errorMsg_[i][0] - ts) / 2);
                if (a > 255) a = 255;
                console.log(a)
                a = a << 24;
                pm2f.FillRect(0, y, Scr.logicw, 20, 0xff0000 | a);
                pm2f.DrawText(this.errorMsg_[i][1], 18, Scr.logicw / 2, y, 1, 1, 0, -2, -1, 0xffffff | a);
            }

        }
        this.delay_ += 1;
        this.ProcAll();
    }
    ProcTouch(msg, x, y) {
        this.mousePos_.x = x;
        this.mousePos_.y = y;
        if (this.dropAll_.locked) {
            if (msg == 2) {
                this.offX_ += (x - this.dropAll_.oldx);
                this.offY_ += (y - this.dropAll_.oldy);
                this.dropAll_.oldx = x;
                this.dropAll_.oldy = y;
            }
            if (msg == 3) {
                this.dropAll_.locked = false;
            }
            return true;
        }
        if (this.btnSelectTemp.ProcTouch(msg, x, y)) {
            if (this.btnSelectTemp.IsClicked()) {

            }
            return true;
        }
        if (this.sltInclude.ProcTouch(msg, x, y)) {
            return true;
        }
        if (this.selectNode_.type != null) {
            if (this.btnCancelSelect_.ProcTouch(msg, x, y)) {
                if (this.btnCancelSelect_.IsClicked()) {
                    this.selectNode_.type = null;
                }
                return true;
            }
        }
        for (let i = 0; i < this.nodeBtnPoint_; i++) {
            if (this.nodeBtns[i].ProcTouch(msg, x, y)) {
                if (this.nodeBtns[i].IsClicked()) {
                    if (this.selectNode_.type != null &&
                        (this.selectNode_.type != "copy_node" && this.selectNode_.type != "cut_node")) {
                        if (this.selectNode_.type == "change_target") {//可以拷贝的是：数据不继承，复制类节点，模板类节点，数据类继承
                            let pn = this.nodeBtns[i].node_;
                            if (pn.type_ == DataType.NODE) {
                                if (this.selectNode_.pnode.type_ == DataType.NODE) {
                                    if (NodeTools.GetPathByNode(this.selectNode_.pnode.parent_) == NodeTools.GetPathByNode(pn.parent_)) {
                                        this.selectNode_.pnode.ref_ = pn.name_;
                                    }
                                    else {
                                        this.selectNode_.pnode.ref_ = NodeTools.GetPathByNode(pn);
                                    }
                                }
                                else if (this.selectNode_.pnode.type_ == DataType.REFERENCE) {
                                    if (NodeTools.GetPathByNode(this.selectNode_.pnode.parent_.parent_) == NodeTools.GetPathByNode(pn.parent_)) {
                                        this.selectNode_.pnode.value_ = pn.name_;
                                    }
                                    else {
                                        this.selectNode_.pnode.value_ = NodeTools.GetPathByNode(pn);
                                    }
                                }

                                this.CheckAllError()
                                this.selectNode_.type = null;
                                this.OnAttributeChange("writefile");
                            }
                        }
                    }
                    else this.nodePoint_ = this.nodeBtns[i].node_;
                    AttrEditor.gi().FreshEditor(this.filePoint_, this.nodePoint_)
                }
                return true;
            }
        }
        //拖动屏幕
        if (msg == 1 && !this.dropAll_.locked) {
            this.dropAll_.locked = true;
            this.dropAll_.oldx = x;
            this.dropAll_.oldy = y;
            return true;
        }
    }
    ProcKey(k) {
        if (k == "ctrl+c") {
            if (this.nodePoint_ != null) {
                this.selectNode_ = {
                    type: "copy_node",
                    pnode: this.nodePoint_
                };
            }
        }
        else if (k == "ctrl+x") {
            if (this.nodePoint_ != null) {
                this.selectNode_ = {
                    type: "cut_node",
                    pnode: this.nodePoint_
                };
            }
        }
        else if (k == "ctrl+v") {
            if (this.selectNode_.type != null && this.nodePoint_ != null) {
                let parent = this.nodePoint_
                if (this.nodePoint_.type_ != DataType.NODE)
                    parent = this.nodePoint_.parent_;
                parent.value_.push(NodeTools.CopyNode(this.selectNode_.pnode, parent))
                if (this.selectNode_.type == "cut_node") {
                    ModifyNode.DeleteNode(this.selectNode_.pnode)
                    this.selectNode_.type = null;
                }
                this.CheckAllError();
            }
        }
        else if (k == "Delete") {
            if (this.nodePoint_ != null) {
                ModifyNode.DeleteNode(this.nodePoint_)
                AttrEditor.gi().FreshEditor();
            }
        }
        if (k.length > 1) return;
    }
    ProcAll() {
        while (this.touchQueue_.length > 0) {
            let touch = this.touchQueue_.shift();
            this.ProcTouch(touch[0], touch[1], touch[2]);
        }

        while (this.keyQueue_.length > 0) {
            let k = this.keyQueue_.shift();
            this.ProcKey(k);
        }
    }
    OnSelectInclude(sel) {
        MainEditor.gi().filePoint_ = sel;
        AttrEditor.gi().FreshEditor();
        XMessage.gi().Send("selectchange", sel);
    }
    OnAttributeChange(type, value) {
        let pme = MainEditor.gi();
        if (type == "writefile") {
            pme.CheckAllError();
            XMessage.gi().Send("writefile", {
                fn: pme.filePoint_,
                data: Generator.gi().MakeHcs(pme.filePoint_, pme.files_[pme.filePoint_])//Generator.gi().ObjToHcs(pme.files_[pme.filePoint_], 0)
            })
        }
        else if (type.substring(0, 13) == "change_target") {
            pme.selectNode_.type = type;
            pme.selectNode_.pnode = value;
        }
    }
    OnError(msg) {
        // MainEditor.gi().errorMsg_.push([(new Date()).getTime() + 5000, msg])
    }
    OnTouch(msg, x, y) {
        this.touchQueue_.push([msg, x, y]);
    }
    OnKey(k) {
        this.keyQueue_.push(k)
    }
    OnReceive(type, data) {
        console.log(type)
        let me = MainEditor.gi();
        if (type == "parse") {
            me.Parse(data)
        }
        else if (type == "filedata") {
            Lexer.FILE_AND_DATA[data.fn] = data.data;
            me.Parse(data.fn)
        }
        else if (type == "freshfiledata") {
            Lexer.FILE_AND_DATA[data.fn] = data.data;
            let t = Generator.gi().HscToAst(data.fn);
            me.files_[data.fn] = Generator.gi().AstToObj(t[data.fn].ast.astRoot_);
            me.CheckAllError();
            AttrEditor.gi().FreshEditor(this.filePoint_, this.nodePoint_);
        }
    }
    Parse(fn) {
        if (this.rootPoint_ == null) {
            this.rootPoint_ = fn
        }
        let t = Generator.gi().HscToAst(fn);
        if (!t) return;
        let fs = []
        for (let i in t) {
            this.files_[i] = Generator.gi().AstToObj(t[i].ast.astRoot_);
            fs.push(i)
        }
        this.filePoint_ = this.rootPoint_;
        this.sltInclude.ResetList(fs, this.filePoint_)
        AttrEditor.gi().SetFiles(this.files_)

        this.CheckAllError();
    }
    CheckAllError() {
        NapiLog.clearError()
        let n1 = Generator.gi().MergeObj(this.files_)
        if (n1) {
            n1 = Generator.gi().ExpandObj(n1)
            if (NapiLog.getResult()[0])
                return true;
        }
        return false;
    }
}
MainEditor.LINE_HEIGHT = 30

MainEditor.pInstance_ = null;
MainEditor.gi = function () {
    if (MainEditor.pInstance_ == null) MainEditor.pInstance_ = new MainEditor();
    return MainEditor.pInstance_;
}

module.exports = {
    MainEditor
}
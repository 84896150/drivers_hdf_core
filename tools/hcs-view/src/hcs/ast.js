const { NapiLog } = require("./NapiLog")
const { TokenType } = require("./lexer")

var ObjectType = {
    PARSEROP_UINT8: 0x01,
    PARSEROP_UINT16: 2,
    PARSEROP_UINT32: 3,
    PARSEROP_UINT64: 4,
    PARSEROP_STRING: 5,
    PARSEROP_CONFNODE: 6,
    PARSEROP_CONFTERM: 7,
    PARSEROP_ARRAY: 8,
    PARSEROP_NODEREF: 9,
    PARSEROP_DELETE: 10,
    PARSEROP_BOOL: 11,
};
var NodeRefType = {
    NODE_NOREF: 0,
    NODE_COPY: 1,
    NODE_REF: 2,
    NODE_DELETE: 3,
    NODE_TEMPLATE: 4,
    NODE_INHERIT: 5,
};

var HcsErrorNo = {
    NOERR: 0,  /* No error */
    EFAIL: 1,      /* Process fail */
    EOOM: 2,       /* Out of memory */
    EOPTION: 3,    /* Option error */
    EREOPENF: 4,   /* Reopen argument */
    EINVALF: 5,    /* Invalid file */
    EINVALARG: 6,  /* Invalid argument */
    EDECOMP: 7,    /* Decompile error */
    EOUTPUT: 8,    /* Output error */
    EASTWALKBREAK: 9,  /* Break ast walk */
};

var UINT8_MAX = 255;
var UINT16_MAX = 65535;
var UINT32_MAX = 0xffffffff;  /* 4294967295U */
var UINT64_MAX = 0xffffffffffffffff; /* 18446744073709551615ULL */

class AstObject {
    constructor(name, type, value, bindToken, jinzhi) {
        let callType = Object.prototype.toString.call(name);
        if (callType == "[object Object]") {
            this.constructor_sis(name.name_, name.type_, name.stringValue_);
            this.integerValue_ = name.integerValue_;
            // console.log("object", this.integerValue_);
            return;
        }
        else if (callType == "[object String]") {
            // console.log("string", name);
            if (Object.prototype.toString.call(value) == "[object Number]") {
                if (Object.prototype.toString.call(bindToken) == "[object Object]") {
                    this.constructor_siit(name, type, value, bindToken, jinzhi)
                }
                else {
                    this.constructor_sii(name, type, value, jinzhi)
                }
            }
            else if (Object.prototype.toString.call(value) == "[object String]") {
                if (Object.prototype.toString.call(bindToken) == "[object Object]") {
                    this.constructor_sist(name, type, value, bindToken)
                }
                else {
                    this.constructor_sis(name, type, value)
                }
            }
            else {
                console.log("err1");
            }
        }
        else {
            console.log("err2")
        }
    }
    constructor_sii(name, type, value, jinzhi) {
        this.type_ = type
        this.name_ = name
        this.parent_ = null;
        this.lineno_ = 0
        this.opCode_ = 0
        this.size_ = 0
        this.subSize_ = 0
        this.hash_ = 0
        this.integerValue_ = value
        this.jinzhi_ = jinzhi
    }
    constructor_sis(name, type, value) {
        this.constructor_sii(name, type, 0, 10)
        this.stringValue_ = value
    }
    constructor_siit(name, type, value, bindToken, jinzhi) {
        this.constructor_sii(name, type, value, jinzhi)
        this.lineno_ = bindToken.lineNo;
        this.src_ = bindToken.src;

        switch (type) {
            case ObjectType.PARSEROP_UINT8:  /* fall-through */
            case ObjectType.PARSEROP_UINT16: /* fall-through */
            case ObjectType.PARSEROP_UINT32: /* fall-through */
            case ObjectType.PARSEROP_UINT64:
                this.type_ = this.FitIntegerValueType(value);
                break;
            default:
                break;
        }
    }
    constructor_sist(name, type, value, bindToken) {
        this.constructor_siit(name, type, 0, bindToken, 10);
        this.stringValue_ = value;
    }
    FitIntegerValueType(value) {
        if (value <= UINT8_MAX) {
            return ObjectType.PARSEROP_UINT8;
        } else if (value <= UINT16_MAX) {
            return ObjectType.PARSEROP_UINT16;
        } else if (value <= UINT32_MAX) {
            return ObjectType.PARSEROP_UINT32;
        } else {
            return ObjectType.PARSEROP_UINT64;
        }
    }
    AddChild(childObj) {
        if (childObj == null) {
            return false;
        }
        if (this.child_ == null) {
            this.child_ = childObj;
            let childNext = childObj;
            while (childNext != null) {
                childNext.parent_ = this;
                childNext = childNext.next_;
            }
        } else {
            return this.child_.AddPeer(childObj);
        }

        return true;
    }
    AddPeer(peerObject) {
        if (peerObject == null) {
            return false;
        }

        if (this == peerObject) {
            Logger().Error() << "add self as peer";
            return false;
        }

        if (this.next_ == null) {
            this.next_ = peerObject;
        } else {
            let lastNode = this.next_;
            while (lastNode.next_ != null) {
                lastNode = lastNode.next_;
            }
            lastNode.next_ = peerObject;
        }

        let peer = peerObject;
        while (peer) {
            peer.parent_ = this.parent_;
            peer = peer.next_;
        }

        return true;
    }
    //operate<<
    Merge(srcObj) {
        if (srcObj.name_ != this.name_) {
            Logger().Error() << this.SourceInfo() << "merge different node to" << srcObj.SourceInfo();
            return false;
        }

        if (srcObj.type_ != this.type_) {
            Logger().Error() << this.SourceInfo() << "conflict type with " << srcObj.SourceInfo();
            return false;
        }

        this.src_ = srcObj.src_;
        this.lineno_ = srcObj.lineno_;
        this.stringValue_ = srcObj.stringValue_;
        this.integerValue_ = srcObj.integerValue_;

        return true;
    }
    Copy(src, overwrite) {
        if (src == null) {
            return false;
        }

        if (overwrite) {
            this.src_ = src.src_;
            this.lineno_ = src.lineno_;
            this.integerValue_ = src.integerValue_;
            this.stringValue_ = src.stringValue_;
        }

        return true;
    }
    Move(src) {
        if (!this.Copy(src, true)) {
            return false;
        }
        src.Separate();
        return true;
    }
    SourceInfo() {
        return this.src_ + ":" + this.lineno_ + " ";
    }
    Remove() {
        this.Separate();
        this.child_ = null;
        this.next_ = null;
    }
    Lookup(name, type) {
        let peer = this.child_;
        while (peer != null) {
            if (peer.name_ == name && (type == 0 || peer.type_ == type)) {
                return peer;
            }

            peer = peer.next_;
        }

        return null;
    }
    IsNumber() {
        return this.type_ >= ObjectType.PARSEROP_UINT8 && type_ <= ObjectType.PARSEROP_UINT64;
    }
    IsNode() {
        return this.type_ == ObjectType.PARSEROP_CONFNODE;
    }
    IsTerm() {
        return this.type_ == ObjectType.PARSEROP_CONFTERM;
    }
    IsArray() {
        return this.type_ == ObjectType.PARSEROP_ARRAY;
    }
    Separate() {
        if (this.parent_ == null) {
            return;
        }
        if (this.parent_.child_ == this) {
            this.parent_.child_ = this.next_; // decref this
            this.next_ = null;
            return;
        }

        let pre = this.parent_.child_;
        while (pre != null) {
            if (pre.next_ == this) {
                let tmp = this.next_;
                this.next_ = null;
                pre.next_ = tmp;

                // pre.next_ = pre.next_.next_; // decref this
                break;
            }

            pre = pre.next_;
        }
        // this.next_ = null;
    }
    SetParent(parent) { this.parent_ = parent; }
    SetSize(size) { this.size_ = size; }
    SetSubSize(size) { this.subSize_ = size; }
    GetSubSize() { return this.subSize_; }
    SetHash(hash) { this.hash_ = hash; }
    GetSize() { return this.size_; }
    GetHash() { return this.hash_; }
    Next() { return this.next_; }
    Child() { return this.child_; }
    Name() { return this.name_; }
    StringValue() { return this.stringValue_; }
    IntegerValue() { return this.integerValue_; }
    Type() { return this.type_; }
    OpCode() { return this.opCode_; }
    SetOpCode(opcode) { this.opCode_ = opcode; }
    HasDuplicateChild() { return false; }
    IsElders(child) {
        let p = child;
        while (p != null) {
            if (p == this) {
                return true;
            }
            p = p.parent_;
        }
        return false;
    }
    Parent() {
        return this.parent_;
    }
}

class ConfigNode extends AstObject {
    constructor(name, nodeType, refName) {
        if (Object.prototype.toString.call(name) == "[object String]") {
            super(name, ObjectType.PARSEROP_CONFNODE, "");
            this.refNodePath_ = refName;
            this.nodeType_ = nodeType;
            this.inheritIndex_ = 0;
            this.inheritCount_ = 0;
            this.templateSignNum_ = 0;
        }
        else if (Object.prototype.toString.call(nodeType) == "[object Number]") {
            super(name.strval, ObjectType.PARSEROP_CONFNODE, 0, name);
            this.refNodePath_ = refName;
            this.nodeType_ = nodeType;
            this.inheritIndex_ = 0;
            this.inheritCount_ = 0;
            this.templateSignNum_ = 0;
        }
        else {
            super(name, ObjectType.PARSEROP_CONFNODE, "");
            this.refNodePath_ = refName;
            this.nodeType_ = nodeType;
            this.inheritIndex_ = 0;
            this.inheritCount_ = 0;
            this.templateSignNum_ = 0;

            let child = name.child_;
            while (child != null) {
                super.AddChild(AstObjectFactory.Build(child));
                child = child.Next();
            }
        }
        this.subClasses_ = [];
    }
    NodeTypeToStr(type) {
        let type2StringMap = {
            0: "",
            1: "NodeCopy",
            2: "NodeReference",
            3: "NodeDelete",
            4: "NodeInherit",
            5: "NodeTemplate",
        };
        return type2StringMap[type];
    }
    //operator<<
    CastFrom(astObject) {
        return astObject;
    }
    GetNodeType() {
        return this.nodeType_;
    }
    GetRefPath() {
        return this.refNodePath_;
    }
    Merge(srcObj) {
        if (srcObj == null) {
            return true;
        }
        if (!srcObj.IsNode() || srcObj.Name() != this.name_) {
            Logger().Error() << SourceInfo() << "merge conflict type with " << srcObj.SourceInfo();
            return false;
        }

        let srcNode = srcObj;
        if (srcNode.GetNodeType() == TokenType.DELETE) {
            srcObj.Separate();
            this.Separate();
            return true;
        }

        this.nodeType_ = srcNode.nodeType_;
        this.refNodePath_ = srcNode.refNodePath_;

        let childSrc = srcObj.Child();
        while (childSrc != null) {
            let childSrcNext = childSrc.Next();
            let childDst = this.Lookup(childSrc.Name(), childSrc.Type());
            if (childDst == null) {
                childSrc.Separate();
                this.AddChild(childSrc);
            } else {
                if (!childDst.Merge(childSrc)) {
                    return false;
                }
            }
            childSrc = childSrcNext;
        }
        return true;
    }
    SetNodeType(nodeType) {
        this.nodeType_ = nodeType;
    }
    SetRefPath(ref) {
        this.refNodePath_ = ref;
    }
    HasDuplicateChild() {
        let symMap = {};
        let child = this.child_;
        while (child != null) {
            let sym = symMap[child.Name()];
            if (sym != undefined) {
                Logger().Error() << child.SourceInfo() << "redefined, first definition at " << sym.second.SourceInfo();
                return true;
            }
            symMap[child.Name()] = child;
            child = child.Next();
        }

        return false;
    }
    InheritExpand(refObj) {
        if (refObj == null) {
            Logger().Error() << SourceInfo() << "inherit invalid node: " << refNodePath_;
            return false;
        }

        if (!this.Copy(refObj, false)) {
            return false;
        }

        let templateNode = this.CastFrom(refObj);
        if (!this.Compare(templateNode)) {
            return false;
        }
        this.inheritIndex_ = templateNode.inheritCount_++;
        templateNode.subClasses_.push(this);
        return true;
    }
    RefExpand(refObj) {
        if (this.nodeType_ == NodeRefType.NODE_DELETE) {
            this.Separate();
            return true;
        }

        if (refObj.IsElders(this)) {
            Logger().Error() << SourceInfo() << "circular reference " << refObj.SourceInfo();
            return false;
        }

        let ret = true;
        if (this.nodeType_ == NodeRefType.NODE_REF) {
            ret = this.NodeRefExpand(refObj);
        } else if (nodeType_ == NodeRefType.NODE_COPY) {
            ret = NodeCopyExpand(refObj);
        }

        return ret;
    }
    Copy(src, overwrite) {
        let child = src.Child();
        while (child != null) {
            // if (child.name_ == "funcNum") {
            //     console.log("sdf")
            // }
            let dst = this.Lookup(child.Name(), child.Type());
            if (dst == null) {
                this.AddChild(AstObjectFactory.Build(child));
            } else {
                if (!dst.Copy(child, overwrite)) {
                    return false;
                }
            }
            child = child.Next();
        }

        return true;
    }
    Move(src) {
        return super.Move(src);
    }
    NodeRefExpand(ref) {
        if (ref == null) {
            Logger().Error() << SourceInfo() << "reference node '" << refNodePath_ << "' not exist";
            return false;
        }
        return ref.Move(this);
    }
    NodeCopyExpand(ref) {
        if (ref == null) {
            Logger().Error() << SourceInfo() << "copy node '" << refNodePath_ << "' not exist";
            return false;
        }
        this.nodeType_ = NodeRefType.NODE_NOREF;
        return this.Copy(ref, false);
    }
    Compare(other) {
        let objChild = this.child_;
        while (objChild != null) {
            let baseObj = this.Lookup(objChild.Name(), objChild.Type());
            if (baseObj == null) {
                Logger().Error() << objChild.SourceInfo() << "not in template node: " << other.SourceInfo();
                return false;
            }
            if (objChild.IsNode()) {
                return this.CastFrom(objChild).Compare(this.CastFrom(baseObj));
            }

            objChild = objChild.Next();
        }
        return true;
    }
    InheritIndex() {
        return this.inheritIndex_;
    }
    InheritCount() {
        return this.inheritCount_;
    }
    TemplateSignNum() {
        return this.templateSignNum_;
    }
    SetTemplateSignNum(sigNum) {
        this.templateSignNum_ = sigNum;
    }
    SubClasses() {
        return this.subClasses_;
    }
}

class ConfigTerm extends AstObject {
    constructor(name, value) {
        if (Object.prototype.toString.call(name) == "[object String]") {
            super(name, ObjectType.PARSEROP_CONFTERM, 0);
            this.signNum_ = 0;
            this.child_ = value;
            if (value != null) value.this.SetParent(this);
        }
        else if (Object.prototype.toString.call(value) == "[object Object]" || Object.prototype.toString.call(value) == "[object Null]") {
            super(name.strval, ObjectType.PARSEROP_CONFTERM, 0, name);
            this.signNum_ = 0;
            this.child_ = value;
            if (value != null) value.this.SetParent(this);
        }
        else {
            super(name.name_, ObjectType.PARSEROP_CONFTERM, 0);
            this.signNum_ = 0;
            this.child_ = value;
            if (value != null) value.this.SetParent(this);
            super.AddChild(AstObjectFactory.Build(name.child_));
        }
    }
    //operator<<
    CastFrom(astObject) {
        return astObject;
    }
    Merge(srcObj) {
        if (!srcObj.IsTerm()) {
            Logger().Error() << SourceInfo() << "merge conflict type with " << srcObj.SourceInfo();
            return false;
        }

        let value = srcObj.Child();
        srcObj.Child().Separate();
        this.child_ = null;
        this.AddChild(value);
        return true;
    }
    RefExpand(refObj) {
        if (child_.Type() == ObjectType.PARSEROP_DELETE) {
            this.Separate();
            return true;
        }

        if (child_.Type() != ObjectType.PARSEROP_NODEREF) {
            return true;
        }

        if (refObj == null || !refObj.IsNode() ||
            ConfigNode.CastFrom(refObj).GetNodeType() == NodeRefType.NODE_REF ||
            ConfigNode.CastFrom(refObj).GetNodeType() == NodeRefType.NODE_TEMPLATE ||
            ConfigNode.CastFrom(refObj).GetNodeType() == NodeRefType.NODE_DELETE) {
            Logger().Error() << SourceInfo() << "reference invalid node '" << child_.StringValue() << '\'';
            return false;
        }

        refNode_ = refObj;
        return true;
    }
    Copy(src, overwrite) {
        if (!overwrite) {
            return true;
        }
        if (this.child_.Type() != src.Child().Type() && (!this.child_.IsNumber() || !src.Child().IsNumber())) {
            Logger().Error() << src.SourceInfo() << "overwrite different type with:" << child_.SourceInfo();
            return false;
        }
        return this.child_.Copy(src.Child(), overwrite);
    }
    Move(src) {
        return this.child_.Move(src.Child());
    }
    RefNode() {
        return this.refNode_;
    }
    SetSigNum(sigNum) {
        this.signNum_ = sigNum;
    }
    SigNum() {
        return this.signNum_;
    }
}
class ConfigArray extends AstObject {
    constructor(array) {
        if (Object.prototype.toString.call(array) == "[object Object]") {
            super("", ObjectType.PARSEROP_ARRAY, 0, array);//bindToken
            this.arrayType_ = 0;
            this.arraySize_ = 0;
            if (array.type == undefined) {
                let child = array.child_;
                while (child != null) {
                    super.AddChild(AstObjectFactory.Build(child));
                    child = child.Next();
                }
                this.arraySize_ = array.arraySize_;
                this.arrayType_ = array.arrayType_;
            }
        }
        else {
            super("", ObjectType.PARSEROP_ARRAY, 0),
                this.arrayType_ = 0;
            this.arraySize_ = 0;
        }

    }
    AddChild(childObj) {
        if (super.AddChild(childObj)) {
            this.arraySize_++;
            this.arrayType_ = this.arrayType_ > childObj.Type() ? this.arrayType_ : childObj.Type();
            return true;
        } else {
            return false;
        }
    }
    Merge(srcObj) {
        if (!srcObj.IsArray()) {
            Logger().Error() << SourceInfo() << "merge conflict type with " << srcObj.SourceInfo();
            return false;
        }

        let value = srcObj.Child();
        value.Separate();
        this.child_ = value;
        return true;
    }
    Copy(src, overwrite) {
        if (!overwrite) {
            return true;
        }
        let array = ConfigArray.CastFrom(src);
        this.child_ = null;
        let t = array.child_;
        while (t != null) {
            AddChild(AstObjectFactory.Build(t));
        }
        return true;
    }
    CastFrom(astObject) {
        return astObject;
    }
    ArraySize() {
        return this.arraySize_;
    }
    ArrayType() {
        return this.arrayType_;
    }
}
class AstObjectFactory { }
AstObjectFactory.Build = function (object) {
    switch (object.Type()) {
        case ObjectType.PARSEROP_CONFNODE:
            return new ConfigNode(object);
        case ObjectType.PARSEROP_CONFTERM:
            return new ConfigTerm(object);
        case ObjectType.PARSEROP_ARRAY:
            return new ConfigArray(object);
        default:
            return new AstObject(object);
    }
}
class Ast {
    constructor(astRoot) {
        this.astRoot_ = astRoot;
        this.redefineChecked_ = false;
    }
    setw(l) {
        let ret = "";
        for (let i = 0; i < l; i++)ret += " ";
        return ret;
    }
    Dump(prefix) {
        NapiLog.logError("Dump ", prefix, " AST:");
        this.WalkForward(this.astRoot_, (current, walkDepth) => {
            // if (current.name_ == "funcNum") {
            //     console.log("sdf");
            //     this.getpath(current)
            // }

            switch (current.type_) {
                case ObjectType.PARSEROP_UINT8:
                case ObjectType.PARSEROP_UINT16:
                case ObjectType.PARSEROP_UINT32:
                case ObjectType.PARSEROP_UINT64:
                    NapiLog.logInfo(this.setw(walkDepth * 4) + "[" + current.integerValue_ + "]"); break;
                case ObjectType.PARSEROP_STRING://5
                    NapiLog.logInfo(this.setw(walkDepth * 4) + current.stringValue_); break;
                case ObjectType.PARSEROP_CONFNODE://6目录
                    if (current.name_ == "blockSize") {
                        current.name_ = "blockSize"
                    }
                    NapiLog.logInfo(this.setw(walkDepth * 4) + current.name_ + " :"); break;
                case ObjectType.PARSEROP_CONFTERM://7属性名字
                    if (current.name_ == "funcNum") {
                        current.name_ = "funcNum"
                    }
                    NapiLog.logInfo(this.setw(walkDepth * 4) + current.name_ + " = "); break;
                case ObjectType.PARSEROP_ARRAY:
                    NapiLog.logInfo(this.setw(walkDepth * 4) + current.name_); break;
                case ObjectType.PARSEROP_NODEREF:
                    NapiLog.logInfo(this.setw(walkDepth * 4) + current.name_); break;
                case ObjectType.PARSEROP_DELETE:
                    NapiLog.logInfo(this.setw(walkDepth * 4) + current.name_); break;
            }

            return HcsErrorNo.NOERR;
        });
    }
    GetAstRoot() {
        return this.astRoot_;
    }
    Merge(astList) {
        if (!this.RedefineCheck()) {
            return false;
        }
        for (let i = 0; i < astList.length; i++) {
            let astIt = astList[i];
            if (!astIt.RedefineCheck()) {
                return false;
            }
            if (this.astRoot_ == null) {
                this.astRoot_ = astIt.GetAstRoot();
                continue;
            }
            // this.Dump("merge this");
            // astIt.Dump("tobe merge");
            if (!this.astRoot_.Merge(astIt.astRoot_)) {
                return false;
            }
            // astIt.Dump("merged");
        }
        return true;
    }
    getchild(node, name) {
        let p = node.child_
        while (p != null) {
            if (p.name_ == name) return p;
            p = p.next_;
        }
        return null;
    }
    getpath(node) {
        console.log("----path start----")
        let p = node
        while (p != null) {
            console.log(p.name_)
            p = p.parent_
        }
        console.log("----path end----")
    }
    Expand() {
        let n1, n2;

        if (!this.RedefineCheck()) {
            return false;
        }

        if (this.astRoot_.Lookup("module", ObjectType.PARSEROP_CONFTERM) == null) {
            Logger().Error() << astRoot_.SourceInfo() << "miss 'module' attribute under root node";
            return false;
        }

        // n1=this.getchild(this.astRoot_,"wlan_config")
        // n2=this.getchild(n1,"hisi")
        // n2=this.getchild(n2,"device0")
        // n2=this.getchild(n2,"bus")
        // n2=this.getchild(n2,"funcNum")
        if (!this.NodeExpand()) {
            return false;
        }
        // n1=this.getchild(this.astRoot_,"wlan_config")
        // n2=this.getchild(n1,"hisi")
        // n2=this.getchild(n2,"device0")
        // n2=this.getchild(n2,"bus")
        // n2=this.getchild(n2,"funcNum")

        if (!this.InheritExpand()) {
            return false;
        };
        this.Dump("expanded");
        return true;
    }



    NodeExpand() {
        return this.WalkBackward(this.astRoot_, (current, walkDepth) => {
            // (void) walkDepth;
            // if(current.name_=="funcNum")
            // if (current.name_ == "hisi") {
            //     console.log("sdf");
            //     this.getpath(current)
            // }
            if (current.IsNode()) {
                let node = current;//ConfigNode::CastFrom(current);
                if (node.GetNodeType() == NodeRefType.NODE_DELETE) {
                    current.Remove();
                    return HcsErrorNo.NOERR;
                }
                if (node.GetNodeType() != NodeRefType.NODE_REF && node.GetNodeType() != NodeRefType.NODE_COPY) {
                    return HcsErrorNo.NOERR;
                }
                // current node maybe deleted after reference expand, never use it after this
                let ret = node.RefExpand(this.Lookup(current, node.GetRefPath()));
                if (!ret) {
                    return HcsErrorNo.EFAIL;
                }
            } else if (current.IsTerm()) {
                let ref;
                if (current.child_.Type() == ObjectType.PARSEROP_DELETE) {
                    current.Remove();
                    return HcsErrorNo.NOERR;
                }
                if (current.child_.Type() == ObjectType.PARSEROP_NODEREF) {
                    ref = Lookup(current, current.child_.StringValue());
                    if (!current.RefExpand(ref)) {
                        return HcsErrorNo.EFAIL;
                    }
                }
            }
            // if (current.name_ == "funcNum") {
            //     console.log("sdf");
            //     this.getpath(current)
            // }
            return HcsErrorNo.NOERR;
        });
    }
    WalkBackward(startObject, callback) {
        let backWalkObj = startObject;
        let next = null;
        let parent = null;
        let walkDepth = 0;
        let preVisited = false;

        while (backWalkObj != null) {
            // if (backWalkObj.name_ == "hisi") {
            //     console.log("sdf");
            // }
            if (backWalkObj.child_ == null || preVisited) {
                next = backWalkObj.next_;
                parent = backWalkObj.Parent();
                /* can safe delete current in callback */
                if (callback(backWalkObj, walkDepth) != HcsErrorNo.NOERR) {
                    return false;
                }
            } else {
                if (backWalkObj.child_) {
                    walkDepth++;
                    backWalkObj = backWalkObj.child_;
                    continue;
                }
            }
            if (backWalkObj == startObject) {
                break;
            }

            if (next != null) {
                backWalkObj = next;
                preVisited = false;
            } else {
                backWalkObj = parent;
                preVisited = true;
                walkDepth--;
            }
        }

        return true;
    }
    InheritExpand() {
        return this.WalkForward(this.astRoot_, (current, ii) => {
            if (current.IsNode()) {
                let node = current;//ConfigNode::CastFrom(current);
                if (node.GetNodeType() != NodeRefType.NODE_INHERIT) {
                    return HcsErrorNo.NOERR;
                }
                let inherit = this.Lookup(current, node.GetRefPath());
                if (!node.InheritExpand(inherit)) {
                    return HcsErrorNo.EFAIL;
                }
            }

            return HcsErrorNo.NOERR;
        });
    }
    RedefineCheck() {
        if (this.redefineChecked_) {
            return true;
        }

        let ret = this.WalkForward(this.astRoot_, (current, ii) => {
            if (current.IsNode() && current.HasDuplicateChild()) {
                return HcsErrorNo.EFAIL;
            }

            return HcsErrorNo.NOERR;
        });

        this.redefineChecked_ = true;
        return ret;
    }
    WalkForward(startObject, callback) {
        let forwardWalkObj = startObject;
        let walkDepth = 0;
        let preVisited = false;

        while (forwardWalkObj != null) {
            if (!preVisited) {
                let ret = callback(forwardWalkObj, walkDepth);
                if (ret && ret != HcsErrorNo.EASTWALKBREAK) {
                    return false;
                } else if (ret != HcsErrorNo.EASTWALKBREAK && forwardWalkObj.child_ != null) {
                    /* when callback return HcsErrorNo.EASTWALKBREAK, not walk current's child */
                    walkDepth++;
                    forwardWalkObj = forwardWalkObj.child_;
                    continue;
                }
            }
            if (forwardWalkObj == startObject) {
                break;
            }

            if (forwardWalkObj.next_ != null) {
                forwardWalkObj = forwardWalkObj.next_;
                preVisited = false;
            } else {
                forwardWalkObj = forwardWalkObj.Parent();
                preVisited = true;
                walkDepth--;
            }
        }

        return true;
    }
    Lookup(startObj, path) {
        if (path.indexOf('.') < 0) {
            return startObj.parent_.Lookup(path, 0);
        }

        let splitPath = this.SplitNodePath(path, '.');
        // if (splitPath.front() != astRoot_.name_) {
        //     Logger().Error() << "lookup ast with invalid path, which not begin with 'root': " << path;
        //     return null;
        // }
        // splitPath.erase(splitPath.begin());

        // std:: shared_ptr < AstObject > target = astRoot_;
        // std:: shared_ptr < AstObject > child = astRoot_.Child();
        // for (auto & it : splitPath) {
        //     // look up children
        //     target = target.Lookup(it);
        //     if (target == null) {
        //         break;
        //     }
        // }

        // return target;
    }
    SplitNodePath(path, separator) {
        let splitList = path.split(separator)
        return splitList;
    }
}

module.exports = {
    AstObject,
    ConfigNode,
    ConfigTerm,
    ConfigArray,
    NodeRefType,
    ObjectType,
    Ast
}
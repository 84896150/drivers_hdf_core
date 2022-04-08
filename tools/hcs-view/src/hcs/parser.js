// const path = require("path")
const { NapiLog } = require("./NapiLog")
const { Lexer, TokenType, CODE } = require("./lexer")
const { AstObject, ConfigNode, ConfigTerm, ConfigArray, NodeRefType, ObjectType, Ast } = require("./ast")

function COPY(obj) {
    return JSON.parse(JSON.stringify(obj));
}
class Parser {
    constructor() {
        this.lexer_ = new Lexer();
        this.current_ = {}
        this.srcQueue_ = []
        this.astList = {}
    }
    Parse(fn) {
        if (fn in this.astList) {
            return this.astList[fn]
        }
        if (this.srcQueue_.indexOf(fn) == -1) this.srcQueue_.push(fn)

        while (this.srcQueue_.length > 0) {
            let includeList = []
            let oneAst = this.ParseOne(this.srcQueue_[0], includeList)
            if (oneAst == null) {
                return false;
            }

            this.astList[fn] = {
                ast: oneAst,
                include: includeList
            };
            this.srcQueue_.shift();
            this.srcQueue_ = this.srcQueue_.concat(includeList)
        }
        return this.astList;
        //todo
        astList.push(astList.shift());
        this.ast_ = astList.shift();

        if (!this.ast_.Merge(astList)) {
            NapiLog.logError("failed to merge ast");
            return false;
        }

        if (astList.length > 0) {
            //this.ast_.Dump("merged");
        }
        if (this.ast_.GetAstRoot() == null) {
            NapiLog.logError(fn + ": Empty hcs file");
            return false;
        }

        if (!this.ast_.Expand()) {
            return false;
        }
        // this.ast_.Dump("expanded");
        return astList[0]
    }
    ConvertAbsPath(includeBy, includePath) {
        if (navigator.userAgent.toLowerCase().indexOf("win") > -1) {//windows
            if (includePath[1] != ':') {
                let currentSrc = includeBy.substring(0, includeBy.lastIndexOf("\\") + 1)
                includePath = currentSrc + includePath.replace(/\//g,"\\");
            }
        }
        else {//mac,linux
            if (includePath[0] != '/') {
                let currentSrc = includeBy.substring(0, includeBy.lastIndexOf("/") + 1)
                includePath = currentSrc + includePath
            }
        }
        return includePath
    }
    ProcessInclude(includeList) {
        do {
            if (!this.lexer_.Lex(this.current_) || this.current_.type != TokenType.STRING) {
                Logger().Error() << lexer_ << "syntax error, expect include path after ’#include‘";
                return false;
            }

            let includePath = this.current_.strval;
            if (includePath.length <= 0) {
                Logger().Error() << lexer_ << "include invalid file: \'" << includePath << '\'';
                return false;
            }
            includePath = this.ConvertAbsPath(this.srcQueue_[0], includePath);

            includeList.push(includePath);

            if (!this.lexer_.Lex(this.current_)) {
                return false;
            }

            if (this.current_.type == TokenType.INCLUDE) {
                continue;
            }

            break;
        } while (true);

        return true;
    }
    ParseOne(src, includeList) {
        if (!this.lexer_.Initialize(src)) {
            return null;
        }

        if (!this.lexer_.Lex(this.current_)) {
            return null;
        }

        if (this.current_.type == TokenType.INCLUDE && !this.ProcessInclude(includeList)) {
            return null;
        }

        let rootNode = null;
        if (this.current_.type == TokenType.ROOT) {
            let preToken = COPY(this.current_);
            preToken.type = TokenType.LITERAL;
            preToken.strval = "root";
            rootNode = this.ParseNode(preToken);
            if (rootNode == null) {
                return null;
            }
        } else if (this.current_.type != TokenType.EOF) {
            NapiLog.logError("syntax error, expect root node of end of file")
            return null;
        }

        if (!this.lexer_.Lex(this.current_) || this.current_.type != TokenType.EOF) {
            NapiLog.logError("syntax error, expect EOF")
            return null;
        }

        let oneAst = new Ast(rootNode);
        // oneAst.Dump(*lexer_.GetSourceName());

        return oneAst;
    }

    ParseNode(name, bracesStart) {
        /* bracesStart： if true, current is '{' , else need to read next token and check with '}' */
        if (!bracesStart) {
            if (!this.lexer_.Lex(this.current_) || this.current_.type != CODE('{')) {
                NapiLog.logError("syntax error, node miss '{'")
                return null;
            }
        }

        let node = new ConfigNode(name, NodeRefType.NODE_NOREF, "");
        let child;
        while (this.lexer_.Lex(this.current_) && this.current_.type != 125) {
            switch (this.current_.type) {
                case TokenType.TEMPLATE:
                    child = this.ParseTemplate();
                    break;
                case TokenType.LITERAL:
                    child = this.ParseNodeAndTerm();
                    break;
                default:
                    NapiLog.logError("syntax error, except '}' or TEMPLATE or LITERAL for node '" + name.strval + '\'')
                    return null;
            }
            if (child == null) {
                return null;
            }

            node.AddChild(child);
        }

        if (this.current_.type != CODE('}')) {
            Logger().Error() << lexer_ << "syntax error, node miss '}'";
            return null;
        }
        return node;
    }

    ParseNodeAndTerm() {
        let name = COPY(this.current_);
        if (!this.lexer_.Lex(this.current_)) {
            Logger().Error() << lexer_ << "syntax error, broken term or node";
            return null;
        }

        switch (this.current_.type) {
            case CODE('='):
                return this.ParseTerm(name);
            case CODE('{'):
                return this.ParseNode(name, true);
            case CODE(':'):
                if (this.lexer_.Lex(this.current_)) {
                    return this.ParseNodeWithRef(name);
                }
                NapiLog.logError("syntax error, unknown node reference type");
                break;
            default:
                NapiLog.logError("syntax error, except '=' or '{' or ':'");
                break;
        }

        return null;
    }
    ParseArray() {
        let array = new ConfigArray(this.current_);
        let arrayType = 0;

        while (this.lexer_.Lex(this.current_) && this.current_.type != CODE(']')) {
            if (this.current_.type == TokenType.STRING) {
                array.AddChild(new AstObject("", ObjectType.PARSEROP_STRING, this.current_.strval, this.current_));
            } else if (this.current_.type == TokenType.NUMBER) {
                array.AddChild(new AstObject("", ObjectType.PARSEROP_UINT64, this.current_.numval, this.current_, this.current_.jinzhi));
            } else {
                Logger().Error() << lexer_ << "syntax error, except STRING or NUMBER in array";
                return nullptr;
            }

            if (arrayType == 0) {
                arrayType = this.current_.type;
            } else if (arrayType != this.current_.type) {
                Logger().Error() << lexer_ << "syntax error, not allow mix type array";
                return null;
            }

            if (this.lexer_.Lex(this.current_)) {
                if (this.current_.type == CODE(',')) {
                    continue;
                } else if (this.current_.type == CODE(']')) {
                    break;
                } else {
                    Logger().Error() << lexer_ << "syntax error, except ',' or ']'";
                    return null;
                }
            }
            return null;//todo return std::shared_ptr<AstObject>();
        }

        if (this.current_.type != CODE(']')) {
            Logger().Error() << lexer_ << "syntax error, miss ']' at end of array";
            return null;
        }

        return array;
    }
    ParseTerm(name) {
        if (!this.lexer_.Lex(this.current_)) {
            NapiLog.logError("syntax error, miss value of config term");
            return null;
        }
        let term = new ConfigTerm(name, null)
        // if(name.strval=="blockSize")
        // {
        //     console.log(name)
        // }
        switch (this.current_.type) {
            case TokenType.BOOL:
                term.AddChild(new AstObject("", ObjectType.PARSEROP_BOOL, this.current_.numval, this.current_));
                break;
            case TokenType.STRING:
                term.AddChild(new AstObject("", ObjectType.PARSEROP_STRING, this.current_.strval, this.current_));
                break;
            case TokenType.NUMBER:
                term.AddChild(new AstObject("", ObjectType.PARSEROP_UINT64, this.current_.numval, this.current_, this.current_.jinzhi));
                break;
            case CODE('['): {
                let list = this.ParseArray();
                if (list == null) {
                    return null;
                } else {
                    term.AddChild(list);
                }
                break;
            }
            case CODE('&'):
                if (!this.lexer_.Lex(this.current_) || (this.current_.type != TokenType.LITERAL && this.current_.type != TokenType.REF_PATH)) {
                    NapiLog.logError("syntax error, invalid config term definition");
                    return null;
                }
                term.AddChild(new AstObject("", ObjectType.PARSEROP_NODEREF, this.current_.strval, this.current_));
                break;
            case TokenType.DELETE:
                term.AddChild(new AstObject("", ObjectType.PARSEROP_DELETE, this.current_.strval, this.current_));
                break;
            default:
                NapiLog.logError("syntax error, invalid config term definition");
                return null;
        }

        if (!this.lexer_.Lex(this.current_) || this.current_.type != CODE(';')) {
            NapiLog.logError("syntax error, miss ';'");
            return null;
        }

        return term;
    }
    ParseTemplate() {
        if (!this.lexer_.Lex(this.current_) || this.current_.type != TokenType.LITERAL) {
            NapiLog.logError("syntax error, template miss name");
            return null;
        }

        let name = COPY(this.current_);
        let node = this.ParseNode(name, false);
        if (node == null) {
            return node;
        }

        node.SetNodeType(NodeRefType.NODE_TEMPLATE);
        return node;
    }

    ParseNodeRef(name) {
        if (!this.lexer_.Lex(this.current_) || (this.current_.type != TokenType.LITERAL && this.current_.type != TokenType.REF_PATH)) {
            Logger().Error() << lexer_ << "syntax error, miss node reference path";
            return nullptr;
        }
        let refPath = this.current_.strval;
        let node = this.ParseNode(name);
        if (node == null) {
            return null;
        }

        let configNode = node;
        configNode.SetNodeType(NodeRefType.NODE_REF);
        configNode.SetRefPath(refPath);
        return node;
    }
    ParseNodeDelete(name) {
        let node = this.ParseNode(name);
        if (node == null) {
            return null;
        }

        node.SetNodeType(NodeRefType.NODE_DELETE);
        return node;
    }
    ParseNodeCopy(name) {
        let nodePath = this.current_.strval;

        let node = this.ParseNode(name);
        if (node == null) {
            return null;
        }

        let nodeCopy = node;
        nodeCopy.SetNodeType(NodeRefType.NODE_COPY);
        nodeCopy.SetRefPath(nodePath);

        return node;
    }
    ParseNodeWithRef(name) {
        switch (this.current_.type) {
            case TokenType.REF_PATH:
            case TokenType.LITERAL:
                return this.ParseNodeCopy(name);
            case CODE('&'):
                return this.ParseNodeRef(name);
            case TokenType.DELETE:
                return this.ParseNodeDelete(name);
            case CODE(':'):
                return this.ParseNodeInherit(name);
            default:
                NapiLog.logError("syntax error, unknown node type");
                break;
        }

        return null;
    }

    ParseNodeInherit(name) {
        if (!this.lexer_.Lex(this.current_) || (this.current_.type != TokenType.LITERAL && this.current_.type != TokenType.REF_PATH)) {
            NapiLog.logError("syntax error, miss node inherit path");
            return null;
        }

        let inheritPath = this.current_.strval;

        let node = this.ParseNode(name);
        if (node == null) {
            return null;
        }

        node.SetNodeType(NodeRefType.NODE_INHERIT);
        node.SetRefPath(inheritPath);
        return node;
    }
}

module.exports = {
    Parser
}
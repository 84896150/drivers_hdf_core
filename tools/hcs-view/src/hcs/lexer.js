const { XMessage } = require("../message/XMessage");
const { NapiLog } = require("./NapiLog")

function CODE(s) {
    return s.charCodeAt(0)
}

function IsSpace(c) {
    return c == CODE(' ') || c == CODE('\t') || c == CODE('\r');
}

function isalpha(c) {
    if (CODE('a') <= c[0] && c[0] <= CODE('z')) return true;
    if (CODE('A') <= c[0] && c[0] <= CODE('Z')) return true;
    return false;
}

function isalnum(c) {
    return isalpha(c) || IsNum(c)
}

function IsNum(c) {
    return CODE('0') <= c[0] && c[0] <= CODE('9')
}

function TOSTR(c) {
    return String.fromCharCode(c[0])
}

class TokenType { }
TokenType.NUMBER = 256;
TokenType.TEMPLATE = 257;
TokenType.LITERAL = 258;
TokenType.STRING = 259;
TokenType.REF_PATH = 260;
TokenType.FILE_PATH = 261;
TokenType.ROOT = 262;
TokenType.INCLUDE = 263;
TokenType.DELETE = 264;
TokenType.BOOL = 265;
TokenType.EOF = -1

class Lexer {
    constructor() {
        this.keyWords_ = {
            "#include": TokenType.INCLUDE,
            "root": TokenType.ROOT,
            "delete": TokenType.DELETE,
            "template": TokenType.TEMPLATE,
        };
    }

    Initialize(sourceName) {
        this.srcName_ = sourceName

        // this.data_ = fs.readFileSync(sourceName)
        if (!(sourceName in Lexer.FILE_AND_DATA)) {
            XMessage.gi().Send("getfiledata", sourceName);
            return false;
        }
        this.data_ = Lexer.FILE_AND_DATA[sourceName]
        console.log("-----------------data start---------------------")
        console.log(sourceName)
        console.log(Lexer.FILE_AND_DATA[sourceName])
        console.log("-----------------data end---------------------")

        this.bufferStart_ = 0;
        this.bufferEnd_ = this.data_.length - 1;
        this.lineno_ = 1;
        this.lineLoc_ = 1;

        return true;
    }

    LexInclude(token) {
        this.ConsumeChar();
        this.LexFromLiteral(token);
        if (token.strval != "include") {
            return false;
        }

        token.type = TokenType.INCLUDE;
        return true;
    }

    Lex(token) {
        let c = [];
        this.InitToken(token);
        do {
            if (!this.PeekChar(c, true)) {
                token.type = TokenType.EOF;
                return true;
            }
            if (c[0] == CODE('#')) {
                return this.LexInclude(token)//todo
            }
            if (isalpha(c)) {
                this.LexFromLiteral(token);
                return true;
            }

            if (IsNum(c)) {
                return this.LexFromNumber(token);
            }
            switch (c[0]) {
                case CODE('/'):
                    if (!this.ProcessComment()) {
                        return false;
                    }
                    continue;
                case CODE(';'): /* fall-through */
                case CODE(','): /* fall-through */
                case CODE('['): /* fall-through */
                case CODE(']'): /* fall-through */
                case CODE('{'): /* fall-through */
                case CODE('}'): /* fall-through */
                case CODE('='): /* fall-through */
                case CODE('&'): /* fall-through */
                case CODE(':'):
                    this.ConsumeChar();
                    token.type = c[0];
                    token.lineNo = this.lineno_;
                    break;
                case CODE('"'):
                    return this.LexFromString(token);
                case CODE('+'): /* fall-through */
                case CODE('-'):
                    return LexFromNumber(token);
                case -1:
                    token.type = -1;
                    break;
                default:
                    NapiLog.logError("can not recognized character '" + TOSTR(c) + "'" + this.bufferStart_);
                    return false;
            }
            break;
        } while (true);

        return true;
    }

    PeekChar(c, skipSpace) {
        if (skipSpace) {
            while (this.bufferStart_ <= this.bufferEnd_ && (IsSpace(this.data_[this.bufferStart_]) || this.data_[this.bufferStart_] == CODE('\n'))) {
                this.lineLoc_++;
                if (this.data_[this.bufferStart_] == CODE('\n')) {
                    this.lineLoc_ = 0;
                    this.lineno_++;
                }
                this.bufferStart_++;
            }
        }

        if (this.bufferStart_ > this.bufferEnd_) {
            return false;
        }
        c[0] = this.data_[this.bufferStart_];
        return true;
    }

    InitToken(token) {
        token.type = 0;
        token.numval = 0;
        token.strval = "";
        token.src = this.srcName_;
        token.lineNo = this.lineno_;
    }

    LexFromLiteral(token) {
        let value = "";
        let c = [];

        while (this.PeekChar(c, false) && !IsSpace(c[0])) {
            if (!isalnum(c) && c != CODE('_') && c != CODE('.') && c != CODE('\\')) {
                break;
            }
            value += TOSTR(c);
            this.ConsumeChar();
        }

        do {
            if (value == "true") {
                token.type = TokenType.BOOL;
                token.numval = 1;
                break;
            } else if (value == "false") {
                token.type = TokenType.BOOL;
                token.numval = 0;
                break;
            }

            if (value in this.keyWords_) {
                token.type = this.keyWords_[value];
                break;
            }

            if (value.indexOf(".") >= 0) {
                token.type = TokenType.REF_PATH;
            } else {
                token.type = TokenType.LITERAL;
            }
        } while (false);


        token.strval = value;
        token.lineNo = this.lineno_;
    }
    GetRawChar() {
        // if (!FillBuffer()) {this.bufferStart_>this.bufferEnd_
        //     return TokenType.EOF;
        // }
        this.lineLoc_++;
        let ret = this.data_[this.bufferStart_];
        this.bufferStart_++;
        return ret
    }
    GetChar(c, skipSpace) {
        let chr = this.GetRawChar();
        if (skipSpace) {
            while (IsSpace(chr)) {
                chr = this.GetRawChar();
            }
        }

        if (chr == CODE('\n')) {
            this.lineno_++;
            this.lineLoc_ = 0;
        }
        c[0] = chr;
        return true;
    }
    ConsumeChar() {
        let c = []
        this.GetChar(c, false);
    }
    LexFromNumber(token) {
        let value = "";
        let c = [];
        let v = 0;
        let errno = 0;
        let jinzhi = 10;

        this.GetChar(c, false);
        switch (c[0]) {
            case CODE('0'):
                if (!this.PeekChar(c, true)) {
                    break;
                }
                if (IsNum(c)) { // Octal number
                    while (this.PeekChar(c) && IsNum(c)) {
                        this.ConsumeChar();
                        value += TOSTR(c);
                    }
                    v = parseInt(value, 8)
                    jinzhi = 8;
                    break;
                }
                switch (c[0]) {
                    case CODE('x'): // fall-through
                    case CODE('X'): // hex number
                        this.ConsumeChar();
                        while (this.PeekChar(c, false) && (IsNum(c) || (c[0] >= CODE('a') && c[0] <= CODE('f'))
                            || (c[0] >= CODE('A') && c[0] <= CODE('F')))) {
                            value += TOSTR(c);
                            this.ConsumeChar();
                        }
                        v = parseInt(value, 16)
                        jinzhi = 16;
                        break;
                    case CODE('b'): // binary number
                        this.ConsumeChar();
                        while (this.PeekChar(c, false) && (c[0] == CODE('0') || c[0] == CODE('1'))) {
                            value += TOSTR(c);
                            this.ConsumeChar();
                        }
                        v = parseInt(value, 2)
                        jinzhi = 2
                        break;
                    default: ; // fall-through
                }
                break;
            case CODE('+'): // fall-through
            case CODE('-'): // fall-through, signed decimal number
            default: // unsigned decimal number
                value += TOSTR(c);
                while (this.PeekChar(c, true) && IsNum(c)) {
                    this.ConsumeChar();
                    value += TOSTR(c);
                }
                v = parseInt(value, 10)
                jinzhi = 10
                break;
        }

        if (errno != 0) {
            NapiLog.logError("illegal number: " + value)
            return false;
        }
        token.type = TokenType.NUMBER;
        token.numval = v;
        token.lineNo = this.lineno_;
        token.jinzhi = jinzhi;
        return true;
    }
    LexFromString(token) {
        let c = [];
        this.GetChar(c, false); // skip first '"'
        let value = "";
        while (this.GetChar(c, false) && c[0] != CODE('"')) {
            value += TOSTR(c);
        }

        if (c != CODE('"')) {
            NapiLog.logError("unterminated string");
            return false;
        }
        token.type = TokenType.STRING;
        token.strval = value;
        token.lineNo = this.lineno_;
        return true;
    }
    ProcessComment() {
        let c = [];
        this.ConsumeChar();// skip first '/'
        if (!this.GetChar(c, true)) {
            NapiLog.logError("unterminated comment");
            return false;
        }

        if (c[0] == CODE('/')) {
            while (c[0] != CODE('\n') && this.GetChar(c, true)) {
            }
            if (c[0] != CODE('\n') && c[0] != TokenType.EOF) {
                NapiLog.logError("unterminated signal line comment");
                return false;
            }
        } else if (c[0] == CODE('*')) {
            while (this.GetChar(c, true)) {
                if (c[0] == CODE('*') && this.GetChar(c, true) && c[0] == CODE('/')) {
                    return true;
                }
            }
            if (c[0] != CODE('/')) {
                NapiLog.logError("unterminated multi-line comment");
                return false;
            }
        } else {
            NapiLog.logError("invalid character");
            return false;
        }

        return true;
    }
}
Lexer.FILE_AND_DATA = {};

module.exports = {
    Lexer,
    TokenType,
    CODE
}
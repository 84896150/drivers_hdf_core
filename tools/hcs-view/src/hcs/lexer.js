/*
* Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd. 
* Licensed under the Apache License, Version 2.0 (the "License"); 
* you may not use this file except in compliance with the License. 
* You may obtain a copy of the License at 
*
* http:// www.apache.org/licenses/LICENSE-2.0 
*
* Unless required by applicable law or agreed to in writing, software 
* distributed under the License is distributed on an "AS IS" BASIS, 
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
* See the License for the specific language governing permissions and 
* limitations under the License. 
*/
const { XMessage } = require("../message/XMessage");
const { NapiLog } = require("./NapiLog")

function CODE(s) {
    return s.charCodeAt(0)
}

function isSpace(c) {
    return c == CODE(' ') || c == CODE('\t') || c == CODE('\r');
}

function isalpha(c) {
    if (CODE('a') <= c[0] && c[0] <= CODE('z')) return true;
    if (CODE('A') <= c[0] && c[0] <= CODE('Z')) return true;
    return false;
}

function isalnum(c) {
    return isalpha(c) || isNum(c)
}

function isNum(c) {
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

    initialize(sourceName) {
        this.srcName_ = sourceName
        if (!(sourceName in Lexer.FILE_AND_DATA)) {
            XMessage.gi().send("getfiledata", sourceName);
            return false;
        }
        this.data_ = Lexer.FILE_AND_DATA[sourceName]
        NapiLog.logError("------------data start----------------");        
        NapiLog.logError(sourceName);
        NapiLog.logError(Lexer.FILE_AND_DATA[sourceName])
        NapiLog.logError("------------data end------------------");

        this.bufferStart_ = 0;
        this.bufferEnd_ = this.data_.length - 1;
        this.lineno_ = 1;
        this.lineLoc_ = 1;

        return true;
    }

    lexInclude(token) {
        this.consumeChar();
        this.lexFromLiteral(token);
        if (token.strval != "include") {
            return false;
        }

        token.type = TokenType.INCLUDE;
        return true;
    }


    lex(token) {
        let c = [];
        this.InitToken(token);
        do {
            if (!this.peekChar(c, true)) {
                token.type = TokenType.EOF;
                return true;
            }
            if (c[0] == CODE('#')) {
                return this.lexInclude(token)//todo
            }
            if (isalpha(c)) {
                this.lexFromLiteral(token);
                return true;
            }

            if (isNum(c)) {
                return this.lexFromNumber(token);
            }

            
            switch (c[0]) {
                case CODE('/'):
                    if (!this.processComment()) {
                        return false;
                    }
                    continue;
                case CODE(';'): 
                case CODE(','): 
                case CODE('['): 
                case CODE(']'): 
                case CODE('{'): 
                case CODE('}'): 
                case CODE('='): 
                case CODE('&'): 
                case CODE(':'):
                    this.consumeChar();
                    token.type = c[0];
                    token.lineNo = this.lineno_;
                    break;
                case CODE('"'):
                    return this.lexFromString(token);
                case CODE('+'): 
                case CODE('-'):
                    return lexFromNumber(token);
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

    peekChar(c, skipSpace) {
        if (skipSpace) {
            while (this.bufferStart_ <= this.bufferEnd_ && (isSpace(this.data_[this.bufferStart_]) ||
              this.data_[this.bufferStart_] == CODE('\n'))) {
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

    lexFromLiteral(token) {
        let value = "";
        let c = [];

        while (this.peekChar(c, false) && !isSpace(c[0])) {
            if (!isalnum(c) && c != CODE('_') && c != CODE('.') && c != CODE('\\')) {
                break;
            }
            value += TOSTR(c);
            this.consumeChar();
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
    getRawChar() {
        // if (!FillBuffer()) {this.bufferStart_>this.bufferEnd_
        //     return TokenType.EOF;
        // }
        this.lineLoc_++;
        let ret = this.data_[this.bufferStart_];
        this.bufferStart_++;
        return ret
    }
    getChar(c, skipSpace) {
        let chr = this.getRawChar();
        if (skipSpace) {
            while (isSpace(chr)) {
                chr = this.getRawChar();
            }
        }

        if (chr == CODE('\n')) {
            this.lineno_++;
            this.lineLoc_ = 0;
        }
        c[0] = chr;
        return true;
    }
    consumeChar() {
        let c = []
        this.getChar(c, false);
    }
    lexFromNumber(token) {
        let value = "";
        let c = [];
        let v = 0;
        let errno = 0;
        let jinzhi = 10;

        this.getChar(c, false);
        switch (c[0]) {
            case CODE('0'):
                if (!this.peekChar(c, true)) {
                    break;
                }
                if (isNum(c)) { // Octal number
                    while (this.peekChar(c) && isNum(c)) {
                        this.consumeChar();
                        value += TOSTR(c);
                    }
                    v = parseInt(value, 8)
                    jinzhi = 8;
                    break;
                }
                switch (c[0]) {
                    case CODE('x'): // fall-through
                    case CODE('X'): // hex number
                        this.consumeChar();
                        while (this.peekChar(c, false) && (isNum(c) || (c[0] >= CODE('a') && c[0] <= CODE('f'))
                            || (c[0] >= CODE('A') && c[0] <= CODE('F')))) {
                            value += TOSTR(c);
                            this.consumeChar();
                        }
                        v = parseInt(value, 16)
                        jinzhi = 16;
                        break;
                    case CODE('b'): // binary number
                        this.consumeChar();
                        while (this.peekChar(c, false) && (c[0] == CODE('0') || c[0] == CODE('1'))) {
                            value += TOSTR(c);
                            this.consumeChar();
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
                while (this.peekChar(c, true) && isNum(c)) {
                    this.consumeChar();
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
    lexFromString(token) {
        let c = [];
        this.getChar(c, false); // skip first '"'
        let value = "";
        while (this.getChar(c, false) && c[0] != CODE('"')) {
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
    processComment() {
        let c = [];
        this.consumeChar();// skip first '/'
        if (!this.getChar(c, true)) {
            NapiLog.logError("unterminated comment");
            return false;
        }

        if (c[0] == CODE('/')) {
            while (c[0] != CODE('\n') && this.getChar(c, true)) {
            }
            if (c[0] != CODE('\n') && c[0] != TokenType.EOF) {
                NapiLog.logError("unterminated signal line comment");
                return false;
            }
        } else if (c[0] == CODE('*')) {
            while (this.getChar(c, true)) {
                if (c[0] == CODE('*') && this.getChar(c, true) && c[0] == CODE('/')) {
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
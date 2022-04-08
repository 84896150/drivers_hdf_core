const { X2DFast } = require("../graphics/X2DFast")

class XSelect {
    constructor(list, default_) {
        this.pm2f_ = X2DFast.gi();
        this.ResetList(list, default_)
        this.open_ = false;
        this.nameColor_ = 0xff20f020;
        this.backgroundColor_ = 0xff804050;
        this.tmpSelect_ = -1;
        this.selectCallback = null;
    }
    ResetList(list, default_) {
        this.list_ = list;
        this.default_ = default_
    }
    Move(x, y, w, h) {
        this.posX_ = x;
        this.posY_ = y;
        this.posW_ = w;
        this.posH_ = h;
        return this;
    }
    Draw() {
        let x = this.posX_
        let y = this.posY_
        let w = this.posW_
        let h = this.posH_

        this.pm2f_.FillRect(x, y, w, h, this.backgroundColor_)
        this.pm2f_.DrawText(this.default_, 18, x, y, 1, 1, 0, -1, -1, this.nameColor_)
        if (this.open_) {
            this.pm2f_.FillRect(x, y + h, w, 20 * this.list_.length, 0xff202020)
            for (let i in this.list_) {
                if (i == this.tmpSelect_) {
                    this.pm2f_.FillRect(x, y + h + i * 20, w, 20, 0x80000080)
                }
                if (this.list_[i] == this.default_) {
                    this.pm2f_.FillRect(x, y + h + i * 20, w, 20, 0x80008000)
                }
                this.pm2f_.DrawText(this.list_[i], 18, x, y + h + i * 20, 1, 1, 0, -1, -1, this.nameColor_)
            }
        }
    }
    IsTouchIn(x, y) {
        if (x < this.posX_) return false;
        if (y < this.posY_) return false;
        if (x > this.posX_ + this.posW_) return false;
        if (y > this.posY_ + this.posH_ + (this.open_ ? 20 * this.list_.length : 0)) return false;
        return true;
    }
    ProcTouch(msg, x, y) {
        let isIn = this.IsTouchIn(x, y);
        switch (msg) {
            case 1:
                if (isIn) {
                    if (!this.open_) this.open_ = true;
                    else {
                        if (this.tmpSelect_ >= 0 && this.tmpSelect_ <= this.list_.length) {
                            if (this.default_ != this.list_[this.tmpSelect_]) {
                                this.default_ = this.list_[this.tmpSelect_];
                                if (this.selectCallback != null) {
                                    this.selectCallback(this.default_)
                                }
                            }
                        }
                        this.open_ = false;
                    }
                }
                break;
            case 2:
                if (this.open_) {
                    this.tmpSelect_ = parseInt((y - this.posY_ - this.posH_) / 20)
                }
                break;
            case 3:
                break;
        }
        return isIn;
    }
    RegistCallback(func) {
        this.selectCallback = func;
    }
}

module.exports = {
    XSelect
}
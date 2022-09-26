/*
 * Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

const { X2DFast } = require("../graphics/X2DFast")

class XButton {
    constructor(x, y, w, h, name) {
        this.pm2f_ = X2DFast.gi();
        this.move(x, y, w, h);
        this.name_ = name;
        this.touchDown_ = false;
        this.clicked_ = false;
        this.disable_ = false;
        this.nameColor_ = 0xffffffff;
        this.backgroundColor_ = 0xff487EB8;
    }

    move(x, y, w, h) {
        this.posX_ = x;
        this.posY_ = y;
        this.posW_ = w;
        this.posH_ = h;
        return this;
    }

    draw() {
        let coloroff = 0;
        if (this.touchDown_) coloroff = 0x00202020;
        this.pm2f_.fillRect(this.posX_, this.posY_, this.posW_, this.posH_, this.backgroundColor_ - coloroff)
        if (this.name_ != undefined && this.name_.length > 0)
            this.pm2f_.drawText(this.name_, 18, this.posX_ + this.posW_ / 2, this.posY_ 
            + this.posH_ / 2 + 2, 1, 1, 0, -2, -2, this.nameColor_ - coloroff);
    }

    isTouchIn(x, y) {
        if (x < this.posX_) return false;
        if (y < this.posY_) return false;
        if (x > this.posX_ + this.posW_) return false;
        if (y > this.posY_ + this.posH_) return false;
        return true;
    }
    procTouch(msg, x, y) {
        let isIn = this.isTouchIn(x, y);
        switch (msg) {
            case 1:
                if (isIn) {
                    this.touchDown_ = true;
                }
                break;
            case 2:
                break;
            case 3:
                if (this.touchDown_ && isIn) {
                    this.clicked_ = true;
                }
                this.touchDown_ = false;
                break;
        }
        return isIn;
    }

    isClicked() {
        if (this.clicked_) {
            this.clicked_ = false;
            return true;
        }
        return false;
    }
}

module.exports = {
    XButton
}
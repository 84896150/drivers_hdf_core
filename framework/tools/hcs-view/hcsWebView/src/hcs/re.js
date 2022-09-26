/*
 * Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

function search(ss, data) {
    ss = replaceAll(ss, "\\.", "\\.")
    let reg = new RegExp(ss);
    let tt = reg.exec(data);
    if (tt == null) return null;
    let ret = { "regs": [] }
    for (let i = 0; i < tt.length; i++) {
        let p = data.indexOf(tt[i]);
        if (tt[i] == null) {
            ret["regs"].push([-1, -1])
        }
        else {
            ret["regs"].push([p, p + tt[i].length])
        }
    }

    return ret;
}

function match(ss, data) {
    let tt = search(ss, data)
    if (tt != null && tt.regs[0][0] == 0) return tt;
    return null;
}

function removeReg(data, reg) {
    return data.substring(0, reg[0]) + data.substring(reg[1], data.length)
}

function getReg(data, reg) {
    return data.substring(reg[0], reg[1])
}

function all(sfrom) {
    return new RegExp(sfrom, "g");
}

function replaceAll(ss, sfrom, sto) {
    return ss.replace(all(sfrom), sto)
}

module.exports = {
    search,
    match,
    removeReg,
    getReg,
    replaceAll,
    all
}
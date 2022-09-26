/*
 * Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

class NapiLog {
    constructor() {
    }
}
NapiLog.LEV_NONE = 0;
NapiLog.LEV_ERROR = 1;
NapiLog.LEV_DEBUG = 2;
NapiLog.LEV_INFO = 3;

const LEV_STR = ["[NON]", "[ERR]", "[DBG]", "[INF]"]
var logLevel = NapiLog.LEV_ERROR;
var logFileName = null;
var logResultMessage = [true, ""]
var errorCallBack = null;

function getDateString() {
    let nowDate = new Date();
    return nowDate.toLocaleString();
}

function saveLog(dateStr, levStr, detail) {
    if (logFileName) {
        let logStr = dateStr + " " + levStr + " " + detail + "\n";
    }
}

NapiLog.init = function (level, fileName) {
    logLevel = level in [NapiLog.LEV_NONE, NapiLog.LEV_ERROR, NapiLog.LEV_DEBUG, NapiLog.LEV_INFO]
        ? level : NapiLog.LEV_ERROR;
    logFileName = fileName ? fileName : "napi_generator.log";
}

function recordLog(lev, ...args) {
    let dataStr = getDateString();
    let detail = args.join(" ");
    saveLog(dataStr, LEV_STR[lev], detail);
    if (lev == NapiLog.LEV_ERROR) {
        logResultMessage = [false, detail];
        if (errorCallBack != null) errorCallBack(detail)
    }
    if (logLevel < lev) return;
    console.log(dataStr, LEV_STR[lev], detail)
}

NapiLog.logError = function (...args) {
    recordLog(NapiLog.LEV_ERROR, args);
}

NapiLog.logDebug = function (...args) {
    recordLog(NapiLog.LEV_DEBUG, args);
}

NapiLog.logInfo = function (...args) {
    recordLog(NapiLog.LEV_INFO, args);
}

NapiLog.getResult = function () {
    return logResultMessage;
}

NapiLog.clearError = function () {
    logResultMessage = [true, ""]
}

NapiLog.registError = function (func) {
    errorCallBack = func;
}

module.exports = {
    NapiLog
}
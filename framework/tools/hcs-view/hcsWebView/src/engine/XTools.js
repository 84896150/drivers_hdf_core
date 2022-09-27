/*
 * Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

export function fAngle(x,y)
{
    return Math.atan2(-y,x)*180/Math.PI
}
export function iDistance(x,y)
{
    return Math.sqrt(x*x+y*y)
}

export var timeMs=0;
export function freshTime()
{
    let t=new Date()
    timeMs=t.getTime()
}
freshTime()
export function TimeMS()
{
    let t=new Date()
    return t.getTime()
}
export function RandInt(min=0,max=100)
{
    return Math.floor(Math.random()*(max-min))+min
}

export function GetURL()
{
    if("undefined" != typeof wx)
    {
        return "https://7465-testegg-19e3c9-1301193145.tcb.qcloud.la/"
    }
    else return ""
}
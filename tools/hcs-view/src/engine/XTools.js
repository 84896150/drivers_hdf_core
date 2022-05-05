export function fAngle(x,y)
{
    return Math.atan2(-y,x)*180/Math.PI
}
export function iDistance(x,y)
{
    return Math.sqrt(x*x+y*y)
}

export var time_ms=0;
export function FreshTime()
{
    let t=new Date()
    time_ms=t.getTime()
}
FreshTime()
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
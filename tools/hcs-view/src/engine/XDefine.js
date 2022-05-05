
export class Scr {
    constructor() {

    }
    static ReSize(w, h) {
        Scr.width, Scr.height = w, h
        if (Scr.keeplogicworh == "width")
            Scr.logich = Scr.logicw * h / w
        else
            Scr.logicw = Scr.logich * w / h
    }
    static SetLogicScreenSize(w, h) {
        if (Scr.logicw == w && Scr.width == w && Scr.logich == h && Scr.height == h) return;
        Scr.logicw = w
        Scr.logich = h
        Scr.width = w
        Scr.height = h
        console.log("SetLogicScreenSize")
        if ("undefined" != typeof wx) {
            var info = wx.getSystemInfoSync();
            Scr.width = info.windowWidth;
            Scr.height = info.windowHeight;
        }
    }
}

Scr.width = 320
Scr.height = 240
Scr.keeplogicworh = "height"
Scr.logicw = 320
Scr.logich = 240
Scr.fps = 60





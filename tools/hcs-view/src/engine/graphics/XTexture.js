import { gl } from "../GLFrame.js"

export class XTexture {
    static gi() {
        if (XTexture.pinstance_ == null) XTexture.pinstance_ = new XTexture();
        return XTexture.pinstance_;
    }
    constructor() {
        this.ximages = [];
        this.all_cuts = {};
        this.tmp_cutid = 0;
        this.ai_cutid = 100;

        this.text_imgs = {};//rid,{mask}
        this.text_idxs = {};//text,{}

        this.text_tmp_rid = this.LoadTexture(1024, 256)
        this.bfirst = true;

        this.text_cvs = document.createElement('canvas');
        this.text_cvs.width = 1024;
        this.text_cvs.height = 256;
        this.text_ctx = this.text_cvs.getContext("2d");
        // ctx.font = "28px Microsoft YaHei"
        this.text_ctx.textBaseline = 'top'
        this.text_ctx.textAlign = 'left'
    }
    static _init_texture_status(tex)
    {
            // gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
            gl.activeTexture(gl.TEXTURE0);
            gl.bindTexture(gl.TEXTURE_2D, tex);
            gl.pixelStorei(gl.UNPACK_PREMULTIPLY_ALPHA_WEBGL, false);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    }
    loadTextureFromImage(path, keepdata = false) {
        if (path == "CUSTOM_TEXTURE_1") {
            var rid = this.ximages.length;

            var texture = gl.createTexture();
            XTexture._init_texture_status(texture);

            let tmp=new Uint8Array([255,255,255,255])
            gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE, tmp)

            this.ximages[rid] = { "stat": 1, "path": path, "tex": texture ,"w":1,"h":1};
            return rid
        }
        else {
            for (let i = 0; i < this.ximages.length; i++) {
                if (this.ximages[i]["path"] == path) {
                    return i;
                }
            }
            var rid = this.ximages.length;
            this.ximages[rid] = { "stat": 0, "path": path, "tex": null };
            var image = new Image();
            image.src = path;//"http://localhost:8910/"+
            image.onload = function () {
                var texture = gl.createTexture();
                XTexture._init_texture_status(texture)

                gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);

                XTexture.pinstance_.ximages[rid].tex = texture;
                XTexture.pinstance_.ximages[rid].img = image;
                XTexture.pinstance_.ximages[rid].stat = 1;
                XTexture.pinstance_.ximages[rid].w = image.width;
                XTexture.pinstance_.ximages[rid].h = image.height;
            };
            return rid;
        }
    }
    TmpCut(rid, x = 0, y = 0, w = -1, h = -1, ww = 1024, hh = 1024) {
        if (this.ximages[rid].stat != 1) return -1;

        if (w == -1) w = ww;
        if (h == -1) h = hh;
        this.all_cuts[this.tmp_cutid] = {
            "rid": rid,
            "x": x, "y": y,
            "w": w, "h": h,
            "u0": x / ww, "v0": y / hh,
            "u1": (x + w) / ww, "v1": y / hh,
            "u2": (x + w) / ww, "v2": (y + h) / hh,
            "u3": x / ww, "v3": (y + h) / hh
        };
        this.tmp_cutid += 1;
        return this.tmp_cutid - 1;
    }
    makeCut(rid, x = 0, y = 0, w = -1, h = -1, ww = -1, hh = -1) {
        if (this.ximages[rid].stat != 1) return -1;

        if (ww == -1) ww = this.ximages[rid].w;
        if (hh == -1) hh = this.ximages[rid].h;
        if (w == -1) w = ww;
        if (h == -1) h = hh;
        this.all_cuts[this.ai_cutid] = {
            "rid": rid,
            "x": x, "y": y,
            "w": w, "h": h,
            "u0": x / ww, "v0": y / hh,
            "u1": (x + w) / ww, "v1": y / hh,
            "u2": (x + w) / ww, "v2": (y + h) / hh,
            "u3": x / ww, "v3": (y + h) / hh
        };
        this.ai_cutid += 1;
        return this.ai_cutid - 1;
    }
    timenow() {
        let myDate = new Date();
        return myDate.getTime() / 1000;
    }

    PutTexture(tex, w, h) {
        var rid = this.ximages.length;
        this.ximages[rid] = { "stat": 1, "path": "put" + rid, "tex": tex, "w": w, "h": h };
        return rid;
    }

    LoadTexture(width, height) {
        var rid = this.ximages.length;

        var texture = gl.createTexture();
        XTexture._init_texture_status(texture)
        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, width, height, 0, gl.RGBA, gl.UNSIGNED_BYTE, null)

        this.ximages[rid] = { "stat": 1, "path": "default" + rid, "tex": texture, "w": width, "h": height };
        return rid;
    }
    InitTextImageData(s, size) {
        this.text_ctx.clearRect(0, 0, 1024, 256)
        this.text_ctx.font = size + "px Microsoft YaHei"
        this.text_ctx.fillStyle = "rgba(255,255,255,1)";
        this.text_ctx.fillText(s, 1, 1)
        let imgd = this.text_ctx.getImageData(0, 0, 1024, 256).data
        let w = 1024;
        let h = size+5;
        let x = 256;
        while (x == 256) {
            h -= 1;
            for (x = 0; x < 128; x++) {
                let p = (h * 1024 + x) * 4;
                if (imgd[p] != 0) break;
            }
        }
        let y = h
        while (y == h) {
            w -= 1
            for (y = 0; y < h; y++) {
                let p = (y * 1024 + w) * 4;
                if (imgd[p] != 0) break;
            }
        }
        // let realw=16
        // let realh=16
        // while(realw<w+1)realw*=2
        // while(realh<h+1)realh*=2
        // return this.text_ctx.getImageData(0, 0, realw, realh)
        return this.text_ctx.getImageData(0, 0, w + 1, h + 1)
    }
    GetText(s, size) {
        let text_idx = s + size

        if (text_idx in this.text_idxs) {
            this.text_idxs[text_idx].time = this.timenow()
            return this.text_idxs[text_idx].cid
        }

        let imgd = this.InitTextImageData(s, size)
        let w = imgd.width
        let h = imgd.height

        let use_height = Math.floor((h + 31) / 32)
        let mask = 0
        for (let i = 0; i < use_height; i++)
            mask |= (1 << i);

        let rid = -1;
        let off = -1;
        for (let k in this.text_imgs) {
            for (let i = 0; i < 32 - use_height + 1; i++) {
                if ((this.text_imgs[k].mask & (mask << i)) == 0) {
                    off = i;
                    break;
                }
            }
            if (off != -1) {
                rid = k;
                break;
            }
        }
        if (rid == -1) {
            rid = this.LoadTexture(1024, 1024)
            this.text_imgs[rid] = { "mask": 0 }
            off = 0
        }

        // console.log("GetText",s, size, h, rid, off, use_height)

        let cid = this.makeCut(rid, 0, off * 32, w, h)
        this.text_imgs[rid]["mask"] |= mask << off
        this.text_idxs[text_idx] = { "cid": cid, "rid": rid, "mask": mask << off, "time": this.timenow() }

        // let td=new Uint8Array(imgd.width*imgd.height*4)
        // for(let x=0;x<imgd.width;x++)
        // {
        //     for(let y=0;y<imgd.height;y++)
        //     {
        //         let p=(y*imgd.width+x)*4
        //         imgd.data[p]=255
        //         imgd.data[p+1]=255
        //         imgd.data[p+2]=255
        //         imgd.data[p+3]=255

        //         td[p]=255
        //         td[p+1]=255
        //         td[p+2]=255
        //         td[p+3]=255

        //         // // console.log(x,y,imgd.data[p],imgd.data[p+1],imgd.data[p+2],imgd.data[p+3])
        //         // if(imgd.data[p+3]==0)continue
        //         // imgd.data[p]=0
        //         // imgd.data[p+1]=0
        //         // imgd.data[p+2]=imgd.data[p+3]
        //         // //imgd.data[p+3]=255
        //     }
        // }
        gl.activeTexture(gl.TEXTURE0);
        gl.bindTexture(gl.TEXTURE_2D, this.ximages[rid].tex);
        gl.pixelStorei(gl.UNPACK_PREMULTIPLY_ALPHA_WEBGL, false);

        //gl.texSubImage2D(gl.TEXTURE_2D, 0, 0, off * 32, imgd.width,imgd.height,gl.RGBA, gl.UNSIGNED_BYTE, imgd.data);
        gl.texSubImage2D(gl.TEXTURE_2D, 0, 0, off * 32, gl.RGBA, gl.UNSIGNED_BYTE, imgd);

        return cid
    }
    _FreshText() {
        this.tmp_cutid = 0;
        let nt=this.timenow()
        let rm=[]
        for(let idx in this.text_idxs)
        {
            if(nt-this.text_idxs[idx].time>10)
            {
                this.text_imgs[this.text_idxs[idx].rid].mask&=(~this.text_idxs[idx].mask)
                delete this.all_cuts[this.text_idxs[idx].cid]
                rm.push(idx)
            }
        }
        for(let idx in rm)
        {
            delete this.text_idxs[rm[idx]]
        }
        //console.log("free",rm.length,Object.keys(this.all_cuts).length,Object.keys(this.text_imgs).length,this.ai_cutid)
    }
}

XTexture.pinstance_ = null;
import { XMat4 } from "./XMat4.js"
import { XShader } from "./XShader.js"
import { Scr } from "../XDefine.js"
import { XTexture } from "./XTexture.js"
import { gl } from "../GLFrame.js"
import { fAngle, iDistance } from "../XTools.js"

export class X2DFast {
    static gi() {
        if (X2DFast.px2f == null) X2DFast.px2f = new X2DFast();
        return X2DFast.px2f;
    }

    constructor() {
        this.local_buffer = gl.createBuffer();
        this.tex_sample_idx = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15];

        this.vertex_array = new ArrayBuffer(1024 * 1024 * 4 * 2)
        // let v = new DataView(a)
        this.vertex_float32 = new Float32Array(this.vertex_array)
        this.vertex_uint32 = new Uint32Array(this.vertex_array)
        // b[0]=123.456
        // c[1]=0xffff00ff
        // this.vertex_array=new Float32Array(1024*1024);
        this.white_img = XTexture.gi().loadTextureFromImage("CUSTOM_TEXTURE_1")
        this.white_cut = XTexture.gi().makeCut(this.white_img, 0, 0, 1, 1)

        XShader.gi();

        this.ResetMat();
    }
    ResetMat() {
        X2DFast.transform2D.Unit()
        X2DFast.transform2D.OrthoMat(0, 0, Scr.logicw, Scr.logich);
        let tm = X2DFast.transform2D.mat;
        this.t2d_ext = [
            tm[0][0], tm[1][0], tm[2][0], tm[3][0],
            tm[0][1], tm[1][1], tm[2][1], tm[3][1],
            tm[0][2], tm[1][2], tm[2][2], tm[3][2],
            tm[0][3], tm[1][3], tm[2][3], tm[3][3]
        ]
    }

    swapMode2D() {
        gl.disable(gl.DEPTH_TEST)

        gl.enable(gl.BLEND)
        gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA)

        //gl.enable(gl.ALPHA_TEST)
        //gl.alphaFunc(gl.GREATER, 0.01)
    }

    DrawCircle(ox, oy, rw, rh, c = 0xffffffff, lw = 1) {
        let lx = -1
        let ly = -1
        let i = 0
        let gap = Math.PI * 2 / 32
        while (i < Math.PI * 2 + 0.00001) {
            let dx = Math.cos(i) * rw + ox
            let dy = Math.sin(i) * rh + oy
            if (lx != -1) {
                this.drawLine(lx, ly, dx, dy, c, lw)
            }
            lx = dx
            ly = dy
            i += gap
        }
    }

    fillRect(x, y, w, h, c = 0xffffffff) {
        this.drawCut(this.white_cut, x, y, w, h, 0, 0, 0, c)
    }

    drawLine(x1, y1, x2, y2, c = 0xffffffff, linewidth = 1) {
        this.drawCut(this.white_cut, x1, y1, iDistance(x1 - x2, y1 - y2), linewidth, fAngle(x2 - x1, y2 - y1), 0, 0.5, c)
    }

    drawRect(x, y, w, h, c = 0xffffffff, lw = 1) {
        this.drawLine(x - lw / 2, y, x + w + lw / 2, y, c, lw)
        this.drawLine(x, y, x, y + h, c, lw)
        this.drawLine(x + w, y + h, x + w, y, c, lw)
        this.drawLine(x + w + lw / 2, y + h, x - lw / 2, y + h, c, lw)
    }

    static test_transform(x, y, sw, sh, ra, ox, oy, realw, realh) {
        X2DFast.tmp_mat.Unit()
        if (ox == -1) ox = 0
        if (ox == -2) ox = Math.floor(realw / 2)
        if (ox == -3) ox = realw
        if (oy == -1) oy = 0
        if (oy == -2) oy = Math.floor(realh / 2)
        if (oy == -3) oy = realh
        if (ox != 0 || oy != 0) X2DFast.tmp_mat.move(-ox, -oy, 0)
        if (sw != 1 || sh != 1) X2DFast.tmp_mat.Scale(sw, sh, 1)
        if (ra != 0) X2DFast.tmp_mat.Rotate(0, 0, ra)
        if (x != 0 || y != 0) X2DFast.tmp_mat.move(x, y, 0)
    }
    clearBuffer() {
        this.rid_dict = {}
        this.rid_point = 0
        this.draw_count = 0
    }
    swap_c(c) {
        let r = Math.floor(((c >> 16) & 0xff) * 63 / 255)
        let g = Math.floor(((c >> 8) & 0xff) * 63 / 255)
        let b = Math.floor((c & 0xff) * 63 / 255)
        let a = Math.floor(((c >> 24) & 0xff) * 63 / 255)
        return ((a * 64 + r) * 64 + g) * 64 + b
    }
    _draw_cut(pcut, m00, m01, m10, m11, m22, m30, m31, c = 0xffffffff) {
        if (c == -1) c = 0xffffffff
        c = this.swap_c(c)
        this.vertex_float32.set([0.0, 0.0, 0.0, pcut.u0, pcut.v0, m00, m01, m10, m11, m22, m30, m31, this.rid_dict[pcut.rid], c,
            pcut.w, 0.0, 0.0, pcut.u1, pcut.v1, m00, m01, m10, m11, m22, m30, m31, this.rid_dict[pcut.rid], c,
            pcut.w, pcut.h, 0.0, pcut.u2, pcut.v2, m00, m01, m10, m11, m22, m30, m31, this.rid_dict[pcut.rid], c,

            0.0, 0.0, 0.0, pcut.u0, pcut.v0, m00, m01, m10, m11, m22, m30, m31, this.rid_dict[pcut.rid], c,
            pcut.w, pcut.h, 0.0, pcut.u2, pcut.v2, m00, m01, m10, m11, m22, m30, m31, this.rid_dict[pcut.rid], c,
            0.0, pcut.h, 0.0, pcut.u3, pcut.v3, m00, m01, m10, m11, m22, m30, m31, this.rid_dict[pcut.rid], c], this.draw_count * 14 * 6)
        this.draw_count += 1
    }
    drawCutEx(cid, tmat, c = 0xffffffff) {
        let pcut = XTexture.pinstance_.all_cuts[cid];
        if (!(pcut.rid in this.rid_dict)) {
            this.rid_dict[pcut.rid] = this.rid_point;
            this.rid_point += 1;
        }
        tmat = tmat.mat
        this._draw_cut(pcut, tmat[0][0], tmat[0][1], tmat[1][0], tmat[1][1], tmat[2][2], tmat[3][0], tmat[3][1], c)
    }
    drawCut(cid, x = 0, y = 0, sw = 1, sh = 1, ra = 0, ox = 0, oy = 0, c = 0xffffffff) {
        let pcut = XTexture.gi().all_cuts[cid];
        if (pcut == null)
            return;
        if (!(pcut.rid in this.rid_dict)) {
            this.rid_dict[pcut.rid] = this.rid_point;
            this.rid_point += 1;
        }
        X2DFast.test_transform(x, y, sw, sh, ra, ox, oy, pcut.w, pcut.h)
        let tmat = X2DFast.tmp_mat.mat;
        this._draw_cut(pcut, tmat[0][0], tmat[0][1], tmat[1][0], tmat[1][1], tmat[2][2], tmat[3][0], tmat[3][1], c)
    }
    drawText(s, size = 24, x = 0, y = 0, sw = 1, sh = 1, ra = 0, ox = 0, oy = 0, c = 0xffffffff) {
        if (s.length <= 0) return 0;
        let cid = XTexture.gi().GetText(s, size)
        if (cid >= 0)
            this.drawCut(cid, x, y, sw, sh, ra, ox, oy, c)
        return XTexture.gi().all_cuts[cid].w;
    }
    getTextWidth(s, size) {
        if (s.length <= 0) return 0;
        let cid = XTexture.gi().GetText(s, size)
        return XTexture.gi().all_cuts[cid].w;
    }
    freshBuffer() {
        XTexture.gi()._FreshText()
        if (this.draw_count == 0) return
        let ps = XShader.gi().use(XShader.ID_SHADER_FAST)
        for (let rid in this.rid_dict) {
            gl.activeTexture(gl.TEXTURE0 + this.rid_dict[rid])
            gl.bindTexture(gl.TEXTURE_2D, XTexture.gi().ximages[rid].tex);

            gl.uniform1i(ps.tex[this.rid_dict[rid]], this.rid_dict[rid]);
        }

        gl.uniformMatrix4fv(ps.u_mat, false, this.t2d_ext)

        gl.bindBuffer(gl.ARRAY_BUFFER, this.local_buffer);
        gl.bufferData(gl.ARRAY_BUFFER, this.vertex_array, gl.STATIC_DRAW)
        gl.vertexAttribPointer(ps.position, 3, gl.FLOAT, false, 4 * 14, 0)
        gl.enableVertexAttribArray(ps.position)
        gl.vertexAttribPointer(ps.aTexCoord, 2, gl.FLOAT, false, 4 * 14, 4 * 3)
        gl.enableVertexAttribArray(ps.aTexCoord)
        gl.vertexAttribPointer(ps.ext1, 4, gl.FLOAT, false, 4 * 14, 4 * 5)
        gl.enableVertexAttribArray(ps.ext1)
        gl.vertexAttribPointer(ps.ext2, 4, gl.FLOAT, false, 4 * 14, 4 * 9)
        gl.enableVertexAttribArray(ps.ext2)
        gl.vertexAttribPointer(ps.in_color, 1, gl.FLOAT, false, 4 * 14, 4 * 13)
        gl.enableVertexAttribArray(ps.in_color)

        gl.drawArrays(gl.TRIANGLES, 0, 6 * this.draw_count);
    }
}
X2DFast.tmp_mat = new XMat4();
X2DFast.transform2D = new XMat4();

X2DFast.px2f = null;



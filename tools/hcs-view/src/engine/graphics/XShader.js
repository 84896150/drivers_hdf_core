const { shader_fast_vs, shader_fast_fs } = require("./shaders/shader_fast")
import { gl } from "../GLFrame.js"

export class XShader {
    static gi() {
        if (XShader.pinstance_ == null) XShader.pinstance_ = new XShader();
        return XShader.pinstance_;
    }
    constructor() {
        this.pUseingShader = null;
        this.shaders = [];
        this.InitFastShader();
    }
    InitFastShader() {
        this.shader_fast = { "program": this.InitShader(shader_fast_vs, shader_fast_fs) }

        this.shader_fast.position = gl.getAttribLocation(this.shader_fast["program"], "position");
        this.shader_fast.aTexCoord = gl.getAttribLocation(this.shader_fast["program"], "aTexCoord");
        this.shader_fast.ext1 = gl.getAttribLocation(this.shader_fast["program"], "ext1");
        this.shader_fast.ext2 = gl.getAttribLocation(this.shader_fast["program"], "ext2");
        this.shader_fast.in_color = gl.getAttribLocation(this.shader_fast["program"], "in_color");

        this.shader_fast.u_mat = gl.getUniformLocation(this.shader_fast["program"], 'u_mat')

        this.shader_fast.tex = {}
        for (let i = 0; i < 16; i++) {
            this.shader_fast.tex[i] = gl.getUniformLocation(this.shader_fast["program"], 'tex' + i)
        }

        this.shaders[XShader.ID_SHADER_FAST] = this.shader_fast
        this.use(XShader.ID_SHADER_FAST)
    }
    use(sid) {
        gl.useProgram(this.shaders[sid]["program"])
        this.pUseingShader = this.shaders[sid]
        return this.pUseingShader
    }
    InitShader(vss, fss) {
        var vs = gl.createShader(gl.VERTEX_SHADER)
        gl.shaderSource(vs, vss)
        gl.compileShader(vs)
        if (!gl.getShaderParameter(vs, gl.COMPILE_STATUS)) {
            alert('error occured compiling the shaders:' + gl.getShaderInfoLog(vs));
            return null;
        }

        var fs = gl.createShader(gl.FRAGMENT_SHADER)
        gl.shaderSource(fs, fss)
        gl.compileShader(fs)
        if (!gl.getShaderParameter(fs, gl.COMPILE_STATUS)) {
            alert('error occured compiling the shaders:' + gl.getShaderInfoLog(fs));
            return null;
        }

        var ret = gl.createProgram()

        gl.attachShader(ret, vs)
        gl.attachShader(ret, fs)
        gl.linkProgram(ret)

        if (!gl.getProgramParameter(ret, gl.LINK_STATUS)) {
            alert('unable to initialize!');
            return;
        }

        gl.deleteShader(vs)
        gl.deleteShader(fs)
        return ret
    }
}

XShader.ID_SHADER_FAST = 2

XShader.pinstance_ = null;
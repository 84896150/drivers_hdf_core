class AttributeArea {
    constructor() {
        document.attrCallback = this;
        this.area = document.getElementById("attribute_area");
        this.htmlStr = ""
        this.callback_func = null;
        this.freshInputValue_ = []
    }
    Clear() {
        this.htmlStr = ""
    }
    Flush() {
        this.area.innerHTML = this.htmlStr
        while (this.freshInputValue_.length > 0) {
            let v = this.freshInputValue_.shift();
            let e = document.getElementById(v[0]);
            e.value = v[1];
        }
    }
    AddTitle(name) {
        this.htmlStr += '<p class="att_title">' + name + '</p>';
    }
    AddInput(searchId, label, default_, disable) {
        let ret = '<label class="input_text_readonly">' + label + '</label>'
        ret += '<input id="' + searchId + '"';
        ret += ' class="input_text"';
        if (default_.indexOf('"') >= 0) {
            ret += ' value=""';
            this.freshInputValue_.push([searchId, default_])
        }
        else
            ret += ' value="' + default_ + '"';
        if (disable)
            ret += ' disabled="disabled"';
        ret += ' oninput="document.attrCallback.Event(' + "'input', " + "'" + searchId + "'" + ')" /><br>'
        this.htmlStr += ret;
    }
    AddTextArea(searchId, label, default_) {
        let ret = '<label class="input_text_readonly">' + label + '</label><br>'
        ret += '<textarea id="' + searchId + '"';
        ret += ' class="text_area"';
        ret += ' oninput="document.attrCallback.Event(' + "'input', " + "'" + searchId + "'" + ')">'
        ret += default_;
        ret += '</textarea><br>';//cols="20" rows="10"
        this.htmlStr += ret;
    }
    AddButton(searchId, label) {
        if (label.length > 13) label = label.substring(0, 10) + "..."
        this.htmlStr += '<button id="' + searchId + '" class="button_click" type="button" onclick="document.attrCallback.Event(' + "'button', " + "'" + searchId + "'" + ')">' + label + '</button><br>';
    }
    AddSelect(searchId, label, selectList, default_, disable) {
        let ret = '<label class="input_text_readonly">' + label + '</label>'
        ret += '<select id="' + searchId + '"';
        ret += ' class="select_list"'
        ret += ' size="1"'
        if (disable)
            ret += ' disabled="disabled"';
        ret += ' onchange="document.attrCallback.Event(' + "'select', " + "'" + searchId + "'" + ')">';
        for (let i in selectList) {
            if (selectList[i] == default_) {
                ret += '<option selected="selected">' + selectList[i] + "</option>";
            }
            else {
                ret += "<option>" + selectList[i] + "</option>";
            }
        }
        ret += "</select><br>";
        this.htmlStr += ret;
    }
    AddGap(type) {
        if (type == 0) this.htmlStr += '<br>';
    }
    Event(type, value) {
        let cbv = "";
        if (type == "input") {
            cbv = document.getElementById(value).value
        }
        else if (type == "select") {
            cbv = document.getElementById(value).value
        }
        if (this.callback_func != null) {
            this.callback_func(value, type, cbv)
        }
    }
    RegistRecvCallback(func) {
        this.callback_func = func
    }
}
AttributeArea.pInstance_ = null;
AttributeArea.gi = function () {
    if (AttributeArea.pInstance_ == null) AttributeArea.pInstance_ = new AttributeArea();
    return AttributeArea.pInstance_;
}

module.exports = {
    AttributeArea
}
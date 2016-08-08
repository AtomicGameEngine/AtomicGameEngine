//
// Copyright (c) 2016 THUNDERBEAST GAMES LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// http://axonflux.com/handy-rgb-to-hsl-and-rgb-to-hsv-color-model-c

import EditorUI = require("ui/EditorUI");
import Preferences = require("editor/Preferences");

class ColorChooser extends Atomic.UIWindow {

    rgbhsla : number[] = [ 0, 0, 0, 0, 0, 0, 255 ]; // 0=R, 1=G , 2=B, 3=H, 4=S, 5=L, 6=Alpha
    r_ils : Atomic.UIInlineSelect;
    g_ils : Atomic.UIInlineSelect;
    b_ils : Atomic.UIInlineSelect;
    a_ils : Atomic.UIInlineSelect;
    r_sld : Atomic.UISlider;
    g_sld : Atomic.UISlider;
    b_sld : Atomic.UISlider;
    a_sld : Atomic.UISlider;
    oldcolor : Atomic.UIColorWidget;
    newcolor : Atomic.UIColorWidget;
    wheel : Atomic.UIColorWheel;
    lslide : Atomic.UISlider;
    infohex : Atomic.UIEditField;
    infohsl : Atomic.UIEditField;

    constructor( startRGBA:number[] ) {

        super();

        var view = EditorUI.getView();
        view.addChild(this);

        this.text = "Color Chooser";
        this.load("AtomicEditor/editor/ui/colorchooser.tb.txt");

        this.r_ils = <Atomic.UIInlineSelect>this.getWidget("redselect");
        this.g_ils = <Atomic.UIInlineSelect>this.getWidget("greenselect");
        this.b_ils = <Atomic.UIInlineSelect>this.getWidget("blueselect");
        this.a_ils = <Atomic.UIInlineSelect>this.getWidget("alphaselect");
        this.r_sld = <Atomic.UISlider>this.getWidget("redslider");
        this.g_sld = <Atomic.UISlider>this.getWidget("greenslider");
        this.b_sld = <Atomic.UISlider>this.getWidget("blueslider");
        this.a_sld = <Atomic.UISlider>this.getWidget("alphaslider");
        this.newcolor = <Atomic.UIColorWidget>this.getWidget("colornew" );
        this.oldcolor = <Atomic.UIColorWidget>this.getWidget("colorold" );
        this.infohex = <Atomic.UIEditField>this.getWidget("infohex" );
        this.infohsl = <Atomic.UIEditField>this.getWidget("infohsl" );
        this.wheel = <Atomic.UIColorWheel>this.getWidget("colorwheel" );
        this.lslide = <Atomic.UISlider>this.getWidget("lslider");

        (<Atomic.UIButton>this.getWidget("ccancelbutton")).onClick = () => {

            this.sendEvent("UIWidgetEditCanceled", { widget : this });
            this.unsubscribeFromEvent("WidgetDeleted");
            this.close();

        };

        (<Atomic.UIButton>this.getWidget("cokbutton")).onClick = () => {

            Preferences.getInstance().addColorHistory(this.infohex.text);

            this.sendEvent("UIWidgetEditComplete", { widget : this });
            this.unsubscribeFromEvent("WidgetDeleted");
            this.close();
        };

        this.subscribeToEvent(this, "WidgetDeleted", (event: Atomic.UIWidgetDeletedEvent) => {

            this.sendEvent("UIWidgetEditCanceled", { widget : this });

        });

        this.subscribeToEvent(this, "WidgetEvent", (data) => this.handleWidgetEvent(data));

        this.resizeToFitContent();
        this.center();
        this.setFocus();

        let rx = Math.min(startRGBA[0] * 255, 255);
        let gx = Math.min(startRGBA[1] * 255, 255);
        let bx = Math.min(startRGBA[2] * 255, 255);
        let ax = Math.min(startRGBA[3] * 255, 255);

        let oldcolor = "#";
        var rrr =  Math.round(rx).toString(16);
        if (rrr.length < 2) oldcolor = oldcolor + "0";
        oldcolor = oldcolor + rrr;
        var ggg =  Math.round(gx).toString(16);
        if (ggg.length < 2) oldcolor = oldcolor + "0";
        oldcolor = oldcolor + ggg;
        var bbb =  Math.round(bx).toString(16);
        if (bbb.length < 2) oldcolor = oldcolor + "0";
        oldcolor = oldcolor + bbb;
        this.oldcolor.setColorString( oldcolor );

        // start with current color instead of black
        this.rgbhsla[0] = rx;
        this.rgbhsla[1] = gx;
        this.rgbhsla[2] = bx;
        this.recalc_hsl();
        this.fixcolor();
        this.update_rgbwidgets();
        this.update_hslwidgets();

        Atomic.ui.blockChangedEvents = true;

        var colorhist = Preferences.getInstance().colorHistory;
        (<Atomic.UIColorWidget>this.getWidget("chistory0")).setColorString(colorhist[0]);
        (<Atomic.UIColorWidget>this.getWidget("chistory1")).setColorString(colorhist[1]);
        (<Atomic.UIColorWidget>this.getWidget("chistory2")).setColorString(colorhist[2]);
        (<Atomic.UIColorWidget>this.getWidget("chistory3")).setColorString(colorhist[3]);
        (<Atomic.UIColorWidget>this.getWidget("chistory4")).setColorString(colorhist[4]);
        (<Atomic.UIColorWidget>this.getWidget("chistory5")).setColorString(colorhist[5]);
        (<Atomic.UIColorWidget>this.getWidget("chistory6")).setColorString(colorhist[6]);
        (<Atomic.UIColorWidget>this.getWidget("chistory7")).setColorString(colorhist[7]);

        this.oldcolor.setAlpha( startRGBA[3] * 255 );
        this.a_sld.setValue(this.rgbhsla[6]);

        Atomic.ui.blockChangedEvents = false;

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        let changed = false;
        if (ev.type == Atomic.UI_EVENT_TYPE_CHANGED) {
            let hsltargets = ["colorwheel", "lslider"];
            let rgbtargets = {
                "redselect" : 0,
                "redslider" : 0,
                "greenselect" : 1,
                "greenslider" : 1,
                "blueselect" : 2,
                "blueslider" : 2,
                "alphaslider" : 3
            };

            if (hsltargets.indexOf(ev.target.id) > -1) {

                changed = true;
                this.rgbhsla[3] = this.wheel.getHue() / 360;
                this.rgbhsla[4] = this.wheel.getSaturation() / 128;
                this.rgbhsla[5] = this.lslide.getValue() / 255;
                this.recalc_rgb();
                this.fixcolor();
                this.update_rgbwidgets();

            } else if (ev.target.id in rgbtargets) {

                changed = true;
                this.rgbhsla[rgbtargets[ev.target.id]] = ev.target.getValue();
                this.recalc_hsl();
                this.fixcolor();
                this.update_hslwidgets();

            }
        }

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            if ( ev.target.id == "history0" || ev.target.id == "history1"
                    || ev.target.id == "history2" || ev.target.id == "history3"
                    || ev.target.id == "history4" || ev.target.id == "history5"
                    || ev.target.id == "history6" || ev.target.id == "history7") {
                var which = ev.target.id.charAt(7); //parseInt(ev.target.id); // which button was pressed
                var colorcell = Preferences.getInstance().colorHistory[which]; // get the string
                this.ColorFromString ( colorcell );
            }

        }

        if (changed) {

            this.sendEvent("ColorChooserChanged", { widget : this });

        }

    }

    ColorFromString ( colorString: String ) {

        let changed = false;
        let hslx = colorString.split ( "," );
        if ( colorString.indexOf( "#" ) == 0 && colorString.length == 7 ) { // decode well formed RGB

            this.rgbhsla[0] = parseInt ( colorString.slice ( 1, 3 ), 16 );
            this.rgbhsla[1] = parseInt ( colorString.slice ( 3, 5 ), 16 );
            this.rgbhsla[2] = parseInt ( colorString.slice ( 5, 7 ), 16 );
            this.recalc_hsl();
            changed = true;

        }
        else if ( hslx.length == 2 ) {  // decode HSL

            this.rgbhsla[3] = parseFloat ( hslx[0] );
            this.rgbhsla[4] = parseFloat ( hslx[1] );
            this.rgbhsla[5] = parseFloat ( hslx[2] );
            this.recalc_rgb();
            changed = true;

        }

        if (changed) {

            this.fixcolor();
            this.update_hslwidgets();
            this.update_rgbwidgets();
            this.sendEvent("ColorChooserChanged", { widget : this });

        }

    }

    // return current color choice as a RGBA array
    getRGBA() : number[] {

        var rr = this.rgbhsla[0] / 255;
        var gg = this.rgbhsla[1] / 255;
        var bb = this.rgbhsla[2] / 255;
        var aa = this.rgbhsla[6] / 255;

        return [rr, gg, bb, aa];

    }

    hue2rgb(p: number, q: number, t: number) { // part of rgb conversion
        if (t < 0) t += 1;
        if (t > 1) t -= 1;
        if (t < 1 / 6) return p + (q - p) * 6 * t;
        if (t < 1 / 2) return q;
        if (t < 2 / 3) return p + (q - p) * (2 / 3 - t) * 6;
        return p;
    }

    recalc_rgb() {
        var rr, gg, bb;
        let hh = this.rgbhsla[3];
        let ss = this.rgbhsla[4];
        let ll = this.rgbhsla[5];

        if (ss == 0) {
            rr = gg = bb = ll; // achromatic
        } else {
            var q = ll < 0.5 ? ll * (1 + ss) : ll + ss - ll * ss;
            var p = 2 * ll - q;
            rr = this.hue2rgb(p, q, hh + 1 / 3);
            gg = this.hue2rgb(p, q, hh);
            bb = this.hue2rgb(p, q, hh - 1 / 3);
        }

        this.rgbhsla[0] = rr * 255;
        this.rgbhsla[1] = gg * 255;
        this.rgbhsla[2] = bb * 255;
    }

    recalc_hsl() {
        let rr = this.rgbhsla[0] / 255;
        let gg = this.rgbhsla[1] / 255;
        let bb = this.rgbhsla[2] / 255;

        var max = Math.max(rr, gg, bb), min = Math.min(rr, gg, bb);
        var hh, ss, ll = (max + min) / 2;

        if (max == min) {
            hh = ss = 0; // achromatic
        } else {
            var d = max - min;
            ss = ll > 0.5 ? d / (2 - max - min) : d / (max + min);
            switch (max) {
                case rr: hh = (gg - bb) / d + (gg < bb ? 6 : 0); break;
                case gg: hh = (bb - rr) / d + 2; break;
                case bb: hh = (rr - gg) / d + 4; break;
            }
            hh /= 6;
        }
        this.rgbhsla[3] = hh;
        this.rgbhsla[4] = ss;
        this.rgbhsla[5] = ll;
    }

    update_rgbwidgets() {
        Atomic.ui.blockChangedEvents = true;
        this.r_ils.setValue(this.rgbhsla[0]);
        this.g_ils.setValue(this.rgbhsla[1]);
        this.b_ils.setValue(this.rgbhsla[2]);
        this.r_sld.setValue(this.rgbhsla[0]);
        this.g_sld.setValue(this.rgbhsla[1]);
        this.b_sld.setValue(this.rgbhsla[2]);
        Atomic.ui.blockChangedEvents = false;
    }

    update_hslwidgets() {
        Atomic.ui.blockChangedEvents = true;
        this.wheel.setHueSaturation (this.rgbhsla[3], this.rgbhsla[4]);
        this.lslide.setValue(this.rgbhsla[5] * 128);
        Atomic.ui.blockChangedEvents = false;
   }

    fixcolor() {
        let mycolor = "#";  // create a new string

        var rrr =  Math.round(this.rgbhsla[0]).toString(16);
        if (rrr.length < 2) mycolor = mycolor + "0";
        mycolor = mycolor + rrr;

        var ggg =  Math.round(this.rgbhsla[1]).toString(16);
        if (ggg.length < 2) mycolor = mycolor + "0";
        mycolor = mycolor + ggg;

        var bbb =  Math.round(this.rgbhsla[2]).toString(16);
        if (bbb.length < 2) mycolor = mycolor + "0";
        mycolor = mycolor + bbb;

        this.newcolor.setColorString( mycolor );
        this.newcolor.setAlpha( this.rgbhsla[6] / 255 );
        this.infohex.text = mycolor;

        let myhsl = Number(this.rgbhsla[3]).toFixed(2);
        myhsl += ", ";
        myhsl += Number(this.rgbhsla[4]).toFixed(2);
        myhsl += ", ";
        myhsl += Number(this.rgbhsla[5]).toFixed(2);
        this.infohsl.text = myhsl;
    }

}

export = ColorChooser;

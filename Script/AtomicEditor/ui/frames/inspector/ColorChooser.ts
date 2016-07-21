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
    infotext : Atomic.UIEditField;
    parent_rils : Atomic.UIInlineSelect;
    parent_gils : Atomic.UIInlineSelect;
    parent_bils : Atomic.UIInlineSelect;
    parent_ails : Atomic.UIInlineSelect;

    constructor( pred : Atomic.UIInlineSelect, pgreen : Atomic.UIInlineSelect,
                pblue : Atomic.UIInlineSelect, palpha : Atomic.UIInlineSelect ) {

        super();

        var view = EditorUI.getView();
        view.addChild(this);

        this.parent_rils = pred;
        this.parent_gils = pgreen;
        this.parent_bils = pblue;
        this.parent_ails = palpha;
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
        this.infotext = <Atomic.UIEditField>this.getWidget("infotext" );
        this.wheel = <Atomic.UIColorWheel>this.getWidget("colorwheel" );
        this.lslide = <Atomic.UISlider>this.getWidget("lslider");

        (<Atomic.UIButton>this.getWidget("ccancelbutton")).onClick = () => {
            this.close();
        };

        (<Atomic.UIButton>this.getWidget("cokbutton")).onClick = () => {

            this.parent_rils.setValue( this.rgbhsla[0] / 255 );
            this.parent_gils.setValue( this.rgbhsla[1] / 255 );
            this.parent_bils.setValue( this.rgbhsla[2] / 255 );
            this.parent_ails.setValue( this.rgbhsla[6] / 255 );
            this.close();
        };

        this.subscribeToEvent(this, "WidgetEvent", (data) => this.handleWidgetEvent(data));

        this.resizeToFitContent();
        this.center();
        this.setFocus();

        let rx = this.parent_rils.getValue() * 255;
        if ( rx > 255 ) rx = 255;
        let gx = this.parent_gils.getValue() * 255;
        if ( gx > 255 ) gx = 255;
        let bx = this.parent_bils.getValue() * 255;
        if ( bx > 255 ) bx = 255;
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
        this.oldcolor.setColor( oldcolor );

        Atomic.ui.blockChangedEvents = true;

        this.oldcolor.setAlpha( this.parent_ails.getValue() * 255 );
        this.a_sld.setValue(this.rgbhsla[6]);

        Atomic.ui.blockChangedEvents = false;

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.target.id == "colorwheel" && ev.type == Atomic.UI_EVENT_TYPE_CHANGED ) {
            var myhue = this.wheel.getHue();
            var mysat = this.wheel.getSaturation();
            var mylit = this.lslide.getValue();
            this.rgbhsla[3] = myhue / 360;
            this.rgbhsla[4] = mysat / 128;
            this.rgbhsla[5] = mylit / 255;
            this.recalc_rgb();
            this.fixcolor();
            this.update_rgbwidgets();
        }
        if (ev.target.id == "lslider" && ev.type == Atomic.UI_EVENT_TYPE_CHANGED ) {
            var myhue = this.wheel.getHue();
            var mysat = this.wheel.getSaturation();
            var mylit = ev.target.getValue();
            this.rgbhsla[3] = myhue / 360;
            this.rgbhsla[4] = mysat / 128;
            this.rgbhsla[5] = mylit / 255;
            this.recalc_rgb();
            this.fixcolor();
            this.update_rgbwidgets();
        }
        if ( ev.target.id == "redselect" && ev.type == Atomic.UI_EVENT_TYPE_CHANGED ) {
            this.rgbhsla[0] = ev.target.getValue();
            this.recalc_hsl();
            this.fixcolor();
            this.update_hslwidgets();
        }
        if ( ev.target.id == "redslider" && ev.type == Atomic.UI_EVENT_TYPE_CHANGED ) {
            this.rgbhsla[0] = ev.target.getValue();
            this.recalc_hsl();
            this.fixcolor();
            this.update_hslwidgets();
        }
        if ( ev.target.id == "greenselect" && ev.type == Atomic.UI_EVENT_TYPE_CHANGED ) {
            this.rgbhsla[1] = ev.target.getValue();
            this.recalc_hsl();
            this.fixcolor();
            this.update_hslwidgets();
        }
        if ( ev.target.id == "greenslider"  && ev.type == Atomic.UI_EVENT_TYPE_CHANGED ) {
            this.rgbhsla[1] = ev.target.getValue();
            this.recalc_hsl();
            this.fixcolor();
            this.update_hslwidgets();
        }
        if ( ev.target.id == "blueselect" && ev.type == Atomic.UI_EVENT_TYPE_CHANGED ) {
            this.rgbhsla[2] = ev.target.getValue();
            this.recalc_hsl();
            this.fixcolor();
            this.update_hslwidgets();
        }
        if ( ev.target.id == "blueslider" && ev.type == Atomic.UI_EVENT_TYPE_CHANGED ) {
            this.rgbhsla[2] = ev.target.getValue();
            this.recalc_hsl();
            this.fixcolor();
            this.update_hslwidgets();
        }
        if ( ev.target.id == "alphaslider" && ev.type == Atomic.UI_EVENT_TYPE_CHANGED ) {
            this.rgbhsla[6] = ev.target.getValue();
            this.fixcolor();
        }

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

        this.newcolor.setColor( mycolor );
        this.newcolor.setAlpha( this.rgbhsla[6] / 255 );
        this.infotext.text = mycolor;
    }

}

export = ColorChooser;

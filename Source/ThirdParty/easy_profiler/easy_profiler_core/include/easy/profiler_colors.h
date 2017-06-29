/**
Lightweight profiler library for c++
Copyright(C) 2016-2017  Sergey Yagovtsev, Victor Zarubkin

Licensed under either of
	* MIT license (LICENSE.MIT or http://opensource.org/licenses/MIT)
    * Apache License, Version 2.0, (LICENSE.APACHE or http://www.apache.org/licenses/LICENSE-2.0)
at your option.

The MIT License
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights 
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
	of the Software, and to permit persons to whom the Software is furnished 
	to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all 
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
	PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
	LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE 
	USE OR OTHER DEALINGS IN THE SOFTWARE.


The Apache License, Version 2.0 (the "License");
	You may not use this file except in compliance with the License.
	You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.

**/

#ifndef EASY_PROFILER_COLORS_H
#define EASY_PROFILER_COLORS_H

#include <stdint.h>

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

namespace profiler {

    typedef uint32_t color_t; // Standard four-byte ARGB color format

	namespace colors {

        ///< Change alpha for color. Only 8 major bytes (0xff000000) used from alpha.
        inline color_t modify_alpha32(color_t _color, color_t _alpha) {
            return (_alpha & 0xff000000) | (_color & 0x00ffffff);
        }

        ///< Change alpha for color.
        inline color_t modify_alpha8(color_t _color, uint8_t _alpha) {
            return (static_cast<color_t>(_alpha) << 24) | (_color & 0x00ffffff);
        }

        ///< Create color from ARGB components.
        inline color_t color(uint8_t _red, uint8_t _green, uint8_t _blue, uint8_t _alpha = 0xff) {
            return (static_cast<color_t>(_alpha) << 24) | (static_cast<color_t>(_red) << 16) | (static_cast<color_t>(_green) << 8) | static_cast<color_t>(_blue);
        }

#if !defined(EASY_OPTION_BUILTIN_COLORS) || EASY_OPTION_BUILTIN_COLORS != 0
        // Google Material Design colors
        // See https://material.google.com/style/color.html

        const color_t Red50   = 0xffffebee;
        const color_t Red100  = 0xffffcdd2;
        const color_t Red200  = 0xffef9a9a;
        const color_t Red300  = 0xffe57373;
        const color_t Red400  = 0xffef5350;
        const color_t Red500  = 0xfff44336;
        const color_t Red600  = 0xffe53935;
        const color_t Red700  = 0xffd32f2f;
        const color_t Red800  = 0xffc62828;
        const color_t Red900  = 0xffb71c1c;
        const color_t RedA100 = 0xffff8a80;
        const color_t RedA200 = 0xffff5252;
        const color_t RedA400 = 0xffff1744;
        const color_t RedA700 = 0xffd50000;

        const color_t Pink50   = 0xfffce4ec;
        const color_t Pink100  = 0xfff8bbd0;
        const color_t Pink200  = 0xfff48fb1;
        const color_t Pink300  = 0xfff06292;
        const color_t Pink400  = 0xffec407a;
        const color_t Pink500  = 0xffe91e63;
        const color_t Pink600  = 0xffd81b60;
        const color_t Pink700  = 0xffc2185b;
        const color_t Pink800  = 0xffad1457;
        const color_t Pink900  = 0xff880e4f;
        const color_t PinkA100 = 0xffff80ab;
        const color_t PinkA200 = 0xffff4081;
        const color_t PinkA400 = 0xfff50057;
        const color_t PinkA700 = 0xffc51162;

        const color_t Purple50   = 0xfff3e5f5;
        const color_t Purple100  = 0xffe1bee7;
        const color_t Purple200  = 0xffce93d8;
        const color_t Purple300  = 0xffba68c8;
        const color_t Purple400  = 0xffab47bc;
        const color_t Purple500  = 0xff9c27b0;
        const color_t Purple600  = 0xff8e24aa;
        const color_t Purple700  = 0xff7b1fa2;
        const color_t Purple800  = 0xff6a1b9a;
        const color_t Purple900  = 0xff4a148c;
        const color_t PurpleA100 = 0xffea80fc;
        const color_t PurpleA200 = 0xffe040fb;
        const color_t PurpleA400 = 0xffd500f9;
        const color_t PurpleA700 = 0xffaa00ff;

        const color_t DeepPurple50   = 0xffede7f6;
        const color_t DeepPurple100  = 0xffd1c4e9;
        const color_t DeepPurple200  = 0xffb39ddb;
        const color_t DeepPurple300  = 0xff9575cd;
        const color_t DeepPurple400  = 0xff7e57c2;
        const color_t DeepPurple500  = 0xff673ab7;
        const color_t DeepPurple600  = 0xff5e35b1;
        const color_t DeepPurple700  = 0xff512da8;
        const color_t DeepPurple800  = 0xff4527a0;
        const color_t DeepPurple900  = 0xff311b92;
        const color_t DeepPurpleA100 = 0xffb388ff;
        const color_t DeepPurpleA200 = 0xff7c4dff;
        const color_t DeepPurpleA400 = 0xff651fff;
        const color_t DeepPurpleA700 = 0xff6200ea;

        const color_t Indigo50   = 0xffe8eaf6;
        const color_t Indigo100  = 0xffc5cae9;
        const color_t Indigo200  = 0xff9fa8da;
        const color_t Indigo300  = 0xff7986cb;
        const color_t Indigo400  = 0xff5c6bc0;
        const color_t Indigo500  = 0xff3f51b5;
        const color_t Indigo600  = 0xff3949ab;
        const color_t Indigo700  = 0xff303f9f;
        const color_t Indigo800  = 0xff283593;
        const color_t Indigo900  = 0xff1a237e;
        const color_t IndigoA100 = 0xff8c9eff;
        const color_t IndigoA200 = 0xff536dfe;
        const color_t IndigoA400 = 0xff3d5afe;
        const color_t IndigoA700 = 0xff304ffe;

        const color_t Blue50   = 0xffe3f2fd;
        const color_t Blue100  = 0xffbbdefb;
        const color_t Blue200  = 0xff90caf9;
        const color_t Blue300  = 0xff64b5f6;
        const color_t Blue400  = 0xff42a5f5;
        const color_t Blue500  = 0xff2196f3;
        const color_t Blue600  = 0xff1e88e5;
        const color_t Blue700  = 0xff1976d2;
        const color_t Blue800  = 0xff1565c0;
        const color_t Blue900  = 0xff0d47a1;
        const color_t BlueA100 = 0xff82b1ff;
        const color_t BlueA200 = 0xff448aff;
        const color_t BlueA400 = 0xff2979ff;
        const color_t BlueA700 = 0xff2962ff;

        const color_t LightBlue50   = 0xffe1f5fe;
        const color_t LightBlue100  = 0xffb3e5fc;
        const color_t LightBlue200  = 0xff81d4fa;
        const color_t LightBlue300  = 0xff4fc3f7;
        const color_t LightBlue400  = 0xff29b6f6;
        const color_t LightBlue500  = 0xff03a9f4;
        const color_t LightBlue600  = 0xff039be5;
        const color_t LightBlue700  = 0xff0288d1;
        const color_t LightBlue800  = 0xff0277bd;
        const color_t LightBlue900  = 0xff01579b;
        const color_t LightBlueA100 = 0xff80d8ff;
        const color_t LightBlueA200 = 0xff40c4ff;
        const color_t LightBlueA400 = 0xff00b0ff;
        const color_t LightBlueA700 = 0xff0091ea;

        const color_t Cyan50   = 0xffe0f7fa;
        const color_t Cyan100  = 0xffb2ebf2;
        const color_t Cyan200  = 0xff80deea;
        const color_t Cyan300  = 0xff4dd0e1;
        const color_t Cyan400  = 0xff26c6da;
        const color_t Cyan500  = 0xff00bcd4;
        const color_t Cyan600  = 0xff00acc1;
        const color_t Cyan700  = 0xff0097a7;
        const color_t Cyan800  = 0xff00838f;
        const color_t Cyan900  = 0xff006064;
        const color_t CyanA100 = 0xff84ffff;
        const color_t CyanA200 = 0xff18ffff;
        const color_t CyanA400 = 0xff00e5ff;
        const color_t CyanA700 = 0xff00b8d4;

        const color_t Teal50   = 0xffe0f2f1;
        const color_t Teal100  = 0xffb2dfdb;
        const color_t Teal200  = 0xff80cbc4;
        const color_t Teal300  = 0xff4db6ac;
        const color_t Teal400  = 0xff26a69a;
        const color_t Teal500  = 0xff009688;
        const color_t Teal600  = 0xff00897b;
        const color_t Teal700  = 0xff00796b;
        const color_t Teal800  = 0xff00695c;
        const color_t Teal900  = 0xff004d40;
        const color_t TealA100 = 0xffa7ffeb;
        const color_t TealA200 = 0xff64ffda;
        const color_t TealA400 = 0xff1de9b6;
        const color_t TealA700 = 0xff00bfa5;

        const color_t Green50   = 0xffe8f5e9;
        const color_t Green100  = 0xffc8e6c9;
        const color_t Green200  = 0xffa5d6a7;
        const color_t Green300  = 0xff81c784;
        const color_t Green400  = 0xff66bb6a;
        const color_t Green500  = 0xff4caf50;
        const color_t Green600  = 0xff43a047;
        const color_t Green700  = 0xff388e3c;
        const color_t Green800  = 0xff2e7d32;
        const color_t Green900  = 0xff1b5e20;
        const color_t GreenA100 = 0xffb9f6ca;
        const color_t GreenA200 = 0xff69f0ae;
        const color_t GreenA400 = 0xff00e676;
        const color_t GreenA700 = 0xff00c853;

        const color_t LightGreen50   = 0xfff1f8e9;
        const color_t LightGreen100  = 0xffdcedc8;
        const color_t LightGreen200  = 0xffc5e1a5;
        const color_t LightGreen300  = 0xffaed581;
        const color_t LightGreen400  = 0xff9ccc65;
        const color_t LightGreen500  = 0xff8bc34a;
        const color_t LightGreen600  = 0xff7cb342;
        const color_t LightGreen700  = 0xff689f38;
        const color_t LightGreen800  = 0xff558b2f;
        const color_t LightGreen900  = 0xff33691e;
        const color_t LightGreenA100 = 0xffccff90;
        const color_t LightGreenA200 = 0xffb2ff59;
        const color_t LightGreenA400 = 0xff76ff03;
        const color_t LightGreenA700 = 0xff64dd17;

        const color_t Lime50   = 0xfff9ebe7;
        const color_t Lime100  = 0xfff0f4c3;
        const color_t Lime200  = 0xffe6ee9c;
        const color_t Lime300  = 0xffdce775;
        const color_t Lime400  = 0xffd4e157;
        const color_t Lime500  = 0xffcddc39;
        const color_t Lime600  = 0xffc0ca33;
        const color_t Lime700  = 0xffafb42b;
        const color_t Lime800  = 0xff9e9d24;
        const color_t Lime900  = 0xff827717;
        const color_t LimeA100 = 0xfff4ff81;
        const color_t LimeA200 = 0xffeeff41;
        const color_t LimeA400 = 0xffc6ff00;
        const color_t LimeA700 = 0xffaeea00;

        const color_t Yellow50   = 0xfffffde7;
        const color_t Yellow100  = 0xfffff9c4;
        const color_t Yellow200  = 0xfffff59d;
        const color_t Yellow300  = 0xfffff176;
        const color_t Yellow400  = 0xffffee58;
        const color_t Yellow500  = 0xffffeb3b;
        const color_t Yellow600  = 0xfffdd835;
        const color_t Yellow700  = 0xfffbc02d;
        const color_t Yellow800  = 0xfff9a825;
        const color_t Yellow900  = 0xfff57f17;
        const color_t YellowA100 = 0xffffff8d;
        const color_t YellowA200 = 0xffffff00;
        const color_t YellowA400 = 0xffffea00;
        const color_t YellowA700 = 0xffffd600;

        const color_t Amber50   = 0xfffff8e1;
        const color_t Amber100  = 0xffffecb3;
        const color_t Amber200  = 0xffffe082;
        const color_t Amber300  = 0xffffd54f;
        const color_t Amber400  = 0xffffca28;
        const color_t Amber500  = 0xffffc107;
        const color_t Amber600  = 0xffffb300;
        const color_t Amber700  = 0xffffa000;
        const color_t Amber800  = 0xffff8f00;
        const color_t Amber900  = 0xffff6f00;
        const color_t AmberA100 = 0xffffe57f;
        const color_t AmberA200 = 0xffffd740;
        const color_t AmberA400 = 0xffffc400;
        const color_t AmberA700 = 0xffffab00;

        const color_t Orange50   = 0xfffff3e0;
        const color_t Orange100  = 0xffffe0b2;
        const color_t Orange200  = 0xffffcc80;
        const color_t Orange300  = 0xffffb74d;
        const color_t Orange400  = 0xffffa726;
        const color_t Orange500  = 0xffff9800;
        const color_t Orange600  = 0xfffb8c00;
        const color_t Orange700  = 0xfff57c00;
        const color_t Orange800  = 0xffef6c00;
        const color_t Orange900  = 0xffe65100;
        const color_t OrangeA100 = 0xffffd180;
        const color_t OrangeA200 = 0xffffab40;
        const color_t OrangeA400 = 0xffff9100;
        const color_t OrangeA700 = 0xffff6d00;

        const color_t DeepOrange50   = 0xfffbe9e7;
        const color_t DeepOrange100  = 0xffffccbc;
        const color_t DeepOrange200  = 0xffffab91;
        const color_t DeepOrange300  = 0xffff8a65;
        const color_t DeepOrange400  = 0xffff7043;
        const color_t DeepOrange500  = 0xffff5722;
        const color_t DeepOrange600  = 0xfff4511e;
        const color_t DeepOrange700  = 0xffe64a19;
        const color_t DeepOrange800  = 0xffd84315;
        const color_t DeepOrange900  = 0xffbf360c;
        const color_t DeepOrangeA100 = 0xffff9e80;
        const color_t DeepOrangeA200 = 0xffff6e40;
        const color_t DeepOrangeA400 = 0xffff3d00;
        const color_t DeepOrangeA700 = 0xffdd2c00;

        const color_t Brown50  = 0xffefebe9;
        const color_t Brown100 = 0xffd7ccc8;
        const color_t Brown200 = 0xffbcaaa4;
        const color_t Brown300 = 0xffa1887f;
        const color_t Brown400 = 0xff8d6e63;
        const color_t Brown500 = 0xff795548;
        const color_t Brown600 = 0xff6d4c41;
        const color_t Brown700 = 0xff5d4037;
        const color_t Brown800 = 0xff4e342e;
        const color_t Brown900 = 0xff3e2723;

        const color_t Grey50  = 0xfffafafa;
        const color_t Grey100 = 0xfff5f5f5;
        const color_t Grey200 = 0xffeeeeee;
        const color_t Grey300 = 0xffe0e0e0;
        const color_t Grey400 = 0xffbdbdbd;
        const color_t Grey500 = 0xff9e9e9e;
        const color_t Grey600 = 0xff757575;
        const color_t Grey700 = 0xff616161;
        const color_t Grey800 = 0xff424242;
        const color_t Grey900 = 0xff212121;

        const color_t BlueGrey50  = 0xffeceff1;
        const color_t BlueGrey100 = 0xffcfd8dc;
        const color_t BlueGrey200 = 0xffb0bec5;
        const color_t BlueGrey300 = 0xff90a4ae;
        const color_t BlueGrey400 = 0xff78909c;
        const color_t BlueGrey500 = 0xff607d8b;
        const color_t BlueGrey600 = 0xff546e7a;
        const color_t BlueGrey700 = 0xff455a64;
        const color_t BlueGrey800 = 0xff37474f;
        const color_t BlueGrey900 = 0xff263238;

        const color_t Black  = 0xff000000;
        const color_t White  = 0xffffffff;
        const color_t Null   = 0x00000000;


        const color_t Red         = Red500;
        const color_t DarkRed     = Red900;
        const color_t Coral       = Red200;
        const color_t RichRed     = 0xffff0000;
        const color_t Pink        = Pink500;
        const color_t Rose        = PinkA100;
        const color_t Purple      = Purple500;
        const color_t Magenta     = PurpleA200;
        const color_t DarkMagenta = PurpleA700;
        const color_t DeepPurple  = DeepPurple500;
        const color_t Indigo      = Indigo500;
        const color_t Blue        = Blue500;
        const color_t DarkBlue    = Blue900;
        const color_t RichBlue    = 0xff0000ff;
        const color_t LightBlue   = LightBlue500;
        const color_t SkyBlue     = LightBlueA100;
        const color_t Navy        = LightBlue800;
        const color_t Cyan        = Cyan500;
        const color_t DarkCyan    = Cyan900;
        const color_t Teal        = Teal500;
        const color_t DarkTeal    = Teal900;
        const color_t Green       = Green500;
        const color_t DarkGreen   = Green900;
        const color_t RichGreen   = 0xff00ff00;
        const color_t LightGreen  = LightGreen500;
        const color_t Mint        = LightGreen900;
        const color_t Lime        = Lime500;
        const color_t Olive       = Lime900;
        const color_t Yellow      = Yellow500;
        const color_t RichYellow  = YellowA200;
        const color_t Amber       = Amber500;
        const color_t Gold        = Amber300;
        const color_t PaleGold    = AmberA100;
        const color_t Orange      = Orange500;
        const color_t Skin        = Orange100;
        const color_t DeepOrange  = DeepOrange500;
        const color_t Brick       = DeepOrange900;
        const color_t Brown       = Brown500;
        const color_t DarkBrown   = Brown900;
        const color_t CreamWhite  = Orange50;
        const color_t Wheat       = Amber100;
        const color_t Grey        = Grey500;
        const color_t Dark        = Grey900;
        const color_t Silver      = Grey300;
        const color_t BlueGrey    = BlueGrey500;

        const color_t Default = Wheat;
#else
        const color_t Default = 0xffffecb3;
#endif // #if !defined(EASY_OPTION_BUILTIN_COLORS) || EASY_OPTION_BUILTIN_COLORS == 0

	} // END of namespace colors.

} // END of namespace profiler.

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#endif // EASY_PROFILER_COLORS_H

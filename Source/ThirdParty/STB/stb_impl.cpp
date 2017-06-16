//
// Copyright (c) 2017 the Atomic project.
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
#define STB_RECT_PACK_IMPLEMENTATION 1
#include "stb_rect_pack.h"

#define STB_IMAGE_IMPLEMENTATION 1
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION 1
#include "stb_image_write.h"

#undef STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.h"

#define STB_TRUETYPE_IMPLEMENTATION 1
#include "stb_truetype.h"

// So far only imgui is using textedit. Also use of this header requires extensive configuration. Due to these reasons
// header implementation is not enabled here until other parts of project require it.
//#define STB_TEXTEDIT_IMPLEMENTATION 1
//#include "stb_textedit.h"


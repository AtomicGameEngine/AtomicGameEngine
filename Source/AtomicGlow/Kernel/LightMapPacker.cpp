// Copyright (c) 2014-2017, THUNDERBEAST GAMES LLC All rights reserved
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

#include <ThirdParty/STB/stb_rect_pack.h>

#include <Atomic/IO/Log.h>
#include <Atomic/Resource/Image.h>

#include "BakeMesh.h"
#include "LightMapPacker.h"

namespace AtomicGlow
{

LightMapPacker::LightMapPacker(Context* context) : Object(context)
{

}

LightMapPacker::~LightMapPacker()
{

}

void LightMapPacker::AddRadianceMap(RadianceMap* radianceMap)
{
    if (!radianceMap)
        return;

    radMaps_.Push(SharedPtr<RadianceMap>(radianceMap));

}


bool LightMapPacker::TryAddRadianceMap(RadianceMap* radMap)
{

    if (radMap->packed_)
        return false;

    const int numnodes = GlobalGlowSettings.lightmapSize_;

    SharedArrayPtr<unsigned char> nodes(new unsigned char[sizeof(stbrp_node) * numnodes]);
    SharedArrayPtr<unsigned char> rects(new unsigned char[sizeof(stbrp_rect) * (workingSet_.Size() + 1)]);

    stbrp_context rectContext;

    stbrp_init_target (&rectContext, numnodes, numnodes, (stbrp_node *) nodes.Get(), numnodes);
    stbrp_rect* rect = (stbrp_rect*) rects.Get();

    // add working set, we do this brute force for best results
    for (unsigned i = 0; i < workingSet_.Size(); i++)
    {
        RadianceMap* wmap = workingSet_[i];

        rect->id = (int) i;
        rect->w = wmap->GetWidth() + LIGHTMAP_PADDING * 2;
        rect->h = wmap->GetHeight() + LIGHTMAP_PADDING * 2;

        rect++;
    }

    rect->id = (int) workingSet_.Size();
    rect->w = radMap->GetWidth() + LIGHTMAP_PADDING * 2;
    rect->h = radMap->GetHeight() + LIGHTMAP_PADDING * 2;


    if (!stbrp_pack_rects (&rectContext, (stbrp_rect *)rects.Get(), workingSet_.Size() + 1))
    {
        return false;
    }

    return true;

}

void LightMapPacker::DilatedBlit(const Image* srcImage, Image* destImage, const IntRect& destRect)
{
    for (int y = 0; y < srcImage->GetHeight(); y++)
    {
        for (int x = 0; x < srcImage->GetWidth(); x++)
        {
            destImage->SetPixelInt(destRect.left_ + x + LIGHTMAP_PADDING,
                                   destRect.top_ + y + LIGHTMAP_PADDING,
                                   srcImage->GetPixelInt(x, y));
        }
    }

    // dilate top and bottom
    for (int x = 0; x < srcImage->GetWidth(); x++)
    {
        for (int i = 0; i < LIGHTMAP_PADDING; i++)
        {
            destImage->SetPixelInt(destRect.left_ + x + LIGHTMAP_PADDING,
                                   destRect.top_ + i,
                                   srcImage->GetPixelInt(x, 0));

            destImage->SetPixelInt(destRect.left_ + x + LIGHTMAP_PADDING,
                                   destRect.bottom_ - i,
                                   srcImage->GetPixelInt(x, srcImage->GetHeight() - 1));
        }

    }

    // dilate left and right
    for (int y = 0; y < srcImage->GetHeight(); y++)
    {
        for (int i = 0; i < LIGHTMAP_PADDING; i++)
        {
            destImage->SetPixelInt(destRect.left_ + i,
                                   destRect.top_ + y + LIGHTMAP_PADDING,
                                   srcImage->GetPixelInt(0, y));

            destImage->SetPixelInt(destRect.right_ - i,
                                   destRect.top_ + y + LIGHTMAP_PADDING,
                                   srcImage->GetPixelInt(srcImage->GetWidth() - 1, y));
        }

    }

    // dilate corners
    for (int y = LIGHTMAP_PADDING - 1; y >= 0 ; y--)
    {
        for (int x = LIGHTMAP_PADDING - 1; x >= 0 ; x--)
        {

            // upper left
            unsigned pixel = destImage->GetPixelInt(destRect.left_ + x + 1, destRect.top_ + y + 1);
            //pixel = Color::BLUE.ToUInt();
            destImage->SetPixelInt(destRect.left_ + x, destRect.top_ + y, pixel);

            // upper right
            pixel = destImage->GetPixelInt(destRect.right_ - x - 1, destRect.top_ + y + 1);
            //pixel = Color::RED.ToUInt();
            destImage->SetPixelInt(destRect.right_ - x, destRect.top_ + y, pixel);

            // lower left
            pixel = destImage->GetPixelInt(destRect.left_ + x + 1, destRect.bottom_ - y - 1);
            //pixel = Color::GREEN.ToUInt();
            destImage->SetPixelInt(destRect.left_ + x, destRect.bottom_ - y, pixel);

            // lower right
            pixel = destImage->GetPixelInt(destRect.right_ - x - 1, destRect.bottom_ - y - 1);
            //pixel = Color::MAGENTA.ToUInt();
            destImage->SetPixelInt(destRect.right_ - x, destRect.bottom_ - y, pixel);
        }
    }

}

void LightMapPacker::EmitLightmap(unsigned lightMapID)
{
    int width = GlobalGlowSettings.lightmapSize_;
    int height = GlobalGlowSettings.lightmapSize_;

    // see note in stbrp_init_target docs
    int numnodes = width;

    SharedArrayPtr<unsigned char> nodes(new unsigned char[sizeof(stbrp_node) * numnodes]);
    SharedArrayPtr<unsigned char> rects(new unsigned char[sizeof(stbrp_rect) * workingSet_.Size()]);

    stbrp_context rectContext;

    stbrp_init_target (&rectContext, width, height, (stbrp_node *) nodes.Get(), numnodes);
    stbrp_rect* rect = (stbrp_rect*) rects.Get();

    for (unsigned i = 0; i < workingSet_.Size(); i++)
    {
        RadianceMap* radMap = workingSet_[i];

        rect->id = (int) i;
        rect->w = radMap->GetWidth() + LIGHTMAP_PADDING * 2;
        rect->h = radMap->GetHeight() + LIGHTMAP_PADDING * 2;

        rect++;
    }

    if (!stbrp_pack_rects (&rectContext, (stbrp_rect *)rects.Get(), workingSet_.Size()))
    {
        ATOMIC_LOGERROR("SceneBaker::Light() - not all rects packed");
        return;
    }

    SharedPtr<Image> image(new Image(context_));
    image->SetSize(width, height, 3);
    image->Clear(Color::CYAN);

    rect = (stbrp_rect*) rects.Get();

    for (unsigned i = 0; i < workingSet_.Size(); i++)
    {
        RadianceMap* radMap = workingSet_[i];

        if (!rect->was_packed)
        {
            ATOMIC_LOGERROR("LightMapPacker::Light() - skipping unpacked lightmap");
            continue;
        }

        DilatedBlit(radMap->image_, image, IntRect(rect->x, rect->y, rect->x + rect->w - 1, rect->y + rect->h - 1));

        radMap->bakeMesh_->Pack(lightMapID, Vector4(float(radMap->image_->GetWidth())/float(width),
                                                    float(radMap->image_->GetHeight())/float(height),
                                                    float(rect->x + LIGHTMAP_PADDING)/float(width),
                                                    float(rect->y + LIGHTMAP_PADDING)/float(height)));

        rect++;
    }

    // dilate left and right maximum extents
    for (int i = 0; i < height; i++)
    {
        image->SetPixelInt(width -1, i, image->GetPixelInt(0, i));
    }

    for (int i = 0; i < width; i++)
    {
        image->SetPixelInt(i, height - 1, image->GetPixelInt(i, 0));
    }

    SharedPtr<LightMap> lightmap(new LightMap(context_));
    lightMaps_.Push(lightmap);

    lightmap->SetID(lightMapID);
    lightmap->SetImage(image);

    workingSet_.Clear();

}

void LightMapPacker::SaveLightmaps()
{
    for (unsigned i = 0; i < lightMaps_.Size(); i++)
    {
        LightMap* lightmap = lightMaps_[i];

#ifdef ATOMIC_PLATFORM_WINDOWS
        String filename = ToString("C:/Dev/atomic/AtomicExamplesPrivate/AtomicGlowTests/TestScene1/Resources/Textures/Scene_Lightmap%u.png", lightmap->GetID());
#else
        const char* format = GlobalGlowSettings.outputFormat_ == GLOW_OUTPUT_PNG ? "png" : "dds";
        String filename = ToString("%s/Resources/Textures/Scene_Lightmap%u.%s", GlobalGlowSettings.projectPath_.CString(), lightmap->GetID(), format);
#endif

        //ATOMIC_LOGINFOF("Saving Lightmap: %s", filename.CString());

        GlobalGlowSettings.outputFormat_ == GLOW_OUTPUT_PNG ? lightmap->GetImage()->SavePNG(filename) : lightmap->GetImage()->SaveDDS(filename);

    }

}

static bool CompareRadianceMap(RadianceMap* lhs, RadianceMap* rhs)
{
    int lhsWeight = lhs->GetWidth();
    lhsWeight += lhs->GetHeight();

    int rhsWeight = rhs->GetWidth();
    rhsWeight += rhs->GetHeight();

    // sort from biggest to smallest
    return lhsWeight > rhsWeight;
}


void LightMapPacker::Pack()
{
    unsigned lightmapID = 0;

    SharedPtr<LightMap> lightmap;

    Sort(radMaps_.Begin(), radMaps_.End(), CompareRadianceMap);

    for (unsigned i = 0; i < radMaps_.Size(); i++)
    {
        RadianceMap* radMap = radMaps_[i];

        if (radMap->packed_)
            continue;

        if (radMap->GetWidth() >= GlobalGlowSettings.lightmapSize_ || radMap->GetHeight() >= GlobalGlowSettings.lightmapSize_)
        {
            lightmap = new LightMap(context_);
            lightMaps_.Push(lightmap);

            lightmap->SetID(lightmapID);
            lightmap->SetImage(radMap->image_);

            radMap->bakeMesh_->Pack(lightmapID, Vector4(1.0f, 1.0f, 0.0f, 0.0f));

            lightmapID++;

            continue;
        }

        workingSet_.Push(radMap);

        for (unsigned j = 0; j < radMaps_.Size(); j++)
        {
            if (i == j)
                continue;

            RadianceMap* otherMap = radMaps_[j];

            if (TryAddRadianceMap(otherMap))
            {
                workingSet_.Push(otherMap);
            }

        }

        EmitLightmap(lightmapID++);
        workingSet_.Clear();

    }

}


}

//
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

#pragma once

#include <Atomic/Math/MathDefs.h>
#include <Atomic/Container/Str.h>
#include <Atomic/IO/VectorBuffer.h>

using namespace Atomic;

namespace AtomicGlow

{
    enum GlowPreset
    {
        GLOW_PRESET_FAST_LOW_QUALITY,
        GLOW_PRESET_MEDIUM_QUALITY,
        GLOW_PRESET_HIGH_QUALITY,
        GLOW_PRESET_SLOW_EXTREME_QUALITY
    };

    enum GlowOutputFormat
    {
        GLOW_OUTPUT_PNG,
        GLOW_OUTPUT_DDS
    };

    struct GlowSettings
    {
        int lightmapSize_;
        GlowOutputFormat outputFormat_;

        // global scalar
        float lexelDensity_;

        float sceneLexelDensityScale_;

        // global illumination
        bool giEnabled_;
        int giGranularity_;
        int giMaxBounces_;

        // ambient occlusion
        bool aoEnabled_;
        float aoDepth_;
        unsigned nsamples_;
        float aoMin_;
        float aoMultiply_;


        String projectPath_;

        GlowSettings()
        {
            SetDefaults();
            Validate();
        }

        void Pack(VectorBuffer& buffer) const
        {
            buffer.WriteInt(lightmapSize_);
            buffer.WriteUInt((unsigned) outputFormat_);

            buffer.WriteFloat(lexelDensity_);
            buffer.WriteFloat(sceneLexelDensityScale_);

            buffer.WriteBool(giEnabled_);
            buffer.WriteInt(giGranularity_);
            buffer.WriteInt(giMaxBounces_);

            buffer.WriteBool(aoEnabled_);
            buffer.WriteFloat(aoDepth_);
            buffer.WriteUInt(nsamples_);
            buffer.WriteFloat(aoDepth_);
            buffer.WriteFloat(aoMin_);
            buffer.WriteFloat(aoMultiply_);
        }

        void Unpack(VectorBuffer& buffer)
        {
            lightmapSize_ = buffer.ReadInt();
            outputFormat_ = (GlowOutputFormat) buffer.ReadUInt();

            lexelDensity_ = buffer.ReadFloat();
            sceneLexelDensityScale_ = buffer.ReadFloat();

            giEnabled_ = buffer.ReadBool();
            giGranularity_ = buffer.ReadInt();
            giMaxBounces_ = buffer.ReadInt();

            aoEnabled_ = buffer.ReadBool();
            aoDepth_ = buffer.ReadFloat();
            nsamples_ = buffer.ReadUInt( );
            aoDepth_= buffer.ReadFloat();
            aoMin_ = buffer.ReadFloat();
            aoMultiply_ = buffer.ReadFloat();
        }

        void Validate()
        {
            // always use 2048 for lightmap size
            lightmapSize_ = 2048;

            sceneLexelDensityScale_ = Atomic::Clamp<float>(sceneLexelDensityScale_, 0.01f, 1.0f);

            lexelDensity_ = Atomic::Clamp<float>(lexelDensity_, 0.01f, 1.0f);
            nsamples_ = Atomic::Clamp<unsigned>(nsamples_, 16, 256);

            giMaxBounces_ = Atomic::Clamp<int>(giMaxBounces_, 0, 8);
            giGranularity_ = Atomic::Clamp<int>(giGranularity_, 4, 16);

            aoDepth_ = Atomic::Clamp<float>(aoDepth_, 0.01f, 10.0f);
            aoMin_ = Atomic::Clamp<float>(aoMin_, 0.0f, 0.95f);
            aoMultiply_ = Atomic::Clamp<float>(aoMultiply_, 0.01f, 100.0f);
        }

        void SetDefaults(GlowPreset preset = GLOW_PRESET_FAST_LOW_QUALITY)
        {
            // fix me
            projectPath_ = "/Users/jenge/Dev/atomic/AtomicExamplesPrivate/AtomicGlowTests/TestScene1/";

            // common settings

            // lightmap size
            lightmapSize_ = 2048;

            giMaxBounces_ = 3;

            sceneLexelDensityScale_ = 0.35f;

            // TODO: Factor in DDS scene lighting loader, which have tested
            // and minimal artifacts with significant runtime memory savings
            outputFormat_ = GLOW_OUTPUT_PNG;

            aoEnabled_ = false;
            aoDepth_ = 0.25f;
            aoMin_ = 0.45f;
            aoMultiply_ = 1.0f;

            switch (preset)
            {
                case GLOW_PRESET_FAST_LOW_QUALITY:
                    lexelDensity_ = 0.16f;
                    nsamples_ = 16;
                    giEnabled_ = false;
                    break;
                case GLOW_PRESET_MEDIUM_QUALITY:
                    lexelDensity_ = 0.32f;
                    nsamples_ = 64;
                    giEnabled_ = true;
                    giGranularity_ = 8;
                    break;
                case GLOW_PRESET_HIGH_QUALITY:
                    lexelDensity_ = 0.5f;                    
                    giEnabled_ = true;
                    nsamples_ = 256;
                    giGranularity_ = 8;
                    break;
                case GLOW_PRESET_SLOW_EXTREME_QUALITY:
                    lexelDensity_ = 0.65f;
                    nsamples_ = 256;
                    giEnabled_ = true;
                    giGranularity_ = 8;
                    break;
            }
        }

    };

    extern GlowSettings GlobalGlowSettings;

}

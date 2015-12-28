//
// Copyright (c) 2008-2014 the Urho3D project.
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
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

#include "Precompiled.h"
#include "../Graphics/Graphics.h"
#include "../Math/Matrix3x4.h"
#include "../Graphics/ShaderVariation.h"
#include "../Graphics/Texture.h"
#include "../UI/UIBatch.h"

#include "../DebugNew.h"

namespace Atomic
{

#ifdef ATOMIC_OPENGL
static const float posAdjust = 0.0f;
#else
static const float posAdjust = 0.5f;
#endif

static const Vector3 posAdjustVec(posAdjust, posAdjust, 0.0f);

UIBatch::UIBatch() :
    blendMode_(BLEND_REPLACE),
    texture_(0),
    invTextureSize_(Vector2::ONE),
    vertexData_(0),
    vertexStart_(0),
    vertexEnd_(0)
{
    SetDefaultColor();
}

UIBatch::UIBatch(BlendMode blendMode, const IntRect& scissor, Texture* texture, PODVector<float>* vertexData) :
    blendMode_(blendMode),
    scissor_(scissor),
    texture_(texture),
    invTextureSize_(texture ? Vector2(1.0f / (float)texture->GetWidth(), 1.0f / (float)texture->GetHeight()) : Vector2::ONE),
    vertexData_(vertexData),
    vertexStart_(vertexData->Size()),
    vertexEnd_(vertexData->Size())
{
    SetDefaultColor();
}

void UIBatch::SetColor(const Color& color, bool overrideAlpha)
{
    useGradient_ = false;
    color_ = color.ToUInt();
}

void UIBatch::SetDefaultColor()
{
    color_ = 0xffffffff;
    useGradient_ = false;
}

void UIBatch::AddQuad(int x, int y, int width, int height, int texOffsetX, int texOffsetY, int texWidth, int texHeight)
{
    // If alpha is 0, nothing will be rendered, so do not add the quad
    if (!(color_ & 0xff000000))
        return;

    //const IntVector2& screenPos = element_->GetScreenPosition();
   IntVector2 screenPos(0, 0);

    float left = (float)(x + screenPos.x_) - posAdjust;
    float right = left + (float)width;
    float top = (float)(y + screenPos.y_) - posAdjust;
    float bottom = top + (float)height;

    float leftUV = texOffsetX * invTextureSize_.x_;
    float topUV = texOffsetY * invTextureSize_.y_;
    float rightUV = (texOffsetX + (texWidth ? texWidth : width)) * invTextureSize_.x_;
    float bottomUV = (texOffsetY + (texHeight ? texHeight : height)) * invTextureSize_.y_;

    unsigned begin = vertexData_->Size();
    vertexData_->Resize(begin + 6 * UI_VERTEX_SIZE);
    float* dest = &(vertexData_->At(begin));
    vertexEnd_ = vertexData_->Size();

    dest[0] = left; dest[1] = top; dest[2] = 0.0f;
    ((unsigned&)dest[3]) = color_;
    dest[4] = leftUV; dest[5] = topUV;

    dest[6] = right; dest[7] = top; dest[8] = 0.0f;
    ((unsigned&)dest[9]) = color_;
    dest[10] = rightUV; dest[11] = topUV;

    dest[12] = left; dest[13] = bottom; dest[14] = 0.0f;
    ((unsigned&)dest[15]) = color_;
    dest[16] = leftUV; dest[17] = bottomUV;

    dest[18] = right; dest[19] = top; dest[20] = 0.0f;
    ((unsigned&)dest[21]) = color_;
    dest[22] = rightUV; dest[23] = topUV;

    dest[24] = right; dest[25] = bottom; dest[26] = 0.0f;
    ((unsigned&)dest[27]) = color_;
    dest[28] = rightUV; dest[29] = bottomUV;

    dest[30] = left; dest[31] = bottom; dest[32] = 0.0f;
    ((unsigned&)dest[33]) = color_;
    dest[34] = leftUV; dest[35] = bottomUV;

    dest += 36;
}

void UIBatch::AddQuad(const Matrix3x4& transform, int x, int y, int width, int height, int texOffsetX, int texOffsetY,
    int texWidth, int texHeight)
{
    Vector3 v1 = (transform * Vector3((float)x, (float)y, 0.0f)) - posAdjustVec;
    Vector3 v2 = (transform * Vector3((float)x + (float)width, (float)y, 0.0f)) - posAdjustVec;
    Vector3 v3 = (transform * Vector3((float)x, (float)y + (float)height, 0.0f)) - posAdjustVec;
    Vector3 v4 = (transform * Vector3((float)x + (float)width, (float)y + (float)height, 0.0f)) - posAdjustVec;

    float leftUV = ((float)texOffsetX) * invTextureSize_.x_;
    float topUV = ((float)texOffsetY) * invTextureSize_.y_;
    float rightUV = ((float)(texOffsetX + (texWidth ? texWidth : width))) * invTextureSize_.x_;
    float bottomUV = ((float)(texOffsetY + (texHeight ? texHeight : height))) * invTextureSize_.y_;

    unsigned begin = vertexData_->Size();
    vertexData_->Resize(begin + 6 * UI_VERTEX_SIZE);
    float* dest = &(vertexData_->At(begin));
    vertexEnd_ = vertexData_->Size();

    dest[0] = v1.x_; dest[1] = v1.y_; dest[2] = 0.0f;
    ((unsigned&)dest[3]) = color_;
    dest[4] = leftUV; dest[5] = topUV;

    dest[6] = v2.x_; dest[7] = v2.y_; dest[8] = 0.0f;
    ((unsigned&)dest[9]) = color_;
    dest[10] = rightUV; dest[11] = topUV;

    dest[12] = v3.x_; dest[13] = v3.y_; dest[14] = 0.0f;
    ((unsigned&)dest[15]) = color_;
    dest[16] = leftUV; dest[17] = bottomUV;

    dest[18] = v2.x_; dest[19] = v2.y_; dest[20] = 0.0f;
    ((unsigned&)dest[21]) = color_;
    dest[22] = rightUV; dest[23] = topUV;

    dest[24] = v4.x_; dest[25] = v4.y_; dest[26] = 0.0f;
    ((unsigned&)dest[27]) = color_;
    dest[28] = rightUV; dest[29] = bottomUV;

    dest[30] = v3.x_; dest[31] = v3.y_; dest[32] = 0.0f;
    ((unsigned&)dest[33]) = color_;
    dest[34] = leftUV; dest[35] = bottomUV;
}

void UIBatch::AddQuad(int x, int y, int width, int height, int texOffsetX, int texOffsetY, int texWidth, int texHeight, bool tiled)
{

    if (!tiled)
    {
        AddQuad(x, y, width, height, texOffsetX, texOffsetY, texWidth, texHeight);
        return;
    }

    int tileX = 0;
    int tileY = 0;
    int tileW = 0;
    int tileH = 0;

    while (tileY < height)
    {
        tileX = 0;
        tileH = Min(height - tileY, texHeight);

        while (tileX < width)
        {
            tileW = Min(width - tileX, texWidth);

            AddQuad(x + tileX, y + tileY, tileW, tileH, texOffsetX, texOffsetY, tileW, tileH);

            tileX += tileW;
        }

        tileY += tileH;
    }
}

bool UIBatch::Merge(const UIBatch& batch)
{
    if (batch.blendMode_ != blendMode_ ||
        batch.scissor_ != scissor_ ||
        batch.texture_ != texture_ ||
        batch.vertexData_ != vertexData_ ||
        batch.vertexStart_ != vertexEnd_)
        return false;

    vertexEnd_ = batch.vertexEnd_;
    return true;
}

void UIBatch::AddOrMerge(const UIBatch& batch, PODVector<UIBatch>& batches)
{
    if (batch.vertexEnd_ == batch.vertexStart_)
        return;

    if (!batches.Empty() && batches.Back().Merge(batch))
        return;

    batches.Push(batch);
}

}

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

#include <Atomic/IO/Log.h>
#include <Atomic/Resource/ResourceCache.h>

#include "BakeModel.h"

namespace AtomicGlow
{


BakeModelCache::BakeModelCache(Context* context) : Object(context)
{

}

BakeModelCache::~BakeModelCache()
{

}

BakeModel* BakeModelCache::GetBakeModel(Model *model)
{
    if (!model)
        return 0;

    SharedPtr<BakeModel> bakeModel;

    if (bakeCache_.TryGetValue(model->GetName(), bakeModel))
    {
        return bakeModel;
    }

    bakeModel = new BakeModel(context_);

    if (!bakeModel->LoadModel(model))
    {
        return 0;
    }

    bakeCache_[model->GetName()] = bakeModel;

    return bakeModel;

}

BakeModel::BakeModel(Context* context) : Object(context)
{

}

BakeModel::~BakeModel()
{

}

bool BakeModel::LoadModel(Model *model)
{

    modelPacker_ = new ModelPacker(context_);

    ATOMIC_LOGINFOF("Unpacking model: %s", model->GetName().CString());

    if (!modelPacker_->Unpack(model))
        return false;

    /*
    for (unsigned i = 0; i < modelPacker_->lodLevels_.Size(); i++)
    {
        GenerateLODLevelAOMap(modelPacker_->lodLevels_[i]);
    }
    */

    return true;
}


}

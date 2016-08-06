//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
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

#include "AssetImporter.h"

namespace Atomic
{
    class Node;
    class Animation;
}

using namespace Atomic;

namespace ToolCore
{

class AnimationImportInfo : public Object
{
    friend class ModelImporter;

    ATOMIC_OBJECT(AnimationImportInfo, Object);

public:    

    AnimationImportInfo(Context* context) : Object(context), startTime_(-1.0f), endTime_(-1.0f)
    {

    }

    const String& GetName() const { return name_; }
    float GetStartTime() const { return startTime_; }
    float GetEndTime() const { return endTime_; }

    void SetName(const String& name) { name_ = name; }
    void SetStartTime(float time) { startTime_ = time; }
    void SetEndTime(float time) { endTime_ = time; }

private:

    String name_;
    float startTime_;
    float endTime_;
};


class ModelImporter : public AssetImporter
{
    ATOMIC_OBJECT(ModelImporter, AssetImporter)

public:

    /// Construct.
    ModelImporter(Context* context, Asset* asset);
    virtual ~ModelImporter();

    virtual void SetDefaults();

    double GetScale() { return scale_; }
    void SetScale(double scale) {scale_ = scale; }

    bool GetImportAnimations() { return importAnimations_; }
    void SetImportAnimations(bool importAnimations) { importAnimations_ = importAnimations; }
    bool GetImportMaterials() { return importMaterials_; }
    void SetImportMaterials(bool importMat) { importMaterials_ = importMat; };

    unsigned GetAnimationCount();
    void SetAnimationCount(unsigned count);

    Resource* GetResource(const String& typeName = String::EMPTY);

    void GetAnimations(PODVector<Atomic::Animation *>& animations);

    AnimationImportInfo* GetAnimationInfo(unsigned index) { return animationInfo_[index]; }

    /// Instantiate a node from the asset
    Node* InstantiateNode(Node* parent, const String& name);

protected:

    bool Import();

    bool ImportModel();
    bool ImportAnimations();
    bool ImportAnimation(const String &filename, const String& name, float startTime=-1.0f, float endTime=-1.0f);

    virtual bool LoadSettingsInternal(JSONValue& jsonRoot);
    virtual bool SaveSettingsInternal(JSONValue& jsonRoot);


    double scale_;
    bool importAnimations_;
    bool importMaterials_;
    Vector<SharedPtr<AnimationImportInfo>> animationInfo_;

    SharedPtr<Node> importNode_;

};

}

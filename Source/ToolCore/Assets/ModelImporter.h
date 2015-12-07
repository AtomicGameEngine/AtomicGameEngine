//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
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

public:

    OBJECT(AnimationImportInfo);

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
    OBJECT(ModelImporter);

public:

    /// Construct.
    ModelImporter(Context* context, Asset* asset);
    virtual ~ModelImporter();

    virtual void SetDefaults();

    double GetScale() { return scale_; }
    void SetScale(double scale) {scale_ = scale; }

    bool GetImportAnimations() { return importAnimations_; }
    void SetImportAnimations(bool importAnimations) { importAnimations_ = importAnimations; }

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
    Vector<SharedPtr<AnimationImportInfo>> animationInfo_;

    SharedPtr<Node> importNode_;

};

}

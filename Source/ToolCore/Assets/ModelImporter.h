
#pragma once

#include "AssetImporter.h"

namespace Atomic
{
    class Node;
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

    float GetScale() { return scale_; }
    void SetScale(float scale) {scale_ = scale; }

    bool GetImportAnimations() { return importAnimations_; }
    void SetImportAnimations(bool importAnimations) { importAnimations_ = importAnimations; }

    unsigned GetAnimationCount();
    void SetAnimationCount(unsigned count);

    AnimationImportInfo* GetAnimationInfo(unsigned index) { return animationInfo_[index]; }

protected:

    bool Import();

    bool ImportModel();
    bool ImportAnimations();
    bool ImportAnimation(const String &filename, const String& name, float startTime=-1.0f, float endTime=-1.0f);

    virtual bool LoadSettingsInternal();
    virtual bool SaveSettingsInternal();


    float scale_;
    bool importAnimations_;
    Vector<SharedPtr<AnimationImportInfo>> animationInfo_;

    SharedPtr<Node> importNode_;

};

}

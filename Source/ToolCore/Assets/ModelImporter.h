
#pragma once

#include "AssetImporter.h"

namespace ToolCore
{

class ModelImporter : public AssetImporter
{
    OBJECT(ModelImporter);

public:

    /// Construct.
    ModelImporter(Context* context);
    virtual ~ModelImporter();

    virtual void SetDefaults();

    float GetScale() { return scale_; }
    void SetScale(float scale) {scale_ = scale; }

    float GetStartTime() { return startTime_; }
    void SetStartTime(float startTime) { startTime_ = startTime; }

    float GetEndTime() { return endTime_; }
    void SetEndTime(float endTime) { endTime_ = endTime; }

    bool GetImportAnimations() { return importAnimations_; }
    void SetImportAnimations(bool importAnimations) { importAnimations_ = importAnimations; }

    bool Import(const String& guid);

protected:

    virtual bool LoadSettingsInternal();
    virtual bool SaveSettingsInternal();

    float scale_;
    float startTime_;
    float endTime_;
    bool importAnimations_;

};

}

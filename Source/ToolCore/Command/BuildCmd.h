
#pragma once

#include "Command.h"

using namespace Atomic;

namespace ToolCore
{

class BuildCmd: public Command
{
    OBJECT(BuildCmd);

public:

    BuildCmd(Context* context);
    virtual ~BuildCmd();

    bool Parse(const Vector<String>& arguments, unsigned startIndex, String& errorMsg);

    void Run();

private:

    void HandleBuildComplete(StringHash eventType, VariantMap& eventData);
    void HandleBuildFailed(StringHash eventType, VariantMap& eventData);

    String buildPlatform_;

};

}

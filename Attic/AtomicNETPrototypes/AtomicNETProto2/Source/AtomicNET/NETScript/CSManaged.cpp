
#include <ThirdParty/SDL/include/SDL.h>

#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Core/StringUtils.h>

#include "../NETCore/NETCore.h"

#include "CSManaged.h"
#include "CSComponent.h"

namespace Atomic
{


CSManaged::CSManaged(Context* context) :
    Object(context),
    CSComponentCreate_(0),
    CSComponentApplyFields_(0),
    CSComponentCallMethod_(0),
    CSBeginSendEvent_(0),
    NETUpdate_(0)
{

}

CSManaged::~CSManaged()
{

}

bool CSManaged::Initialize()
{
    NETCore* core = GetSubsystem<NETCore>();
    core->CreateDelegate("AtomicNETEngine", "AtomicEngine.ComponentCore", "CSComponentCreate", (void**) &CSComponentCreate_);
    core->CreateDelegate("AtomicNETEngine", "AtomicEngine.ComponentCore", "CSComponentApplyFields", (void**) &CSComponentApplyFields_);
    core->CreateDelegate("AtomicNETEngine", "AtomicEngine.NativeCore", "NETUpdate", (void**) &NETUpdate_);

    return true;
}

void CSManaged::NETUpdate(float timeStep)
{
    if (!NETUpdate_)
        return;

    NETUpdate_(timeStep);
}

CSComponent* CSManaged::CSComponentCreate(const String& assemblyName, const String& componentName)
{
    if (!CSComponentCreate_)
        return 0;

    return CSComponentCreate_(assemblyName.CString(), componentName.CString());

}

void CSManaged::CSComponentApplyFields(CSComponent* component, NETVariantMap* fieldMapPtr)
{
    if (!CSComponentApplyFields_ || !component || !fieldMapPtr)
        return;

    CSComponentApplyFields_(component, fieldMapPtr);

}

void CSManaged::CSComponentCallMethod(unsigned id, CSComponentMethod methodID, float value)
{
    if (!CSComponentCallMethod_)
        return;

    CSComponentCallMethod_(id, methodID, value);

}

void CSManaged::CSBeginSendEvent(unsigned senderRefID, unsigned eventType, VariantMap* eventData)
{
    if (!CSBeginSendEvent_)
        return;

    CSBeginSendEvent_(senderRefID, eventType, eventData);

}

}


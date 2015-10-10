
#include <ThirdParty/SDL/include/SDL.h>

#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Core/StringUtils.h>

#include "NETCore.h"
#include "NETManaged.h"

#include "CSEventHelper.h"
#include "CSComponent.h"

namespace Atomic
{


NETManaged::NETManaged(Context* context) :
    Object(context),
    CSComponentCreate_(0),
    CSComponentCallMethod_(0),
    CSBeginSendEvent_(0),
    NETUpdate_(0)
{

}

NETManaged::~NETManaged()
{

}

bool NETManaged::Initialize()
{
    NETCore* core = GetSubsystem<NETCore>();
    core->CreateDelegate("AtomicNETEngine", "AtomicEngine.ComponentCore", "CSComponentCreate", (void**) &CSComponentCreate_);
    core->CreateDelegate("AtomicNETEngine", "AtomicEngine.NativeCore", "NETUpdate", (void**) &NETUpdate_);

    return true;
}

void NETManaged::SetNETUpdate(NETUpdateFunctionPtr ptr)
{
    NETUpdate_ = ptr;
}

void NETManaged::NETUpdate(float timeStep)
{
    if (!NETUpdate_)
        return;

    NETUpdate_(timeStep);
}

CSComponent* NETManaged::CSComponentCreate(const String& assemblyName, const String& componentName)
{
    if (!CSComponentCreate_)
        return 0;

    return CSComponentCreate_(assemblyName.CString(), componentName.CString());

}

void NETManaged::CSComponentCallMethod(unsigned id, CSComponentMethod methodID, float value)
{
    if (!CSComponentCallMethod_)
        return;

    CSComponentCallMethod_(id, methodID, value);

}

void NETManaged::CSBeginSendEvent(unsigned senderRefID, unsigned eventType, VariantMap* eventData)
{
    if (!CSBeginSendEvent_)
        return;

    CSBeginSendEvent_(senderRefID, eventType, eventData);

}

void NETManaged::SetCSComponentCreate(CSComponentCreateFunctionPtr ptr)
{
    CSComponentCreate_ = ptr;
}

void NETManaged::SetCSComponentCallMethod(CSComponentCallMethodFunctionPtr ptr)
{
    CSComponentCallMethod_ = ptr;
}

void NETManaged::SetCSBeginSendEvent(CSBeginSendEventFunctionPtr ptr)
{
    CSBeginSendEvent_ = ptr;
}

}


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

#include <AtomicNET/NETCore/NETCore.h>
#include <AtomicNET/NETCore/NETManaged.h>
#include <AtomicNET/NETCore/NETCoreThunk.h>
#include <AtomicNET/NETCore/CSScriptObject.h>

#include "AtomicSharpAPI.h"

#ifdef ATOMIC_PLATFORM_WINDOWS
#pragma warning(disable: 4244) // possible loss of data
#define ATOMIC_EXPORT_API __declspec(dllexport)
#else
#define ATOMIC_EXPORT_API
#endif

using namespace Atomic;

static AtomicNETCoreThunk_t AtomicNETCoreThunk;
static bool AtomicNETCoreThunkEnabled = false;

extern "C"
{

ATOMIC_EXPORT_API void csb_package_set_Atomic_NETCore_thunk(AtomicNETCoreThunk_t* thunkIn)
{
    AtomicNETCoreThunk = *thunkIn;
    AtomicNETCoreThunkEnabled = true;
}

ATOMIC_EXPORT_API void csb_AtomicEngine_AtomicInterop_Set_CSComponentCreate(CSComponentCreateFunctionPtr method)
{
    if (AtomicNETCoreThunkEnabled)
    {
        AtomicNETCoreThunk.__Atomic_NETManaged_SetCSComponentCreate(method);
    }
    else
    {
        Atomic_NETManaged_SetCSComponentCreate(method);
    }
}


ATOMIC_EXPORT_API void csb_AtomicEngine_AtomicInterop_Set_CSComponentCallMethod(CSComponentCallMethodFunctionPtr method)
{
    if (AtomicNETCoreThunkEnabled)
    {
        AtomicNETCoreThunk.__Atomic_NETManaged_SetCSComponentCallMethod(method);
    }
    else
    {
        Atomic_NETManaged_SetCSComponentCallMethod(method);
    }
}

// Event Handling

ATOMIC_EXPORT_API void csb_AtomicEngine_AtomicInterop_Set_CSBeginSendEvent(CSBeginSendEventFunctionPtr method)
{
    if (AtomicNETCoreThunkEnabled)
    {
        AtomicNETCoreThunk.__Atomic_NETManaged_SetCSBeginSendEvent(method);
    }
    else
    {
        Atomic_NETManaged_SetCSBeginSendEvent(method);
    }

}

// Instance methods

ATOMIC_EXPORT_API void csb_Atomic_AObject_SendEvent(Object* object, const char* eventType)
{
    if (AtomicNETCoreThunkEnabled)
    {
        AtomicNETCoreThunk.__Atomic_AObject_SendEvent(object, eventType);
    }
    else
    {
        Atomic_AObject_SendEvent(object, eventType);
    }
}


ATOMIC_EXPORT_API ClassID csb_Atomic_RefCounted_GetClassID(RefCounted* refCounted)
{
    if (AtomicNETCoreThunkEnabled)
    {
        return AtomicNETCoreThunk.__Atomic_RefCounted_GetClassID(refCounted);
    }
    else
    {
        return Atomic_RefCounted_GetClassID(refCounted);
    }
}

ATOMIC_EXPORT_API RefCounted* csb_AtomicEngine_GetSubsystem(const char* name)
{
    if (AtomicNETCoreThunkEnabled)
    {
        return AtomicNETCoreThunk.__AtomicEngine_GetSubsystem(name);
    }
    else
    {
        return AtomicEngine_GetSubsystem(name);
    }
}

ATOMIC_EXPORT_API void csb_Atomic_RefCounted_SafeAddRef(unsigned id)
{
    if (AtomicNETCoreThunkEnabled)
    {
        AtomicNETCoreThunk.__Atomic_RefCounted_SafeAddRef(id);
    }
    else
    {
        Atomic_RefCounted_SafeAddRef(id);
    }

}

ATOMIC_EXPORT_API void csb_Atomic_RefCounted_SafeReleaseRef(unsigned id)
{
    if (AtomicNETCoreThunkEnabled)
    {
        AtomicNETCoreThunk.__Atomic_RefCounted_SafeReleaseRef(id);
    }
    else
    {
        Atomic_RefCounted_SafeReleaseRef(id);
    }

}

ATOMIC_EXPORT_API unsigned csb_Atomic_StringToStringHash(const char* stringValue)
{
    if (AtomicNETCoreThunkEnabled)
    {
        return AtomicNETCoreThunk.__Atomic_StringToStringHash(stringValue);
    }
    else
    {
        return Atomic_StringToStringHash(stringValue);
    }
}

// Variant Map

ATOMIC_EXPORT_API RefCounted* csb_Atomic_VariantMap_GetInstance(VariantMap& vmap, const char* key)
{
    if (AtomicNETCoreThunkEnabled)
    {
        return AtomicNETCoreThunk.__Atomic_VariantMap_GetInstance(vmap, key);
    }
    else
    {
        return Atomic_VariantMap_GetInstance(vmap, key);
    }

}

}

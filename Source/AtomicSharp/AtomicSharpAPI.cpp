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

#include "AtomicSharp.h"
#include "AtomicSharpAPI.h"
#include "CSScriptObject.h"

#ifdef ATOMIC_PLATFORM_WINDOWS
#pragma warning(disable: 4244) // possible loss of data
#define ATOMIC_EXPORT_API __declspec(dllexport)
#else
#define ATOMIC_EXPORT_API
#endif

using namespace Atomic;

extern "C"
{

typedef CSComponent* (*CSComponentCreatePtr)(const char* csComponentTypeName);
CSComponentCreatePtr _CSComponentCreate = 0;
ATOMIC_EXPORT_API void csb_AtomicEngine_AtomicInterop_Set_CSComponentCreate(CSComponentCreatePtr method)
{
    _CSComponentCreate = method;
}

CSComponent* CSComponentCreate(String name)
{
    assert(_CSComponentCreate);
    return _CSComponentCreate(name.CString());
}


typedef void (*CSComponentCallMethodPtr)(unsigned id, CSComponentMethod method, float value);
CSComponentCallMethodPtr _CSComponentCallMethod = 0;
ATOMIC_EXPORT_API void csb_AtomicEngine_AtomicInterop_Set_CSComponentCallMethod(CSComponentCallMethodPtr method)
{
    _CSComponentCallMethod = method;
}

void CSComponentCallMethod(unsigned id, CSComponentMethod methodID, float value)
{
    assert(_CSComponentCreate);
    _CSComponentCallMethod(id, methodID, value);
}

// Event Handling

typedef void (*CSBeginSendEventPtr)(unsigned senderRefID, unsigned eventType, VariantMap* eventData);
CSBeginSendEventPtr _CSBeginSendEvent = 0;
ATOMIC_EXPORT_API void csb_AtomicEngine_AtomicInterop_Set_CSBeginSendEvent(CSBeginSendEventPtr method)
{
    _CSBeginSendEvent = method;
}

void CSBeginSendEvent(unsigned senderRefID, unsigned eventType, VariantMap* eventData)
{
    assert(_CSBeginSendEvent);
    _CSBeginSendEvent(senderRefID, eventType, eventData);
}

// Instance methods

ATOMIC_EXPORT_API RefCounted* csb_Atomic_CSComponent_Constructor()
{
   return new CSComponent(AtomicSharp::GetContext());
}

ATOMIC_EXPORT_API RefCounted* csb_Atomic_CSScriptObject_Constructor()
{
   return new CSScriptObject(AtomicSharp::GetContext());
}

ATOMIC_EXPORT_API void csb_Atomic_AObject_SendEvent(Object* object, const char* eventType)
{
   object->SendEvent(eventType);
}


ATOMIC_EXPORT_API ClassID csb_Atomic_RefCounted_GetClassID(RefCounted* refCounted)
{
    if (!refCounted)
        return 0;

    return refCounted->GetClassID();
}

ATOMIC_EXPORT_API RefCounted* csb_AtomicEngine_GetSubsystem(const char* name)
{
    return AtomicSharp::GetContext()->GetSubsystem(name);
}

ATOMIC_EXPORT_API void csb_Atomic_RefCounted_SafeAddRef(unsigned id)
{
    RefCounted* ref = RefCounted::GetByID(id);
    if (ref)
        ref->AddRef();

}

ATOMIC_EXPORT_API void csb_Atomic_RefCounted_SafeReleaseRef(unsigned id)
{
    RefCounted* ref = RefCounted::GetByID(id);
    if (ref)
        ref->ReleaseRef();

}

ATOMIC_EXPORT_API unsigned csb_Atomic_StringToStringHash(const char* stringValue)
{
    StringHash hash = stringValue;
    return hash.Value();
}

// Variant Map

ATOMIC_EXPORT_API RefCounted* csb_Atomic_VariantMap_GetInstance(VariantMap& vmap, const char* key)
{
    return vmap[key].GetPtr();
}


}

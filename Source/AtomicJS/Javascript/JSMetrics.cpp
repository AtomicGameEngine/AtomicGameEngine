
#include "JSVM.h"
#include "JSMetrics.h"

namespace Atomic
{

JSMetrics::JSMetrics(Context* context, JSVM *vm) : Object(context),
    vm_(vm)
{

}

JSMetrics::~JSMetrics()
{

}

void JSMetrics::Dump()
{
    List<String>::ConstIterator itr = classNames_.Begin();
    while (itr != classNames_.End())
    {
        const String& classname = *itr;
        LOGINFOF("%s %i", classname.CString(), classInstances_[classname] );
        itr++;
    }

}

void JSMetrics::Capture()
{
    classNames_.Clear();
    classInstances_.Clear();

    HashMap<void*, RefCounted*>::ConstIterator itr = vm_->heapToObject_.Begin();
    while (itr != vm_->heapToObject_.End())
    {
        String classname = "RefCounted";
        if (itr->second_->IsObject())
        {
            classname = ((Object*) itr->second_)->GetTypeName();
        }

        if (!classNames_.Contains(classname))
        {
            classNames_.Push(classname);
            classInstances_[classname] = 1;
        }
        else
        {
            classInstances_[classname]++;
        }

        itr++;
    }

}

}

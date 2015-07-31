
#include <Atomic/Container/Sort.h>
#include <Atomic/Scene/Node.h>

#include "JSVM.h"
#include "JSComponent.h"
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

static bool CompareObjectMetrics(const JSMetrics::ObjectMetric& lhs, const JSMetrics::ObjectMetric& rhs)
{
    return lhs.count > rhs.count;
}

static bool CompareNodeMetrics(const JSMetrics::NodeMetric& lhs, const JSMetrics::NodeMetric& rhs)
{
    return lhs.count > rhs.count;
}


void JSMetrics::Dump()
{
    Vector<ObjectMetric> sorted;

    HashMap<StringHash, ObjectMetric>::ConstIterator itr = objectMetrics_.Begin();
    while (itr != objectMetrics_.End())
    {
        sorted.Push(itr->second_);
        itr++;
    }

    Sort(sorted.Begin(), sorted.End(), CompareObjectMetrics);

    Vector<ObjectMetric>::ConstIterator vitr = sorted.Begin();
    while (vitr != sorted.End())
    {
        const String& classname = (*vitr).classname;
        LOGINFOF("%s %i", classname.CString(), objectMetrics_[classname].count);
        vitr++;
    }

}


void JSMetrics::DumpNodes()
{
    Vector<NodeMetric> sorted;

    HashMap<StringHash, NodeMetric>::ConstIterator itr = nodeMetrics_.Begin();
    while (itr != nodeMetrics_.End())
    {
        sorted.Push(itr->second_);
        itr++;
    }

    Sort(sorted.Begin(), sorted.End(), CompareNodeMetrics);

    Vector<NodeMetric>::ConstIterator vitr = sorted.Begin();
    while (vitr != sorted.End())
    {
        const String& nodename = (*vitr).name;
        LOGINFOF("%s %i", nodename.CString(), nodeMetrics_[nodename].count);
        vitr++;
    }

}


void JSMetrics::DumpJSComponents()
{
    Vector<String> jsnames;
    HashMap<StringHash, int> jscomponents;

    HashMap<void*, RefCounted*>::ConstIterator itr = vm_->heapToObject_.Begin();
    while (itr != vm_->heapToObject_.End())
    {
        String classname = "RefCounted";
        if (itr->second_->IsObject())
        {
            classname = ((Object*) itr->second_)->GetTypeName();
        }

        /*
        if (classname == "JSComponent")
        {
            JSComponent* jsc = (JSComponent*) itr->second_;
            const String& classname = jsc->GetClassName();

            if (jsc->GetDestroyed())
            {
                // this is an error
                LOGINFOF("Destroyed: %s Node: %p JSHeapPtr: %p", jsc->GetClassName().CString(), (void*) jsc->GetNode(), (void*) jsc->JSGetHeapPtr());
            }

            if (!jscomponents.Contains(classname))
            {
                jsnames.Push(classname);
                jscomponents[classname] = 1;
            }
            else
                jscomponents[classname]++;
        }
        */

        itr++;
    }

    for (unsigned i = 0; i < jsnames.Size(); i++)
    {
        const String& classname = jsnames[i];
        LOGINFOF("%s %i", classname.CString(), jscomponents[classname]);
    }

}

void JSMetrics::Capture()
{
    objectMetrics_.Clear();
    nodeMetrics_.Clear();

    // run twice to call finalizers
    // see duktape docs
    duk_gc(vm_->GetJSContext(), 0);
    duk_gc(vm_->GetJSContext(), 0);


    HashMap<void*, RefCounted*>::ConstIterator itr = vm_->heapToObject_.Begin();
    while (itr != vm_->heapToObject_.End())
    {
        String classname = "RefCounted";
        if (itr->second_->IsObject())
        {
            classname = ((Object*) itr->second_)->GetTypeName();

            if (classname == "Node")
            {
                String nodename = ((Node*) itr->second_)->GetName();

                if (nodename == String::EMPTY)
                {
                    nodename = String("(Anonymous)");
                }

                if (!nodeMetrics_.Contains(nodename))
                {
                    JSMetrics::NodeMetric metric;
                    metric.name = nodename;
                    metric.count = 1;
                    nodeMetrics_[nodename] = metric;
                }
                else
                {
                    nodeMetrics_[nodename].count++;
                }

            }

        }



        if (!objectMetrics_.Contains(classname))
        {
            JSMetrics::ObjectMetric metric;
            metric.classname = classname;
            metric.count = 1;

            objectMetrics_[classname] = metric;
        }
        else
        {
            objectMetrics_[classname].count++;
        }

        itr++;
    }

}

}

//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
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
        ATOMIC_LOGINFOF("%s %i", classname.CString(), objectMetrics_[classname].count);
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
        ATOMIC_LOGINFOF("%s %i", nodename.CString(), nodeMetrics_[nodename].count);
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
        ATOMIC_LOGINFOF("%s %i", classname.CString(), jscomponents[classname]);
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

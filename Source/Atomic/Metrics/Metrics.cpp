//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
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

#include "../IO/Log.h"

#include "../Scene/Node.h"
#include "../Script/ScriptComponent.h"
#include "../Metrics/Metrics.h"

#ifdef ATOMIC_PLATFORM_WEB
#include <stdio.h>
#include <stdlib.h>
#endif


namespace Atomic
{

Metrics* Metrics::metrics_ = 0;
bool Metrics::everEnabled_ = false;

bool MetricsSnapshot::CompareInstanceMetrics(const MetricsSnapshot::InstanceMetric& lhs, const MetricsSnapshot::InstanceMetric& rhs)
{
    // TODO: Introduce various sorting modes, for now alphabetical is best "only" option

    return lhs.classname < rhs.classname;

    /*
    if (lhs.count == rhs.count)
        return lhs.classname < rhs.classname;

    return lhs.count > rhs.count;
    */
}

void MetricsSnapshot::Clear()
{
    instanceMetrics_.Clear();
    nodeMetrics_.Clear();
    resourceMetrics_.Clear();
}

void MetricsSnapshot::RegisterInstance(const String& classname, InstantiationType instantiationType, int count)
{
    InstanceMetric *metric = &instanceMetrics_[classname];

    if (!metric->classname.Length())
        metric->classname = classname;

    metric->count += count;

    switch (instantiationType)
    {
    case INSTANTIATION_NATIVE:
        metric->nativeInstances++;
        break;
    case INSTANTIATION_JAVASCRIPT:
        metric->jsInstances++;
        break;
    case INSTANTIATION_NET:
        metric->netInstances++;
        break;
    }

}

String MetricsSnapshot::PrintData(unsigned columns, unsigned minCount)
{
    String output;

    Vector<MetricsSnapshot::InstanceMetric> instanceSort;

    HashMap<StringHash, InstanceMetric>::ConstIterator instanceItr = instanceMetrics_.Begin();

    while (instanceItr != instanceMetrics_.End())
    {
        instanceSort.Push(instanceItr->second_);
        instanceItr++;
    }

    Sort(instanceSort.Begin(), instanceSort.End(), CompareInstanceMetrics);

    Vector<MetricsSnapshot::InstanceMetric>::ConstIterator instanceSortItr = instanceSort.Begin();

    static const int NAME_MAX_LENGTH = 20;
    static const int ENTRY_MAX_LENGTH = 128;
    
    char name[NAME_MAX_LENGTH];
    char entry[ENTRY_MAX_LENGTH];

    String line;

    String header = "Class                 Total ( Native /  JS  /  C# )     ";

    for (unsigned i = 0; i < columns; i++)
        output += header;

    output += "\n\n";

    unsigned column = 0;

    while (instanceSortItr != instanceSort.End())
    {
        const InstanceMetric& metric = *instanceSortItr;

        if (metric.count < minCount)
            continue;

        snprintf(name, NAME_MAX_LENGTH, "%-20s", metric.classname.CString());
        
        // We use snprintf here as ToString doesn't seem to cover the unsigned %5u format
        snprintf(entry, ENTRY_MAX_LENGTH, "%s : %5u ( %5u, %5u, %5u )", name , metric.count, metric.nativeInstances, metric.jsInstances, metric.netInstances);

        if (columns == 1 || column == columns)
        {
            line += "\n";
            output += line;
            line.Clear();                        
            column = 0;
        }

        line += String(entry);
        column++;
        line += "     ";

        instanceSortItr++;
    }

    if (line.Length())
        output += line;

    return output;

}

Metrics::Metrics(Context* context) :
    Object(context),
    enabled_(false)
{    
    Metrics::metrics_ = this;
}

Metrics::~Metrics()
{
    Disable();
    Metrics::metrics_ = 0;
}

void Metrics::ProcessInstances()
{
    const static StringHash scriptComponentType("ScriptComponent");
    const static StringHash jsComponentType("JSComponent");
    const static StringHash csComponentType("CSComponent");

    RefCountedInfo ninfo;
    PODVector<RefCounted*>::ConstIterator itr = processInstances_.Begin();

    while (itr != processInstances_.End())
    {
        RefCounted* r = *itr;

        Object* o = r->IsObject() ? (Object *)r : 0;

        const String& typeName = r->GetTypeName();

        ninfo.refCounted = r;
        ninfo.instantiationType = r->GetInstantiationType();

        ninfo.typeID = StringHash(typeName);

        if (!names_.Contains(ninfo.typeID))
        {
            names_[ninfo.typeID] = typeName;
        }
        
        ninfo.typeNameOverride = StringHash::ZERO;
       
        // for script components, setup typeNameOverride
        if (o && o->IsInstanceOf(scriptComponentType))
        {
            const String& classname = ((ScriptComponent*)o)->GetComponentClassName();

            if (classname.Length())
            {
                String name;

                if (o->GetType() == jsComponentType)
                {
                    name = classname + " (JS)";
                }                    
                else
                {
                    name = classname + " (C#)";
                }

                ninfo.typeNameOverride = StringHash(name);

                if (!names_.Contains(ninfo.typeNameOverride))
                {
                    names_[ninfo.typeNameOverride] = name;
                }

            }
        }
        
        PODVector<RefCountedInfo>& infos = instances_[ninfo.typeID];

        RefCountedInfo* info = infos.Buffer();
        int count = (int) infos.Size();

        while (count > 0)
        {
            if (!info->refCounted)
            {
                *info = ninfo;
                break;
            }

            count--;
            info++;
        }

        if (!count)
        {
            if (infos.Capacity() <= (infos.Size() + 1))
            {
                infos.Reserve(infos.Size() + 32768);
            }

            infos.Push(ninfo);
        }

        itr++;
    }

    processInstances_.Clear();
}

void Metrics::CaptureInstances(MetricsSnapshot* snapshot)
{    
    ProcessInstances();

    HashMap<StringHash, PODVector<RefCountedInfo>>::ConstIterator itr = instances_.Begin();

    while (itr != instances_.End())
    {
        StringHash typeID = itr->first_;
        String* typeName = &names_[typeID];

        RefCountedInfo* info = itr->second_.Buffer();
        int count = (int) itr->second_.Size();

        while (count > 0)
        {
            // free spot
            if (!info->refCounted)
            {
                info++;
                count--;
                continue;
            }

            if (info->typeNameOverride != StringHash::ZERO)
                typeName = &names_[info->typeNameOverride];

            snapshot->RegisterInstance(*typeName, info->instantiationType);

            info++;
            count--;
        }

        itr++;
    }
}

void Metrics::Capture(MetricsSnapshot* snapshot)
{
    if (!enabled_)
    {
        ATOMIC_LOGERROR("Metrics::Capture - Metrics subsystem is not enabled");
        return;
    }

    if (!snapshot)
        return;

    snapshot->Clear();

    CaptureInstances(snapshot);

}

bool Metrics::Enable()
{
    if (enabled_)
        return false;

    if (everEnabled_)
    {
        ATOMIC_LOGERROR("Metrics::Enable - Metrics subsystem is not designed to be restarted");
        return false;
    }

    enabled_ = everEnabled_ = true;

    ATOMIC_LOGINFO("Metrics subsystem enabled, performance will be degraded and there may be stutter while instrumenting");
    ATOMIC_LOGINFO("IMPORTANT: Do not ship applications with performance metrics enabled");

    RefCounted::AddRefCountedCreatedFunction(Metrics::OnRefCountedCreated);
    RefCounted::AddRefCountedDeletedFunction(Metrics::OnRefCountedDeleted);

    return true;
}

void Metrics::Disable()
{
    if (!enabled_)
        return;

    enabled_ = false;

    instances_.Clear();

    RefCounted::RemoveRefCountedCreatedFunction(Metrics::OnRefCountedCreated);
    RefCounted::RemoveRefCountedDeletedFunction(Metrics::OnRefCountedDeleted);
}


String Metrics::PrintNodeNames() const
{
    const static StringHash nodeType("Node");

    String output;

    PODVector<RefCountedInfo>& infos = instances_[nodeType];

    PODVector<RefCountedInfo>::Iterator itr = infos.Begin();

    for (; itr != infos.End(); itr++)
    {
        RefCounted* r = itr->refCounted;

        if (!r)
            continue;

        Object* o = r->IsObject() ? (Object *)r : 0;

        if (!o)
            continue;

        if (o->GetType() == nodeType)
        {
            const String& name = ((Node*)o)->GetName();
            output.AppendWithFormat("Node: %s\n", name.Length() ? name.CString() : "Anonymous Node");
        }

    }

    return output;
}

void Metrics::AddRefCounted(RefCounted* refCounted)
{
    // We're called from the RefCounted constructor, so we don't know whether we're an object, etc

    if (processInstances_.Capacity() <= (processInstances_.Size() + 1))
    {
        processInstances_.Reserve(processInstances_.Size() + 32768);
    }

    processInstances_.Push(refCounted);

}

void Metrics::RemoveRefCounted(RefCounted* refCounted)
{
    processInstances_.Remove(refCounted);

    // this is called from RefCounted destructor, so can't access refCounted though pointer address is still valid
    // we also don't want to hash every RefCounted ptr due to possible collisions, so need to search:

    HashMap<StringHash, PODVector<RefCountedInfo>>::Iterator itr = instances_.Begin();

    while (itr != instances_.End())
    {
        RefCountedInfo* info = itr->second_.Buffer();
        int count = (int)itr->second_.Size();

        while (count > 0)
        {
            if (info->refCounted == refCounted)
            {
                info->refCounted = 0;
                break;
            }

            info++;
            count--;
        }

        if (count)
            break;

        itr++;
    }
}

void Metrics::OnRefCountedCreated(RefCounted* refCounted)
{
    if (!metrics_)
    {
        ATOMIC_LOGERROR("Metrics::OnRefCountedCreated - null instance");
        return;
    }

    metrics_->AddRefCounted(refCounted);
}

void Metrics::OnRefCountedDeleted(RefCounted* refCounted)
{
    if (!metrics_)
    {
        ATOMIC_LOGERROR("Metrics::OnRefCountedDeleted - null instance");
        return;
    }

    metrics_->RemoveRefCounted(refCounted);    
}


}

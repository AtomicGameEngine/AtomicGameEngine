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

void Metrics::CaptureInstances(MetricsSnapshot* snapshot)
{
    const String unkClassName("-Unknown Class-");

    for (unsigned i = 0; i < instances_.Size(); i++)
    {
        RefCounted* r = instances_[i];

        const String& name = r->GetTypeName();

        MetricsSnapshot::InstanceMetric& metric = snapshot->instanceMetrics_[name];

        metric.classname = name;
        metric.count++;

        if (r->GetInstantiationType() == INSTANTIATION_NATIVE)
            metric.nativeInstances++;
        else if (r->GetInstantiationType() == INSTANTIATION_JAVASCRIPT)
            metric.jsInstances++;
        else if (r->GetInstantiationType() == INSTANTIATION_NET)
            metric.netInstances++;
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

void Metrics::OnRefCountedCreated(RefCounted* refCounted)
{
    if (!metrics_)
    {
        ATOMIC_LOGERROR("Metrics::OnRefCountedCreated - null instance");
        return;
    }

    // We're called from the RefCounted constructor, so we don't know whether we're an object, etc
    metrics_->instances_.Push(refCounted);

}

void Metrics::OnRefCountedDeleted(RefCounted* refCounted)
{
    if (!metrics_)
    {
        ATOMIC_LOGERROR("Metrics::OnRefCountedDeleted - null instance");
        return;
    }

    Vector<RefCounted*>::Iterator itr = metrics_->instances_.Find(refCounted);

    if (itr != metrics_->instances_.End())
    {
        metrics_->instances_.Erase(itr);
    }
}


}

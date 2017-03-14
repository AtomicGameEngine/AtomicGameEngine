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

#pragma once

#include "../Core/Object.h"

namespace Atomic
{

class MetricsSnapshot : public RefCounted
{
    friend class Metrics;

    ATOMIC_REFCOUNTED(MetricsSnapshot)

public:

    MetricsSnapshot() {}

    String PrintData(unsigned columns = 1, unsigned minCount = 0);

    void Clear();

private:

    struct InstanceMetric
    {
        String classname;
        unsigned count;
        unsigned nativeInstances;
        unsigned jsInstances;
        unsigned netInstances;

        InstanceMetric()
        {
            count = nativeInstances = jsInstances = netInstances = 0;
        }

    };

    struct NodeMetric
    {
        String name;
        unsigned count;

        NodeMetric()
        {
            count = 0;
        }
    };

    struct ResourceMetric
    {
        String name;
        String group;
        unsigned memoryUsage;

        ResourceMetric()
        {
            memoryUsage = 0;
        }
    };

    static bool CompareInstanceMetrics(const InstanceMetric& lhs, const InstanceMetric& rhs);

    // StringHash(classname) => InstanceMetrics
    HashMap<StringHash, InstanceMetric> instanceMetrics_;

    // StringHash(node name) => NodeMetrics
    HashMap<StringHash, NodeMetric> nodeMetrics_;

    // StringHash(resource name) => ResourceMetrics
    HashMap<StringHash, ResourceMetric> resourceMetrics_;

};

/// Metrics subsystem
class ATOMIC_API Metrics : public Object
{
    friend class Application;

    ATOMIC_OBJECT(Metrics, Object)

public:

    /// Construct.
    Metrics(Context* context);
    /// Destruct.
    virtual ~Metrics();

    bool Enable();

    bool GetEnabled() const { return enabled_; }    

    void Capture(MetricsSnapshot* snapshot);

private:    

    void Disable();

    void CaptureInstances(MetricsSnapshot* snapshot);

    static void OnRefCountedCreated(RefCounted* refCounted);
    static void OnRefCountedDeleted(RefCounted* refCounted);

    static Metrics* metrics_;

    static bool everEnabled_;

    bool enabled_;

    Vector<RefCounted*> instances_;

};


}

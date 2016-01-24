//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//
// Wraps the performance API / performance API polyfill
import performance = require("modules/usertiming");

const METRICS_ENABLED = true;

/**
 * Decorator to wrap a function with enter/exit profile
 * @param  {Object} target
 * @param  {string} propertyKey
 * @param  {TypedPropertyDescriptor<any>} descriptor
 * @return {TypedPropertyDesciptor<any>}
 */
export function profileDecorator(target: Object, propertyKey: string, descriptor: TypedPropertyDescriptor<any>) {
    const originalMethod = descriptor.value;

    descriptor.value = function(...args: any[]) {
        let result;
        start(propertyKey);
        try {
            result = originalMethod.apply(this, args);
        } finally {
            stop(propertyKey);
            return result;
        }
    };

    return descriptor;
}

/**
 * add a start mark to the profiler
 * @param  {string} key
 */
export function start(key: string) {
    if (METRICS_ENABLED) {
        performance.mark(key + "_start");
    }
}

/**
 * Add a stop mark to the profiler and capture a measurement
 * @param  {string} key
 */
export function stop(key: string) {
    if (METRICS_ENABLED) {
        performance.mark(key + "_stop");
        performance.measure(key, key + "_start", key + "_stop");
    }

}

export interface MetricRollup {
    key: string;
    timesCalled: number;
    avgDuration: number;
    totalDuration: number;
}

/**
 * Returns an array of metric rollups
 * @return {[MetricRollup]}
 */
export function getMetricRollups(): MetricRollup[] {
    if (METRICS_ENABLED) {
        const metrics = {};

        performance.getEntriesByType("measure").forEach((entry) => {
            if (!metrics[entry.name]) {
                metrics[entry.name] = [];
            }
            metrics[entry.name].push(entry);
        });
        const vals = [];
        for (let mname in metrics) {
            let totalduration = 0;
            let metricBlock = metrics[mname];
            for (let i = 0; i < metricBlock.length; i++) {
                totalduration += metricBlock[i].duration;
            }
            vals.push({
                key: mname,
                timesCalled: metricBlock.length,
                avgDuration: totalduration / metricBlock.length,
                totalDuration: totalduration
            });
        }
        return vals;
    } else {
        return [];
    }
}

/**
 * Write metrics to the console
 */
export function logMetrics() {
    getMetricRollups().forEach((metric) => {
        console.log(`${metric.key}  Called: ${metric.timesCalled}, Average Duration:${metric.avgDuration}, Total Duration: ${metric.totalDuration}`);
    });
}

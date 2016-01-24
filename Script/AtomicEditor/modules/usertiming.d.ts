declare interface PerformanceEntryFilterOptions {
    // the name of a performance entry.
    name?: string;
    // the entry type. The valid entry types are listed in the PerformanceEntry.entryType method.
    entryType?: string;

    // the type of the initiating resource (for example an HTML element). The values are defined by the
    initiatorType?: string;
}

declare interface PerformanceEntry {
    // A DOMString representing the name of a performance entry when the metric was created.
    name: string;
    // A DOMString representing the type of performance metric such as "mark". See entryType for a list of valid values.
    entryType: string;
    // A DOMHighResTimeStamp representing the starting time for the performance metric.
    startTime: Date;
    // A DOMHighResTimeStamp representing the time value of the duration of the performance event.
    duration: Number;
}

declare interface Performance {
    mark(name: string);
    measure(name: string, startMark: string, endMark: string);
    getEntries();
    getEntries(performanceEntryFilterOptions: PerformanceEntryFilterOptions): PerformanceEntry[];
    getEntriesByType(entryType:string);
}


declare const performance: Performance;
export = performance;

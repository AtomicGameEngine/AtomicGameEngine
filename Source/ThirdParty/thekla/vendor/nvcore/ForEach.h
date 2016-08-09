// This code is in the public domain -- Ignacio Castaño <castano@gmail.com>

#pragma once
#ifndef NV_CORE_FOREACH_H
#define NV_CORE_FOREACH_H

/*
These foreach macros are very non-standard and somewhat confusing, but I like them.
*/

#include "nvcore.h"

#if NV_CC_GNUC // If typeof is available:

#define NV_FOREACH(i, container) \
    typedef typeof(container) NV_STRING_JOIN2(cont,__LINE__); \
    for(NV_STRING_JOIN2(cont,__LINE__)::PseudoIndex i((container).start()); !(container).isDone(i); (container).advance(i))
/*
#define NV_FOREACH(i, container) \
for(typename typeof(container)::PseudoIndex i((container).start()); !(container).isDone(i); (container).advance(i))
*/

#else // If typeof not available:

#include <new> // placement new

struct PseudoIndexWrapper {
    template <typename T>
    PseudoIndexWrapper(const T & container) {
        nvStaticCheck(sizeof(typename T::PseudoIndex) <= sizeof(memory));
        new (memory) typename T::PseudoIndex(container.start());
    }
    // PseudoIndex cannot have a dtor!

    template <typename T> typename T::PseudoIndex & operator()(const T * /*container*/) {
        return *reinterpret_cast<typename T::PseudoIndex *>(memory);
    }
    template <typename T> const typename T::PseudoIndex & operator()(const T * /*container*/) const {
        return *reinterpret_cast<const typename T::PseudoIndex *>(memory);
    }

    uint8 memory[4];	// Increase the size if we have bigger enumerators.
};

#define NV_FOREACH(i, container) \
    for(PseudoIndexWrapper i(container); !(container).isDone(i(&(container))); (container).advance(i(&(container))))

#endif

// Declare foreach keyword.
#if !defined NV_NO_USE_KEYWORDS
#   define foreach NV_FOREACH
#endif


#endif // NV_CORE_FOREACH_H

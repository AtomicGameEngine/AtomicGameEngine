# UserTiming.js

v0.1.7

Copyright 2014 Nic Jansma

http://nicj.net

Licensed under the MIT license

## Introduction

UserTiming.js is a polyfill that adds [UserTiming](http://www.w3.org/TR/user-timing/) support to browsers
that do not natively support it.

UserTiming is accessed via the [PerformanceTimeline](http://www.w3.org/TR/performance-timeline/), and requires
[``window.performance.now()``](http://www.w3.org/TR/hr-time/) support, so UserTiming.js
adds a limited version of these interfaces if the browser does not support them (which is likely the case if the browser
does not natively support UserTiming).

As of 2013-04-15, UserTiming is natively supported by the following browsers:

* IE 10+
* Chrome 25+ (prefixed)

UserTiming.js has been verified to add UserTiming support to the following browsers:

* IE 6-9
* Firefox 3.6+ (previous versions not tested)
* Safari 4.0.5+ (previous versions not tested)
* Opera 10.50+ (previous versions not tested)

UserTiming.js will detect native implementations of UserTiming, ``window.performance.now()`` and
the PerformanceTimeline and will not make any changes if those interfaces already exist.  When a prefixed version
is found, it is copied over to the unprefixed name.

## Download

Releases are available for download from [GitHub](https://github.com/nicjansma/usertiming.js).

__Development:__ [usertiming.js](https://github.com/nicjansma/usertiming.js/raw/master/src/usertiming.js)
    - 18.1kb

__Production:__ [usertiming.min.js](https://github.com/nicjansma/usertiming.js/raw/master/dist/usertiming.min.js)
    - 1,187b (minified / gzipped)

usertiming.js is also available as the [npm usertiming module](https://npmjs.org/package/usertiming). You can install
using  Node Package Manager (npm):

    npm install usertiming

usertiming.js is also available via [bower](http://bower.io/). You can install using:

    bower install usertiming

## Usage

Please see the [W3C UserTiming API Reference](http://www.w3.org/TR/user-timing/) for details on how to use the
UserTiming API.

To include the UserTiming.js polyfill, simply include it via a script tag:

```html
<script type="text/javascript" src="usertiming.min.js"></script>
```

## Disadvantages of UserTiming.js over native implementations

UserTiming.js provides a 100% functional JavaScript implementation of UserTiming. However, implementing the API in
JavaScript has some disadvantages over native (built into the browser) implementations.

1. If the browser does not natively support
    [DOMHighResolutionTimestamps / ``window.performance.now()``](http://www.w3.org/TR/hr-time/), UserTiming.js adds a
    small shim (via the `Date` object) to mock this interface. However, DOMHighResolutionTimestamp provides higher
    precision (0.1 milliseconds or better) than the native `Date` object can (1.0 millisecond -- or worse in older
    browsers).  So if `window.performance.now()` has to be mocked, it will not be as precise as native implementations.

2. UserTiming marks and measures are queried via the [PerformanceTimeline](http://www.w3.org/TR/performance-timeline/),
    for example by using `getEntries()`, `getEntriesByType()` or `getEntriesByName()`.  UserTiming.js adds these
    interfaces so you can query for marks and measures, but they obviously will not support any other PerformanceEntrys
    such as [ResourceTiming](http://www.w3.org/TR/resource-timing/).

3. UserTiming.js is implemented in JavaScript, so it will be less performant than a native implementation.  If you
    are calling `mark()` or `measure()` at a high-rate, you might incur a performance cost from the UserTiming.js
    JavaScript implementation versus a native implementation.

## Tests

### UserTiming.js tests

UserTiming.js tests are provided in the ``test/`` directory, and can be run via ``nodeunit``:

    nodeunit test/test.js

Or via ``grunt``:

    grunt test

The tests can also be run in a web browser:

    test/test.html

### W3C tests

The latest [W3C UserTiming tests](http://w3c-test.org/webperf/tests/#ut) (as of 2013-04-15) were copied into the
``test-w3c/`` directory and can be run in any browser to validate UserTiming.js.  In browsers that natively support
UserTiming, UserTiming.js does not change anything so the tests will be running against the native browser interface.

The following changes were made to the W3C test suite to work with UserTiming.js:

* The relative test harness JS/CSS urls were changed to point to the local ``test-w3c/`` directory
    (such as ``testharness.js``, ``webperftestharness.js``, etc)

* All tests were updated to add a reference to ``../src/usertiming.js`` so usertiming.js is actually used

* ``test_user_timing_measure.htm`` and ``test_user_timing_mark.htm``: The threshold was increased from 20ms to 50ms
    (due to inefficiencies in the test suite)

Additional notes:

* ``usertiming.js`` does **not pass** the IDL tests (``idlharness.html``), nor will it ever.

* The W3C test harness itself does not appear to work in several older browsers (IE <= 8, Firefox 3.6, etc).  The
    UserTiming.js [test cases](#Tests) should cover most of what the W3C tests are doing, and the nodeunit test harness
    works in these older browsers.

## Version History

* v0.1.0 - 2013-04-15: Initial version
* v0.1.1 - 2014-02-19: Updated dependencies, grunt lint task, bower package name
* v0.1.2 - 2014-02-19: Small bower.json fix
* v0.1.3 - 2014-08-07: Include dist/ dir in bower and npm packages
* v0.1.4 - 2014-10-28: Fix for Safari iOS 8
* v0.1.5 - 2015-01-12: Fix for FF 35
* v0.1.6 - 2015-02-01: Better FF 35 support (or any browser that has RT but not UT)
* v0.1.7 - 2015-09-14: Dev-only changes: Lint, switched from NodeUnit to Mocha

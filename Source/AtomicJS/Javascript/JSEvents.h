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

#pragma once

#include <Atomic/Core/Object.h>

namespace Atomic
{

/*
name	standard	Name of error, e.g. TypeError, inherited
message	standard	Optional message of error, own property, empty message inherited if absent
fileName	Rhino	Filename related to error source, inherited accessor
lineNumber	Rhino	Linenumber related to error source, inherited accessor
stack	V8	Traceback as a multi-line human redable string, inherited accessor
*/

ATOMIC_EVENT(E_JSOBJECTADDED, ObjectAdded)
{
    ATOMIC_PARAM(P_OBJECT, Object); // object
}

ATOMIC_EVENT(E_JSOBJECTREMOVED, ObjectRemoved)
{
    ATOMIC_PARAM(P_OBJECT, Object); // object
}

ATOMIC_EVENT(E_JSERROR, JSError)
{
    ATOMIC_PARAM(P_ERRORNAME, ErrorName); // string
    ATOMIC_PARAM(P_ERRORMESSAGE, ErrorMessage); // string
    ATOMIC_PARAM(P_ERRORFILENAME, ErrorFileName); // string
    ATOMIC_PARAM(P_ERRORLINENUMBER, ErrorLineNumber); // int
    ATOMIC_PARAM(P_ERRORSTACK, ErrorStack); // string
}

ATOMIC_EVENT(E_JSPRINT, JSPrint)
{
    ATOMIC_PARAM(P_TEXT, Text); // string
}

}

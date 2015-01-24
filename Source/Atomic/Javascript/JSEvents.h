
#pragma once

#include "../Core/Object.h"

namespace Atomic
{

/*
name	standard	Name of error, e.g. TypeError, inherited
message	standard	Optional message of error, own property, empty message inherited if absent
fileName	Rhino	Filename related to error source, inherited accessor
lineNumber	Rhino	Linenumber related to error source, inherited accessor
stack	V8	Traceback as a multi-line human redable string, inherited accessor
*/

EVENT(E_JSERROR, JSError)
{
    PARAM(P_ERRORNAME, ErrorName); // string
    PARAM(P_ERRORMESSAGE, ErrorMessage); // string
    PARAM(P_ERRORFILENAME, ErrorFileName); // string
    PARAM(P_ERRORLINENUMBER, ErrorLineNumber); // int
    PARAM(P_ERRORSTACK, ErrorStack); // string
}

EVENT(E_JSPRINT, JSPrint)
{
    PARAM(P_TEXT, Text); // string
}

}

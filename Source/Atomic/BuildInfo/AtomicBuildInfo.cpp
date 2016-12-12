
#include <Atomic/Core/StringUtils.h>
#include "AtomicGitSHA.h"
#include "AtomicBuildInfo.h"

namespace Atomic
{

String AtomicBuildInfo::GetBuildString()
{
    return ToString("(%s): %s %s Git: %s", GetBuildName().CString(), GetBuildDate().CString(), GetBuildTime().CString(), GetGitSHA().CString());
}

String AtomicBuildInfo::GetGitSHA()
{
    return ATOMIC_BUILDINFO_GITSHA;
}

String AtomicBuildInfo::GetBuildDate()
{
    return __DATE__;
}

String AtomicBuildInfo::GetBuildTime()
{
    return __TIME__;
}

String AtomicBuildInfo::GetBuildName()
{
#ifdef ATOMIC_BUILD_NAME
    return ATOMIC_BUILD_NAME;
#else
    return "The Unamable";
#endif
}

bool AtomicBuildInfo::GetDevBuild()
{
#ifdef ATOMIC_DEV_BUILD
    return true;
#else
    return false;
#endif
}

bool AtomicBuildInfo::GetDistBuild()
{
#ifdef ATOMIC_BUILD_DIST
    return true;
#else
    return false;
#endif
}

#ifndef ATOMIC_BUILD_VENDOR
static String buildVendor("Unknown Vendor");
#else
static String buildVendor(ATOMIC_BUILD_VENDOR);
#endif

const String& AtomicBuildInfo::GetBuildVendor()
{
    return buildVendor;
}



}

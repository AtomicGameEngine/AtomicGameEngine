//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <ThirdParty/PugiXml/src/pugixml.hpp>

#include "../Build/BuildIOS.h"
#include "../License/LicenseSystem.h"
#include "PlatformIOS.h"

#ifdef ATOMIC_PLATFORM_OSX

#include "PlatformIOSUtils.h"

#endif

namespace ToolCore
{

PlatformIOS::PlatformIOS(Context* context) : Platform(context)
{

}

PlatformIOS::~PlatformIOS()
{

}

String PlatformIOS::ParseProvisionAppIdentifierPrefix(const String& provisionFile)
{
#if defined(ATOMIC_PLATFORM_WINDOWS) || defined(ATOMIC_PLATFORM_LINUX)

    return String::EMPTY;

#else

    String pdata = GetMobileProvisionData(provisionFile.CString());

    if (!pdata.Length())
        return String::EMPTY;

    pugi::xml_document doc;

    if (!doc.load(pdata.CString()))
    {
        return String::EMPTY;
    }

    String AppIDName;
    String ApplicationIdentifierPrefix;

    pugi::xml_node dict = doc.document_element().child("dict");

    for (pugi::xml_node key = dict.child("key"); key; key = key.next_sibling("key"))
    {
        String keyName = key.child_value();

        if (keyName == "AppIDName")
        {
            pugi::xml_node value = key.next_sibling();
            if (!strcmp(value.name(), "string"))
                AppIDName = value.child_value();
        }
        else if (keyName == "ApplicationIdentifierPrefix")
        {
            pugi::xml_node array = key.next_sibling();
            if (!strcmp(array.name(), "array"))
            {
                pugi::xml_node value = array.first_child();
                if (!strcmp(value.name(), "string"))
                    ApplicationIdentifierPrefix = value.child_value();
            }

        }
    }

    return ApplicationIdentifierPrefix;

#endif

}

BuildBase* PlatformIOS::NewBuild(Project *project)
{
    return new BuildIOS(context_, project);
}

bool PlatformIOS::GetLicense()
{
// BEGIN LICENSE MANAGEMENT
    return GetSubsystem<LicenseSystem>()->GetLicenseIOS();
// END LICENSE MANAGEMENT
}


}

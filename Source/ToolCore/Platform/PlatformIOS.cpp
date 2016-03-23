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

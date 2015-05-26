#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Resource/JSONFile.h>

#include "JSBind.h"
#include "JSBModule.h"
#include "JSBPackage.h"

namespace ToolCore
{

Vector<SharedPtr<JSBPackage> > JSBPackage::allPackages_;

JSBPackage::JSBPackage(Context* context) : Object(context)
{

}

JSBPackage::~JSBPackage()
{

}

void JSBPackage::PreprocessModules()
{
    for (unsigned i = 0; i < modules_.Size(); i++)
    {
        modules_[i]->PreprocessHeaders();
    }
}

void JSBPackage::ProcessModules()
{
    for (unsigned i = 0; i < modules_.Size(); i++)
    {
        modules_[i]->VisitHeaders();
    }
}

JSBClass* JSBPackage::GetClass(const String& name)
{
    for (unsigned i = 0; i < modules_.Size(); i++)
    {
        JSBClass* cls = modules_[i]->GetClass(name);
        if (cls)
            return cls;
    }

    return 0;
}

JSBClass* JSBPackage::GetClassAllPackages(const String& name)
{
    for (unsigned i = 0; i < allPackages_.Size(); i++)
    {
        JSBClass* cls = allPackages_[i]->GetClass(name);
        if (cls)
            return cls;
    }

    return 0;

}

JSBEnum* JSBPackage::GetEnum(const String& name)
{
    for (unsigned i = 0; i < modules_.Size(); i++)
    {
        JSBEnum* cls = modules_[i]->GetEnum(name);
        if (cls)
            return cls;
    }

    return 0;
}

JSBEnum* JSBPackage::GetEnumAllPackages(const String& name)
{
    for (unsigned i = 0; i < allPackages_.Size(); i++)
    {
        JSBEnum* cls = allPackages_[i]->GetEnum(name);
        if (cls)
            return cls;
    }

    return 0;

}

bool JSBPackage::ContainsConstant(const String& constantName)
{
    for (unsigned i = 0; i < modules_.Size(); i++)
        if (modules_[i]->ContainsConstant(constantName))
            return true;

    return false;
}

bool JSBPackage::ContainsConstantAllPackages(const String& constantName)
{
    for (unsigned i = 0; i < allPackages_.Size(); i++)
    {
        if (allPackages_[i]->ContainsConstant(constantName))
            return true;
    }

    return false;

}

bool JSBPackage::Load(const String& packageFolder)
{
    LOGINFOF("Loading Package: %s", packageFolder.CString());

    SharedPtr<File> jsonFile(new File(context_, packageFolder + "Package.json"));

    if (!jsonFile->IsOpen())
    {
        LOGERRORF("Unable to open package json: %s", (packageFolder + "Package.json").CString());
        return false;
    }

    SharedPtr<JSONFile> packageJSON(new JSONFile(context_));

    if (!packageJSON->BeginLoad(*jsonFile))
    {
        LOGERRORF("Unable to parse package json: %s", (packageFolder + "Package.json").CString());
        return false;
    }

    JSONValue root = packageJSON->GetRoot();

    // first load dependencies
    JSONValue deps = root.GetChild("dependencies");

    if (deps.IsArray())
    {
        JSBind* jsbind = GetSubsystem<JSBind>();

        for (unsigned i = 0; i < deps.GetSize(); i++)
        {
            String dpackageFolder = AddTrailingSlash(deps.GetString(i));

            SharedPtr<JSBPackage> depPackage (new JSBPackage(context_));

            if (!depPackage->Load(jsbind->GetSourceRootFolder() + dpackageFolder))
            {
                LOGERRORF("Unable to load package dependency: %s", dpackageFolder.CString());
                return false;
            }

        }

    }

    name_ = root.GetString("name");
    namespace_ = root.GetString("namespace");

    JSONValue modules = root.GetChild("modules");

    for (unsigned i = 0; i < modules.GetSize(); i++)
    {
        String moduleName = modules.GetString(i);

        SharedPtr<JSBModule> module(new JSBModule(context_, this));

        if (!module->Load(packageFolder + moduleName + ".json"))
        {
            LOGERRORF("Unable to load module json: %s", (packageFolder + moduleName + ".json").CString());
            return false;
        }

        modules_.Push(module);

    }

    allPackages_.Push(SharedPtr<JSBPackage>(this));

    PreprocessModules();
    ProcessModules();

    return true;
}

}

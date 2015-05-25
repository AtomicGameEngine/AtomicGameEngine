#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Resource/JSONFile.h>

#include "Atomic/Core/ProcessUtils.h"

#include "JSBind.h"
#include "JSBPackage.h"
#include "JSBModule.h"
#include "JSBHeader.h"
#include "JSBClass.h"
#include "JSBEnum.h"

namespace ToolCore
{

JSBModule::JSBModule(Context* context, JSBPackage* package) : Object(context),
    package_(package)
{

}

JSBModule::~JSBModule()
{

}

void JSBModule::PreprocessHeaders()
{
    for (unsigned i = 0; i < headers_.Size(); i++)
    {
        headers_[i]->VisitPreprocess();
    }
}

void JSBModule::ScanHeaders()
{
    JSBind* jsbind = GetSubsystem<JSBind>();
    FileSystem* fs = GetSubsystem<FileSystem>();

    const String& sourceRoot = jsbind->GetSourceRootFolder();

    for (unsigned i = 0; i < sourceDirs_.Size(); i++)
    {
        const String& dir = sourceRoot + sourceDirs_[i] + "/";

        Vector<String> fileNames;
        fs->ScanDir(fileNames, dir, "*.h", SCAN_FILES, false);

        for (unsigned k = 0; k < fileNames.Size(); k++)
        {
            String filepath = dir + fileNames[k];

            SharedPtr<JSBHeader> header(new JSBHeader(context_, this, filepath));

            // Parse the C++ header
            header->Parse();

            headers_.Push(header);
        }

    }

}

JSBClass* JSBModule::GetClass(const String& name)
{
    if (classes_.Contains(name))
        return classes_[name];

    return 0;
}

void JSBModule::RegisterClass(String name)
{
    String nativeName = name;

    if (classnames_.Contains(name))
    {
        if (classRenames_.Contains(name))
        {
            name = classRenames_[name];
        }

        if (JSBPackage::GetClassAllPackages(nativeName))
        {
            ErrorExit(ToString("Class collision: %s", name.CString()));
        }

        JSBClass* cls = new JSBClass(context_, this, name, nativeName);

        classes_[nativeName] = cls;

        LOGINFOF("Registered Class: %s", cls->GetName().CString());
    }
}

void JSBModule::RegisterEnum(JSBEnum* jenum)
{
    if (JSBPackage::GetClassAllPackages(jenum->GetName()))
    {
        ErrorExit(ToString("Enum collision: %s", jenum->GetName().CString()));
    }

    enums_[jenum->GetName()] = jenum;

    LOGINFOF("Registered Enum: %s", jenum->GetName().CString());
}

JSBEnum* JSBModule::GetEnum(const String& name)
{
    if (enums_.Contains(name))
    {
        return enums_[name];
    }

    return 0;

}

bool JSBModule::ContainsConstant(const String& constantName)
{
    return constants_.Contains(constantName);
}

void JSBModule::RegisterConstant(const String& constantName)
{
    if (JSBPackage::ContainsConstantAllPackages(constantName))
    {
        ErrorExit(ToString("Constant collision: %s", constantName.CString()));
    }

    constants_.Push(constantName);

}

bool JSBModule::Load(const String& jsonFilename)
{
    LOGINFOF("Loading Module: %s", jsonFilename.CString());

    SharedPtr<File> jsonFile(new File(context_, jsonFilename));

    if (!jsonFile->IsOpen())
    {
        LOGERRORF("Unable to open module json: %s", jsonFilename.CString());
        return false;
    }

    SharedPtr<JSONFile> moduleJSON(new JSONFile(context_));

    if (!moduleJSON->BeginLoad(*jsonFile))
    {
        LOGERRORF("Unable to parse module json: %s", jsonFilename.CString());
        return false;
    }

    JSONValue root = moduleJSON->GetRoot();

    name_ = root.GetString("name");

    JSONValue classes = root.GetChild("classes");

    for (unsigned i = 0; i < classes.GetSize(); i++)
    {
        classnames_.Push(classes.GetString(i));
    }

    JSONValue classes_rename = root.GetChild("classes_rename");

    if (classes_rename.IsObject())
    {
        Vector<String> childNames = classes_rename.GetValueNames();
        for (unsigned j = 0; j < childNames.Size(); j++)
        {
            String classname = childNames.At(j);
            String crename = classes_rename.GetString(classname);

            classRenames_[classname] = crename;

        }

    }

    JSONValue sources = root.GetChild("sources");

    for (unsigned i = 0; i < sources.GetSize(); i++)
    {
        sourceDirs_.Push(sources.GetString(i));
    }

    ScanHeaders();

    return true;
}

}

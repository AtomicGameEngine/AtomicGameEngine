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
#include "JSBModuleWriter.h"
#include "JSBType.h"

namespace ToolCore
{

JSBModule::JSBModule(Context* context, JSBPackage* package) : Object(context),
    package_(package)
{

}

JSBModule::~JSBModule()
{

}

Vector<SharedPtr<JSBClass>> JSBModule::GetClasses()
{
    return classes_.Values();
}

Vector<SharedPtr<JSBEnum>> JSBModule::GetEnums()
{
    return enums_.Values();
}

void JSBModule::PreprocessHeaders()
{
    for (unsigned i = 0; i < headers_.Size(); i++)
    {
        headers_[i]->VisitPreprocess();
    }
}

void JSBModule::VisitHeaders()
{
    for (unsigned i = 0; i < headers_.Size(); i++)
    {
        headers_[i]->VisitHeader();
    }

    // validate that all classes found
    for (unsigned i = 0; i < classnames_.Size(); i++)
    {
        JSBClass* cls = GetClass(classnames_[i]);

        if (!cls)
        {
            ErrorExit(ToString("Module class not found %s", classnames_[i].CString()));
        }
    }

    ProcessOverloads();
    ProcessExcludes();
    ProcessTypeScriptDecl();
    ProcessHaxeDecl();
}

void JSBModule::PreprocessClasses()
{
    HashMap<StringHash, SharedPtr<JSBClass> >::Iterator itr;

    for (itr = classes_.Begin(); itr != classes_.End(); itr++)
    {
        itr->second_->Preprocess();
    }
}

void JSBModule::ProcessClasses()
{
    HashMap<StringHash, SharedPtr<JSBClass> >::Iterator itr;

    for (itr = classes_.Begin(); itr != classes_.End(); itr++)
    {
        itr->second_->Process();
    }
}

void JSBModule::PostProcessClasses()
{
    HashMap<StringHash, SharedPtr<JSBClass> >::Iterator itr;

    for (itr = classes_.Begin(); itr != classes_.End(); itr++)
    {
        itr->second_->PostProcess();
    }
}

void JSBModule::ProcessOverloads()
{
    // overloads

    JSONValue root = moduleJSON_->GetRoot();

    JSONValue overloads = root.GetChild("overloads");

    if (overloads.IsObject())
    {
        Vector<String> childNames = overloads.GetChildNames();

        for (unsigned j = 0; j < childNames.Size(); j++)
        {
            String classname = childNames.At(j);

            JSBClass* klass = GetClass(classname);

            if (!klass)
            {
                ErrorExit("Bad overload klass");
            }

            JSONValue classoverloads = overloads.GetChild(classname);

            Vector<String> functionNames = classoverloads.GetChildNames();

            for (unsigned k = 0; k < functionNames.Size(); k++)
            {
                JSONValue sig = classoverloads.GetChild(functionNames[k]);

                if (!sig.IsArray())
                {
                    ErrorExit("Bad overload defintion");
                }

                Vector<String> values;
                for (unsigned x = 0; x < sig.GetSize(); x++)
                {
                    values.Push(sig.GetString(x));
                }

                JSBFunctionSignature* fo = new JSBFunctionSignature(functionNames[k], values);
                klass->AddFunctionOverride(fo);

            }
        }
    }
}

void JSBModule::ProcessExcludes()
{
    // excludes

    JSONValue root = moduleJSON_->GetRoot();

    JSONValue excludes = root.GetChild("excludes");

    if (excludes.IsObject())
    {
        Vector<String> childNames = excludes.GetChildNames();

        for (unsigned j = 0; j < childNames.Size(); j++)
        {
            String classname = childNames.At(j);

            JSBClass* klass = GetClass(classname);

            if (!klass)
            {
                ErrorExit("Bad exclude klass");
            }

            JSONValue classexcludes = excludes.GetChild(classname);

            Vector<String> functionNames = classexcludes.GetChildNames();

            for (unsigned k = 0; k < functionNames.Size(); k++)
            {
                JSONValue sig = classexcludes.GetChild(functionNames[k]);

                if (!sig.IsArray())
                {
                    ErrorExit("Bad exclude defintion");
                }

                Vector<String> values;
                for (unsigned x = 0; x < sig.GetSize(); x++)
                {
                    values.Push(sig.GetString(x));
                }

                JSBFunctionSignature* fe = new JSBFunctionSignature(functionNames[k], values);
                klass->AddFunctionExclude(fe);

            }
        }
    }
}

void JSBModule::ProcessTypeScriptDecl()
{
    // TypeScript declarations

    JSONValue root = moduleJSON_->GetRoot();

    JSONValue decl = root.GetChild("typescript_decl");

    if (decl.IsObject())
    {
        Vector<String> childNames = decl.GetChildNames();

        for (unsigned j = 0; j < childNames.Size(); j++)
        {
            String classname = childNames.At(j);

            JSBClass* klass = GetClass(classname);

            if (!klass)
            {
                ErrorExit("Bad TypeScript decl klass");
            }

            JSONValue classdecl = decl.GetChild(classname);

            for (unsigned k = 0; k < classdecl.GetSize(); k++)
            {
                klass->AddTypeScriptDecl(classdecl.GetString(k));
            }
        }
    }
}

void JSBModule::ProcessHaxeDecl()
{
    // Haxe declarations

    JSONValue root = moduleJSON_->GetRoot();

    JSONValue decl = root.GetChild("haxe_decl");

    if (decl.IsObject())
    {
        Vector<String> childNames = decl.GetChildNames();

        for (unsigned j = 0; j < childNames.Size(); j++)
        {
            String classname = childNames.At(j);

            JSBClass* klass = GetClass(classname);

            if (!klass)
            {
                ErrorExit("Bad Haxe decl class");
            }

            JSONValue classdecl = decl.GetChild(classname);

            for (unsigned k = 0; k < classdecl.GetSize(); k++)
            {
                klass->AddHaxeDecl(classdecl.GetString(k));
            }
        }
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

        package_->RegisterClass(cls);
    }
}

void JSBModule::RegisterEnum(JSBEnum* jenum)
{
    if (JSBPackage::GetClassAllPackages(jenum->GetName()))
    {
        ErrorExit(ToString("Enum collision: %s", jenum->GetName().CString()));
    }

    enums_[jenum->GetName()] = jenum;

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

void JSBModule::RegisterConstant(const String& constantName, unsigned type)
{
    // MAX_CASCADE_SPLITS is defined differently for desktop/mobile
    if (constantName == "MAX_CASCADE_SPLITS" && JSBPackage::ContainsConstantAllPackages(constantName))
    {
        return;
    }

    if (JSBPackage::ContainsConstantAllPackages(constantName))
    {
        ErrorExit(ToString("Constant collision: %s", constantName.CString()));
    }

    constants_[constantName] = new JSBPrimitiveType(type);
}

bool JSBModule::Load(const String& jsonFilename)
{
    JSBind* jsbind = GetSubsystem<JSBind>();

    LOGINFOF("Loading Module: %s", jsonFilename.CString());

    SharedPtr<File> jsonFile(new File(context_, jsonFilename));

    if (!jsonFile->IsOpen())
    {
        LOGERRORF("Unable to open module json: %s", jsonFilename.CString());
        return false;
    }

    moduleJSON_ = new JSONFile(context_);

    if (!moduleJSON_->BeginLoad(*jsonFile))
    {
        LOGERRORF("Unable to parse module json: %s", jsonFilename.CString());
        return false;
    }

    JSONValue root = moduleJSON_->GetRoot();

    name_ = root.GetString("name");

    JSONValue requires = root.GetChild("requires");

    if (requires.IsArray())
    {
        for (unsigned j = 0; j < requires.GetSize(); j++)
        {
            requirements_.Push(requires.GetString(j));
        }

    }

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

    JSONValue includes = root.GetChild("includes");

    if (includes.IsArray())
    {
        for (unsigned j = 0; j < includes.GetSize(); j++)
        {
            includes_.Push(includes.GetString(j));

        }
    }

    JSONValue sources = root.GetChild("sources");

    for (unsigned i = 0; i < sources.GetSize(); i++)
    {
        sourceDirs_.Push(sources.GetString(i));
    }

    if (name_ == "Graphics")
    {
#ifdef _MSC_VER
        if (jsbind->GetPlatform() == "ANDROID" || jsbind->GetPlatform() == "WEB")
        {
            sourceDirs_.Push("Source/Atomic/Graphics/OpenGL");
        }
        else
        {
#ifdef ATOMIC_D3D11
            sourceDirs_.Push("Source/Atomic/Graphics/Direct3D11");
#else
            sourceDirs_.Push("Source/Atomic/Graphics/Direct3D9");
#endif
        }
#else
        sourceDirs_.Push("Source/Atomic/Graphics/OpenGL");
#endif
    }


    ScanHeaders();

    return true;
}

void JSBModule::GenerateSource(const String& outPath)
{
    JSBModuleWriter writer(this);
    writer.GenerateSource(source_);

    String filepath = outPath + "/JSModule" + name_ + ".cpp";
    File file(context_);
    file.Open(filepath, FILE_WRITE);
    file.Write(source_.CString(), source_.Length());
    file.Close();
}

}

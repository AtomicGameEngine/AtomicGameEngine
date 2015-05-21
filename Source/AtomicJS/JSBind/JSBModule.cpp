// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include <Atomic/Atomic.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Resource/JSONFile.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Core/ProcessUtils.h>

#include "JSBind.h"
#include "JSBindings.h"
#include "JSBHeader.h"
#include "JSBModule.h"
#include "JSBFunction.h"

void JSBModule::ParseHeaders()
{
    for (unsigned i = 0; i < headerFiles_.Size(); i++)
    {
        JSBHeader* header = new JSBHeader(this, headerFiles_.At(i));
        headers_.Push(header);
        header->Parse();
    }
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
}

void JSBModule::WriteClassDeclaration(String& source)
{

    source += "static void jsb_declare_classes(JSVM* vm)\n{\n";

    source += "duk_context* ctx = vm->GetJSContext();\n";

    for (unsigned i = 0; i < classes_.Size(); i++)
    {
        JSBClass* klass = classes_.At(i);

        if (klass->isNumberArray())
            continue;

        source.AppendWithFormat("   js_class_declare(vm, \"%s\", jsb_constructor_%s);\n", klass->GetName().CString(), klass->GetName().CString());

        if (klass->hasProperties())
        {
            source.AppendWithFormat("js_class_push_propertyobject(vm, \"%s\");\n", klass->GetName().CString());


            Vector<String> pnames;
            klass->GetPropertyNames(pnames);

            for (unsigned j = 0; j < pnames.Size(); j++)
            {
                JSBProperty* prop = klass->GetProperty(pnames[j]);

                source.Append("duk_push_object(ctx);\n");

                if (prop->getter_ && !prop->getter_->Skip())
                {
                    source.AppendWithFormat("duk_push_c_function(ctx, jsb_class_%s_%s, 0);\n",
                                            klass->GetName().CString(), prop->getter_->name_.CString());
                    source.Append("duk_put_prop_string(ctx, -2, \"get\");\n");
                }
                if (prop->setter_ && !prop->setter_->Skip())
                {
                    source.AppendWithFormat("duk_push_c_function(ctx, jsb_class_%s_%s, 1);\n",
                                            klass->GetName().CString(), prop->setter_->name_.CString());
                    source.Append("duk_put_prop_string(ctx, -2, \"set\");\n");
                }

                pnames[j][0] = tolower(pnames[j][0]);
                source.AppendWithFormat("duk_put_prop_string(ctx, -2, \"%s\");\n", pnames[j].CString());

            }

            source.Append("duk_pop(ctx);\n");

        }
    }

    source += "\n}\n\n";

}

void JSBModule::WriteClassDefine(String& source)
{

    source += "static void jsb_init_classes(JSVM* vm)\n{\n";

    for (unsigned i = 0; i < classes_.Size(); i++)
    {
        JSBClass* klass = classes_.At(i);

        if (klass->isNumberArray())
            continue;

        source.AppendWithFormat("   jsb_class_define_%s(vm);\n", klass->GetName().CString());
    }

    source += "\n}\n\n";

}


void JSBModule::WriteModulePreInit(String& source)
{
    source.AppendWithFormat("\nvoid jsb_preinit_%s (JSVM* vm)\n{\n\njsb_declare_classes(vm);\n", name_.ToLower().CString());

    // register enums and constants
    source += "// enums and constants\n";
    source += "duk_context* ctx = vm->GetJSContext();\n";
    source += "duk_get_global_string(ctx, \"Atomic\");\n";

    source += "// enums\n";
    for (unsigned i = 0; i < enums_.Size(); i++)
    {
        JSBEnum* jenum = enums_[i];
        for (unsigned k = 0; k < jenum->values_.Size(); k++)
        {
            source.AppendWithFormat("duk_push_number(ctx, (double) %s);\n", jenum->values_.At(k).CString());
            source.AppendWithFormat("duk_put_prop_string(ctx, -2, \"%s\");\n", jenum->values_.At(k).CString());
        }
    }
    source += "// constants\n";

    for (unsigned i = 0; i < constants_.Size(); i++)
    {
        source.AppendWithFormat("duk_push_number(ctx, (double) %s);\n", constants_.At(i).CString());
        source.AppendWithFormat("duk_put_prop_string(ctx, -2, \"%s\");\n", constants_.At(i).CString());
    }

    source += "duk_pop(ctx);\n";
    source += "// end enums and constants\n";

    source += "\n}\n";

}

void JSBModule::WriteModuleInit(String& source)
{
    source.AppendWithFormat("\nvoid jsb_init_%s (JSVM* vm)\n{\n\n    jsb_init_classes(vm);\n\n}\n\n", name_.ToLower().CString());
}


void JSBModule::WriteIncludes(String& source)
{

    for (unsigned i = 0; i < includes_.Size(); i++)
    {
      if (includes_[i].StartsWith("<"))
          source.AppendWithFormat("#include %s\n", includes_[i].CString());
      else
          source.AppendWithFormat("#include \"%s\"\n", includes_[i].CString());
    }

    Vector<JSBHeader*> allheaders;

    for (unsigned i = 0; i < enums_.Size(); i++)
    {
        allheaders.Push(enums_.At(i)->header_);
    }

    for (unsigned i = 0; i < classes_.Size(); i++)
    {
        allheaders.Push(classes_.At(i)->GetHeader());
    }

    Vector<JSBHeader*> included;

    for (unsigned i = 0; i < allheaders.Size(); i++)
    {
        JSBHeader* header = allheaders.At(i);

        if (included.Contains(header))
            continue;

        String headerPath = GetPath(header->filepath_);

		    String headerfile = GetFileNameAndExtension(header->filepath_);

        headerPath.Replace(JSBind::ROOT_FOLDER + "/Source/Atomic/", "Atomic/");

        source.AppendWithFormat("#include <%s%s>\n", headerPath.CString(), headerfile.CString());

        included.Push(header);
    }

}

void JSBModule::EmitSource(const String& filepath)
{
    File file(JSBind::context_);
    file.Open(filepath, FILE_WRITE);

    source_ = "// This file was autogenerated by JSBind, changes will be lost\n";

    source_ += "#ifdef ATOMIC_PLATFORM_WINDOWS\n";

    source_ += "#pragma warning(disable: 4244) // possible loss of data\n";

    source_ += "#endif\n";

    if (Requires("3D"))
    {
        source_ += "#ifdef ATOMIC_3D\n";
    }

    source_ += "#include <Duktape/duktape.h>\n";
    source_ += "#include <AtomicJS/Javascript/JSVM.h>\n";
    source_ += "#include <AtomicJS/Javascript/JSAPI.h>\n";

    WriteIncludes(source_);

    source_ += "\n\nnamespace Atomic\n{\n \n";

    source_ += "// Begin Class Declarations\n";

    for (unsigned i = 0; i < classes_.Size(); i++)
    {
        classes_[i]->WriteForwardDeclarations(source_);
    }

    source_ += "// End Class Declarations\n\n";

    source_ += "// Begin Classes\n";

    for (unsigned i = 0; i < classes_.Size(); i++)
    {
        classes_[i]->Write(source_);
    }

    source_ += "// End Classes\n\n";


    WriteClassDeclaration(source_);

    WriteClassDefine(source_);

    WriteModulePreInit(source_);

    WriteModuleInit(source_);

    // end Atomic namespace
    source_ += "\n}\n";

    if (Requires("3D"))
    {
        source_ += "#endif //ATOMIC_3D\n";
    }


    file.Write(source_.CString(), source_.Length());

    file.Close();

}

void JSBModule::Load(const String &moduleJSONFilename)
{
    ResourceCache* cache = JSBind::context_->GetSubsystem<ResourceCache>();

    JSONFile* moduleJSONFile = cache->GetResource<JSONFile>(moduleJSONFilename);

    if (!moduleJSONFile)
    {
        LOGERRORF("Couldn't load module json: %s", moduleJSONFilename.CString());
        ErrorExit("Couldn't load module json");
    }

    JSONValue moduleJSON = moduleJSONFile->GetRoot();
    JSONValue sources = moduleJSON.GetChild("sources");
    JSONValue classes = moduleJSON.GetChild("classes");
    JSONValue includes = moduleJSON.GetChild("includes");
    JSONValue classes_rename = moduleJSON.GetChild("classes_rename");
    JSONValue overloads = moduleJSON.GetChild("overloads");
    JSONValue requires = moduleJSON.GetChild("requires");

    HashMap<String, String> rename;

    if (requires.IsArray())
    {
        for (unsigned j = 0; j < requires.GetSize(); j++)
        {
            requirements_.Push(requires.GetString(j));
        }

    }

    if (classes_rename.IsObject())
    {
        Vector<String> childNames = classes_rename.GetValueNames();
        for (unsigned j = 0; j < childNames.Size(); j++)
        {
            String classname = childNames.At(j);
            String crename = classes_rename.GetString(classname);

            rename[classname] = crename;

        }

    }

    if (includes.IsArray())
    {
        for (unsigned j = 0; j < includes.GetSize(); j++)
        {
            includes_.Push(includes.GetString(j));

        }
    }

    if (classes.IsArray())
    {
        for (unsigned j = 0; j < classes.GetSize(); j++)
        {
            String classname = classes.GetString(j);

            if (rename.Contains(classname))
                bindings_->RegisterClass(classname, rename[classname]);
            else
                bindings_->RegisterClass(classname);

        }
    }

    if (overloads.IsObject())
    {
        Vector<String> childNames = overloads.GetChildNames();

        for (unsigned j = 0; j < childNames.Size(); j++)
        {
            String classname = childNames.At(j);

            JSBClass* klass = bindings_->GetClass(classname);
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

                JSBFunctionOverride* fo = new JSBFunctionOverride(functionNames[k], values);
                klass->AddFunctionOverride(fo);

            }

        }

    }

    this->name_ = moduleJSON.GetString("name");

    if (this->name_ == "Graphics")
    {
#ifdef _MSC_VER
        if (JSBind::PLATFORM == "ANDROID" || JSBind::PLATFORM == "WEB")
        {
            sources.AddString("Graphics/OpenGL");
        }
        else
        {
#ifdef ATOMIC_D3D11
            sources.AddString("Graphics/Direct3D11");
#else
            sources.AddString("Graphics/Direct3D9");
#endif
        }
#else
        sources.AddString("Graphics/OpenGL");
#endif
    }

    for (unsigned j = 0; j < sources.GetSize(); j++)
    {
        String sourceFolder = sources.GetString(j);
        Vector<String> fileNames;

        String sourceRoot = "Atomic";

        if (sourceFolder == "Javascript")
            sourceRoot = "AtomicJS";

        JSBind::fileSystem_->ScanDir(fileNames, JSBind::ROOT_FOLDER + "/Source/" + sourceRoot + "/" + sourceFolder, "*.h", SCAN_FILES, false);
        for (unsigned k = 0; k < fileNames.Size(); k++)
        {
            // TODO: filter
            String filepath = JSBind::ROOT_FOLDER + "/Source/" + sourceRoot + "/" + sourceFolder + "/" + fileNames[k];

            this->headerFiles_.Push(filepath);
        }

    }

}

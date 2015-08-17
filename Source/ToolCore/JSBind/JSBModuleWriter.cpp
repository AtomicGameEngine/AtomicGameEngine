
#include <Atomic/IO/FileSystem.h>

#include "JSBind.h"
#include "JSBPackage.h"
#include "JSBModule.h"
#include "JSBEnum.h"
#include "JSBClass.h"
#include "JSBFunction.h"
#include "JSBModuleWriter.h"
#include "JSBClassWriter.h"

namespace ToolCore
{

JSBModuleWriter::JSBModuleWriter(JSBModule *module) : module_(module)
{

}

void JSBModuleWriter::WriteForwardDeclarations(String& source)
{
    Vector<SharedPtr<JSBClass>> classes = module_->classes_.Values();

    for (unsigned i = 0; i < classes.Size(); i++)
    {
        JSBClass* cls = classes.At(i);

        if (cls->IsNumberArray())
            continue;

        source.AppendWithFormat("static duk_ret_t jsb_constructor_%s(duk_context* ctx);\n", cls->GetName().CString());
        source.AppendWithFormat("static void jsb_class_define_%s(JSVM* vm);\n", cls->GetName().CString());

    }
}

void JSBModuleWriter::WriteClassDeclaration(String& source)
{
    Vector<SharedPtr<JSBClass>> classes = module_->classes_.Values();

    source += "static void jsb_declare_classes(JSVM* vm)\n{\n";

    source += "duk_context* ctx = vm->GetJSContext();\n";

    String packageName = module_->GetPackage()->GetName();

    for (unsigned i = 0; i < classes.Size(); i++)
    {
        JSBClass* klass = classes.At(i);

        if (klass->IsNumberArray())
            continue;

        source.AppendWithFormat("   js_class_declare<%s>(vm, \"%s\", \"%s\", jsb_constructor_%s);\n", klass->GetNativeName().CString(), packageName.CString(), klass->GetName().CString(), klass->GetName().CString());

        if (klass->HasProperties())
        {
            source.AppendWithFormat("js_class_push_propertyobject(vm, \"%s\", \"%s\");\n", packageName.CString(), klass->GetName().CString());

            Vector<String> pnames;
            klass->GetPropertyNames(pnames);

            for (unsigned j = 0; j < pnames.Size(); j++)
            {
                JSBProperty* prop = klass->GetProperty(pnames[j]);

                source.Append("duk_push_object(ctx);\n");

                if (prop->getter_ && !prop->getter_->Skip())
                {
                    source.AppendWithFormat("duk_push_c_function(ctx, jsb_class_%s_%s, 0);\n",
                                            klass->GetName().CString(), prop->getter_->GetName().CString());
                    source.Append("duk_put_prop_string(ctx, -2, \"get\");\n");
                }
                if (prop->setter_ && !prop->setter_->Skip())
                {
                    source.AppendWithFormat("duk_push_c_function(ctx, jsb_class_%s_%s, 1);\n",
                                            klass->GetName().CString(), prop->setter_->GetName().CString());
                    source.Append("duk_put_prop_string(ctx, -2, \"set\");\n");
                }

                String propertyName = prop->GetCasePropertyName();
                source.AppendWithFormat("duk_put_prop_string(ctx, -2, \"%s\");\n", propertyName.CString());

            }

            source.Append("duk_pop(ctx);\n");

        }
    }

    source += "\n}\n\n";

}

void JSBModuleWriter::WriteIncludes(String& source)
{

    Vector<String>& includes = module_->includes_;
    for (unsigned i = 0; i < includes.Size(); i++)
    {
      if (includes[i].StartsWith("<"))
          source.AppendWithFormat("#include %s\n", includes[i].CString());
      else
          source.AppendWithFormat("#include \"%s\"\n", includes[i].CString());
    }

    Vector<JSBHeader*> allheaders;

    HashMap<StringHash, SharedPtr<JSBEnum> >::Iterator eitr = module_->enums_.Begin();
    while (eitr != module_->enums_.End())
    {
        allheaders.Push(eitr->second_->GetHeader());
        eitr++;
    }

    HashMap<StringHash, SharedPtr<JSBClass> >::Iterator citr = module_->classes_.Begin();
    while (citr != module_->classes_.End())
    {
        allheaders.Push(citr->second_->GetHeader());
        citr++;
    }

    Vector<JSBHeader*> included;

    for (unsigned i = 0; i < allheaders.Size(); i++)
    {
        JSBHeader* header = allheaders.At(i);

        if (included.Contains(header))
            continue;

        String headerPath = GetPath(header->GetFilePath());

        String headerfile = GetFileNameAndExtension(header->GetFilePath());

        JSBind* jsbind = header->GetSubsystem<JSBind>();

        headerPath.Replace(jsbind->GetSourceRootFolder() + "Source/", "");

        source.AppendWithFormat("#include <%s%s>\n", headerPath.CString(), headerfile.CString());

        included.Push(header);
    }

}

void JSBModuleWriter::WriteClassDefine(String& source)
{
    Vector<SharedPtr<JSBClass>> classes = module_->classes_.Values();

    source += "static void jsb_init_classes(JSVM* vm)\n{\n";

    for (unsigned i = 0; i < classes.Size(); i++)
    {
        JSBClass* klass = classes.At(i);

        if (klass->IsNumberArray())
            continue;

        source.AppendWithFormat("   jsb_class_define_%s(vm);\n", klass->GetName().CString());
    }

    source += "\n}\n\n";

}


void JSBModuleWriter::WriteModulePreInit(String& source)
{
    source.AppendWithFormat("\nvoid jsb_package_%s_preinit_%s (JSVM* vm)\n{\n\njsb_declare_classes(vm);\n",
                            module_->package_->GetName().ToLower().CString(), module_->GetName().ToLower().CString());

    // register enums and constants
    source += "// enums and constants\n";
    source += "duk_context* ctx = vm->GetJSContext();\n";
    source += "duk_get_global_string(ctx, \"Atomic\");\n";

    source += "// enums\n";

    Vector<SharedPtr<JSBEnum>> enums = module_->enums_.Values();

    for (unsigned i = 0; i < enums.Size(); i++)
    {
        JSBEnum* jenum = enums[i];

        Vector<String>& values = jenum->GetValues();

        for (unsigned k = 0; k < values.Size(); k++)
        {
            source.AppendWithFormat("duk_push_number(ctx, (double) %s);\n", values[k].CString());
            source.AppendWithFormat("duk_put_prop_string(ctx, -2, \"%s\");\n",values[k].CString());
        }
    }
    source += "// constants\n";

    Vector<String> constants = module_->constants_.Keys();

    for (unsigned i = 0; i < constants.Size(); i++)
    {
        source.AppendWithFormat("duk_push_number(ctx, (double) %s);\n", constants.At(i).CString());
        source.AppendWithFormat("duk_put_prop_string(ctx, -2, \"%s\");\n", constants.At(i).CString());
    }

    source += "duk_pop(ctx);\n";
    source += "// end enums and constants\n";

    source += "\n}\n";

}

void JSBModuleWriter::WriteModuleInit(String& source)
{
    source.AppendWithFormat("\nvoid jsb_package_%s_init_%s (JSVM* vm)\n{\n\n    jsb_init_classes(vm);\n\n}\n\n",
                            module_->package_->GetName().ToLower().CString(), module_->name_.ToLower().CString());
}

void JSBModuleWriter::GenerateSource(String& sourceOut)
{
    source_ = "// This file was autogenerated by JSBind, changes will be lost\n";

    source_ += "#ifdef ATOMIC_PLATFORM_WINDOWS\n";

    source_ += "#pragma warning(disable: 4244) // possible loss of data\n";

    source_ += "#endif\n";

    if (module_->Requires("3D"))
    {
        source_ += "#ifdef ATOMIC_3D\n";
    }

    source_ += "#include <Duktape/duktape.h>\n";
    source_ += "#include <AtomicJS/Javascript/JSVM.h>\n";
    source_ += "#include <AtomicJS/Javascript/JSAPI.h>\n";

    WriteIncludes(source_);

    String ns = module_->GetPackage()->GetNamespace();

    if (ns != "Atomic")
    {
        source_ += "\n\nusing namespace " + ns + ";\n\n";
    }

    source_ += "\n\nnamespace Atomic\n{\n \n";

    source_ += "// Begin Class Declarations\n";

    WriteForwardDeclarations(source_);

    source_ += "// End Class Declarations\n\n";

    source_ += "// Begin Classes\n";

    Vector<SharedPtr<JSBClass>> classes = module_->classes_.Values();

    for (unsigned i = 0; i < classes.Size(); i++)
    {
        JSBClassWriter clsWriter(classes[i]);
        clsWriter.GenerateSource(source_);
    }

    source_ += "// End Classes\n\n";

    WriteClassDeclaration(source_);

    WriteClassDefine(source_);

    WriteModulePreInit(source_);

    WriteModuleInit(source_);

    // end Atomic namespace
    source_ += "\n}\n";

    if (module_->Requires("3D"))
    {
        source_ += "#endif //ATOMIC_3D\n";
    }

    sourceOut = source_;
}

}

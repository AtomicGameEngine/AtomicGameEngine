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

#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>

#include "../JSBind.h"
#include "../JSBPackage.h"
#include "../JSBModule.h"
#include "../JSBEnum.h"
#include "../JSBClass.h"
#include "../JSBFunction.h"


#include "JSModuleWriter.h"
#include "JSClassWriter.h"

namespace ToolCore
{

JSModuleWriter::JSModuleWriter(JSBModule *module) : JSBModuleWriter(module)
{

}

void JSModuleWriter::WriteForwardDeclarations(String& source)
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

void JSModuleWriter::WriteClassDeclaration(String& source)
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

void JSModuleWriter::WriteIncludes(String& source)
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

void JSModuleWriter::WritePreamble(String& source)
{
    if (!module_->jsmodulePreamble_.Size())
        return;

    source += "\n// Begin Module Preamble\n\n";

    for (unsigned i = 0; i < module_->jsmodulePreamble_.Size(); i++)
    {
        source += module_->jsmodulePreamble_[i] + "\n";
    }

    source += "\n// End Module Preamble\n\n";

}

void JSModuleWriter::WriteClassDefine(String& source)
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


void JSModuleWriter::WriteModulePreInit(String& source)
{
    source.AppendWithFormat("\nvoid jsb_package_%s_preinit_%s (JSVM* vm)\n{\n\njsb_declare_classes(vm);\n",
                            module_->package_->GetName().ToLower().CString(), module_->GetName().ToLower().CString());

    // register enums and constants
    source += "// enums and constants\n";
    source += "duk_context* ctx = vm->GetJSContext();\n";
    source.AppendWithFormat("duk_get_global_string(ctx, \"%s\");\n", module_->package_->GetName().CString());
    source += "// enums\n";

    Vector<SharedPtr<JSBEnum>> enums = module_->enums_.Values();

    for (unsigned i = 0; i < enums.Size(); i++)
    {
        JSBEnum* jenum = enums[i];

        HashMap<String, String>& values = jenum->GetValues();

        HashMap<String, String>::ConstIterator itr = values.Begin();

        while (itr != values.End())
        {
            String name = (*itr).first_;
            source.AppendWithFormat("duk_push_number(ctx, (double) %s);\n", name.CString());
            source.AppendWithFormat("duk_put_prop_string(ctx, -2, \"%s\");\n",name.CString());
            itr++;
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

void JSModuleWriter::WriteModuleInit(String& source)
{
    source.AppendWithFormat("\nvoid jsb_package_%s_init_%s (JSVM* vm)\n{\n\n    jsb_init_classes(vm);\n\n}\n\n",
                            module_->package_->GetName().ToLower().CString(), module_->name_.ToLower().CString());
}

void JSModuleWriter::GenerateSource()
{
    String source = "// This file was autogenerated by JSBind, changes will be lost\n";

    source += "#ifdef ATOMIC_PLATFORM_WINDOWS\n";

    source += "#pragma warning(disable: 4244) // possible loss of data\n";

    source += "#endif\n";

    if (module_->Requires("3D"))
    {
        source += "#ifdef ATOMIC_3D\n";
    }

    source += "#include <Duktape/duktape.h>\n";
    source += "#include <AtomicJS/Javascript/JSVM.h>\n";
    source += "#include <AtomicJS/Javascript/JSAPI.h>\n";

    WriteIncludes(source);

    WritePreamble(source);

    String ns = module_->GetPackage()->GetNamespace();

    if (ns != "Atomic")
    {
        source += "\n\nusing namespace " + ns + ";\n\n";
    }

    source += "\n\nnamespace Atomic\n{\n \n";

    source += "// Begin Class Declarations\n";

    WriteForwardDeclarations(source);

    source += "// End Class Declarations\n\n";

    source += "// Begin Classes\n";

    Vector<SharedPtr<JSBClass>> classes = module_->classes_.Values();

    for (unsigned i = 0; i < classes.Size(); i++)
    {
        JSClassWriter clsWriter(classes[i]);
        clsWriter.GenerateSource(source);
    }

    source += "// End Classes\n\n";

    WriteClassDeclaration(source);

    WriteClassDefine(source);

    WriteModulePreInit(source);

    WriteModuleInit(source);

    // end Atomic namespace
    source += "\n}\n";

    if (module_->Requires("3D"))
    {
        source += "#endif //ATOMIC_3D\n";
    }

    JSBind* jsbind = module_->GetSubsystem<JSBind>();

    String filepath = jsbind->GetDestNativeFolder() + "/JSModule" + module_->name_ + ".cpp";

    File file(module_->GetContext());
    file.Open(filepath, FILE_WRITE);
    file.Write(source.CString(), source.Length());
    file.Close();

}

}

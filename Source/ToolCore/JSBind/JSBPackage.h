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

#pragma once

#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace ToolCore
{

class JSBModule;
class JSBClass;
class JSBEnum;

class JSBPackageWriter;

class JSBPackage : public Object
{

    friend class JSPackageWriter;
    friend class CSPackageWriter;

    ATOMIC_OBJECT(JSBPackage, Object)

public:

    enum BindingType
    {
        JAVASCRIPT,
        CSHARP
    };

    JSBPackage(Context* context);
    virtual ~JSBPackage();

    bool Load(const String& packageFolder);

    Vector<SharedPtr<JSBModule>>& GetModules() {return modules_;}

    void PreprocessModules();

    void ProcessModules();

    const String& GetName() { return name_; }
    const String& GetNamespace() { return namespace_; }

    /// Returns whether bindings for a specific type should be generated for this package
    bool GenerateBindings(BindingType type) { return bindingTypes_.Contains(type); }

    JSBClass* GetClass(const String& name);

    PODVector<JSBClass*>& GetAllClasses() { return allClasses_; }
    void RegisterClass(JSBClass* cls) {allClasses_.Push(cls); }

    // get a class by name across all loaded packages
    static JSBClass* GetClassAllPackages(const String& name);

    JSBEnum* GetEnum(const String& name);

    // get an enum by name across all loaded packages
    static JSBEnum* GetEnumAllPackages(const String& name);

    bool ContainsConstant(const String& constantName);

    static bool ContainsConstantAllPackages(const String& constantName);

    void GenerateSource(JSBPackageWriter& packageWriter);

private:

    // The name of the package
    String name_;

    // The name of the C++ namespace if any
    String namespace_;

    Vector<SharedPtr<JSBPackage> > dependencies_;

    HashMap<String, Vector<String>> moduleExcludes_;

    Vector<SharedPtr<JSBModule> > modules_;
    PODVector<JSBClass*> allClasses_;

    PODVector<BindingType> bindingTypes_;

    static Vector<SharedPtr<JSBPackage> > allPackages_;

};


}

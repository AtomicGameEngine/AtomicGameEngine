// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/IO/Log.h>
#include <Atomic/Container/Str.h>
#include <Atomic/Container/Vector.h>

using namespace Atomic;

class JSBModule;
class JSBFunction;
class JSBHeader;
class JSBType;

// chosen function overrides
class JSBFunctionOverride
{

public:

    JSBFunctionOverride(const String& name, const Vector<String>& sig);

    String name_;
    Vector<String> sig_;
    Vector<JSBType*> types_;

    void Parse() ;

    bool parsed_;

};


class JSBEnum
{

public:

    JSBHeader* header_;
    String name_;
    Vector<String> values_;

};

class JSBProperty
{
public:
    String name_;
    JSBFunction* getter_;
    JSBFunction* setter_;

    JSBProperty() : getter_(0), setter_(0)
    {

    }

};

class JSBClass
{
    // class name, possibly renamed for bindings
    String name_;
    // original classname
    String className_;

    bool isObject_;
    bool isAbstract_;
    bool hasProperties_;

    HashMap<String, JSBProperty*> properties_;

    // Vector3, Color, etc can be marshalled via arrays
    int numberArrayElements_;
    String arrayElementType_;

    JSBHeader* header_;
    JSBModule* module_;

    Vector<JSBClass*> baseClasses_;
    static Vector<JSBClass*> allClasses_;

    Vector<JSBFunction*> functions_;
    Vector<JSBFunctionOverride*> overrides_;

    static void RecursiveAddBaseClass(JSBClass* klass, Vector<JSBClass*>& baseClasses)
    {
        for (unsigned i = 0; i < baseClasses.Size(); i++)
        {
            JSBClass* base = baseClasses.At(i);

            if (!klass->baseClasses_.Contains(base))
                klass->baseClasses_.Push(base);

            RecursiveAddBaseClass(klass, base->baseClasses_);
        }

    }

    static void WriteProtoTypeRecursive(String& source, JSBClass* klass,  Vector<JSBClass *> &written);

public:

    JSBClass(const String& name, const String& className = String::EMPTY) :
        isObject_(false), isAbstract_(false) , hasProperties_(false), numberArrayElements_(0), arrayElementType_("float"),
        header_(0), module_(0)
    {
        name_ = name;
        if (className.Length())
            className_ = className;
        else
            className_  = name_;

        if (name_ == "Color")
            numberArrayElements_ = 4;
        else if (name_ == "Vector2")
            numberArrayElements_ = 2;
        else if (name_ == "Vector3")
            numberArrayElements_ = 3;
        else if (name_ == "Vector4")
            numberArrayElements_ = 4;
        else if (name_ == "Quaternion")
            numberArrayElements_ = 4;
        else if (name_ == "BoundingBox")
            numberArrayElements_ = 6;
        else if (name_ == "Rect")
            numberArrayElements_ = 4;
        else if (name_ == "IntRect")
        {
            numberArrayElements_ = 4;
            arrayElementType_ = "int";
        }
        else if (name_ == "IntVector2")
        {
            numberArrayElements_ = 2;
            arrayElementType_ = "int";
        }

        allClasses_.Push(this);
    }

    JSBClass* GetBaseClass() { return baseClasses_.Size() ? baseClasses_[0] : NULL; }

    void AddBaseClass(JSBClass* base)
    {
        baseClasses_.Push(base);
    }

    void SetSkipFunction(const String& name, bool skip = true);

    JSBFunction* GetFunction(const String& name);

    void GetPropertyNames(Vector<String>& names) { names = properties_.Keys(); }

    JSBProperty* GetProperty(const String& name)
    {
        if (!properties_.Contains(name))
            return 0;

        return properties_[name];
    }


    void AddFunction(JSBFunction* function);
    void AddFunctionOverride(JSBFunctionOverride* functionOverride);

    void SetModule(JSBModule* module) {module_ = module;}
    JSBModule* GetModule() {return module_;}

    void SetHeader(JSBHeader* header) {header_ = header;}
    JSBHeader* GetHeader() {return header_;}

    void Dump();

    static void Preprocess();
    static void Process();

    void AddPropertyFunction(JSBFunction* function);

    unsigned GetFunctionCount() {return functions_.Size(); }
    JSBFunction* GetFunction(unsigned idx) {return functions_.At(idx); }

    // get constructor if defined and not skipped
    JSBFunction* GetConstructor();

    static void DumpAllClasses()
    {
        for (unsigned i = 0; i < allClasses_.Size(); i++)
        {
            allClasses_[i]->Dump();
        }
    }

    bool isAbstract() { return isAbstract_;}
    void setAbstract(bool value) { isAbstract_ = value;}

    bool isObject() { return isObject_;}
    void setObject(bool value) { isObject_ = value;}
    bool hasProperties() { return hasProperties_; }

    bool isNumberArray() { return numberArrayElements_ != 0; }
    int GetNumberArrayElements() { return numberArrayElements_;}
    const String& GetArrayElementType() const { return arrayElementType_; }

    const String& GetName() { return name_; }
    const String& GetClassName() { return className_; }

    void Write(String& source);    
    void WriteFunctions(String& source);
    void WriteDefinition(String& source);
    void WriteForwardDeclarations(String& source);
    static void WriteProtoTypeSetup(String& source);

};

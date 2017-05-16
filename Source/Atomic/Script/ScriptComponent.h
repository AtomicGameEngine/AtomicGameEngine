//
// Copyright (c) 2014-2016, THUNDERBEAST GAMES LLC All rights reserved
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

#include <Atomic/Scene/Component.h>

namespace Atomic
{

class ScriptComponentFile;

class ATOMIC_API ScriptComponent : public Component
{
    ATOMIC_OBJECT(ScriptComponent, Component)

public:

    /// Construct.
    ScriptComponent(Context* context);
    /// Destruct.
    virtual ~ScriptComponent();

    static void RegisterObject(Context* context);

    virtual const String& GetComponentClassName() const = 0;
    virtual ScriptComponentFile* GetComponentFile() const = 0;

    /// Get script field value map
    VariantMap& GetFieldValues() { return fieldValues_; }

    /// Load from binary data. Return true if successful.
    bool Load(Deserializer& source, bool setInstanceDefault);
    /// Load from XML data. Return true if successful.
    bool LoadXML(const XMLElement& source, bool setInstanceDefault);

    /// Save as binary data. Return true if successful.
    virtual bool Save(Serializer& dest) const;
    /// Save as XML data. Return true if successful.
    virtual bool SaveXML(XMLElement& dest) const;

protected:

    const VariantMap& GetFieldValuesAttr() const;
    void SetFieldValuesAttr(const VariantMap& value);

    VariantMap fieldValues_;
    mutable VariantMap fieldValuesAttr_;

    mutable bool saving_;
    bool loading_;


};

}

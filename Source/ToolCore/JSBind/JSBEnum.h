//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace ToolCore
{

class JSBPackage;
class JSBModule;
class JSBHeader;


class JSBEnum : public Object
{

    OBJECT(JSBEnum)

public:

    JSBEnum(Context* context, JSBModule* module, const String& name);
    virtual ~JSBEnum();

    const String& GetName() const { return name_; }
    JSBHeader* GetHeader() { return header_; }

    JSBPackage* GetPackage();

    HashMap<String, String>& GetValues() { return values_; }

    void SetHeader(JSBHeader* header) { header_ = header; }

    void Preprocess();

    void AddValue(const String& name, const String& constantValue = String::EMPTY) { values_[name] = constantValue; }

private:

    String name_;

    SharedPtr<JSBModule> module_;

    HashMap<String, String> values_;

    JSBHeader* header_;

};


}

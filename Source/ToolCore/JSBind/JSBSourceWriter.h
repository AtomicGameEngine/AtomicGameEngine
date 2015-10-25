//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Container/Str.h>

using namespace Atomic;

namespace ToolCore
{

class JSBSourceWriter
{

public:

    void SetIndentSpaces(int spaces) { indentSpaces_ = spaces; }

    void Indent() { indentLevel_++; GenerateIndentSpaces(); }
    void Dedent() { indentLevel_--; assert(indentLevel_ >= 0); GenerateIndentSpaces(); }

    String IndentLine(const String& line) { return indent_ + line; }

    const String& GetIndent() { return indent_; }

protected:

    void GenerateIndentSpaces() {

        int spaces = indentSpaces_ * indentLevel_;
        indent_.Clear();
        while (spaces--) {
            indent_ += " ";
        }
    }

    String indent_;

    JSBSourceWriter() : indentLevel_(0), indentSpaces_(3) {}
    ~JSBSourceWriter() {}

    int indentSpaces_;
    int indentLevel_;

};

}

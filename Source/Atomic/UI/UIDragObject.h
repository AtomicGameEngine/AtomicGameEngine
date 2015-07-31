
#pragma once

#include <Atomic/Core/Object.h>

namespace Atomic
{


class UIDragObject : public Object
{

    OBJECT(UIDragObject);

public:
    /// Construct.
    UIDragObject(Context* context, Object* object = NULL, const String& text = String::EMPTY, const String& icon = String::EMPTY);
    virtual ~UIDragObject();

    const String& GetText() { return text_; }
    const String& GetIcon() { return icon_; }
    Object* GetObject() { return object_; }

    const Vector<String>& GetFilenames() { return filenames_; }

    void SetText(const String& text) { text_ = text; }
    void SetIcon(const String& icon) { icon_ = icon; }
    void SetObject(Object* object) { object_ = object; }

    void AddFilename(const String& filename) {filenames_.Push(filename); }

private:

    String text_;
    String icon_;
    Vector<String> filenames_;
    SharedPtr<Object> object_;

};


}

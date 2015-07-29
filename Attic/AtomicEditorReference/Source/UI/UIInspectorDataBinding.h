
#pragma once

#include <TurboBadger/tb_widgets.h>
#include <Atomic/Scene/Scene.h>

namespace tb
{
    class TBWidget;
    class TBMenuWindow;
    class TBInlineSelect;
}

using namespace Atomic;
using namespace tb;

namespace AtomicEditor
{

class InspectorDataBinding
{

public:

    static InspectorDataBinding* Create(Serializable* object, const AttributeInfo* attrInfo);

    void SetWidgetValueFromObject();
    void SetObjectValueFromWidget(TBWidget* srcWidget = NULL);

    TBWidget* GetWidget() { return widget_; }

    bool OnEvent(const TBWidgetEvent &ev);

private:

    InspectorDataBinding() : widget_(0), object_(0), attrInfo_(0), objectLocked_(false), widgetLocked_(false) {}

    TBWidget* widget_;
    Serializable* object_;
    const AttributeInfo* attrInfo_;

    bool objectLocked_;
    bool widgetLocked_;

};

}

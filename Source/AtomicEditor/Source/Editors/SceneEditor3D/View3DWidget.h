
#pragma once

#include <TurboBadger/tb_widgets.h>
#include <Atomic/Graphics/Texture2D.h>
#include <Atomic/UI/View3D.h>
#include <Atomic/UI/TBUI.h>

using namespace tb;
using namespace Atomic;

namespace  AtomicEditor
{

class View3DWidget : public TBWidget
{
public:
    // For safe typecasting
    TBOBJECT_SUBCLASS(View3DWidget, TBWidget);

    View3DWidget();

    void SetView3D(TBUI* tbui, View3D* view3D) { tbui_ = tbui, view3D_ = view3D; }

    virtual void OnPaint(const PaintProps &paint_props);

private:

    WeakPtr<TBUI> tbui_;
    SharedPtr<View3D> view3D_;

    PODVector<float> vertexData_;

};


}

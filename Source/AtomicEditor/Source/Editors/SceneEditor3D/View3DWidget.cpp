
#include "AtomicEditor.h"

#include <Atomic/UI/TBUI.h>
#include "View3DWidget.h"

namespace AtomicEditor
{

View3DWidget::View3DWidget()
{
    vertexData_.Resize(6 * UI_VERTEX_SIZE);
    float color;
    ((unsigned&)color) = 0xFFFFFFFF;

    float* data = &vertexData_[0];

    data[2] = 0; data[3] = color; data[4] = 0; data[5] = 0;
    data[8] = 0; data[9] = color; data[10] = 1; data[11] = 0;
    data[14] = 0; data[15] = color; data[16] = 1; data[17] = 1;
    data[20] = 0; data[21] = color; data[22] = 0; data[23] = 0;
    data[26] = 0; data[27] = color; data[28] = 1; data[29] = 1;
    data[32] = 0; data[33] = color; data[34] = 0; data[35] = 1;
}

void View3DWidget::OnPaint(const PaintProps &paint_props)
{
    if (view3D_.Null())
        return;

    TBRect rect = GetRect();

    ConvertToRoot(rect.x, rect.y);

    IntVector2 size = view3D_->GetSize();

    if (size.x_ != rect.w || size.y_ != rect.h)
    {
        size.x_ = rect.w;
        size.y_ = rect.h;

        view3D_->SetSize(size);
    }

    float* data = &vertexData_[0];

    data[0] = rect.x;
    data[1] = rect.y;

    data[6] = rect.x + rect.w;
    data[7] =  rect.y;

    data[12] = rect.x + rect.w;
    data[13] = rect.y + rect.h;

    data[18] = rect.x;
    data[19] = rect.y;

    data[24] = rect.x + rect.w;
    data[25] = rect.y + rect.h;

    data[30] = rect.x;
    data[31] = rect.y + rect.h;

    tbui_->SubmitBatchVertexData(view3D_->GetRenderTexture(), vertexData_);

}

}

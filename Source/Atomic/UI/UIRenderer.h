
#pragma once

#include <TurboBadger/tb_types.h>
#include <TurboBadger/renderers/tb_renderer_batcher.h>
#include "../Core/Context.h"
#include "UIBatch.h"

namespace tb
{
class TBBitmap;
class TBRect;
}

namespace Atomic
{

class TBUI;

class UIRenderer : public tb::TBRendererBatcher
{
    friend class UIState;
public:

    UIRenderer(Context* context);
    virtual ~UIRenderer();

    void BeginPaint(int render_target_w, int render_target_h);
    void EndPaint();

    tb::TBBitmap *CreateBitmap(int width, int height, tb::uint32 *data);

    void RenderBatch(Batch *batch);

    void SetClipRect(const tb::TBRect &rect);

    Context* GetContext() { return context_; }

private:
    WeakPtr<Context> context_;
    PODVector<UIBatch>* batches_;
    PODVector<float>* vertexData_;
    IntRect currentScissor_;
};

}

#pragma once

#ifdef ATOMIC_TBUI

#include "../Core/Object.h"
#include "../UI/UIBatch.h"

namespace tb
{

class TBWidget;

}

namespace Atomic
{

class VertexBuffer;

/// %UI element which renders a 3D scene.
class ATOMIC_API TBUI : public Object
{
    OBJECT(TBUI)

    tb::TBWidget* rootWidget_;
    bool initialized_;    

public:
    /// Construct.
    TBUI(Context* context);
    /// Destruct.
    virtual ~TBUI();

    tb::TBWidget* GetRootWidget() { return rootWidget_; }

    bool LoadResourceFile(tb::TBWidget* widget, const String& filename);

    void Initialize();

    void Shutdown();

private:

    bool shuttingDown_;
};

}

#endif

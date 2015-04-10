#pragma once

#ifdef ATOMIC_TBUI

#include "../Core/Object.h"
namespace tb
{

class TBWidget;

}

namespace Atomic
{

class UIState;

class ATOMIC_API TBUI : public Object
{
    OBJECT(TBUI)

public:
    /// Construct.
    TBUI(Context* context);
    /// Destruct.
    virtual ~TBUI();

    void Render();
    tb::TBWidget* GetRootWidget();
    bool LoadResourceFile(tb::TBWidget* widget, const String& filename);

    void Initialize();

    void Shutdown();

private:

    SharedPtr<UIState> theView_;

};

}

#endif

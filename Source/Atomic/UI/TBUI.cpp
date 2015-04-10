#include "Precompiled.h"

#ifdef ATOMIC_TBUI

#include "../Core/Context.h"
#include "../Core/CoreEvents.h"
#include "../Core/Profiler.h"
#include "../IO/Log.h"
#include "../IO/File.h"
#include "../Resource/ResourceCache.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/GraphicsEvents.h"
#include "../Graphics/Texture2D.h"
#include "../Graphics/VertexBuffer.h"
#include "../Input/Input.h"
#include "../Input/InputEvents.h"
#include "../UI/TBUI.h"

#include <TurboBadger/tb_core.h>
#include <TurboBadger/tb_system.h>

#include "UIView.h"

using namespace tb;

namespace tb
{

void TBSystem::RescheduleTimer(double fire_time)
{

}

}

namespace Atomic
{

TBUI::TBUI(Context* context) :
    Object(context)
{

}

TBUI::~TBUI()
{

}

void TBUI::Render()
{
    theView_->Render();
}

tb::TBWidget* TBUI::GetRootWidget()
{
    return theView_->GetRootWidget();
}

bool TBUI::LoadResourceFile(tb::TBWidget* widget, const String& filename)
{
    return theView_->LoadResourceFile(widget, filename);
}

void TBUI::Shutdown()
{
    theView_->SetInputDisabled(true);
    theView_ = 0;
}


void TBUI::Initialize()
{
    theView_ = new UIView(context_);

}








}

#endif

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
#include <TurboBadger/tb_debug.h>
#include <TurboBadger/animation/tb_widget_animation.h>
#include <TurboBadger/renderers/tb_renderer_batcher.h>
#include <TurboBadger/tb_font_renderer.h>
#include <TurboBadger/tb_node_tree.h>
#include <TurboBadger/tb_widgets_reader.h>
#include <TurboBadger/tb_window.h>


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
    Object(context),
    rootWidget_(0),
    initialized_(false),
    shuttingDown_(false)
{
//    /SubscribeToEvent(E_SCREENMODE, HANDLER(TBUI, HandleScreenMode));
}

TBUI::~TBUI()
{
    if (initialized_)
    {
        tb::TBWidgetsAnimationManager::Shutdown();
        delete rootWidget_;
        // leak
        //delete TBUIRenderer::renderer_;
        tb_core_shutdown();
    }
}


void TBUI::Shutdown()
{
    shuttingDown_ = true;
    //SetInputDisabled(true);
}


void TBUI::Initialize()
{

}


bool TBUI::LoadResourceFile(TBWidget* widget, const String& filename)
{

    tb::TBNode node;

    // TODO: use Urho resources
    if (!node.ReadFile(filename.CString()))
        return false;

    tb::g_widgets_reader->LoadNodeTree(widget, &node);
    return true;
}






}

#endif

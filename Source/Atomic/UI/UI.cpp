//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <TurboBadger/tb_core.h>
#include <TurboBadger/tb_system.h>
#include <TurboBadger/tb_debug.h>
#include <TurboBadger/animation/tb_widget_animation.h>
#include <TurboBadger/renderers/tb_renderer_batcher.h>
#include <TurboBadger/tb_font_renderer.h>
#include <TurboBadger/tb_node_tree.h>
#include <TurboBadger/tb_widgets_reader.h>
#include <TurboBadger/tb_window.h>
#include <TurboBadger/tb_message_window.h>
#include <TurboBadger/tb_editfield.h>
#include <TurboBadger/tb_select.h>
#include <TurboBadger/tb_inline_select.h>
#include <TurboBadger/tb_tab_container.h>
#include <TurboBadger/tb_toggle_container.h>
#include <TurboBadger/tb_scroll_container.h>
#include <TurboBadger/tb_menu_window.h>
#include <TurboBadger/tb_popup_window.h>
#include <TurboBadger/image/tb_image_widget.h>
#include <TurboBadger/tb_atomic_widgets.h>

void register_tbbf_font_renderer();
void register_stb_font_renderer();
void register_freetype_font_renderer();

using namespace tb;

#include "../Core/CoreEvents.h"
#include "../IO/Log.h"
#include "../IO/FileSystem.h"
#include "../Input/Input.h"
#include "../Input/InputEvents.h"
#include "../Resource/ResourceCache.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/GraphicsEvents.h"
#include "../Graphics/Texture2D.h"
#include "../Graphics/VertexBuffer.h"

#include "UIEvents.h"

#include "UIRenderer.h"
#include "UI.h"
#include "UIButton.h"
#include "UITextField.h"
#include "UIEditField.h"
#include "UILayout.h"
#include "UIImageWidget.h"
#include "UIClickLabel.h"
#include "UICheckBox.h"
#include "UISelectList.h"
#include "UIMessageWindow.h"
#include "UISkinImage.h"
#include "UITabContainer.h"
#include "UISceneView.h"
#include "UIDragDrop.h"
#include "UIContainer.h"
#include "UISection.h"
#include "UIInlineSelect.h"
#include "UIScrollContainer.h"
#include "UISeparator.h"
#include "UIDimmer.h"
#include "UISelectDropdown.h"
#include "UIMenuWindow.h"
#include "UIPopupWindow.h"
#include "UISlider.h"
#include "UIColorWidget.h"
#include "UIColorWheel.h"

#include "SystemUI/SystemUI.h"
#include "SystemUI/SystemUIEvents.h"
#include "SystemUI/DebugHud.h"
#include "SystemUI/Console.h"
#include "SystemUI/MessageBox.h"

namespace tb
{

void TBSystem::RescheduleTimer(double fire_time)
{

}

}


namespace Atomic
{

WeakPtr<Context> UI::uiContext_;


UI::UI(Context* context) :
    Object(context),
    rootWidget_(0),
    inputDisabled_(false),
    keyboardDisabled_(false),
    initialized_(false),
    skinLoaded_(false),
    consoleVisible_(false),
    exitRequested_(false),
    changedEventsBlocked_(0)
{

    SubscribeToEvent(E_EXITREQUESTED, ATOMIC_HANDLER(UI, HandleExitRequested));

}

UI::~UI()
{
    if (initialized_)
    {
        initialized_ = false;

        tb::TBAnimationManager::AbortAllAnimations();
        tb::TBWidgetListener::RemoveGlobalListener(this);

        TBFile::SetReaderFunction(0);
        TBID::tbidRegisterCallback = 0;
        
        tb::TBWidgetsAnimationManager::Shutdown();

        widgetWrap_.Clear();
        delete rootWidget_;
        // leak
        //delete TBUIRenderer::renderer_;
        tb_core_shutdown();
    }

    uiContext_ = 0;

}

void UI::HandleExitRequested(StringHash eventType, VariantMap& eventData)
{
    Shutdown();
}

void UI::Shutdown()
{

}

bool UI::GetFocusedWidget()
{
    if (!TBWidget::focused_widget)
        return false;

    return TBWidget::focused_widget->IsOfType<TBEditField>();
}

void UI::SetBlockChangedEvents(bool blocked)
{
    if (blocked)
        changedEventsBlocked_++;
    else
    {
        changedEventsBlocked_--;

        if (changedEventsBlocked_ < 0)
        {
            ATOMIC_LOGERROR("UI::BlockChangedEvents - mismatched block calls, setting to 0");
            changedEventsBlocked_ = 0;
        }
    }

}

void UI::Initialize(const String& languageFile)
{
    Graphics* graphics = GetSubsystem<Graphics>();
    assert(graphics);
    assert(graphics->IsInitialized());
    graphics_ = graphics;

    vertexBuffer_ = new VertexBuffer(context_);

    uiContext_ = context_;

    TBFile::SetReaderFunction(TBFileReader);
    TBID::tbidRegisterCallback = UI::TBIDRegisterStringCallback;

    TBWidgetsAnimationManager::Init();

    renderer_ = new UIRenderer(graphics_->GetContext());
    tb_core_init(renderer_, languageFile.CString());

    //register_tbbf_font_renderer();
    //register_stb_font_renderer();
    register_freetype_font_renderer();

    rootWidget_ = new TBWidget();

    int width = graphics_->GetWidth();
    int height = graphics_->GetHeight();
    rootWidget_->SetSize(width, height);
    rootWidget_->SetVisibilility(tb::WIDGET_VISIBILITY_VISIBLE);

    SubscribeToEvent(E_MOUSEBUTTONDOWN, ATOMIC_HANDLER(UI, HandleMouseButtonDown));
    SubscribeToEvent(E_MOUSEBUTTONUP, ATOMIC_HANDLER(UI, HandleMouseButtonUp));
    SubscribeToEvent(E_MOUSEMOVE, ATOMIC_HANDLER(UI, HandleMouseMove));
    SubscribeToEvent(E_MOUSEWHEEL, ATOMIC_HANDLER(UI, HandleMouseWheel));
    SubscribeToEvent(E_KEYDOWN, ATOMIC_HANDLER(UI, HandleKeyDown));
    SubscribeToEvent(E_KEYUP, ATOMIC_HANDLER(UI, HandleKeyUp));
    SubscribeToEvent(E_TEXTINPUT, ATOMIC_HANDLER(UI, HandleTextInput));
    SubscribeToEvent(E_UPDATE, ATOMIC_HANDLER(UI, HandleUpdate));
    SubscribeToEvent(E_SCREENMODE, ATOMIC_HANDLER(UI, HandleScreenMode));
    SubscribeToEvent(SystemUI::E_CONSOLECLOSED, ATOMIC_HANDLER(UI, HandleConsoleClosed));

    SubscribeToEvent(E_TOUCHBEGIN, ATOMIC_HANDLER(UI, HandleTouchBegin));
    SubscribeToEvent(E_TOUCHEND, ATOMIC_HANDLER(UI, HandleTouchEnd));
    SubscribeToEvent(E_TOUCHMOVE, ATOMIC_HANDLER(UI, HandleTouchMove));

    SubscribeToEvent(E_RENDERUPDATE, ATOMIC_HANDLER(UI, HandleRenderUpdate));

    // register the UIDragDrop subsystem (after we have subscribed to events, so it is processed after)
    context_->RegisterSubsystem(new UIDragDrop(context_));

    tb::TBWidgetListener::AddGlobalListener(this);

    initialized_ = true;

    SystemUI::SystemUI* systemUI = new SystemUI::SystemUI(context_);
    context_->RegisterSubsystem(systemUI);
    systemUI->CreateConsoleAndDebugHud();

    //TB_DEBUG_SETTING(LAYOUT_BOUNDS) = 1;
}

void UI::LoadSkin(const String& skin, const String& overrideSkin)
{
    // Load the default skin, and override skin (if any)
    tb::g_tb_skin->Load(skin.CString(), overrideSkin.CString());
    skinLoaded_ = true;
}

void UI::LoadDefaultPlayerSkin()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    String skin = "DefaultUI/skin/skin.tb.txt";
    String overrideSkin;

    // see if we have an override skin
    SharedPtr<File> skinFile = cache->GetFile("UI/Skin/skin.ui.txt", false);
    if (skinFile.NotNull())
    {
        skinFile->Close();
        skin = "UI/Skin/skin.ui.txt";
    }

    // see if we have an override skin
    SharedPtr<File> overrideFile = cache->GetFile("UI/Skin/Override/skin.ui.txt", false);

    if (overrideFile.NotNull())
    {
        overrideFile->Close();
        overrideSkin = "UI/Skin/Override/skin.ui.txt";
    }

    LoadSkin(skin, overrideSkin);

    if (skin == "DefaultUI/skin/skin.tb.txt")
    {
        AddFont("DefaultUI/fonts/vera.ttf", "Vera");
        SetDefaultFont("Vera", 12);
    }
}

void UI::SetDefaultFont(const String& name, int size)
{
    tb::TBFontDescription fd;
    fd.SetID(tb::TBIDC(name.CString()));
    fd.SetSize(tb::g_tb_skin->GetDimensionConverter()->DpToPx(12));
    tb::g_font_manager->SetDefaultFontDescription(fd);

    // Create the font now.
    tb::TBFontFace *font = tb::g_font_manager->CreateFontFace(tb::g_font_manager->GetDefaultFontDescription());

    // Render some glyphs in one go now since we know we are going to use them. It would work fine
    // without this since glyphs are rendered when needed, but with some extra updating of the glyph bitmap.
    if (font)
        font->RenderGlyphs(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~•·åäöÅÄÖ");
}

void UI::AddFont(const String& fontFile, const String& name)
{
    tb::g_font_manager->AddFontInfo(fontFile.CString(), name.CString());
}

void UI::Render(VertexBuffer* buffer, const PODVector<UIBatch>& batches, unsigned batchStart, unsigned batchEnd)
{

    if (batches.Empty())
        return;

    Vector2 invScreenSize(1.0f / (float)graphics_->GetWidth(), 1.0f / (float)graphics_->GetHeight());
    Vector2 scale(2.0f * invScreenSize.x_, -2.0f * invScreenSize.y_);
    Vector2 offset(-1.0f, 1.0f);

    Matrix4 projection(Matrix4::IDENTITY);
    projection.m00_ = scale.x_;
    projection.m03_ = offset.x_;
    projection.m11_ = scale.y_;
    projection.m13_ = offset.y_;
    projection.m22_ = 1.0f;
    projection.m23_ = 0.0f;
    projection.m33_ = 1.0f;

    graphics_->ClearParameterSources();
    graphics_->SetColorWrite(true);
    graphics_->SetCullMode(CULL_NONE);
    graphics_->SetDepthTest(CMP_ALWAYS);
    graphics_->SetDepthWrite(false);
    graphics_->SetFillMode(FILL_SOLID);
    graphics_->SetStencilTest(false);

    graphics_->ResetRenderTargets();

    graphics_->SetVertexBuffer(buffer);

    ShaderVariation* noTextureVS = graphics_->GetShader(VS, "Basic", "VERTEXCOLOR");
    ShaderVariation* diffTextureVS = graphics_->GetShader(VS, "Basic", "DIFFMAP VERTEXCOLOR");
    ShaderVariation* noTexturePS = graphics_->GetShader(PS, "Basic", "VERTEXCOLOR");
    ShaderVariation* diffTexturePS = graphics_->GetShader(PS, "Basic", "DIFFMAP VERTEXCOLOR");
    ShaderVariation* diffMaskTexturePS = graphics_->GetShader(PS, "Basic", "DIFFMAP ALPHAMASK VERTEXCOLOR");
    ShaderVariation* alphaTexturePS = graphics_->GetShader(PS, "Basic", "ALPHAMAP VERTEXCOLOR");

    unsigned alphaFormat = Graphics::GetAlphaFormat();

    for (unsigned i = batchStart; i < batchEnd; ++i)
    {
        const UIBatch& batch = batches[i];
        if (batch.vertexStart_ == batch.vertexEnd_)
            continue;

        ShaderVariation* ps;
        ShaderVariation* vs;

        if (!batch.texture_)
        {
            ps = noTexturePS;
            vs = noTextureVS;
        }
        else
        {
            // If texture contains only an alpha channel, use alpha shader (for fonts)
            vs = diffTextureVS;

            if (batch.texture_->GetFormat() == alphaFormat)
                ps = alphaTexturePS;
            else if (batch.blendMode_ != BLEND_ALPHA && batch.blendMode_ != BLEND_ADDALPHA && batch.blendMode_ != BLEND_PREMULALPHA)
                ps = diffMaskTexturePS;
            else
                ps = diffTexturePS;
        }

        graphics_->SetShaders(vs, ps);
        if (graphics_->NeedParameterUpdate(SP_OBJECT, this))
            graphics_->SetShaderParameter(VSP_MODEL, Matrix3x4::IDENTITY);
        if (graphics_->NeedParameterUpdate(SP_CAMERA, this))
            graphics_->SetShaderParameter(VSP_VIEWPROJ, projection);
        if (graphics_->NeedParameterUpdate(SP_MATERIAL, this))
            graphics_->SetShaderParameter(PSP_MATDIFFCOLOR, Color(1.0f, 1.0f, 1.0f, 1.0f));

        graphics_->SetBlendMode(batch.blendMode_);
        graphics_->SetScissorTest(true, batch.scissor_);
        graphics_->SetTexture(0, batch.texture_);
        graphics_->Draw(TRIANGLE_LIST, batch.vertexStart_ / UI_VERTEX_SIZE, (batch.vertexEnd_ - batch.vertexStart_) /
            UI_VERTEX_SIZE);
    }
}

void UI::SetVertexData(VertexBuffer* dest, const PODVector<float>& vertexData)
{
    if (vertexData.Empty())
        return;

    // Update quad geometry into the vertex buffer
    // Resize the vertex buffer first if too small or much too large
    unsigned numVertices = vertexData.Size() / UI_VERTEX_SIZE;
    if (dest->GetVertexCount() < numVertices || dest->GetVertexCount() > numVertices * 2)
        dest->SetSize(numVertices, MASK_POSITION | MASK_COLOR | MASK_TEXCOORD1, true);

    dest->SetData(&vertexData[0]);
}


void UI::Render(bool resetRenderTargets)
{
    SetVertexData(vertexBuffer_, vertexData_);
    Render(vertexBuffer_, batches_, 0, batches_.Size());

    SystemUI::SystemUI* systemUI = GetSubsystem<SystemUI::SystemUI>();
    if (systemUI)
        systemUI->Render();
}

void UI::HandleRenderUpdate(StringHash eventType, VariantMap& eventData)
{
    // Get rendering batches from the non-modal UI elements
    batches_.Clear();
    vertexData_.Clear();

    tb::TBRect rect = rootWidget_->GetRect();

    IntRect currentScissor = IntRect(0, 0, rect.w, rect.h);
    GetBatches(batches_, vertexData_, currentScissor);

}

void UI::GetBatches(PODVector<UIBatch>& batches, PODVector<float>& vertexData, const IntRect& currentScissor)
{
    //if (!initialized_)
    //    return;

    TBAnimationManager::Update();

    rootWidget_->InvokeProcessStates();
    rootWidget_->InvokeProcess();

    tb::g_renderer->BeginPaint(rootWidget_->GetRect().w, rootWidget_->GetRect().h);

    renderer_->currentScissor_ = currentScissor;
    renderer_->batches_ = &batches;
    renderer_->vertexData_ = &vertexData;
    rootWidget_->InvokePaint(tb::TBWidget::PaintProps());

    tb::g_renderer->EndPaint();
}

void UI::SubmitBatchVertexData(Texture* texture, const PODVector<float>& vertexData)
{
    UIBatch b(BLEND_ALPHA , renderer_->currentScissor_, texture, &vertexData_);

    unsigned begin = b.vertexData_->Size();
    b.vertexData_->Resize(begin + vertexData.Size());
    float* dest = &(b.vertexData_->At(begin));
    b.vertexEnd_ = b.vertexData_->Size();

    for (unsigned i = 0; i < vertexData.Size(); i++, dest++)
    {
        *dest = vertexData[i];
    }

    UIBatch::AddOrMerge(b, batches_);

}

void UI::TBFileReader(const char* filename, void** data, unsigned* length)
{
    *data = 0;
    *length = 0;

    ResourceCache* cache = uiContext_->GetSubsystem<ResourceCache>();
    SharedPtr<File> file = cache->GetFile(filename);
    if (!file || !file->IsOpen())
    {
        ATOMIC_LOGERRORF("UI::TBFileReader: Unable to load file: %s", filename);
        return;
    }

    unsigned size = file->GetSize();

    if (!size)
        return;

    void* _data = malloc(size);
    if (!_data)
        return;

    if (file->Read(_data, size) != size)
    {
        free(_data);
        return;
    }

    *length = size;
    *data = _data;

}

void UI::GetTBIDString(unsigned id, String& value)
{
    if (!id)
    {
        value = "";
    }
    else
    {
        value = tbidToString_[id];
    }
}

void UI::TBIDRegisterStringCallback(unsigned id, const char* value)
{
    uiContext_->GetSubsystem<UI>()->tbidToString_[id] = String(value);
}

bool UI::LoadResourceFile(TBWidget* widget, const String& filename)
{

    tb::TBNode node;

    if (!node.ReadFile(filename.CString()))
        return false;

    tb::g_widgets_reader->LoadNodeTree(widget, &node);
    return true;
}


void UI::HandleScreenMode(StringHash eventType, VariantMap& eventData)
{
    using namespace ScreenMode;
    rootWidget_->SetSize(eventData[P_WIDTH].GetInt(), eventData[P_HEIGHT].GetInt());
}

void UI::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    if (exitRequested_) {
        SendEvent(E_EXITREQUESTED);
        exitRequested_ = false;
        return;
    }
    
    tooltipHoverTime_ += eventData[Update::P_TIMESTEP].GetFloat();

    if (tooltipHoverTime_ >= 0.5f)
    {
        UIWidget* hoveredWidget = GetHoveredWidget();
        if (hoveredWidget && !tooltip_ && (hoveredWidget->GetShortened() || hoveredWidget->GetTooltip().Length() > 0))
        {
            tooltip_ = new UIPopupWindow(context_, true, hoveredWidget, "tooltip");
            UILayout* tooltipLayout = new UILayout(context_, UI_AXIS_Y, true);
            if (hoveredWidget->GetShortened())
            {
                UITextField* fullTextField = new UITextField(context_, true);
                fullTextField->SetText(hoveredWidget->GetText());
                tooltipLayout->AddChild(fullTextField);
            }
            if (hoveredWidget->GetTooltip().Length() > 0)
            {
                UITextField* tooltipTextField = new UITextField(context_, true);
                tooltipTextField->SetText(hoveredWidget->GetTooltip());
                tooltipLayout->AddChild(tooltipTextField);
            }
            Input* input = GetSubsystem<Input>();
            IntVector2 mousePosition = input->GetMousePosition();
            tooltip_->AddChild(tooltipLayout);
            tooltip_->Show(mousePosition.x_ + 8, mousePosition.y_ + 8);
        }
    }
    else 
    {
        if (tooltip_) tooltip_->Close();
    }

    SendEvent(E_UIUPDATE);
    TBMessageHandler::ProcessMessages();
}

UIWidget* UI::GetHoveredWidget()
{
    return WrapWidget(TBWidget::hovered_widget);
}

bool UI::IsWidgetWrapped(tb::TBWidget* widget)
{
    return widgetWrap_.Contains(widget);
}

bool UI::UnwrapWidget(tb::TBWidget* widget)
{
    if (widgetWrap_.Contains(widget))
    {
        widgetWrap_.Erase(widget);
        return true;
    }

    return false;

}

void UI::PruneUnreachableWidgets()
{
    HashMap<tb::TBWidget*, SharedPtr<UIWidget>>::Iterator itr;

    for (itr = widgetWrap_.Begin(); itr != widgetWrap_.End(); )
    {
        if ((*itr).first_->GetParent() || (*itr).second_->JSGetHeapPtr())
        {
            itr++;
            continue;
        }

        tb::TBWidget* toDelete = (*itr).first_;

        itr.GotoNext();

        delete toDelete;

        // this will likely be flagged by valgrind as accessing invalid memory
        assert(!widgetWrap_.Contains(toDelete));
    }
}

void UI::WrapWidget(UIWidget* widget, tb::TBWidget* tbwidget)
{
    assert (!widgetWrap_.Contains(tbwidget));

    widgetWrap_[tbwidget] = widget;
    widget->AddRef();
}

UIWidget* UI::WrapWidget(tb::TBWidget* widget)
{
    if (!widget)
        return NULL;

    if (widgetWrap_.Contains(widget))
        return widgetWrap_[widget];

    // switch this to use a factory?

    // this is order dependent as we're using IsOfType which also works if a base class

    if (widget->IsOfType<TBPopupWindow>())
    {
        UIPopupWindow* popupWindow = new UIPopupWindow(context_, false);
        popupWindow->SetWidget(widget);
        WrapWidget(popupWindow, widget);
        return popupWindow;
    }

    if (widget->IsOfType<TBDimmer>())
    {
        UIDimmer* dimmer = new UIDimmer(context_, false);
        dimmer->SetWidget(widget);
        WrapWidget(dimmer, widget);
        return dimmer;
    }

    if (widget->IsOfType<TBScrollContainer>())
    {
        UIScrollContainer* container = new UIScrollContainer(context_, false);
        container->SetWidget(widget);
        WrapWidget(container, widget);
        return container;
    }

    if (widget->IsOfType<TBInlineSelect>())
    {
        UIInlineSelect* select = new UIInlineSelect(context_, false);
        select->SetWidget(widget);
        WrapWidget(select, widget);
        return select;
    }

    if (widget->IsOfType<TBSlider>())
    {
        UISlider* slider = new UISlider(context_, false);
        slider->SetWidget(widget);
        WrapWidget(slider, widget);
        return slider;
    }

    if (widget->IsOfType<TBColorWidget>())
    {
        UIColorWidget* colorWidget = new UIColorWidget(context_, false);
        colorWidget->SetWidget(widget);
        WrapWidget(colorWidget, widget);
        return colorWidget;
    }

    if (widget->IsOfType<TBColorWheel>())
    {
        UIColorWheel* colorWheel = new UIColorWheel(context_, false);
        colorWheel->SetWidget(widget);
        WrapWidget(colorWheel, widget);
        return colorWheel;
    }

    if (widget->IsOfType<TBSection>())
    {
        UISection* section = new UISection(context_, false);
        section->SetWidget(widget);
        WrapWidget(section, widget);
        return section;
    }

    if (widget->IsOfType<TBSeparator>())
    {
        UISeparator* sep = new UISeparator(context_, false);
        sep->SetWidget(widget);
        WrapWidget(sep, widget);
        return sep;
    }

    if (widget->IsOfType<TBContainer>())
    {
        UIContainer* container = new UIContainer(context_, false);
        container->SetWidget(widget);
        WrapWidget(container, widget);
        return container;
    }

    if (widget->IsOfType<TBSelectDropdown>())
    {
        UISelectDropdown* select = new UISelectDropdown(context_, false);
        select->SetWidget(widget);
        WrapWidget(select, widget);
        return select;
    }

    if (widget->IsOfType<TBButton>())
    {
        // don't wrap the close button of a TBWindow.close
        if (widget->GetID() == TBIDC("TBWindow.close"))
            return 0;

        UIButton* button = new UIButton(context_, false);
        button->SetWidget(widget);
        WrapWidget(button, widget);
        return button;
    }

    if (widget->IsOfType<TBTextField>())
    {
        UITextField* textfield = new UITextField(context_, false);
        textfield->SetWidget(widget);
        WrapWidget(textfield, widget);
        return textfield;
    }

    if (widget->IsOfType<TBEditField>())
    {
        UIEditField* editfield = new UIEditField(context_, false);
        editfield->SetWidget(widget);
        WrapWidget(editfield, widget);
        return editfield;
    }

    if (widget->IsOfType<TBSkinImage>())
    {
        UISkinImage* skinimage = new UISkinImage(context_, "", false);
        skinimage->SetWidget(widget);
        WrapWidget(skinimage, widget);
        return skinimage;
    }

    if (widget->IsOfType<TBImageWidget>())
    {
        UIImageWidget* imagewidget = new UIImageWidget(context_, false);
        imagewidget->SetWidget(widget);
        WrapWidget(imagewidget, widget);
        return imagewidget;
    }
    if (widget->IsOfType<TBClickLabel>())
    {
        UIClickLabel* nwidget = new UIClickLabel(context_, false);
        nwidget->SetWidget(widget);
        WrapWidget(nwidget, widget);
        return nwidget;
    }

    if (widget->IsOfType<TBCheckBox>())
    {
        UICheckBox* nwidget = new UICheckBox(context_, false);
        nwidget->SetWidget(widget);
        WrapWidget(nwidget, widget);
        return nwidget;
    }

    if (widget->IsOfType<TBSelectList>())
    {
        UISelectList* nwidget = new UISelectList(context_, false);
        nwidget->SetWidget(widget);
        WrapWidget(nwidget, widget);
        return nwidget;
    }

    if (widget->IsOfType<TBMessageWindow>())
    {
        UIMessageWindow* nwidget = new UIMessageWindow(context_, NULL, "", false);
        nwidget->SetWidget(widget);
        WrapWidget(nwidget, widget);
        return nwidget;
    }

    if (widget->IsOfType<TBTabContainer>())
    {
        UITabContainer* nwidget = new UITabContainer(context_, false);
        nwidget->SetWidget(widget);
        WrapWidget(nwidget, widget);
        return nwidget;
    }

    if (widget->IsOfType<SceneViewWidget>())
    {
        UISceneView* nwidget = new UISceneView(context_, false);
        nwidget->SetWidget(widget);
        WrapWidget(nwidget, widget);
        return nwidget;
    }


    if (widget->IsOfType<TBLayout>())
    {
        UILayout* layout = new UILayout(context_, (UI_AXIS) widget->GetAxis(), false);
        layout->SetWidget(widget);
        WrapWidget(layout, widget);
        return layout;
    }

    if (widget->IsOfType<TBWidget>())
    {
        UIWidget* nwidget = new UIWidget(context_, false);
        nwidget->SetWidget(widget);
        WrapWidget(nwidget, widget);
        return nwidget;
    }


    return 0;
}

void UI::OnWidgetDelete(tb::TBWidget *widget)
{

}

bool UI::OnWidgetDying(tb::TBWidget *widget)
{
    return false;
}

void UI::OnWidgetFocusChanged(TBWidget *widget, bool focused)
{
    if (widgetWrap_.Contains(widget))
    {
        VariantMap evData;
        UIWidget* uiWidget = widgetWrap_[widget];
        evData[UIWidgetFocusChanged::P_WIDGET]  = uiWidget;
        evData[UIWidgetFocusChanged::P_FOCUSED]  = focused;
        uiWidget->SendEvent(E_UIWIDGETFOCUSCHANGED, evData);
    }
}

void UI::ShowDebugHud(bool value)
{
    SystemUI::DebugHud* hud = GetSubsystem<SystemUI::DebugHud>();

    if (!hud)
        return;

    if (value)
        hud->SetMode(SystemUI::DEBUGHUD_SHOW_ALL);
    else
        hud->SetMode(SystemUI::DEBUGHUD_SHOW_NONE);
}

void UI::ToggleDebugHud()
{
    SystemUI::DebugHud* hud = GetSubsystem<SystemUI::DebugHud>();

    if (!hud)
        return;

    hud->ToggleAll();
}

void UI::ShowConsole(bool value)
{
    SystemUI::Console* console = GetSubsystem<SystemUI::Console>();

    if (!console)
        return;

    console->SetVisible(value);
    consoleVisible_ = console->IsVisible();
}

void UI::ToggleConsole()
{
    SystemUI::Console* console = GetSubsystem<SystemUI::Console>();

    if (!console)
        return;

    console->Toggle();
    consoleVisible_ = console->IsVisible();
}

void UI::HandleConsoleClosed(StringHash eventType, VariantMap& eventData)
{
    consoleVisible_ = false;
}

SystemUI::MessageBox* UI::ShowSystemMessageBox(const String& title, const String& message)
{

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* xmlFile = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");

    SystemUI::MessageBox* messageBox = new SystemUI::MessageBox(context_, message, title, 0, xmlFile);

    return messageBox;


}

UIWidget* UI::GetWidgetAt(int x, int y, bool include_children)
{
    return WrapWidget(rootWidget_->GetWidgetAt(x, y, include_children));
}

bool UI::OnWidgetInvokeEvent(tb::TBWidget *widget, const tb::TBWidgetEvent &ev)
{
    return false;
}

void UI::DebugShowSettingsWindow(UIWidget* parent)
{

#ifdef ATOMIC_DEBUG
    if (parent && parent->GetInternalWidget())
        tb::ShowDebugInfoSettingsWindow(parent->GetInternalWidget());
#endif

}



}


#include <TurboBadger/tb_core.h>
#include <TurboBadger/tb_system.h>
#include <TurboBadger/tb_debug.h>
#include <TurboBadger/animation/tb_widget_animation.h>
#include <TurboBadger/renderers/tb_renderer_batcher.h>
#include <TurboBadger/tb_font_renderer.h>
#include <TurboBadger/tb_node_tree.h>
#include <TurboBadger/tb_widgets_reader.h>
#include <TurboBadger/tb_window.h>

void register_tbbf_font_renderer();
void register_stb_font_renderer();
void register_freetype_font_renderer();

using namespace tb;

#include "../Core/CoreEvents.h"
#include "../Input/Input.h"
#include "../Input/InputEvents.h"
#include "../Resource/ResourceCache.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/GraphicsEvents.h"
#include "../Graphics/Texture2D.h"
#include "../Graphics/VertexBuffer.h"

#include "UIRenderer.h"
#include "UIView.h"

namespace Atomic
{

WeakPtr<Context> UIView::readerContext_;


UIView::UIView(Context* context) :
    Object(context),
    rootWidget_(0),
    inputDisabled_(false),
    keyboardDisabled_(false)
{
    Graphics* graphics = GetSubsystem<Graphics>();
    assert(graphics);
    assert(graphics->IsInitialized());
    graphics_ = graphics;

    vertexBuffer_ = new VertexBuffer(context_);

}

UIView::~UIView()
{

}

// refactor
void UIView::Initialize()
{
    readerContext_ = context_;
    TBFile::SetReaderFunction(TBFileReader);

    TBWidgetsAnimationManager::Init();

    renderer_ = new UIRenderer(graphics_->GetContext());
    tb_core_init(renderer_, "AtomicEditor/resources/language/lng_en.tb.txt");

    // Load the default skin, and override skin that contains the graphics specific to the demo.
    tb::g_tb_skin->Load("AtomicEditor/resources/default_skin/skin.tb.txt", "AtomicEditor/editor/skin/skin.tb.txt");

    //register_tbbf_font_renderer();
    //register_stb_font_renderer();
    register_freetype_font_renderer();

    tb::g_font_manager->AddFontInfo("AtomicEditor/resources/MesloLGS-Regular.ttf", "Monaco");
    tb::g_font_manager->AddFontInfo("AtomicEditor/resources/vera.ttf", "Vera");

    tb::TBFontDescription fd;
    fd.SetID(tb::TBIDC("Vera"));
    fd.SetSize(tb::g_tb_skin->GetDimensionConverter()->DpToPx(12));
    tb::g_font_manager->SetDefaultFontDescription(fd);

    // Create the font now.
    tb::TBFontFace *font = tb::g_font_manager->CreateFontFace(tb::g_font_manager->GetDefaultFontDescription());

    // Render some glyphs in one go now since we know we are going to use them. It would work fine
    // without this since glyphs are rendered when needed, but with some extra updating of the glyph bitmap.
    if (font)
        font->RenderGlyphs(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~•·åäöÅÄÖ");

    rootWidget_ = new TBWidget();
    //rootWidget_->SetSkinBg(tb::TBIDC("background"));

    int width = graphics_->GetWidth();
    int height = graphics_->GetHeight();
    rootWidget_->SetSize(width, height);
    //SetSize(width, height);

    rootWidget_->SetVisibilility(tb::WIDGET_VISIBILITY_VISIBLE);

    SubscribeToEvent(E_MOUSEBUTTONDOWN, HANDLER(UIView, HandleMouseButtonDown));
    SubscribeToEvent(E_MOUSEBUTTONUP, HANDLER(UIView, HandleMouseButtonUp));
    SubscribeToEvent(E_MOUSEMOVE, HANDLER(UIView, HandleMouseMove));
    SubscribeToEvent(E_MOUSEWHEEL, HANDLER(UIView, HandleMouseWheel));
    SubscribeToEvent(E_KEYDOWN, HANDLER(UIView, HandleKeyDown));
    SubscribeToEvent(E_KEYUP, HANDLER(UIView, HandleKeyUp));
    SubscribeToEvent(E_TEXTINPUT, HANDLER(UIView, HandleTextInput));
    SubscribeToEvent(E_UPDATE, HANDLER(UIView, HandleUpdate));

    SubscribeToEvent(E_RENDERUPDATE, HANDLER(UIView, HandleRenderUpdate));

    //TB_DEBUG_SETTING(LAYOUT_BOUNDS) = 1;
}


void UIView::Render(VertexBuffer* buffer, const PODVector<UIBatch>& batches, unsigned batchStart, unsigned batchEnd)
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
    graphics_->SetDrawAntialiased(false);
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
        if (graphics_->NeedParameterUpdate(SP_OBJECTTRANSFORM, this))
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

void UIView::SetVertexData(VertexBuffer* dest, const PODVector<float>& vertexData)
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


void UIView::Render()
{
    SetVertexData(vertexBuffer_, vertexData_);
    Render(vertexBuffer_, batches_, 0, batches_.Size());
}

void UIView::HandleRenderUpdate(StringHash eventType, VariantMap& eventData)
{
    // Get rendering batches from the non-modal UI elements
    batches_.Clear();
    vertexData_.Clear();

    tb::TBRect rect = rootWidget_->GetRect();

    IntRect currentScissor = IntRect(0, 0, rect.w, rect.h);
    GetBatches(batches_, vertexData_, currentScissor);

}

void UIView::GetBatches(PODVector<UIBatch>& batches, PODVector<float>& vertexData, const IntRect& currentScissor)
{
    //if (!initialized_)
    //    return;

    // TBAnimationManager::Update();

    rootWidget_->InvokeProcessStates();
    rootWidget_->InvokeProcess();

    tb::g_renderer->BeginPaint(rootWidget_->GetRect().w, rootWidget_->GetRect().h);

    renderer_->currentScissor_ = currentScissor;
    renderer_->batches_ = &batches;
    renderer_->vertexData_ = &vertexData;
    rootWidget_->InvokePaint(tb::TBWidget::PaintProps());

    tb::g_renderer->EndPaint();
}

void UIView::SubmitBatchVertexData(Texture* texture, const PODVector<float>& vertexData)
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

void UIView::TBFileReader(const char* filename, void** data, unsigned* length)
{
    *data = 0;
    *length = 0;

    ResourceCache* cache = readerContext_->GetSubsystem<ResourceCache>();
    SharedPtr<File> file = cache->GetFile(filename);
    if (!file || !file->IsOpen())
        return;

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

void UIView::HandleScreenMode(StringHash eventType, VariantMap& eventData)
{
    using namespace ScreenMode;
    rootWidget_->SetSize(eventData[P_WIDTH].GetInt(), eventData[P_HEIGHT].GetInt());
    //SetSize(eventData[P_WIDTH].GetInt(), eventData[P_HEIGHT].GetInt());
}

static MODIFIER_KEYS GetModifierKeys(int qualifiers, bool superKey)
{
    MODIFIER_KEYS code = TB_MODIFIER_NONE;
    if (qualifiers & QUAL_ALT)	code |= TB_ALT;
    if (qualifiers & QUAL_CTRL)	code |= TB_CTRL;
    if (qualifiers & QUAL_SHIFT)	code |= TB_SHIFT;
    if (superKey)	code |= TB_SUPER;
    return code;
}


// @return Return the upper case of a ascii charcter. Only for shortcut handling.
static int toupr_ascii(int ascii)
{
    if (ascii >= 'a' && ascii <= 'z')
        return ascii + 'A' - 'a';
    return ascii;
}

void UIView::HandleMouseButtonDown(StringHash eventType, VariantMap& eventData)
{
    if (inputDisabled_)
        return;

    using namespace MouseButtonDown;
    unsigned button = eventData[P_BUTTON].GetUInt();

    IntVector2 pos;
    pos = GetSubsystem<Input>()->GetMousePosition();

    Input* input = GetSubsystem<Input>();
    int qualifiers = input->GetQualifiers();

#ifdef ATOMIC_PLATFORM_WINDOWS
    bool superdown = input->GetKeyDown(KEY_LCTRL) || input->GetKeyDown(KEY_RCTRL);
#else
    bool superdown = input->GetKeyDown(KEY_LGUI) || input->GetKeyDown(KEY_RGUI);
#endif

    MODIFIER_KEYS mod = GetModifierKeys(qualifiers, superdown);


    static double last_time = 0;
    static int counter = 1;

    Time* t = GetSubsystem<Time>();

    double time = t->GetElapsedTime() * 1000;
    if (time < last_time + 600)
        counter++;
    else
        counter = 1;

    last_time = time;
    if (button == MOUSEB_RIGHT)
        rootWidget_->InvokeRightPointerDown(pos.x_, pos.y_, counter, mod);
    else
        rootWidget_->InvokePointerDown(pos.x_, pos.y_, counter, mod, false);


}


void UIView::HandleMouseButtonUp(StringHash eventType, VariantMap& eventData)
{
    if (inputDisabled_)
        return;

    using namespace MouseButtonUp;
    unsigned button = eventData[P_BUTTON].GetUInt();

    IntVector2 pos;

    Input* input = GetSubsystem<Input>();
    pos = input->GetMousePosition();
    int qualifiers = input->GetQualifiers();

#ifdef ATOMIC_PLATFORM_WINDOWS
    bool superdown = input->GetKeyDown(KEY_LCTRL) || input->GetKeyDown(KEY_RCTRL);
#else
    bool superdown = input->GetKeyDown(KEY_LGUI) || input->GetKeyDown(KEY_RGUI);
#endif

    MODIFIER_KEYS mod = GetModifierKeys(qualifiers, superdown);


    if (button == MOUSEB_RIGHT)
        rootWidget_->InvokeRightPointerUp(pos.x_, pos.y_, mod);
    else
        rootWidget_->InvokePointerUp(pos.x_, pos.y_, mod, false);
}


void UIView::HandleMouseMove(StringHash eventType, VariantMap& eventData)
{
    using namespace MouseMove;

    if (inputDisabled_)
        return;

    int px = eventData[P_X].GetInt();
    int py = eventData[P_Y].GetInt();

    rootWidget_->InvokePointerMove(px, py, tb::TB_MODIFIER_NONE, false);

}


void UIView::HandleMouseWheel(StringHash eventType, VariantMap& eventData)
{
    if (inputDisabled_)
        return;

    using namespace MouseWheel;

    int delta = eventData[P_WHEEL].GetInt();

    Input* input = GetSubsystem<Input>();

    rootWidget_->InvokeWheel(input->GetMousePosition().x_, input->GetMousePosition().y_, 0, delta > 0 ? -1 : 1, tb::TB_MODIFIER_NONE);

}

static bool InvokeShortcut(int key, SPECIAL_KEY special_key, MODIFIER_KEYS modifierkeys, bool down)
{
#ifdef __APPLE__
    bool shortcut_key = (modifierkeys & TB_SUPER) ? true : false;
#else
    bool shortcut_key = (modifierkeys & TB_CTRL) ? true : false;
#endif
    if (!TBWidget::focused_widget || !down || (!shortcut_key && special_key ==TB_KEY_UNDEFINED))
        return false;
    bool reverse_key = (modifierkeys & TB_SHIFT) ? true : false;
    int upper_key = toupr_ascii(key);
    TBID id;
    if (upper_key == 'X')
        id = TBIDC("cut");
    else if (upper_key == 'C' || special_key == TB_KEY_INSERT)
        id = TBIDC("copy");
    else if (upper_key == 'V' || (special_key == TB_KEY_INSERT && reverse_key))
        id = TBIDC("paste");
    else if (upper_key == 'A')
        id = TBIDC("selectall");
    else if (upper_key == 'Z' || upper_key == 'Y')
    {
        bool undo = upper_key == 'Z';
        if (reverse_key)
            undo = !undo;
        id = undo ? TBIDC("undo") : TBIDC("redo");
    }
    else if (upper_key == 'N')
        id = TBIDC("new");
    else if (upper_key == 'O')
        id = TBIDC("open");
    else if (upper_key == 'S')
        id = TBIDC("save");
    else if (upper_key == 'W')
        id = TBIDC("close");
    else if (upper_key == 'F')
        id = TBIDC("find");
 #ifdef ATOMIC_PLATFORM_OSX
    else if (upper_key == 'G' && (modifierkeys & TB_SHIFT))
        id = TBIDC("findprev");
    else if (upper_key == 'G')
        id = TBIDC("findnext");
#else
    else if (special_key == TB_KEY_F3 && (modifierkeys & TB_SHIFT))
        id = TBIDC("findprev");
    else if (special_key == TB_KEY_F3)
        id = TBIDC("findnext");

#endif
    else if (upper_key == 'P')
        id = TBIDC("play");
    else if (upper_key == 'I')
        id = TBIDC("beautify");
    else if (special_key == TB_KEY_PAGE_UP)
        id = TBIDC("prev_doc");
    else if (special_key == TB_KEY_PAGE_DOWN)
        id = TBIDC("next_doc");
    else
        return false;

    TBWidgetEvent ev(EVENT_TYPE_SHORTCUT);
    ev.modifierkeys = modifierkeys;
    ev.ref_id = id;
    return TBWidget::focused_widget->InvokeEvent(ev);
}

static bool InvokeKey(TBWidget* root, unsigned int key, SPECIAL_KEY special_key, MODIFIER_KEYS modifierkeys, bool keydown)
{
    if (InvokeShortcut(key, special_key, modifierkeys, keydown))
        return true;
    root->InvokeKey(key, special_key, modifierkeys, keydown);
    return true;
}


void UIView::HandleKey(bool keydown, int keycode, int scancode)
{

#ifdef ATOMIC_PLATFORM_WINDOWS
    if (keycode == KEY_LCTRL || keycode == KEY_RCTRL)
        return;
#else
    if (keycode == KEY_LGUI || keycode == KEY_RGUI)
        return;
#endif

    Input* input = GetSubsystem<Input>();
    int qualifiers = input->GetQualifiers();

#ifdef ATOMIC_PLATFORM_WINDOWS
    bool superdown = input->GetKeyDown(KEY_LCTRL) || input->GetKeyDown(KEY_RCTRL);
#else
    bool superdown = input->GetKeyDown(KEY_LGUI) || input->GetKeyDown(KEY_RGUI);
#endif
    MODIFIER_KEYS mod = GetModifierKeys(qualifiers, superdown);

    switch (keycode)
    {
    case KEY_RETURN:
    case KEY_RETURN2:
    case KEY_KP_ENTER:
        InvokeKey(rootWidget_, 0, TB_KEY_ENTER, mod, keydown);
        break;

    case KEY_F1:
        InvokeKey(rootWidget_, 0, TB_KEY_F1, mod, keydown);
        break;
    case KEY_F2:
        InvokeKey(rootWidget_, 0, TB_KEY_F2, mod, keydown);
        break;
    case KEY_F3:
        InvokeKey(rootWidget_, 0, TB_KEY_F3, mod, keydown);
        break;
    case KEY_F4:
        InvokeKey(rootWidget_, 0, TB_KEY_F4, mod, keydown);
        break;
    case KEY_F5:
        InvokeKey(rootWidget_, 0, TB_KEY_F5, mod, keydown);
        break;
    case KEY_F6:
        InvokeKey(rootWidget_, 0, TB_KEY_F6, mod, keydown);
        break;
    case KEY_F7:
        InvokeKey(rootWidget_, 0, TB_KEY_F7, mod, keydown);
        break;
    case KEY_F8:
        InvokeKey(rootWidget_, 0, TB_KEY_F8, mod, keydown);
        break;
    case KEY_F9:
        InvokeKey(rootWidget_, 0, TB_KEY_F9, mod, keydown);
        break;
    case KEY_F10:
        InvokeKey(rootWidget_, 0, TB_KEY_F10, mod, keydown);
        break;
    case KEY_F11:
        InvokeKey(rootWidget_, 0, TB_KEY_F11, mod, keydown);
        break;
    case KEY_F12:
        InvokeKey(rootWidget_, 0, TB_KEY_F12, mod, keydown);
        break;
    case KEY_LEFT:
        InvokeKey(rootWidget_, 0, TB_KEY_LEFT, mod, keydown);
        break;
    case KEY_UP:
        InvokeKey(rootWidget_, 0, TB_KEY_UP, mod, keydown);
        break;
    case KEY_RIGHT:
        InvokeKey(rootWidget_, 0, TB_KEY_RIGHT, mod, keydown);
        break;
    case KEY_DOWN:
        InvokeKey(rootWidget_, 0, TB_KEY_DOWN, mod, keydown);
        break;
    case KEY_PAGEUP:
        InvokeKey(rootWidget_, 0, TB_KEY_PAGE_UP, mod, keydown);
        break;
    case KEY_PAGEDOWN:
        InvokeKey(rootWidget_, 0, TB_KEY_PAGE_DOWN, mod, keydown);
        break;
    case KEY_HOME:
        InvokeKey(rootWidget_, 0, TB_KEY_HOME, mod, keydown);
        break;
    case KEY_END:
        InvokeKey(rootWidget_, 0, TB_KEY_END, mod, keydown);
        break;
    case KEY_INSERT:
        InvokeKey(rootWidget_, 0, TB_KEY_INSERT, mod, keydown);
        break;
    case KEY_TAB:
        InvokeKey(rootWidget_, 0, TB_KEY_TAB, mod, keydown);
        break;
    case KEY_DELETE:
        InvokeKey(rootWidget_, 0, TB_KEY_DELETE, mod, keydown);
        break;
    case KEY_BACKSPACE:
        InvokeKey(rootWidget_, 0, TB_KEY_BACKSPACE, mod, keydown);
        break;
    case KEY_ESC:
        InvokeKey(rootWidget_, 0, TB_KEY_ESC, mod, keydown);
        break;
    default:
        if (mod & TB_SUPER)
        {
            InvokeKey(rootWidget_, keycode, TB_KEY_UNDEFINED, mod, keydown);
        }

    }

}

void UIView::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
    if (inputDisabled_ || keyboardDisabled_)
        return;

    using namespace KeyDown;

    int keycode = eventData[P_KEY].GetInt();
    int scancode = eventData[P_SCANCODE].GetInt();

    HandleKey(true, keycode, scancode);

}

void UIView::HandleKeyUp(StringHash eventType, VariantMap& eventData)
{
    if (inputDisabled_ || keyboardDisabled_)
        return;

    using namespace KeyUp;

    int keycode = eventData[P_KEY].GetInt();
    int scancode = eventData[P_SCANCODE].GetInt();

    HandleKey(false, keycode, scancode);

}

void UIView::HandleTextInput(StringHash eventType, VariantMap& eventData)
{
    if (inputDisabled_ || keyboardDisabled_)
        return;

    using namespace TextInput;

    const String& text = eventData[P_TEXT].GetString();

    for (unsigned i = 0; i < text.Length(); i++)
    {
        InvokeKey(rootWidget_, text[i], TB_KEY_UNDEFINED, TB_MODIFIER_NONE, true);
        InvokeKey(rootWidget_, text[i], TB_KEY_UNDEFINED, TB_MODIFIER_NONE, false);
    }

}

void UIView::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    TBMessageHandler::ProcessMessages();
}


}

//
// Copyright (c) 2017 the Atomic project.
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
#define STB_TEXTEDIT_IMPLEMENTATION 1
#include "../../Input/InputEvents.h"
#include "../../Input/Input.h"
#include "../../Core/CoreEvents.h"
#include "../../Core/Context.h"
#include "../../Core/Profiler.h"
#include "../../Engine/EngineEvents.h"
#include "../../Graphics/GraphicsEvents.h"
#include "../../Graphics/Graphics.h"
#include "../../Resource/ResourceCache.h"
#include "../../UI/SystemUI/SystemUI.h"
#include "../../UI/SystemUI/Console.h"
#include "../../UI/SystemUI/DebugHud.h"
#include <SDL.h>


using namespace std::placeholders;
namespace Atomic
{

SystemUI::SystemUI(Atomic::Context* context)
        : Object(context)
          , vertexBuffer_(context)
          , indexBuffer_(context)
{
    ImGuiIO& io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Tab] = SCANCODE_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = SCANCODE_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = SCANCODE_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = SCANCODE_UP;
    io.KeyMap[ImGuiKey_DownArrow] = SCANCODE_DOWN;
    io.KeyMap[ImGuiKey_Home] = SCANCODE_HOME;
    io.KeyMap[ImGuiKey_End] = SCANCODE_END;
    io.KeyMap[ImGuiKey_Delete] = SCANCODE_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = SCANCODE_BACKSPACE;
    io.KeyMap[ImGuiKey_Enter] = SCANCODE_RETURN;
    io.KeyMap[ImGuiKey_Escape] = SCANCODE_ESCAPE;
    io.KeyMap[ImGuiKey_A] = SCANCODE_A;
    io.KeyMap[ImGuiKey_C] = SCANCODE_C;
    io.KeyMap[ImGuiKey_V] = SCANCODE_V;
    io.KeyMap[ImGuiKey_X] = SCANCODE_X;
    io.KeyMap[ImGuiKey_Y] = SCANCODE_Y;
    io.KeyMap[ImGuiKey_Z] = SCANCODE_Z;

    io.RenderDrawListsFn = [](ImDrawData* data)
    {
        static_cast<SystemUI*>(ImGui::GetIO().UserData)->OnRenderDrawLists(data);
    };
    io.SetClipboardTextFn = [](void* user_data, const char* text)
    { SDL_SetClipboardText(text); };
    io.GetClipboardTextFn = [](void* user_data) -> const char*
    { return SDL_GetClipboardText(); };

    io.UserData = this;

    io.Fonts->AddFontDefault();
    ReallocateFontTexture();
    UpdateProjectionMatrix();

    // Subscribe to events
    SubscribeToEvent(E_POSTUPDATE, std::bind(&SystemUI::OnPostUpdate, this, _2));
    SubscribeToEvent(E_ENDRENDERING, [&](StringHash, VariantMap&)
    {
        ATOMIC_PROFILE(SystemUiRender);
        ImGui::Render();
    });
    SubscribeToEvent(E_SDLRAWINPUT, std::bind(&SystemUI::OnRawEvent, this, _2));
    SubscribeToEvent(E_SCREENMODE, std::bind(&SystemUI::UpdateProjectionMatrix, this));
}

SystemUI::~SystemUI()
{
    ImGui::Shutdown();
}

void SystemUI::UpdateProjectionMatrix()
{
    // Update screen size
    auto graphics = GetSubsystem<Graphics>();
    ImGui::GetIO().DisplaySize = ImVec2((float)graphics->GetWidth(), (float)graphics->GetHeight());

    // Update projection matrix
    IntVector2 viewSize = graphics->GetViewport().Size();
    Vector2 invScreenSize(1.0f / viewSize.x_, 1.0f / viewSize.y_);
    Vector2 scale(2.0f * invScreenSize.x_, -2.0f * invScreenSize.y_);
    Vector2 offset(-1.0f, 1.0f);

    projection_ = Matrix4(Matrix4::IDENTITY);
    projection_.m00_ = scale.x_ * uiScale_;
    projection_.m03_ = offset.x_;
    projection_.m11_ = scale.y_ * uiScale_;
    projection_.m13_ = offset.y_;
    projection_.m22_ = 1.0f;
    projection_.m23_ = 0.0f;
    projection_.m33_ = 1.0f;
}

void SystemUI::OnRawEvent(VariantMap& args)
{
    auto evt = static_cast<SDL_Event*>(args[SDLRawInput::P_SDLEVENT].Get<void*>());
    auto& io = ImGui::GetIO();
    switch (evt->type)
    {
    case SDL_KEYUP:
    case SDL_KEYDOWN:
    {
        auto code = evt->key.keysym.scancode;
        auto down = evt->type == SDL_KEYDOWN;
        if (code < 512)
            io.KeysDown[code] = down;
        if (evt->key.keysym.sym == SDLK_LCTRL || evt->key.keysym.sym == SDLK_RCTRL)
            io.KeyCtrl = down;
        else if (evt->key.keysym.sym == SDLK_LSHIFT || evt->key.keysym.sym == SDLK_RSHIFT)
            io.KeyShift = down;
        else if (evt->key.keysym.sym == SDLK_LALT || evt->key.keysym.sym == SDLK_RALT)
            io.KeyAlt = down;
        else if (evt->key.keysym.sym == SDLK_LGUI || evt->key.keysym.sym == SDLK_RGUI)
            io.KeySuper = down;
        break;
    }
    case SDL_MOUSEWHEEL:
        io.MouseWheel = evt->wheel.y;
        break;
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN:
        io.MouseDown[evt->button.button - 1] = evt->type == SDL_MOUSEBUTTONDOWN;
    case SDL_MOUSEMOTION:
        io.MousePos.x = evt->motion.x / uiScale_;
        io.MousePos.y = evt->motion.y / uiScale_;
        break;
    case SDL_FINGERUP:
        io.MouseDown[0] = false;
        io.MousePos.x = -1;
        io.MousePos.y = -1;
    case SDL_FINGERDOWN:
        io.MouseDown[0] = true;
    case SDL_FINGERMOTION:
        io.MousePos.x = evt->tfinger.x / uiScale_;
        io.MousePos.y = evt->tfinger.y / uiScale_;
        break;
    case SDL_TEXTINPUT:
        ImGui::GetIO().AddInputCharactersUTF8(evt->text.text);
        break;
    default:
        break;
    }

    switch (evt->type)
    {
    case SDL_KEYUP:
    case SDL_KEYDOWN:
    case SDL_TEXTINPUT:
        args[SDLRawInput::P_CONSUMED] = ImGui::IsAnyItemActive();
        break;
    case SDL_MOUSEWHEEL:
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEMOTION:
    case SDL_FINGERUP:
    case SDL_FINGERDOWN:
    case SDL_FINGERMOTION:
        args[SDLRawInput::P_CONSUMED] = ImGui::IsMouseHoveringAnyWindow();
        break;
    default:
        break;
    }
}

void SystemUI::OnPostUpdate(VariantMap& args)
{
    auto& io = ImGui::GetIO();
    float timeStep = args[PostUpdate::P_TIMESTEP].GetFloat();
    io.DeltaTime = timeStep > 0.0f ? timeStep : 1.0f / 60.0f;
    ImGui::NewFrame();
    ATOMIC_PROFILE(SystemUiFrame);
    SendEvent(E_SYSTEMUIFRAME);
}

void SystemUI::OnRenderDrawLists(ImDrawData* data)
{
    auto _graphics = GetSubsystem<Graphics>();
    // Engine does not render when window is closed or device is lost
    assert(_graphics && _graphics->IsInitialized() && !_graphics->IsDeviceLost());

    ImGuiIO& io = ImGui::GetIO();
    int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
    if (fb_width == 0 || fb_height == 0)
        return;
    data->ScaleClipRects(io.DisplayFramebufferScale);

    for (int n = 0; n < data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = data->CmdLists[n];
        unsigned int idx_buffer_offset = 0;

        // Resize vertex and index buffers on the fly. Once buffer becomes too small for data that is to be rendered
        // we reallocate buffer to be twice as big as we need now. This is done in order to minimize memory reallocation
        // in rendering loop.
        if (cmd_list->VtxBuffer.Size > vertexBuffer_.GetVertexCount())
        {
            PODVector<VertexElement> elems = {VertexElement(TYPE_VECTOR2, SEM_POSITION),
                                              VertexElement(TYPE_VECTOR2, SEM_TEXCOORD),
                                              VertexElement(TYPE_UBYTE4_NORM, SEM_COLOR)
            };
            vertexBuffer_.SetSize((unsigned int)(cmd_list->VtxBuffer.Size * 2), elems, true);
        }
        if (cmd_list->IdxBuffer.Size > indexBuffer_.GetIndexCount())
            indexBuffer_.SetSize((unsigned int)(cmd_list->IdxBuffer.Size * 2), false, true);

#if (defined(_WIN32) && !defined(ATOMIC_D3D11) && !defined(ATOMIC_OPENGL)) || defined(ATOMIC_D3D9)
        for (int i = 0; i < cmd_list->VtxBuffer.Size; i++)
        {
            ImDrawVert& v = cmd_list->VtxBuffer.Data[i];
            v.pos.x += 0.5f;
            v.pos.y += 0.5f;
        }
#endif

        vertexBuffer_.SetDataRange(cmd_list->VtxBuffer.Data, 0, (unsigned int)cmd_list->VtxBuffer.Size, true);
        indexBuffer_.SetDataRange(cmd_list->IdxBuffer.Data, 0, (unsigned int)cmd_list->IdxBuffer.Size, true);

        _graphics->ClearParameterSources();
        _graphics->SetColorWrite(true);
        _graphics->SetCullMode(CULL_NONE);
        _graphics->SetDepthTest(CMP_ALWAYS);
        _graphics->SetDepthWrite(false);
        _graphics->SetFillMode(FILL_SOLID);
        _graphics->SetStencilTest(false);
        _graphics->SetVertexBuffer(&vertexBuffer_);
        _graphics->SetIndexBuffer(&indexBuffer_);

        for (const ImDrawCmd* cmd = cmd_list->CmdBuffer.begin(); cmd != cmd_list->CmdBuffer.end(); cmd++)
        {
            if (cmd->UserCallback)
                cmd->UserCallback(cmd_list, cmd);
            else
            {
                ShaderVariation* ps;
                ShaderVariation* vs;

                Texture2D* texture = static_cast<Texture2D*>(cmd->TextureId);
                if (!texture)
                {
                    ps = _graphics->GetShader(PS, "Basic", "VERTEXCOLOR");
                    vs = _graphics->GetShader(VS, "Basic", "VERTEXCOLOR");
                }
                else
                {
                    // If texture contains only an alpha channel, use alpha shader (for fonts)
                    vs = _graphics->GetShader(VS, "Basic", "DIFFMAP VERTEXCOLOR");
                    if (texture->GetFormat() == Graphics::GetAlphaFormat())
                        ps = _graphics->GetShader(PS, "Basic", "ALPHAMAP VERTEXCOLOR");
                    else
                        ps = _graphics->GetShader(PS, "Basic", "DIFFMAP VERTEXCOLOR");
                }

                _graphics->SetShaders(vs, ps);
                if (_graphics->NeedParameterUpdate(SP_OBJECT, this))
                    _graphics->SetShaderParameter(VSP_MODEL, Matrix3x4::IDENTITY);
                if (_graphics->NeedParameterUpdate(SP_CAMERA, this))
                    _graphics->SetShaderParameter(VSP_VIEWPROJ, projection_);
                if (_graphics->NeedParameterUpdate(SP_MATERIAL, this))
                    _graphics->SetShaderParameter(PSP_MATDIFFCOLOR, Color(1.0f, 1.0f, 1.0f, 1.0f));

                float elapsedTime = GetSubsystem<Time>()->GetElapsedTime();
                _graphics->SetShaderParameter(VSP_ELAPSEDTIME, elapsedTime);
                _graphics->SetShaderParameter(PSP_ELAPSEDTIME, elapsedTime);

                IntRect scissor = IntRect(int(cmd->ClipRect.x * uiScale_), int(cmd->ClipRect.y * uiScale_),
                                          int(cmd->ClipRect.z * uiScale_), int(cmd->ClipRect.w * uiScale_));

                _graphics->SetBlendMode(BLEND_ALPHA);
                _graphics->SetScissorTest(true, scissor);
                _graphics->SetTexture(0, texture);
                _graphics->Draw(TRIANGLE_LIST, idx_buffer_offset, cmd->ElemCount, 0, 0,
                                vertexBuffer_.GetVertexCount());
                idx_buffer_offset += cmd->ElemCount;
            }
        }
    }
    _graphics->SetScissorTest(false);
}

ImFont* SystemUI::AddFont(const String& font_path, float size, const unsigned short* ranges, bool merge)
{
    auto io = ImGui::GetIO();

    if (size == 0)
    {
        if (io.Fonts->Fonts.size() == 0)
            return 0;
        size = io.Fonts->Fonts.back()->FontSize;
    }

    if (auto font_file = GetSubsystem<ResourceCache>()->GetFile(font_path))
    {
        PODVector<uint8_t> data;
        data.Resize(font_file->GetSize());
        auto bytes_len = font_file->Read(&data.Front(), data.Size());
        ImFontConfig cfg;
        cfg.MergeMode = merge;
        cfg.FontDataOwnedByAtlas = false;
        if (auto new_font = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(&data.Front(), bytes_len, size, &cfg, ranges))
        {
            ReallocateFontTexture();
            return new_font;
        }
    }
    return 0;
}

ImFont* SystemUI::AddFont(const Atomic::String& font_path, float size,
                          const std::initializer_list<unsigned short>& ranges, bool merge)
{
    return AddFont(font_path, size, ranges.size() ? &*ranges.begin() : 0, merge);
}

void SystemUI::ReallocateFontTexture()
{
    auto io = ImGui::GetIO();
    // Create font texture.
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width,
                                 &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.

    if (!fontTexture_ || width != fontTexture_->GetWidth() || height != fontTexture_->GetHeight())
    {
        fontTexture_ = new Texture2D(context_);
        fontTexture_->SetNumLevels(1);
        fontTexture_->SetSize(width, height, Graphics::GetRGBAFormat());
        fontTexture_->SetData(0, 0, 0, width, height, pixels);
        fontTexture_->SetFilterMode(FILTER_BILINEAR);

        // Store our identifier
        io.Fonts->TexID = (void*)fontTexture_.Get();
        io.Fonts->ClearTexData();
    }
}

void SystemUI::SetScale(float scale)
{
    if (uiScale_ == scale)
        return;
    uiScale_ = scale;
    UpdateProjectionMatrix();
}

void SystemUI::CreateConsoleAndDebugHud()
{
    Console* console = new Console(context_);
    DebugHud* debugHud = new DebugHud(context_);

    // Create console & debug hud
    context_->RegisterSubsystem(console);
    context_->RegisterSubsystem(debugHud);

}

}


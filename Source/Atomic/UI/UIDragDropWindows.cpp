
#define WIN32_LEAN_AND_MEAN
#define STRICT
#ifndef UNICODE
#define UNICODE 1
#endif
#undef _WIN32_WINNT
#define _WIN32_WINNT  0x501   /* Need 0x410 for AlphaBlend() and 0x500 for EnumDisplayDevices(), 0x501 for raw input */

#include <Atomic/IO/Log.h>
#include <Atomic/Core/Context.h>
#include <Atomic/Core/CoreEvents.h>
#include <Atomic/Input/InputEvents.h>
#include <Atomic/Graphics/Graphics.h>

#include "UIDragDrop.h"
#include "UIDragDropMac.h"

#include <ThirdParty/SDL/include/SDL_syswm.h>

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>

#define WIN_StringToUTF8(S) SDL_iconv_string("UTF-8", "UTF-16LE", (char *)(S), (SDL_wcslen(S)+1)*sizeof(WCHAR))

namespace Atomic
{
    class UIDragDropWindows : public Object
    {

        OBJECT(UIDragDropWindows);

    public:
        /// Construct.
        UIDragDropWindows(Context* context);
        virtual ~UIDragDropWindows();

        void HandleUpdate(StringHash eventType, VariantMap& eventData);

        static WNDPROC sdlWndProc_;

        HWND hwnd_;

    private:

    };

    WNDPROC UIDragDropWindows::sdlWndProc_ = NULL;
    static WeakPtr<UIDragDrop> dragAndDrop_;

    void UpdateMousePos(HWND hwnd)
    {
        if (!hwnd || dragAndDrop_.Null())
            return;

        POINT p;
        GetCursorPos(&p);
        ScreenToClient(hwnd, &p);
        using namespace Atomic::MouseMove;

        Atomic::VariantMap mvEventData;
        int x = p.x;
        int y = p.y;

        Graphics* graphics = dragAndDrop_->GetSubsystem<Graphics>();

        if ((x >= 0 && x < graphics->GetWidth()) && y >= 0 && y < graphics->GetHeight())
        {
            mvEventData[P_X] = x;
            mvEventData[P_Y] = y;
            mvEventData[P_DX] = 0;
            mvEventData[P_DY] = 0;
            mvEventData[P_BUTTONS] = 0;
            mvEventData[P_QUALIFIERS] = 0;

            dragAndDrop_->SendEvent(E_MOUSEMOVE, mvEventData);
        }

    }

    LRESULT CALLBACK Atomic_WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (dragAndDrop_.Null())
        {
            return CallWindowProc(UIDragDropWindows::sdlWndProc_, hwnd, msg, wParam, lParam);
        }

        switch (msg) {

        case WM_DROPFILES:
        {

            //implement the IDropTarget interface and then register the HWND using RegisterDragDrop() to start receiving notifications. Be sure to call RevokeDragDrop() before exiting the app.

            UINT i;
            HDROP drop = (HDROP)wParam;
            UINT count = DragQueryFile(drop, 0xFFFFFFFF, NULL, 0);

            if (count)
            {
                dragAndDrop_->FileDragEntered();

                UpdateMousePos(hwnd);

                for (i = 0; i < count; ++i) {
                    UINT size = DragQueryFile(drop, i, NULL, 0) + 1;
                    LPTSTR buffer = SDL_stack_alloc(TCHAR, size);
                    if (buffer) {
                        if (DragQueryFile(drop, i, buffer, size)) {

                            char *file = WIN_StringToUTF8(buffer);
                            dragAndDrop_->FileDragAddFile(file);
                            SDL_free(file);
                        }
                        SDL_stack_free(buffer);
                    }
                }

                dragAndDrop_->FileDragConclude();

            }

            // sdlWndProc will handle this
            //DragFinish(drop);
            break;
        }

        };

        return CallWindowProc(UIDragDropWindows::sdlWndProc_, hwnd, msg, wParam, lParam);
    }

    UIDragDropWindows::UIDragDropWindows(Context* context) : Object(context),
        hwnd_(NULL)
    {
        SDL_Window* window = (SDL_Window*)GetSubsystem<Graphics>()->GetSDLWindow();

        SDL_SysWMinfo info;
        SDL_VERSION(&info.version);

        if (SDL_GetWindowWMInfo(window, &info)) {

            hwnd_ = info.info.win.window;
            // hook the wnd proc so we can handle drag/drop ourselves
            sdlWndProc_ = (WNDPROC)GetWindowLongPtr(hwnd_, GWLP_WNDPROC);
            SetWindowLongPtr(hwnd_, GWLP_WNDPROC, (LONG_PTR)Atomic_WindowProc);
        }

        SubscribeToEvent(E_UPDATE, HANDLER(UIDragDropWindows, HandleUpdate));

    }

    UIDragDropWindows::~UIDragDropWindows()
    {

    }

    void UIDragDropWindows::HandleUpdate(StringHash eventType, VariantMap & eventData)
    {

        if (!hwnd_ || dragAndDrop_.Null())
            return;

        if (GetActiveWindow() == hwnd_)
            return;

        UpdateMousePos(hwnd_);

    }

    void InitDragAndDrop(UIDragDrop *dragAndDrop)
    {
        dragAndDrop_ = dragAndDrop;
        dragAndDrop->GetContext()->RegisterSubsystem(new UIDragDropWindows(dragAndDrop->GetContext()));

    }

}
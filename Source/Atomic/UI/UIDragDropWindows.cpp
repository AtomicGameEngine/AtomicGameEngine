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

#ifdef ATOMIC_PLATFORM_WINDOWS

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
#include <OleIdl.h>

namespace Atomic
{

    // global weakptr to dragAndDrop system as it is sometimes accessed outside of an Object method
    static WeakPtr<UIDragDrop> dragAndDrop_;

    // Win32 proc hook so we can catch close and unregister drag and drop
    static LRESULT CALLBACK Atomic_WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    class CDropTarget : public IDropTarget
    {
    public:
        // IUnknown implementation
        HRESULT __stdcall QueryInterface(REFIID iid, void ** ppvObject);
        ULONG	__stdcall AddRef(void);
        ULONG	__stdcall Release(void);

        void UpdateMousePos();

        // IDropTarget implementation
        HRESULT __stdcall DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);
        HRESULT __stdcall DragOver(DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);
        HRESULT __stdcall DragLeave(void);
        HRESULT __stdcall Drop(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);

        void OpenFilesFromDataObject(IDataObject * pdto);

        // Constructor
        CDropTarget(HWND hwnd);
        ~CDropTarget();

    private:

        // internal helper function
        DWORD DropEffect(DWORD grfKeyState, POINTL pt, DWORD dwAllowed);
        bool  QueryDataObject(IDataObject *pDataObject);

        // Private member variables
        LONG	m_lRefCount;
        HWND	m_hWnd;
        bool    m_fAllowDrop;

        IDataObject *m_pDataObject;

    };

    CDropTarget::CDropTarget(HWND hwnd)
    {
        m_lRefCount = 1;
        m_hWnd = hwnd;
        m_fAllowDrop = false;
    }

    CDropTarget::~CDropTarget()
    {

    }

    HRESULT __stdcall CDropTarget::QueryInterface(REFIID iid, void ** ppvObject)
    {
        if (iid == IID_IDropTarget || iid == IID_IUnknown)
        {
            AddRef();
            *ppvObject = this;
            return S_OK;
        }
        else
        {
            *ppvObject = 0;
            return E_NOINTERFACE;
        }
    }

    ULONG __stdcall CDropTarget::AddRef(void)
    {
        return InterlockedIncrement(&m_lRefCount);
    }

    ULONG __stdcall CDropTarget::Release(void)
    {
        LONG count = InterlockedDecrement(&m_lRefCount);

        if (count == 0)
        {
            delete this;
            return 0;
        }
        else
        {
            return count;
        }
    }

    void CDropTarget::UpdateMousePos()
    {
        if (!m_hWnd || dragAndDrop_.Null())
            return;

        POINT p;
        GetCursorPos(&p);
        ScreenToClient(m_hWnd, &p);
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


    bool CDropTarget::QueryDataObject(IDataObject *pDataObject)
    {
        FORMATETC fmte = { CF_HDROP, NULL, DVASPECT_CONTENT,
            -1, TYMED_HGLOBAL };


        // does the data object support CF_HDROP using a HGLOBAL?
        return pDataObject->QueryGetData(&fmte) == S_OK ? true : false;
    }

    DWORD CDropTarget::DropEffect(DWORD grfKeyState, POINTL pt, DWORD dwAllowed)
    {
        DWORD dwEffect = 0;

        // 1. check "pt" -> do we allow a drop at the specified coordinates?

        // 2. work out that the drop-effect should be based on grfKeyState
        if (grfKeyState & MK_CONTROL)
        {
            dwEffect = dwAllowed & DROPEFFECT_COPY;
        }
        else if (grfKeyState & MK_SHIFT)
        {
            dwEffect = dwAllowed & DROPEFFECT_MOVE;
        }

        // 3. no key-modifiers were specified (or drop effect not allowed), so
        //    base the effect on those allowed by the dropsource
        if (dwEffect == 0)
        {
            if (dwAllowed & DROPEFFECT_COPY) dwEffect = DROPEFFECT_COPY;
            if (dwAllowed & DROPEFFECT_MOVE) dwEffect = DROPEFFECT_MOVE;
        }

        return dwEffect;
    }

    HRESULT __stdcall CDropTarget::DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
    {

        if (dragAndDrop_.Null())
            return S_OK;

        // does the dataobject contain data we want?
        m_fAllowDrop = QueryDataObject(pDataObject);

        if (m_fAllowDrop)
        {
            // get the dropeffect based on keyboard state
            *pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);

            SetFocus(m_hWnd);

            dragAndDrop_->FileDragEntered();
            UpdateMousePos();

        }
        else
        {
            *pdwEffect = DROPEFFECT_NONE;
        }

        return S_OK;
    }

    HRESULT __stdcall CDropTarget::DragOver(DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
    {
        if (m_fAllowDrop)
        {
            *pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
            UpdateMousePos();
        }
        else
        {
            *pdwEffect = DROPEFFECT_NONE;
        }

        return S_OK;
    }

    HRESULT __stdcall CDropTarget::DragLeave(void)
    {
        return S_OK;
    }

    void CDropTarget::OpenFilesFromDataObject(IDataObject *pdto)
    {
        FORMATETC fmte = { CF_HDROP, NULL, DVASPECT_CONTENT,
            -1, TYMED_HGLOBAL };

        STGMEDIUM stgm;

        if (SUCCEEDED(pdto->GetData(&fmte, &stgm)))
        {
            PVOID data = GlobalLock(stgm.hGlobal);

            HDROP hdrop = reinterpret_cast<HDROP>(stgm.hGlobal);

            UINT cFiles = DragQueryFile(hdrop, 0xFFFFFFFF, NULL, 0);

            for (UINT i = 0; i < cFiles; i++)
            {
                TCHAR szFile[MAX_PATH];
                UINT cch = DragQueryFile(hdrop, i, szFile, MAX_PATH);

                if (cch > 0 && cch < MAX_PATH)
                {
                    dragAndDrop_->FileDragAddFile(szFile);
                }
            }

            DragFinish(hdrop);

            GlobalUnlock(stgm.hGlobal);

            ReleaseStgMedium(&stgm);

            dragAndDrop_->FileDragConclude();
        }
    }

    HRESULT __stdcall CDropTarget::Drop(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
    {
        if (dragAndDrop_.Null())
            return S_OK;

        UpdateMousePos();

        if (m_fAllowDrop)
        {
            OpenFilesFromDataObject(pDataObject);

            *pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
        }
        else
        {
            *pdwEffect = DROPEFFECT_NONE;
        }

        return S_OK;
    }


    static void RegisterDropWindow(HWND hwnd, IDropTarget **ppDropTarget)
    {
        OleInitialize(0);

        CDropTarget *pDropTarget = new CDropTarget(hwnd);

        // acquire a strong lock
        CoLockObjectExternal(pDropTarget, TRUE, FALSE);

        // tell OLE that the window is a drop target
        HRESULT result = RegisterDragDrop(hwnd, pDropTarget);

        *ppDropTarget = pDropTarget;
    }

    static void UnregisterDropWindow(HWND hwnd, IDropTarget *pDropTarget)
    {
        // remove drag+drop
        HRESULT result = RevokeDragDrop(hwnd);

        // remove the strong lock
        CoLockObjectExternal(pDropTarget, FALSE, TRUE);

        // release our own reference
        pDropTarget->Release();

        OleUninitialize();
    }

    // UIDragDropWindows

    class UIDragDropWindows : public Object
    {
        ATOMIC_OBJECT(UIDragDropWindows, Object);

    public:
        /// Construct.
        UIDragDropWindows(Context* context);
        virtual ~UIDragDropWindows();

        void Shutdown();

        static WNDPROC sdlWndProc_;

    private:

        IDropTarget* dropTarget_;
        HWND hwnd_;

    };

    WNDPROC UIDragDropWindows::sdlWndProc_ = NULL;

    UIDragDropWindows::UIDragDropWindows(Context* context) : Object(context),
        dropTarget_(NULL), hwnd_(NULL)
    {
        SDL_Window* window = (SDL_Window*)GetSubsystem<Graphics>()->GetSDLWindow();

        SDL_SysWMinfo info;
        SDL_VERSION(&info.version);

        if (SDL_GetWindowWMInfo(window, &info))
        {

            hwnd_ = info.info.win.window;

            // hook the wnd proc so we can catch close and deregister drag and drop
            sdlWndProc_ = (WNDPROC)GetWindowLongPtr(hwnd_, GWLP_WNDPROC);
            SetWindowLongPtr(hwnd_, GWLP_WNDPROC, (LONG_PTR)Atomic_WindowProc);

            RegisterDropWindow(hwnd_, &dropTarget_);

        }

    }

    UIDragDropWindows::~UIDragDropWindows()
    {

    }

    void UIDragDropWindows::Shutdown()
    {
        if (!hwnd_ || !dropTarget_)
            return;

        UnregisterDropWindow(hwnd_, dropTarget_);

        // restore SDL winproc
        SetWindowLongPtr(hwnd_, GWLP_WNDPROC, (LONG_PTR)sdlWndProc_);

    }

    void InitDragAndDrop(UIDragDrop *dragAndDrop)
    {
        dragAndDrop_ = dragAndDrop;
        dragAndDrop->GetContext()->RegisterSubsystem(new UIDragDropWindows(dragAndDrop->GetContext()));

    }

    LRESULT CALLBACK Atomic_WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (dragAndDrop_.Null())
        {
            return CallWindowProc(UIDragDropWindows::sdlWndProc_, hwnd, msg, wParam, lParam);
        }

        switch (msg) {

        case WM_CLOSE:
        {
            dragAndDrop_->GetSubsystem<UIDragDropWindows>()->Shutdown();
            break;
        }

        };

        return CallWindowProc(UIDragDropWindows::sdlWndProc_, hwnd, msg, wParam, lParam);
    }

}

#endif

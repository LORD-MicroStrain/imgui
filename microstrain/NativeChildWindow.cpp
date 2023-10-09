#include <microstrain/NativeChildWindow.hpp>

  


////////////////////////////////////////////////////////////////////////////////
///

#if defined _WIN32

    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif

    #include <windows.h>
    #include <stdio.h>

    bool NativeChildWindow::create(void *parent_window, int x_pos, int y_pos, int x_size, int y_size)
    {
        m_parent_window = parent_window;

        HWND window;

        HINSTANCE hInstance = GetModuleHandle(nullptr);
        HICON icon = (HICON)LoadImage(
            hInstance, IDI_APPLICATION, IMAGE_ICON, GetSystemMetrics(SM_CXICON),
            GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);

        WNDCLASSEXW wc;
        ZeroMemory(&wc, sizeof(WNDCLASSEX));
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.hInstance = GetModuleHandle(0);
        wc.lpszClassName = L"nativeChildWindow";
        wc.hIcon = icon;
        wc.lpfnWndProc =
            (WNDPROC)(+[](HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) -> LRESULT
            {
                    switch(msg)
                    {
                    //Stop the mouse wheel event from propagating to the parent window!
                    case WM_MOUSEWHEEL:
                    {
                    }break;

                    default: return DefWindowProcW(hwnd, msg, wp, lp);
                    }
                   
                  return 0;
            });

        RegisterClassExW(&wc);
                  
        window = CreateWindowW(L"nativeChildWindow", L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
        CW_USEDEFAULT, CW_USEDEFAULT, x_size, y_size, (static_cast<HWND>(m_parent_window)),
        nullptr, GetModuleHandle(0), nullptr);

        if (window == nullptr)
        {
            DWORD result = GetLastError();
            return false;
        }

        m_native_window = static_cast<void*>(window);
   
        SetWindowPos((HWND)m_native_window, HWND_TOP, x_pos, y_pos, x_size, y_size, SWP_NOCOPYBITS | SWP_NOACTIVATE);
        BringWindowToTop((HWND)m_native_window);
        ShowWindow((HWND)m_native_window, SW_SHOW);
        UpdateWindow((HWND)m_native_window);

        return true;
    }

    bool NativeChildWindow::destroy()
    {
        if(m_native_window != nullptr)
           DestroyWindow((HWND)m_native_window);

        m_native_window = nullptr;
        return false;
    }

    bool NativeChildWindow::enableHighDpi()
    {
        /*
        auto user32 = native_library(L"user32.dll");
        if (auto fn = user32.get(user32_symbols::SetProcessDpiAwarenessContext)) {
            if (fn(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE)) {
                return true;
            }
            return GetLastError() == ERROR_ACCESS_DENIED;
        }
        if (auto shcore = native_library(L"shcore.dll")) {
            if (auto fn = shcore.get(shcore_symbols::SetProcessDpiAwareness)) {
                auto result = fn(shcore_symbols::PROCESS_PER_MONITOR_DPI_AWARE);
                return result == S_OK || result == E_ACCESSDENIED;
            }
        }
        if (auto fn = user32.get(user32_symbols::SetProcessDPIAware)) {
            return !!fn();
        }
        */
        return true;
    }

    void NativeChildWindow::setSize(int x, int y, int x_size, int y_size)
    {
        if(!m_native_window)
         return;

        SetWindowPos((HWND)m_native_window, nullptr, x, y, x_size, y_size,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

    }

    void NativeChildWindow::hide() 
    {
        if(!m_native_window)
         return;

        ShowWindow((HWND)m_native_window, SW_HIDE);
    }

    void NativeChildWindow::show()
    {
        if(!m_native_window)
         return;

        ShowWindow((HWND)m_native_window, SW_SHOW);
    }

#endif



////////////////////////////////////////////////////////////////////////////////
///

//Note: due to the complexity of setting up an XWindow, we are leveraging SDL on Linux

#if defined __linux__
    #include <X11/Xlib.h>
    #include <SDL2/SDL.h>
    #include <SDL_syswm.h>
    //#include "SDL_internal.h"
    //#include "SDL2/src/ase-2.26.5-fdc15bb8fb.clean/src/video/x11/SDL_x11video.h"
    //#include "SDL_x11video.h"

    bool NativeChildWindow::create(void *parent_window, int x_pos, int y_pos, int x_size, int y_size)
    {
        if(parent_window == nullptr)
          return false;

        m_parent_window = parent_window;
         
        Display *parent_display;
        Window   parent;
  
        SDL_SysWMinfo info = SDL_SysWMinfo();
        SDL_VERSION(&info.version);

        if (SDL_GetWindowWMInfo((SDL_Window*)parent_window, &info) == SDL_TRUE)
        {
            parent_display = info.info.x11.display;
            parent         = info.info.x11.window;
        }
        else
          return false;

        //Try native X11 window
        m_native_window = (void*)XCreateWindow(parent_display, parent, x_pos, y_pos, x_size, y_size, 0, CopyFromParent, 
                       CopyFromParent, CopyFromParent, 0, 0);

        XSelectInput(parent_display, (Window)m_native_window,
                 (FocusChangeMask | EnterWindowMask | LeaveWindowMask |
                 ExposureMask | ButtonPressMask | ButtonReleaseMask |
                 PointerMotionMask | KeyPressMask | KeyReleaseMask |
                 PropertyChangeMask | StructureNotifyMask |
                 KeymapStateMask));

        XFlush(parent_display);
        m_parent_display = (void*)parent_display;

        return true;
    }


    bool NativeChildWindow::destroy()
    {
        if(!m_parent_display || !m_native_window)
            return false;

        XDestroyWindow((Display*)m_parent_display, (Window)m_native_window);

        return false;
    }

    bool NativeChildWindow::enableHighDpi()
    {
       return true;
    }

    void NativeChildWindow::hide() 
    {
        if(!m_parent_display || !m_native_window)
            return;

        XUnmapWindow((Display*)m_parent_display, (Window)m_native_window);
     }

    void NativeChildWindow::show()
    {
        if(!m_parent_display || !m_native_window)
            return;
        
        XMapWindow((Display*)m_parent_display, (Window)m_native_window);
    }

    void NativeChildWindow::setSize(int x, int y, int x_size, int y_size)
    {
        if(!m_parent_display || !m_native_window)
            return;
        
        XResizeWindow((Display*)m_parent_display, (Window)m_native_window, x_size, y_size);
        XMoveWindow((Display*)m_parent_display, (Window)m_native_window, x, y);
     }


#endif



////////////////////////////////////////////////////////////////////////////////
///

#if defined __APPLE__
#include <CoreGraphics/CoreGraphics.h>
#include <objc/NSObjCRuntime.h>
#include <objc/objc-runtime.h>

 
 #define NSWindowAbove 1

enum NSBackingStoreType : NSUInteger { NSBackingStoreBuffered = 2 };

       enum NSWindowStyleMask : NSUInteger {
  NSWindowStyleMaskBorderless = 0,
  NSWindowStyleMaskTitled = 1,
  NSWindowStyleMaskClosable = 2,
  NSWindowStyleMaskMiniaturizable = 4,
  NSWindowStyleMaskResizable = 8
};

enum NSApplicationActivationPolicy : NSInteger {
  NSApplicationActivationPolicyRegular = 0
};

// Convenient conversion of string literals.
inline id operator"" _cls(const char *s, std::size_t) {
  return (id)objc_getClass(s);
}
inline SEL operator"" _sel(const char *s, std::size_t) {
  return sel_registerName(s);
}

namespace objc {

template <typename Result, typename Callable, typename... Args>
Result invoke(Callable callable, Args... args) noexcept {
  return reinterpret_cast<Result (*)(Args...)>(callable)(args...);
}

// Calls objc_msgSend.
template <typename Result, typename... Args>
Result msg_send(Args... args) noexcept {
  return invoke<Result>(objc_msgSend, args...);
}


inline id operator"" _str(const char *s, std::size_t) {
  return objc::msg_send<id>("NSString"_cls, "stringWithUTF8String:"_sel, s);
}
}

 bool NativeChildWindow::create(void* parent_window, int x_pos, int y_pos, int x_size, int y_size)
 {
      m_parent_window = parent_window;
      m_native_window = objc::msg_send<id>("NSWindow"_cls, "alloc"_sel);

      if(!m_native_window)
        return false;

      auto style = NSWindowStyleMaskBorderless;
      m_native_window = objc::msg_send<id>(
          m_native_window, "initWithContentRect:styleMask:backing:defer:"_sel,
          CGRectMake(0, 0, 0, 0), style, NSBackingStoreBuffered, NO);

      objc::msg_send<id>(m_parent_window, "addChildWindow:ordered:"_sel, m_native_window, NSWindowAbove);
      objc::msg_send<id>(m_native_window, "setLevel:"_sel, 6);
             
      //objc::msg_send<void>(m_native_window, "orderFront:"_sel, nullptr);
      
      return true;
    }

bool NativeChildWindow::destroy()
{
    if (!m_native_window)
        return false;

    objc::msg_send<void>(m_native_window, "close"_sel);

    return true;
}

bool NativeChildWindow::enableHighDpi()
{
      if (!m_native_window)
        return false;

      void *view = objc::msg_send<id>(m_native_window, "contentView"_sel);
      objc::msg_send<id>(view, "setWantsBestResolutionOpenGLSurface:"_sel, YES);

      return true;
}

    void NativeChildWindow::hide() 
    {
        if (!m_native_window)
           return;

      objc::msg_send<id>(m_native_window, "orderOut:"_sel, "self");
    }

    void NativeChildWindow::show()
    {
        if (!m_native_window)
           return;

        objc::msg_send<id>(m_parent_window, "addChildWindow:ordered:"_sel, m_native_window, NSWindowAbove);
    
    }

    void NativeChildWindow::setSize(int x, int y, int x_size, int y_size)
    {
       auto style = static_cast<NSWindowStyleMask>(NSWindowStyleMaskBorderless);
        
       objc::msg_send<void>(m_native_window, "setStyleMask:"_sel, style);
       objc::msg_send<void>(m_native_window, "setFrame:display:animate:"_sel,
                           CGRectMake(x, y, x_size, y_size), YES, NO);
    }
   

#endif






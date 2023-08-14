#include <microstrain/NativeChildWindow.hpp>

  


////////////////////////////////////////////////////////////////////////////////
///

#if defined _WIN32

    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif

    #include <windows.h>

    bool NativeChildWindow::create(void *parent_window)
    {
        m_parent_window = parent_window;

        WNDCLASSEXW wc;
        ZeroMemory(&wc, sizeof(WNDCLASSEX));
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.hInstance = GetModuleHandle(0);
        wc.lpszClassName = L"nativeChildWindow";
        //wc.hIcon = icon;
        wc.lpfnWndProc =
            (WNDPROC)(+[](HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) -> LRESULT
            {
                auto w = (NativeChildWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
                
                switch (msg)
                {
                    case WM_SIZE:
                        //w->resize(hwnd);
                        break;
                    case WM_CLOSE:
                        DestroyWindow(hwnd);
                        break;
                    case WM_DESTROY:
                        //w->terminate();
                        break;
                        /*
                    case WM_GETMINMAXINFO:
                    {
                        auto lpmmi = (LPMINMAXINFO)lp;
                        if (w == nullptr)
                        {
                            return 0;
                        }
                        if (w->m_maxsz.x > 0 && w->m_maxsz.y > 0) {
                            lpmmi->ptMaxSize = w->m_maxsz;
                            lpmmi->ptMaxTrackSize = w->m_maxsz;
                        }
                        if (w->m_minsz.x > 0 && w->m_minsz.y > 0) {
                            lpmmi->ptMinTrackSize = w->m_minsz;
                        }
                    } break;*/
                    default:
                        return DefWindowProcW(hwnd, msg, wp, lp);
                }
                return 0;
            });

        RegisterClassExW(&wc);

        m_native_window = CreateWindowW(L"nativeChildWindow", L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
            CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, *(static_cast<HWND*>(m_parent_window)),
            nullptr, GetModuleHandle(0), nullptr);

        if (m_native_window == nullptr)
        {
            DWORD result = GetLastError();
            return false;
        }

        SetWindowLongPtr((HWND)m_native_window, GWLP_USERDATA, (LONG_PTR)this);

        ShowWindow((HWND)m_native_window, SW_SHOW);
        UpdateWindow((HWND)m_native_window);

        return true;
    }

    bool NativeChildWindow::destroy()
    {
        DestroyWindow((HWND)m_native_window);

        return false;
    }

    bool NativeChildWindow::enable_high_dpi()
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


    /*
    void set_size(int left, int top, int width, int height, int hints) {
        if (hints == WEBVIEW_HINT_MAX) {
            m_maxsz.x = width;
            m_maxsz.y = height;
        }
        else if (hints == WEBVIEW_HINT_MIN) {
            m_minsz.x = width;
            m_minsz.y = height;
        }
        else {
            SetWindowPos(m_window, nullptr, left, top, width, height,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
            resize(m_window);
        }
    }*/
#endif



////////////////////////////////////////////////////////////////////////////////
///

#if defined __linux__

    bool NativeChildWindow::create(void *parent_window)
    {
        return false;
    }


    bool NativeChildWindow::destroy()
    {
        return false;
    }

    void NativeChildWindow::enable_high_dpi()
    {

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

//static uint32_t ref_counter = 0;

/*
  void set_size(int left, int top, int width, int height, int hints) {
    auto style = static_cast<NSWindowStyleMask>(NSWindowStyleMaskBorderless);

    objc::msg_send<void>(m_window, "setStyleMask:"_sel, style);

      objc::msg_send<void>(m_window, "setFrame:display:animate:"_sel,
                           CGRectMake(left, top, width, height), YES, NO);
  }

  void show_window(bool show)
  {
    auto application = get_shared_application();
    auto main_window = objc::msg_send<id>(application, "mainWindow"_sel);

    if(show)
    {
      objc::msg_send<id>(main_window, "addChildWindow:ordered:"_sel, m_window, NSWindowAbove);
      objc::msg_send<id>(m_window, "orderFront"_sel); 
    }
    else
    {
      objc::msg_send<id>(main_window, "removeChildWindow:"_sel, m_window);
      objc::msg_send<id>(m_window, "orderOut:"_sel, "self");
    }
  }
  */

 bool NativeChildWindow::create(void* parent_window)
 {
    m_parent_window = parent_window;

      m_native_window = objc::msg_send<id>("NSWindow"_cls, "alloc"_sel);

      if(!m_native_window)
        return false;

      auto style = NSWindowStyleMaskBorderless;
      m_native_window = objc::msg_send<id>(
          m_native_window, "initWithContentRect:styleMask:backing:defer:"_sel,
          CGRectMake(0, 0, 0, 0), style, NSBackingStoreBuffered, NO);

      //auto application = get_shared_application();
      //auto main_window = objc::msg_send<id>(application, "mainWindow"_sel);

      //objc::msg_send<id>(main_window, "addChildWindow:ordered:"_sel, m_window, NSWindowAbove);
      objc::msg_send<id>(m_parent_window, "addChildWindow:ordered:"_sel, m_native_window, NSWindowAbove);

      //nsview = [nswindow contentView];

      //void *view = objc::msg_send<id>(m_native_window, "contentView"_sel);
     // objc::msg_send<id>(view, "setWantsBestResolutionOpenGLSurface:"_sel, YES);
      
       //objc::msg_send<id>(m_native_window, "orderFront:"_sel, nullptr);
       objc::msg_send<id>(m_native_window, "setLevel:"_sel, 6);
       
      
      //objc::msg_send<void>(m_native_window, "orderFront:"_sel, nullptr);
      
      return true;
    }

bool NativeChildWindow::destroy(void* parent_window)
{
    if (!m_native_window)
        return false;

    objc::msg_send<void>(m_native_window, "close"_sel);

    return true;
}

bool NativeChildWindow::enable_high_dpi()
{
      void *view = objc::msg_send<id>(m_native_window, "contentView"_sel);
      objc::msg_send<id>(view, "setWantsBestResolutionOpenGLSurface:"_sel, YES);

      return true;
}

#endif







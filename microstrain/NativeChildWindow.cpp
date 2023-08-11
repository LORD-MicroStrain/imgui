#include <microstrain/NativeChildWindow.hpp>

    ////////////////////////////////////////////////////////////////////////////////
    ///

    bool NativeChildWindow::create(void *parent_window)
    {
//#if defined IC_LINUX
//       return create_linux(parent_window);
//#elif defined IC_WINDOWS
//        return create_windows(parent_window);
//#elif defined IC_APPLE
       return create_mac(parent_window);
//#else
//        return false;
//#endif


    }

bool NativeChildWindow::destroy()
{
//#if defined IC_LINUX
//       return create_linux(parent_window);
//#elif defined IC_WINDOWS
//        return create_windows(parent_window);
//#elif defined IC_APPLE
       return destroy_mac();
//#else
//        return false;
//#endif



}



////////////////////////////////////////////////////////////////////////////////
///

#if defined IC_WINDOWS

    bool NativeChildWindow::create_windows(void *parent_window)
    {

    }

#else
    bool NativeChildWindow::create_windows(void *parent_window) {return false;};
#endif



////////////////////////////////////////////////////////////////////////////////
///

#if defined IC_LINUX

    bool NativeChildWindow::create_linux(void *parent_window)
    {

    }
#else
    bool NativeChildWindow::create_linux(void *parent_window) {return false;};
#endif



////////////////////////////////////////////////////////////////////////////////
///

// #if defined IC_APPLE
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
bool NativeChildWindow::create_mac(void *parent_window)
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

      objc::msg_send<void>(m_native_window, "orderFront:"_sel, nullptr);
      
      return true;
    }
//#else
//    bool NativeChildWindow::create_mac(void *parent_window) {return false;};
//#endif


bool NativeChildWindow::destroy_windows()
{
    return false;
}
bool NativeChildWindow::destroy_linux()
{
   return false;
}

bool NativeChildWindow::destroy_mac()
{
    if(!m_native_window)
    return false;

    objc::msg_send<void>(m_native_window, "close"_sel);
   
    return true;
}

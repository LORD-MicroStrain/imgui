#include <Utility/NativeChildWindow.hpp>

namespace Utility
{
   


    ////////////////////////////////////////////////////////////////////////////////
    ///

    bool NativeChildWindow::create(void *parent_window)
    {
#if defined IC_LINUX
       return create_linux(parent_window);
#elif defined IC_WINDOWS
        return create_windows(parent_window);
#elif defined IC_APPLE
       return create_mac(parent_window);
#else
        retrun false;
#endif


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

 #if defined IC_APPLE
   bool NativeChildWindow::create_mac(void *parent_window)
    {



    }
#else
    bool NativeChildWindow::create_mac(void *parent_window) {return false;};
#endif

}// namespace Utility

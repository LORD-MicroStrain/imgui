#pragma once

namespace Utility
{
   
    class NativeChildWindow
    {
    public:

        bool create(void *parent_window);
        void *get() { return m_native_window;};

    private:

        bool create_windows(void *parent_window);
        bool create_linux(void *parent_window);
        bool create_mac(void *parent_window);
        
        void *m_parent_window = nullptr;
        void *m_native_window = nullptr;
    };

}// namespace Utility

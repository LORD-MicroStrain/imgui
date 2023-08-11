#pragma once


    class NativeChildWindow
    {
    public:

        bool create(void *parent_window);
        bool destroy();
        void *get() { return m_native_window;};

    private:

        bool create_windows(void *parent_window);
        bool create_linux(void *parent_window);
        bool create_mac(void *parent_window);

        bool destroy_windows();
        bool destroy_linux();
        bool destroy_mac();
        
        void *m_parent_window = nullptr;
        void *m_native_window = nullptr;
    };


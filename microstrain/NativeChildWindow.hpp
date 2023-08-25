#pragma once

    //Note: due to the complexity of setting up an XWindow, we are leveraging SDL on Linux

    class NativeChildWindow
    {
    public:
        bool create(void *parent_window, int x_pos=0, int y_pos=0, int x_size=100, int y_size=100);
        bool destroy();
        bool enableHighDpi();

        void *get() { return m_native_window;};

        void hide();
        void show();

        void setSize(int x, int y, int x_size, int y_size);
        
#ifdef __linux__
        void *get_display() { return m_parent_display;};
#endif

    private:
        void *m_parent_window = nullptr;
        void *m_native_window = nullptr;

#ifdef __linux__
        void *m_parent_display = nullptr;
#endif
    };


#pragma once

    //Note: due to the complexity of setting up an XWindow, we are leveraging SDL on Linux

    class NativeChildWindow
    {
    public:
        bool create(void *parent_window, int x_pos=0, int y_pos=0, int x_size=100, int y_size=100);
        bool destroy();
        bool enable_high_dpi();

        void *get() { return m_native_window;};

    private:
        void *m_parent_window = nullptr;
        void *m_native_window = nullptr;

    };


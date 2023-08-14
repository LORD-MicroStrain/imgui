#pragma once


    class NativeChildWindow
    {
    public:

        bool create(void *parent_window);
        bool destroy();
        bool enable_high_dpi();

        void *get() { return m_native_window;};

    private:

        void *m_parent_window = nullptr;
        void *m_native_window = nullptr;
    };


#include "ui.hpp"
#include "ui_widget.hpp"
#include "ui_navigation.hpp"
#include "string_format.hpp"

namespace ui
{
    class esp32Update : public View                                // App class declaration
    {
    public:
        esp32Update(NavigationView &nav);                          // App class init function declaration
        std::string title() const override { return "esp32 update"; }; // App title

    private:
        // 测试按钮获取I2C设备的状态
        Button button_helloWorld
        {
            {UI_POS_X(0), UI_POS_Y_BOTTOM(5), 100, UI_POS_HEIGHT(2)},
            "Hello World!"
        };

        Button button_get_all_dev
        {
            {UI_POS_X(0), UI_POS_Y_BOTTOM(8), 100, UI_POS_HEIGHT(2)},
            "get all"
        };

        Text text_module_name{
            {
                UI_POS_X(0), UI_POS_Y(3), UI_POS_WIDTH_REMAINING(6), UI_POS_HEIGHT(1)
            },
            "",
        };

        Text text_module_version{
            {
                UI_POS_X(0), UI_POS_Y(4), UI_POS_WIDTH_REMAINING(6), UI_POS_HEIGHT(1)
            },
            "",
        };

        Text text_module_API{
            {
                UI_POS_X(0), UI_POS_Y(5), UI_POS_WIDTH_REMAINING(6), UI_POS_HEIGHT(1)
            },
            "",
        };

        void get_all_info();
        void get_ppmod_device_info();
        void update();                                            // Function declaration

        // 这里如何接收来自ppmod的数据呢？
        MessageHandlerRegistration message_handler_update{        // Example, not required: MessageHandlerRegistration class
            Message::ID::DisplayFrameSync,                        // relays messages to your app code from baseband. Every time you 
            [this](const Message *const) {                        // get a  DisplayFrameSync message the update() function will
                this->update();                                   // be triggered.  
            }};
    };
} 
#include "ui.hpp"
#include "ui_widget.hpp"
#include "ui_navigation.hpp"
#include "string_format.hpp"
#include "file.hpp"

#include "baseband_api.hpp"
#include "replay_thread.hpp"
#include "ui_receiver.hpp"      // AudioVolumeField 定义
#include "receiver_model.hpp"   // receiver_model 声明

namespace ui
{
    struct music_file_entry {
        std::string path{};
        // 文件类型 0表示 MAV 1表示MP3
        int type;
    };

    class mp3player : public View                                // App class declaration
    {
    public:
        mp3player(NavigationView &nav);                       // App class init function declaration
        std::string title() const override { return "mp3 player"; }; // App title
        ~mp3player();

        // 当前mp3的信息
        std::list<music_file_entry> entry_list{};
        // 指向当前路径
        std::filesystem::path current_path{u"/mp3"};
        // 加载当前
        void load_directory_contents();
        // 刷新当前menu view 列表
        void refresh_list();
        // 分页数量
        uint8_t nb_pages = 1;
        // 每页显示的元素个数
        static constexpr size_t items_per_page = 8;
        // 当前页面
        uint8_t pagination = 0;
        // 返回按钮
        const std::string str_back{"<--"};
        // 前进按钮
        const std::string str_next{"-->"};
        // 选择当前页面
        const music_file_entry& get_selected_entry() const;
        // 设置当前页面焦点
        void focus() override;
        void on_focus() override;
        void on_blur() override;
        void paint(Painter& painter) override; 
        // 回调函数必须常驻内存
        std::function<void(KeyEvent)> on_select_entry{nullptr};

        // 发送给tx baseband的文件传输线程
        std::unique_ptr<ReplayThread> replay_thread{};
        // 读取缓冲区大小
        const size_t read_size{2048};
        // 读取缓冲区个数
        const size_t buffer_count{3};
        bool ready_signal{false};
        void on_playback_progress(const uint32_t progress);
        void handle_replay_thread_done(const uint32_t return_code);
        void stop();

        void reflash_special_widget();
        // hz
        uint32_t sample_rate = {0};
        uint32_t sample_count = {0};
        uint8_t bits_per_sample ={0};
        uint8_t wav_channel ={0};
        uint32_t duration_time = {0};


        uint32_t now_second = {0};
        uint32_t now_millionsecond = {0};
    private:
        Labels labels{
        {
            {0, 0}, 
            "Path:", 
            Theme::getInstance()->fg_light->foreground}
        };
        // 显示当前路径
        Text text_current{
            {
                UI_POS_X(6), UI_POS_Y(0), UI_POS_WIDTH_REMAINING(6), UI_POS_HEIGHT(1)
            },
            "",
        };
        Text text_debug{
            {
                UI_POS_X(0), UI_POS_Y(1), UI_POS_WIDTH_REMAINING(6), UI_POS_HEIGHT(1)
            },
            "",
        };
        
        ProgressBar progressbar{
            {UI_POS_X(0), UI_POS_Y_BOTTOM(10), screen_width, 8}
        };

        // 这里显示一些读取文件的参数 channel hz
        Labels music_hz_label{
        {
            {UI_POS_X(0), UI_POS_Y_BOTTOM(9)}, 
            "Hz:", 
            Theme::getInstance()->fg_light->foreground}
        };

        Text music_hz
        {
            {
                UI_POS_X(4), UI_POS_Y_BOTTOM(9), UI_POS_WIDTH_REMAINING(6), UI_POS_HEIGHT(1)
            },
            "",
        };

        Labels music_channel_label{
        {
            {UI_POS_X(0), UI_POS_Y_BOTTOM(8)}, 
            "channel:", 
            Theme::getInstance()->fg_light->foreground}
        };

        Text music_channel
        {
            {
                UI_POS_X(9), UI_POS_Y_BOTTOM(8), UI_POS_WIDTH_REMAINING(6), UI_POS_HEIGHT(1)
            },
            "",
        };

        Labels music_time_duration{
        {
            {UI_POS_X(0), UI_POS_Y_BOTTOM(7)}, 
            "time:               /", 
            Theme::getInstance()->fg_light->foreground}
        };
        Text music_time_receive
        {
            {
                UI_POS_X(6), UI_POS_Y_BOTTOM(7), UI_POS_WIDTH(10), UI_POS_HEIGHT(1)
            },
            ""
        };
        Text music_time_total
        {
            {
                UI_POS_X(22), UI_POS_Y_BOTTOM(7), UI_POS_WIDTH_REMAINING(6), UI_POS_HEIGHT(1)
            },
            ""
        };

        Text play_info{
            {
                UI_POS_X(0), UI_POS_Y_BOTTOM(6), UI_POS_WIDTH_REMAINING(6), UI_POS_HEIGHT(1)
            },
            "not playing",
        };

        // 显示文件列表，可以多创建一些文件测试一下分页
        MenuView menu_view{
            {0, UI_POS_Y(2), UI_POS_MAXWIDTH,UI_POS_HEIGHT(10)},
            true
        };

        NewButton button_start{
            {UI_POS_X(0), UI_POS_Y_BOTTOM(4), UI_POS_WIDTH(8), UI_POS_HEIGHT(3)},
            "play",
            &bitmap_icon_replay,
            Theme::getInstance()->fg_blue->foreground
        };
        
        Text voltext
        {
            {
               UI_POS_X_RIGHT(6), UI_POS_Y_BOTTOM(2), UI_POS_WIDTH_REMAINING(6), UI_POS_HEIGHT(1)
            },
            "vol:",
        };
        
        AudioVolumeField field_volume{
        {UI_POS_X_RIGHT(2), UI_POS_Y_BOTTOM(2)}};


        void set_ready();
        void update();
                                                    // Function declaration
        MessageHandlerRegistration message_handler_update{        // Example, not required: MessageHandlerRegistration class
            Message::ID::DisplayFrameSync,                        // relays messages to your app code from baseband. Every time you 
            [this](const Message *const) {                        // get a  DisplayFrameSync message the update() function will
                this->update();                                   // be triggered.  
        }};


        MessageHandlerRegistration message_handler_tx_progress{
        Message::ID::TXProgress,
        [this](const Message* const p) {
            const auto message = *reinterpret_cast<const TXProgressMessage*>(p);
            this->on_playback_progress(message.progress);
        }};


        MessageHandlerRegistration message_handler_fifo_signal{
            Message::ID::RequestSignal,
            [this](const Message* const p) {
            const auto message = static_cast<const RequestSignalMessage*>(p);
            if (message->signal == RequestSignalMessage::Signal::FillRequest) {
                this->set_ready();
            }
        }};

        MessageHandlerRegistration message_handler_replay_thread_error{
            Message::ID::ReplayThreadDone,
            [this](const Message* const p) {
            const auto message = *reinterpret_cast<const ReplayThreadDoneMessage*>(p);
            this->handle_replay_thread_done(message.return_code);
        }};

    };
} 
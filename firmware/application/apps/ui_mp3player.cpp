#include "ui_mp3player.hpp"
#include "portapack.hpp"
#include <cstring>

#include "usb_serial_asyncmsg.hpp"
#include "io_wave.hpp"
#include "audio.hpp"

using namespace portapack;

namespace ui
{

    mp3player::mp3player(NavigationView &nav) // Application Main
    {
        // 增加一个远端运行的镜像
        baseband::run_image(portapack::spi_flash::image_tag_audio_tx);
        // App code
        add_children({
            &labels,
            &text_current,
            &text_debug,
            &menu_view,
            // 这个点击会图标
            &button_start,
            &voltext,
            &field_volume,
            &progressbar,
            &music_hz_label,
            &music_hz,
            &music_channel_label,
            &music_channel,
            &music_time_duration,
            &music_time_total,
            &music_time_receive,
            &play_info
        });



        menu_view.set_parent_rect({0, UI_POS_Y(2), UI_POS_MAXWIDTH, UI_POS_HEIGHT_REMAINING(7)});

        // 没有sd卡就退出
        if (!sdcIsCardInserted(&SDCD1)) {
            text_current.set("NO SD CARD!");
            return;
        }

        load_directory_contents();
        // 当前列表没有数据
        if (!entry_list.size()) {
            text_current.set("EMPTY SD CARD!");
        }
        else 
        {
            menu_view.on_left = [this]() {
                // std::string tmp = "menu_view.on_left";
                // UsbSerialAsyncmsg::asyncmsg(tmp);
                button_start.focus();
                reflash_special_widget();
            };

            menu_view.on_right = [this]() {
                // std::string tmp = "menu_view.on_left";
                // UsbSerialAsyncmsg::asyncmsg(tmp);
                button_start.focus();
                reflash_special_widget();
            };
        }

        // 设置主页面的焦点函数
        menu_view.on_highlight = [this]() {
            uint32_t idx = menu_view.highlighted_index();
            // std::string tmp = "menu_view.on_highlight value is" + std::to_string(idx);
            // text_debug.set(tmp);
            
            // 当menu_view高亮变化时，重绘所有text组件以防止被覆盖
            reflash_special_widget();
            
        };

        refresh_list();

        // 设置menu item点击函数事件
        on_select_entry = [this](KeyEvent key) {
            if (key == KeyEvent::Select) {
                const music_file_entry tmp = get_selected_entry();
                std::string debug_string = "clicked item and name is : "+tmp.path;
                text_debug.set(debug_string);
                
                if(tmp.path == str_back)
                {
                    pagination--;
                    menu_view.set_highlighted(0);
                    load_directory_contents();
                    refresh_list();
                }
                else if(tmp.path == str_next)
                {
                    pagination++;
                    menu_view.set_highlighted(0);
                    load_directory_contents();
                    refresh_list();
                }
            }
        };

        button_start.on_select = [this]() {
            
            // 如果已经打开就关闭
            if((bool)replay_thread)
            {
                stop();
                return;
            }

            const music_file_entry tmp = get_selected_entry();
            std::string debug_string;
            
            // 加载文件阅览器
            auto reader = std::make_unique<WAVFileReader>();
            stop();
            if (!reader->open("/mp3/"+tmp.path)) {
                debug_string = "cant open "+tmp.path;
                text_debug.set(debug_string);;
                return;
            }

            button_start.set_bitmap(&bitmap_stop);

            sample_rate = reader->sample_rate();
            bits_per_sample = reader->bits_per_sample();
            wav_channel = reader->channels();
            sample_count = reader->sample_count();
            duration_time = reader->ms_duration();

            music_hz.set(std::to_string(sample_rate));
            music_channel.set(std::to_string(wav_channel));
            music_time_total.set(unit_auto_scale(duration_time, 2, 3) + "s");
            ready_signal = false;

            progressbar.set_max(sample_count);
            replay_thread = std::make_unique<ReplayThread>(
                std::move(reader),
                read_size, buffer_count,
                &ready_signal,
                [](uint32_t return_code) {
                ReplayThreadDoneMessage message{return_code};
                EventDispatcher::send_message(message);
            });

            baseband::set_audiotx_config(
                1536000 / 20,     // Rate of sending progress updates
                0,                // Transmit BW = 0 = not transmitting
                0,                // Gain - unused
                8,                // shift_bits_s16, default 8 bits - unused
                bits_per_sample ,  // bits_per_sample
                0,                // tone key disabled
                false,            // AM
                false,            // DSB
                false,            // USB
                false             // LSB
            );

            baseband::set_sample_rate(sample_rate);
            transmitter_model.set_sampling_rate(1536000);
            audio::output::start();
            std::string tmp_s = "playing music "+tmp.path;
            play_info.set(tmp_s);
            // 设置当前音量
            receiver_model.set_normalized_headphone_volume(75);
        };
    }

    void mp3player::set_ready() 
    {
        ready_signal = true;
    }
    
    const music_file_entry& mp3player::get_selected_entry() const {
        auto it = entry_list.begin();
        if (menu_view.highlighted_index() >= 1) std::advance(it, menu_view.highlighted_index());
        return *it;
    }
    void mp3player::reflash_special_widget()
    {
        labels.set_dirty();
        text_current.set_dirty();
        text_debug.set_dirty();
        voltext.set_dirty();
        field_volume.set_dirty();
        progressbar.set_dirty();
        music_hz_label.set_dirty();
        music_hz.set_dirty();
        music_channel_label.set_dirty();
        music_channel.set_dirty();
        music_time_duration.set_dirty();
        music_time_total.set_dirty();
        music_time_receive.set_dirty();
        play_info.set_dirty();
        button_start.set_dirty();
    }
    void mp3player::focus()
    {
        // menu_view.focus();
        button_start.focus();
    }

    // 完全模拟ui_fileman实现
    void mp3player::load_directory_contents()
    {
        // 设置当前页面名称
        text_current.set(current_path.string());
        entry_list.clear();
        menu_view.clear();
        // Collect all entries first
        std::list<music_file_entry> all_entries;
        // 将文件数据放置在
        for (const auto& entry : std::filesystem::directory_iterator(current_path, u"*"))
        {
            all_entries.push_back({entry.path().string(), 1});
        }
        // 计算当前有多少页
        nb_pages = (all_entries.size() + items_per_page - 1) / items_per_page;
        if (nb_pages == 0) 
            nb_pages = 1;
        
        size_t start_idx = pagination * items_per_page;
        size_t end_idx = std::min(start_idx + items_per_page, all_entries.size());

        if (pagination > 0) {
            entry_list.push_back({str_back, 2});
        }

        auto it = all_entries.begin();
        std::advance(it, start_idx);

        for (size_t i = start_idx; i < end_idx && it != all_entries.end(); i++, ++it) 
        {
            entry_list.push_back(*it);
        }

        if (end_idx < all_entries.size()) {
            entry_list.push_back({str_next, 2});
        }
    }

    void mp3player::refresh_list()
    {
        // 这个set_dirty很关键，需要进行重新渲染
        set_dirty();
        int32_t prev_highlight = menu_view.highlighted_index();
        menu_view.clear();

        for (const auto& entry : entry_list) {
            auto entry_name = entry.path;
            menu_view.add_item({
                entry_name,
                Theme::getInstance()->fg_yellow->foreground,
                &bitmap_icon_file_wav,
                [this](KeyEvent key) {
                    if (on_select_entry)
                        on_select_entry(key);
                }
            });
        }
        menu_view.set_highlighted(prev_highlight);
    }


    void mp3player::handle_replay_thread_done(const uint32_t return_code) {
        
        progressbar.set_value(0);
        if (return_code == ReplayThread::READ_ERROR)
        {
            play_info.set("cant open file!");
        }
        else
        {
            play_info.set("thread done with code : "+std::to_string(return_code));
        }
    }

    void mp3player::on_playback_progress(const uint32_t progress) {
        // play_info.set("now : "+std::to_string(progress));
        progressbar.set_value(progress);
        if(sample_rate > 0)
        {
            now_second = progress/sample_rate;
            now_millionsecond =  (progress* 1000ull / sample_rate) %1000;
            music_time_receive.set(std::to_string(now_second)+"."+std::to_string(now_millionsecond)+"s");
        }
    }

     void mp3player::on_focus()
    {
        View::on_focus();  // 必须先调用基类方法！
        // 标记整个view为dirty
        set_dirty();
        // 当mp3player获得焦点时，重绘所有控件
        reflash_special_widget();
    }
    
    void mp3player::on_blur()
    {
        View::on_blur();
        // 当mp3player失去焦点时，也重绘所有控件
        reflash_special_widget();
    }

    void mp3player::paint(Painter& painter)
    {
        // 先调用基类paint绘制背景
        View::paint(painter);
        // 重绘所有子控件
        reflash_special_widget();
    }

    void mp3player::update()                   // Every time you get a DisplayFrameSync message this function will be ran
    {
         // Message code
    }

    void mp3player::stop()
    {
        if((bool)replay_thread)
        {
            replay_thread.reset();
        }
        music_hz.set("");
        music_channel.set("");
        audio::output::stop();
        ready_signal = false;
        button_start.set_bitmap(&bitmap_play);
    }

    mp3player::~mp3player()
    {
        stop();
        baseband::shutdown();
    }
}
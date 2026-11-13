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
        // Add an image that runs remotely
        baseband::run_image(portapack::spi_flash::image_tag_audio_tx);
        // App code
        add_children({
            &labels,
            &text_current,
            &text_debug,
            &menu_view,
            // Clicking this changes the icon
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

        // Exit if there is no SD card
        if (!sdcIsCardInserted(&SDCD1)) {
            text_current.set("NO SD CARD!");
            return;
        }

        load_directory_contents();
        // The current list is empty
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

        // Set the focus function of the main page
        menu_view.on_highlight = [this]() {
            uint32_t idx = menu_view.highlighted_index();
            // std::string tmp = "menu_view.on_highlight value is" + std::to_string(idx);
            // text_debug.set(tmp);
            
            // When the menu_view highlight changes, redraw all text widgets so they are not overwritten
            reflash_special_widget();
            
        };

        refresh_list();

        // Set the menu item click handler
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
            
            // Close it if it is already open
            if((bool)replay_thread)
            {
                stop();
                return;
            }

            const music_file_entry tmp = get_selected_entry();
            std::string debug_string;
            
            // Load the file viewer
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
            // Set the current volume
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

    // Implemented exactly like ui_fileman
    void mp3player::load_directory_contents()
    {
        // Set the current page name
        text_current.set(current_path.string());
        entry_list.clear();
        menu_view.clear();
        // Collect all entries first
        std::list<music_file_entry> all_entries;
        // Place the file data in
        for (const auto& entry : std::filesystem::directory_iterator(current_path, u"*"))
        {
            all_entries.push_back({entry.path().string(), 1});
        }
        // Calculate the number of pages
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
        // This set_dirty call is essential: it forces a redraw
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
        View::on_focus();  // The base class method must be called first!
        // Mark the whole view as dirty
        set_dirty();
        // Redraw all widgets when mp3player gains focus
        reflash_special_widget();
    }
    
    void mp3player::on_blur()
    {
        View::on_blur();
        // Also redraw all widgets when mp3player loses focus
        reflash_special_widget();
    }

    void mp3player::paint(Painter& painter)
    {
        // Call the base class paint first to draw the background
        View::paint(painter);
        // Redraw all child widgets
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
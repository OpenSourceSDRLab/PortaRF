/*
 * Copyright (C) 2015 Jared Boone, ShareBrained Technology, Inc.
 * Copyright (C) 2018 Furrtek
 *
 * This file is part of PortaPack.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef __UI_SD_MP3PLAYER_APP_H__
#define __UI_SD_MP3PLAYER_APP_H__

#include "ui.hpp"
#include "ui_navigation.hpp"
#include "io_wave.hpp"
#include "spectrum_color_lut.hpp"
#include "ui_receiver.hpp"
#include "replay_thread.hpp"


#include "baseband_api.hpp"
#include "replay_thread.hpp"
#include "ui_receiver.hpp"      // AudioVolumeField definition
#include "receiver_model.hpp"   // receiver_model declaration


using namespace ui;


namespace ui::external_app::mp3player_sd {

struct music_file_entry {
        std::string path{};
        // File type: 0 = WAV, 1 = MP3
        int type;
};


class mp3player_sd : public View {
   public:
    mp3player_sd(NavigationView& nav);
    ~mp3player_sd();

    void focus() override;

    std::string title() const override { return "SdMp3PlayerApp"; };

    // Information on the current MP3
        std::list<music_file_entry> entry_list{};
        // Points to the current path
        std::filesystem::path current_path{u"/mp3"};
        // Load the current
        void load_directory_contents();
        // Refresh the current menu view list
        void refresh_list();
        // Number of pages
        uint8_t nb_pages = 1;
        // Items shown per page
        static constexpr size_t items_per_page = 8;
        // Current page
        uint8_t pagination = 0;
        // Back button
        const std::string str_back{"<--"};
        // Forward button
        const std::string str_next{"-->"};
        // Select the current page
        const music_file_entry& get_selected_entry() const;
        
        void on_focus() override;
        void on_blur() override;
        void paint(Painter& painter) override; 
        // The callback must stay resident in memory
        std::function<void(KeyEvent)> on_select_entry{nullptr};

        // File transfer thread feeding the TX baseband
        std::unique_ptr<ReplayThread> replay_thread{};
        // Read buffer size
        const size_t read_size{2048};
        // Number of read buffers
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
        NavigationView& nav_;

        Labels labels{
        {
            {0, 0}, 
            "Path:", 
            Theme::getInstance()->fg_light->foreground}
        };
        // Show the current path
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

        // Show some parameters of the file being read (channel, Hz)
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

        // Show the file list; create more files to test paging
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

}  // namespace ui::external_app::new_external_app

#endif /*__UI_NEW_EXTERNAL_APP_H__*/
/*
 * Copyright (C) 2026
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

#ifndef __UI_LOW_BATTERY_H__
#define __UI_LOW_BATTERY_H__

#include "ui.hpp"
#include "ui_navigation.hpp"
#include "ui_widget.hpp"

namespace ui {

class LowBatteryView : public View {
   public:
    ~LowBatteryView();
    LowBatteryView(NavigationView& nav);
    LowBatteryView(const LowBatteryView&) = delete;
    LowBatteryView(LowBatteryView&&) = delete;
    LowBatteryView& operator=(const LowBatteryView&) = delete;
    LowBatteryView& operator=(LowBatteryView&&) = delete;
    void focus() override;
    std::string title() const override { return "low_battery"; };

   private:
    enum class PendingCommand : uint16_t {
        None = 0,
        DeepSleep = 0xa014,
        LOWPOWER = 0xa016,
        Wake = 0xa015,
    };

    static msg_t static_fn(void* arg);
    void on_timer();
    bool send_ppmod_command(PendingCommand command);
    void trigger_ppmod_command(PendingCommand command, const char* start_text, const char* ok_text, const char* fail_text);

    NavigationView& nav_;
    Thread* thread{nullptr};
    uint16_t timer_counter = 0;
    uint16_t retry_delay_ticks = 0;
    PendingCommand pending_command{PendingCommand::None};
    const char* pending_ok_text{nullptr};
    const char* pending_fail_text{nullptr};

    Labels labels{
        {{UI_POS_X_CENTER(14), 1 * 16}, "low battery control", Theme::getInstance()->fg_light->foreground},
    };

    Button button_enable_low_power{
        {UI_POS_X_CENTER(22), 4 * 16, UI_POS_WIDTH(22), UI_POS_HEIGHT(2)},
        "low power"};

    Button button_disable_low_power{
        {UI_POS_X_CENTER(22), 8 * 16, UI_POS_WIDTH(22), UI_POS_HEIGHT(2)},
        "disable low power"};

    Button button_deep_sleep{
        {UI_POS_X_CENTER(22), 12 * 16, UI_POS_WIDTH(22), UI_POS_HEIGHT(2)},
        "deep sleep"};
    
    Text text_status{
        {UI_POS_X_CENTER(22), 15 * 16, UI_POS_WIDTH(22), UI_POS_HEIGHT(1)},
        ""
    };
};

} /* namespace ui */

#endif /* __UI_LOW_BATTERY_H__ */

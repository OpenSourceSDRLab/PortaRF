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

#include "ui_low_battery.hpp"

#include "event_m0.hpp"
#include "i2cdevmanager.hpp"
#include "i2cdev_ppmod.hpp"

namespace ui {

LowBatteryView::~LowBatteryView() {
    if (thread) {
        chThdTerminate(thread);
        chThdWait(thread);
        thread = nullptr;
    }
}

LowBatteryView::LowBatteryView(NavigationView& nav)
    : nav_(nav) {
    add_children({
        &labels,
        &button_enable_low_power,
        &button_disable_low_power,
        &button_deep_sleep,
        &text_status});
    
    // 先只关闭wifi查看效果
    button_enable_low_power.on_select = [this](Button&) 
    {
        auto dev = static_cast<i2cdev::I2cDev_PPmod*>(
        i2cdev::I2CDevManager::get_dev_by_model(I2C_DEVMDL::I2CDECMDL_PPMOD));
        if (!dev) {
            text_status.set("No ppmod");
            return;
        }
        if (!dev->lockDevice()) {
            text_status.set("ppmod busy");
            return;
        }

        uint16_t cmd = static_cast<uint16_t>(PendingCommand::LOWPOWER);

        char rx[32]{};
        const bool ok = dev->i2c_read(
            reinterpret_cast<uint8_t*>(&cmd),
            sizeof(cmd),
            reinterpret_cast<uint8_t*>(rx),
            sizeof(rx) - 1
        );
        dev->unlockDevice();
        text_status.set(ok ? rx : "I2C timeout!");

        // trigger_ppmod_command(
        //     PendingCommand::LOWPOWER,
        //     "scan ppmod...",
        //     "start low power",
        //     "low power failed");
    };

    button_disable_low_power.on_select = [this](Button&) {

        auto dev = static_cast<i2cdev::I2cDev_PPmod*>(
        i2cdev::I2CDevManager::get_dev_by_model(I2C_DEVMDL::I2CDECMDL_PPMOD));
        if (!dev) {
            text_status.set("No ppmod");
            return;
        }
        if (!dev->lockDevice()) {
            text_status.set("ppmod busy");
            return;
        }

        uint16_t cmd = static_cast<uint16_t>(PendingCommand::Wake);

        char rx[32]{};
        const bool ok = dev->i2c_read(
            reinterpret_cast<uint8_t*>(&cmd),
            sizeof(cmd),
            reinterpret_cast<uint8_t*>(rx),
            sizeof(rx) - 1
        );
        dev->unlockDevice();
        text_status.set(ok ? rx : "I2C timeout!");

        // trigger_ppmod_command(
        //     PendingCommand::Wake,
        //     "scan ppmod...",
        //     "wake low power",
        //     "wake failed");
    };

    // 深度睡眠，唤醒需要搬动开关键 --- 官方低功耗
    button_deep_sleep.on_select = [this](Button&) {

        auto dev = static_cast<i2cdev::I2cDev_PPmod*>(
        i2cdev::I2CDevManager::get_dev_by_model(I2C_DEVMDL::I2CDECMDL_PPMOD));
        if (!dev) {
            text_status.set("No ppmod");
            return;
        }
        if (!dev->lockDevice()) {
            text_status.set("ppmod busy");
            return;
        }

        uint16_t cmd = static_cast<uint16_t>(PendingCommand::DeepSleep);

        char rx[32]{};
        const bool ok = dev->i2c_read(
            reinterpret_cast<uint8_t*>(&cmd),
            sizeof(cmd),
            reinterpret_cast<uint8_t*>(rx),
            sizeof(rx) - 1
        );
        dev->unlockDevice();
        text_status.set(ok ? rx : "I2C timeout!");

        //  trigger_ppmod_command(
        //     PendingCommand::DeepSleep,
        //     "scan ppmod...",
        //     "deep sleep",
        //     "deep sleep failed");

        


    };

    thread = chThdCreateFromHeap(NULL, 1024, NORMALPRIO + 10, LowBatteryView::static_fn, this);
}

void LowBatteryView::focus() {
    button_enable_low_power.focus();
}

bool LowBatteryView::send_ppmod_command(PendingCommand command) {
    auto dev = static_cast<i2cdev::I2cDev_PPmod*>(
        i2cdev::I2CDevManager::get_dev_by_model(I2C_DEVMDL::I2CDECMDL_PPMOD));
    if (!dev) {
        return false;
    }

    if (!dev->lockDevice()) {
        text_status.set("ppmod busy");
        return true;
    }

    uint16_t cmd = static_cast<uint16_t>(command);
    const bool ok = dev->i2c_write(nullptr, 0, reinterpret_cast<uint8_t*>(&cmd), sizeof(cmd));
    dev->unlockDevice();
    text_status.set(ok ? pending_ok_text : pending_fail_text);
    return true;
}

void LowBatteryView::trigger_ppmod_command(
    PendingCommand command,
    const char* start_text,
    const char* ok_text,
    const char* fail_text) {
    pending_command = command;
    pending_ok_text = ok_text;
    pending_fail_text = fail_text;

    if (send_ppmod_command(command)) {
        pending_command = PendingCommand::None;
        return;
    }

    i2cdev::I2CDevManager::manual_scan();
    text_status.set(start_text);
    retry_delay_ticks = 62;  // about 1 second, thread runs every 16ms
}

void LowBatteryView::on_timer() {
    if (retry_delay_ticks > 0) {
        --retry_delay_ticks;
        if (retry_delay_ticks == 0 && pending_command != PendingCommand::None) {
            if (!send_ppmod_command(pending_command)) {
                text_status.set("ppmod not found");
            }
            pending_command = PendingCommand::None;
        }
    }
}

msg_t LowBatteryView::static_fn(void* arg) {
    auto obj = static_cast<LowBatteryView*>(arg);
    while (!chThdShouldTerminate()) {
        chThdSleepMilliseconds(16);
        obj->on_timer();
    }
    return 0;
}

} /* namespace ui */

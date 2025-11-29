#include "ui_esp32update.hpp"
#include "portapack.hpp"
#include <cstring>
#include "i2cdevmanager.hpp"
#include "i2cdev_ppmod.hpp"


using namespace portapack;

namespace ui
{

    esp32Update::esp32Update(NavigationView &nav) // Application Main
    {
        // 添加
        add_children({// Add pointers for widgets
            &button_helloWorld,
            &button_get_all_dev,
            &text_module_name,
            &text_module_version,
            &text_module_API
        });

        button_helloWorld.on_select = [this](Button &)
        {
            // 取得当前PPMOD设备的信息
            get_ppmod_device_info();
        };

        button_get_all_dev.on_select = [this](Button &)
        {
            // 取得当前PPMOD设备的信息
            get_all_info();
        };       
    }
    
    void esp32Update::get_all_info()
    {
        i2cdev::I2CDevManager::manual_scan();
        char addr_list_val[128]="\0";
        auto addr_list = i2cdev::I2CDevManager::get_gev_list_by_addr();
    
        // 遍历每个设备
        for (size_t i = 0; i < addr_list.size(); ++i) {
            uint8_t addr = addr_list[i];
            
            // 通过地址获取设备对象
            auto dev = i2cdev::I2CDevManager::get_dev_by_addr(addr);
            
            if (dev) 
            {
                snprintf(&addr_list_val[strlen(addr_list_val)],128-strlen(addr_list_val),"%d:",addr);
                // // 获取设备模型
                I2C_DEVMDL model = dev->model;
                // // 获取其他信息
                uint8_t query_interval = dev->get_update_interval();

                // 根据设备类型进行不同的处理
                switch (model) 
                {
                    case I2C_DEVMDL::I2CDECMDL_PPMOD: {
                        snprintf(&addr_list_val[strlen(addr_list_val)],128-strlen(addr_list_val),"ppmod,");
                        // auto ppmod_dev = (i2cdev::I2cDev_PPmod*)dev;
                        // auto device_info = ppmod_dev->readDeviceInfo();
                        // if (device_info.has_value()) {
                        //     // 处理 ESP32 AI 开发板或其他 PPmod 设备
                        //     // device_info->module_name, module_version 等
                        // }
                        break;
                    }
                    case I2C_DEVMDL::I2CDEVMDL_MAX17055:
                    {
                        snprintf(&addr_list_val[strlen(addr_list_val)],128-strlen(addr_list_val),"battery,");
                        // 处理电池管理芯片
                        break;
                    }
                        
                    case I2C_DEVMDL::I2CDEVMDL_SHT3X:
                    {
                        snprintf(&addr_list_val[strlen(addr_list_val)],128-strlen(addr_list_val),"humiture,");
                        // 处理温湿度传感器
                        break;
                    }
                        
                    case I2C_DEVMDL::I2CDEVMDL_NOTSET:
                    {
                        snprintf(&addr_list_val[strlen(addr_list_val)],128-strlen(addr_list_val),"unknown,");
                        // 未识别的设备（有地址但没有驱动）
                        break;
                    }
                        
                    // ... 其他设备类型
                }
            } 
            else 
            {
                // 有地址但没有驱动程序的设备（可能是未支持的设备）
                // 地址: addr
                snprintf(&addr_list_val[strlen(addr_list_val)],128-strlen(addr_list_val),"nd:%d,",addr);
            }
        }

        // 
        text_module_name.set(addr_list_val);
    }


    void esp32Update::get_ppmod_device_info()
    {
        auto dev = (i2cdev::I2cDev_PPmod*)i2cdev::I2CDevManager::get_dev_by_model(I2C_DEVMDL::I2CDECMDL_PPMOD);
        if (!dev) {
            text_module_name.set("not connection");
            return;  // 设备未连接
        }

        auto device_info = dev->readDeviceInfo();
        if (device_info.has_value()) 
        {
            text_module_name.set(device_info->module_name);
            text_module_version.set(std::to_string(device_info->module_version));
            text_module_API.set(std::to_string(device_info->api_version));
        }
        else
        {
            text_module_name.set("no info");
        }
        uint64_t features_mask = dev->get_features_mask();
        
        // 这里是esp32 shell通信
        if (features_mask & (uint64_t)SupportedFeatures::FEAT_SHELL) 
        {
            uint16_t shell_bytes = dev->get_shell_buffer_bytes();
            if (shell_bytes > 0) {
                uint8_t buff[65];  // 64字节数据 + 1字节标志
                if (dev->get_shell_get_buffer_data(buff, 65))
                {
                    // buff[0] 的第1位是 has_more 标志，低7位是数据大小
                    bool has_more = buff[0] & 0x80;
                    size_t data_size = buff[0] & 0x7F;
                    // 实际数据在 buff[1] 到 buff[data_size]
                    // 这里可以解析 ESP32 AI 的自定义数据
                }
            }
        }
    }

    // Every time you get a DisplayFrameSync message this function will be ran
    void esp32Update::update()                   
    {
        // Message code
    }
}
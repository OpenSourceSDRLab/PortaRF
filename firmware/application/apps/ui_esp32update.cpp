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
        // Added
        add_children({// Add pointers for widgets
            &button_helloWorld,
            &button_get_all_dev,
            &text_module_name,
            &text_module_version,
            &text_module_API
        });

        button_helloWorld.on_select = [this](Button &)
        {
            // Get information on the current PPmod device
            get_ppmod_device_info();
        };

        button_get_all_dev.on_select = [this](Button &)
        {
            // Get information on the current PPmod device
            get_all_info();
        };       
    }
    
    void esp32Update::get_all_info()
    {
        i2cdev::I2CDevManager::manual_scan();
        char addr_list_val[128]="\0";
        auto addr_list = i2cdev::I2CDevManager::get_gev_list_by_addr();
    
        // Iterate over each device
        for (size_t i = 0; i < addr_list.size(); ++i) {
            uint8_t addr = addr_list[i];
            
            // Get the device object by address
            auto dev = i2cdev::I2CDevManager::get_dev_by_addr(addr);
            
            if (dev) 
            {
                snprintf(&addr_list_val[strlen(addr_list_val)],128-strlen(addr_list_val),"%d:",addr);
                // // Get the device model
                I2C_DEVMDL model = dev->model;
                // // Get other information
                uint8_t query_interval = dev->get_update_interval();

                // Handle each device type differently
                switch (model) 
                {
                    case I2C_DEVMDL::I2CDECMDL_PPMOD: {
                        snprintf(&addr_list_val[strlen(addr_list_val)],128-strlen(addr_list_val),"ppmod,");
                        // auto ppmod_dev = (i2cdev::I2cDev_PPmod*)dev;
                        // auto device_info = ppmod_dev->readDeviceInfo();
                        // if (device_info.has_value()) {
                        //     // Handle the ESP32 AI board or other PPmod devices
                        //     // device_info->module_name, module_version, etc.
                        // }
                        break;
                    }
                    case I2C_DEVMDL::I2CDEVMDL_MAX17055:
                    {
                        snprintf(&addr_list_val[strlen(addr_list_val)],128-strlen(addr_list_val),"battery,");
                        // Handle the battery management chip
                        break;
                    }
                        
                    case I2C_DEVMDL::I2CDEVMDL_SHT3X:
                    {
                        snprintf(&addr_list_val[strlen(addr_list_val)],128-strlen(addr_list_val),"humiture,");
                        // Handle the temperature/humidity sensor
                        break;
                    }
                        
                    case I2C_DEVMDL::I2CDEVMDL_NOTSET:
                    {
                        snprintf(&addr_list_val[strlen(addr_list_val)],128-strlen(addr_list_val),"unknown,");
                        // Unrecognised device (has an address but no driver)
                        break;
                    }
                        
                    // ... other device types
                }
            } 
            else 
            {
                // Device with an address but no driver (possibly unsupported)
                // Address: addr
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
            return;  // Device not connected
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
        
        // ESP32 shell communication
        if (features_mask & (uint64_t)SupportedFeatures::FEAT_SHELL) 
        {
            uint16_t shell_bytes = dev->get_shell_buffer_bytes();
            if (shell_bytes > 0) {
                uint8_t buff[65];  // 64 bytes of data + 1 flag byte
                if (dev->get_shell_get_buffer_data(buff, 65))
                {
                    // Bit 7 of buff[0] is the has_more flag, the low 7 bits are the data size
                    bool has_more = buff[0] & 0x80;
                    size_t data_size = buff[0] & 0x7F;
                    // The actual data is in buff[1] to buff[data_size]
                    // Custom data from the ESP32 AI board can be parsed here
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
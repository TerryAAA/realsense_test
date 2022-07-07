/**
 * @file test.cpp
 * chage_options
 *
 * @author 健宝
 */
#include <librealsense2/rs.hpp>
#include <librealsense2/hpp/rs_internal.hpp>
#include <iostream>


rs2::device get_a_realsense_device()
{
    // 实例化context
    rs2::context ctx;

    // context的query_devices方法获取设备列表，返回一个device_list对象
    rs2::device_list devices = ctx.query_devices();

    //实例化device
    rs2::device selected_device;
    if (devices.size() == 0)
    {
        std::cerr << "\033[31m" << "No device connected!!!!!" << std::endl;
        exit(0);
    }
    else
    {
        //打印设备数量
        std::cout <<"device num: "<<devices.size()<<std::endl;
        //将第一个设备传给device实例化得到的selected_device
        selected_device = devices[0];
    }
    //至此，selected_device就代表我们的当前设备，将它返回
    return selected_device;
}


void change_sensor_option(const rs2::sensor& sensor, rs2_option option_type, float requested_value)
{
    // 首先判断传感器是否支持这个选项设置
    if (!sensor.supports(option_type))
    {
        std::cout << "\033[33m" << "option is not supported by sensor " << "<"  << sensor.get_info(RS2_CAMERA_INFO_NAME) << ">" << "\033[37m"<< std::endl;
        return;
    }
    else
    {
        // 使用set_option函数给选项赋新值
        sensor.set_option(option_type, requested_value);
        std::cout << "<"  << sensor.get_info(RS2_CAMERA_INFO_NAME) << "> " << "change " << "<" << option_type  << ">" << " to " << ": " << requested_value << std::endl;
    }

}


int main()
{
    rs2::device selected_device;
    selected_device = get_a_realsense_device();

    std::vector<rs2::sensor> sensors = selected_device.query_sensors();

    int index = 0;
    
    for (rs2::sensor sensor : sensors){
        ++index;
        if (index == 1) {// Stereo Module
            change_sensor_option(sensor, RS2_OPTION_ENABLE_AUTO_EXPOSURE, 1);
            change_sensor_option(sensor, RS2_OPTION_AUTO_EXPOSURE_LIMIT,50000);
            change_sensor_option(sensor, RS2_OPTION_EMITTER_ENABLED, 1);
        }
        if (index == 2){// RGB Camera
            change_sensor_option(sensor, RS2_OPTION_EXPOSURE,80.f);
        }
        if (index == 3){// Motion Module
            change_sensor_option(sensor, RS2_OPTION_ENABLE_MOTION_CORRECTION,0);
        }
    }
}

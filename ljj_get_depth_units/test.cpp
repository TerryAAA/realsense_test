/**
 * @file test.cpp
 * get_depth_units
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
        std::cout << "\033[31m" << "No device connected!!!!!" << std::endl;
        exit(0);
    }
    else
    {
        //打印设备数量
        std::cout<<"device num: "<<devices.size()<<std::endl;
        //将第一个设备传给device实例化得到的selected_device
        selected_device = devices[0];
    }
    //至此，selected_device就代表我们的当前设备，将它返回
    return selected_device;
}



float get_depth_units(const rs2::sensor& sensor)
{
    if (rs2::depth_sensor dpt_sensor = sensor.as<rs2::depth_sensor>())
    {
        float scale = dpt_sensor.get_depth_scale();
        std::cout << "Scale factor for depth sensor is: " << scale << std::endl;
        return scale;
    }
    else
        std::cout << "\033[33m" << "get_depth_units faile: Given sensor is not a depth sensor" << "\033[37m" << std::endl;
}


int main()
{
    rs2::device selected_device;
    selected_device = get_a_realsense_device();

    std::vector<rs2::sensor> sensors = selected_device.query_sensors();

    int index = 0;
    int deep_scale = 0;

    for (rs2::sensor sensor : sensors){
        ++index;
        if (index == 1) {// Stereo Module
            deep_scale = get_depth_units(sensor);
        }
        if (index == 2){// RGB Camera

        }
        if (index == 3){// Motion Module

        }
        // get_sensor_option(sensor);
    }

    return 0;
}

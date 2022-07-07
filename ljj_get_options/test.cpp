/**
 * @file test.cpp
 * get_options
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



void  get_sensor_option(const rs2::sensor& sensor)
{
    std::cout << "Sensor name: " << "<"  << sensor.get_info(RS2_CAMERA_INFO_NAME) << ">" <<std::endl;
    std::cout << "This sensor supports the following options:\n" << std::endl;
    // 轮循所有参数
    for (int i = 0; i < static_cast<int>(RS2_OPTION_COUNT); i++)
    {
        // 隐式转换，将数字转为枚举类型
        rs2_option option_type = static_cast<rs2_option>(i);
        
        // 首先判断传感器是否支持这个选项设置
        if (sensor.supports(option_type))
        {
            // 获取并打印对当前选项的描述
            const char* description = sensor.get_option_description(option_type);

            // 获取并打印当前选项的值
            float current_value = sensor.get_option(option_type);

            // 获取选项的取值范围、默认值、取值步长
            rs2::option_range range = sensor.get_option_range(option_type);
            float default_value = range.def;
            float maximum_supported_value = range.max;
            float minimum_supported_value = range.min;
            float difference_to_next_value = range.step;

            // 打印
            std::cout << "    " << i << ": "  << "<" << option_type  << ">" << std::endl;
            std::cout << "        Description   : " << description << std::endl;
            std::cout << "        Min Value     : " << minimum_supported_value << std::endl;
            std::cout << "        Max Value     : " << maximum_supported_value << std::endl;
            std::cout << "        Step          : " << difference_to_next_value << std::endl;
            std::cout << "        Default Value : " << default_value << std::endl;
            std::cout << "        Current Value : " << current_value << std::endl;
            std::cout << std::endl;
        }
        else
        {
            //std::cout << "        is not supported by this sensor" << std::endl;
        }
    }
    std::cout << std::endl;
    std::cout << std::endl;
}


int main()
{
    rs2::device selected_device;
    selected_device = get_a_realsense_device();
    
    std::vector<rs2::sensor> sensors = selected_device.query_sensors();
    
    for (rs2::sensor sensor : sensors){
        get_sensor_option(sensor);
    }
    return 0;
}

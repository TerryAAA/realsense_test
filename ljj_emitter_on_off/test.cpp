/**
 * @file test.cpp
 * get_camera_imu_stream
 *
 * @author 健宝
 */
#include <librealsense2/rs.hpp>
#include <librealsense2/hpp/rs_internal.hpp>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;

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
    Mat depth_image, color_depth_image, irL_image, irL_image_E, color_depth_image_filtered;

    double frames_t_last = -1;

    rs2::device selected_device;
    selected_device = get_a_realsense_device();

    // std::vector<rs2::sensor> sensors = selected_device.query_sensors();
    std::vector<rs2::sensor> sensors = selected_device.query_sensors();

    int index = 0;
    for (rs2::sensor sensor : sensors){
        ++index;
        if(index == 1){
            if (sensor.supports(RS2_OPTION_LASER_POWER))
            {
                // auto range = sensor.get_option_range(RS2_OPTION_LASER_POWER);
                // depth_sensor.set_option(RS2_OPTION_LASER_POWER, range.max); // Set max power  , max value is 360.0f
                sensor.set_option(RS2_OPTION_LASER_POWER, 50.0f); // 这个附近的功率值在realsense viewer中查看，深度图中的噪声最少
            }
            // change_sensor_option(sensor, RS2_OPTION_AUTO_EXPOSURE_LIMIT,200); //好像目前SDK存在bug，https://github.com/IntelRealSense/realsense-ros/issues/2673
            // std::cout << "自动曝光 参数为[" << sensor.get_option(RS2_OPTION_EXPOSURE) << "]" << std::endl;
            change_sensor_option(sensor, RS2_OPTION_ENABLE_AUTO_EXPOSURE, 1);  
            change_sensor_option(sensor, RS2_OPTION_EMITTER_ENABLED, 1);
            change_sensor_option(sensor, RS2_OPTION_EMITTER_ON_OFF, 1);
        }
    }

   // 首先实例化pipeline和config
    rs2::pipeline pipe;
    rs2::config cfg;
    

    cfg.enable_stream(RS2_STREAM_INFRARED, 1, 640, 480, RS2_FORMAT_Y8, 60);
    // cfg.enable_stream(RS2_STREAM_DEPTH, 640, 480, RS2_FORMAT_Z16, 90);

    pipe.start(cfg);
    // pipe.start(cfg, stream_callback);
    int mode = 1;
    while (1)
    {
        // 等待一帧数据流
        rs2::frameset frames = pipe.wait_for_frames();

        //Emitter mode: 0 - all emitters disabled. 1 - laser enabled. 2 - auto laser enabled (opt). 3 - LED enabled (opt).
        auto mode = frames.get_frame_metadata(RS2_FRAME_METADATA_FRAME_EMITTER_MODE);
        
        if(mode == 1)//laser enabled.
        {
            rs2::frame irL_frame = frames.get_infrared_frame(1); // 左红外相机图像
            irL_image_E  =   Mat(Size(640, 480), CV_8UC1, (void*)irL_frame.get_data()  ,Mat::AUTO_STEP); // 左红外相机图像
        }
        else if(mode == 0)//all emitters disabled.
        {
            rs2::frame irL_frame = frames.get_infrared_frame(1); // 左红外相机图像
            irL_image  =   Mat(Size(640, 480), CV_8UC1, (void*)irL_frame.get_data()  ,Mat::AUTO_STEP); // 左红外相机图像
        } 
        

        if(!irL_image.empty() && !irL_image_E.empty()){
            imshow("irL", irL_image);
            imshow("irL_E", irL_image_E);
            waitKey(1);
        }

        // 计算、打印帧率
        double frames_fs = 0;
        static double frames_t_last = 0;
        // Get the timestamp of the current frame
        double frames_t_now = frames.get_timestamp();
        if(frames_t_last > 0){
            frames_fs = 1000/(frames_t_now - frames_t_last);
        }
        std::cout <<  "frames_fs: " <<  frames_fs << std::endl;

        frames_t_last = frames_t_now;

    }

    return 0;
}

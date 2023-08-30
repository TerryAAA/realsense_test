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



// post-processing 是运行在本地端的
// 可以在realsenseviewer里打开选项观察CPU占用情况
// 挺耗计算资源，跑不到90帧
int main()
{
    Mat depth_image, color_depth_image, irL_image, color_depth_image_filtered;

    double frames_t_last = -1;

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

    // std::vector<rs2::sensor> sensors = selected_device.query_sensors();
    std::vector<rs2::sensor> sensors = selected_device.query_sensors();


   // 首先实例化pipeline和config
    rs2::pipeline pipe;
    rs2::config cfg;
    rs2::colorizer color_map; //将深度图转换为彩色深度图以便于查看
    rs2::temporal_filter temp_filter;   // Temporal   - reduces temporal noise
    rs2::hole_filling_filter hole_filling_filter;


    temp_filter.set_option(rs2_option::RS2_OPTION_HOLES_FILL, 6);
    temp_filter.set_option(rs2_option::RS2_OPTION_FILTER_SMOOTH_ALPHA, 0.4f);
    temp_filter.set_option(rs2_option::RS2_OPTION_FILTER_SMOOTH_DELTA, 20);


    // cfg.enable_stream(RS2_STREAM_INFRARED, 1, 640, 480, RS2_FORMAT_Y8, 90);
    cfg.enable_stream(RS2_STREAM_DEPTH, 640, 480, RS2_FORMAT_Z16, 90);

    pipe.start(cfg);
    // pipe.start(cfg, stream_callback);
    while (1)
    {
        // 等待一帧数据流
        rs2::frameset frames = pipe.wait_for_frames();

        rs2::depth_frame depth_frame = frames.get_depth_frame();           // 获取深度图像数据
        rs2::frame color_depth = color_map.process(depth_frame);
        depth_image = Mat(Size(640, 480), CV_16U, (void*)depth_frame.get_data(), Mat::AUTO_STEP); // 深度图像   
        color_depth_image = Mat(Size(640, 480), CV_8UC3, (void*)color_depth.get_data(), Mat::AUTO_STEP); // 深度彩色图像


        rs2::frame depth_frame_filtered = temp_filter.process(depth_frame);
        depth_frame_filtered = hole_filling_filter.process(depth_frame_filtered);
        rs2::frame color_depth_filtered = color_map.process(depth_frame_filtered);
        color_depth_image_filtered = Mat(Size(640, 480), CV_8UC3, (void*)color_depth_filtered.get_data(), Mat::AUTO_STEP); // 深度彩色图像（滤波）


        if(!depth_image.empty()){
            imshow("depth", depth_image);
            imshow("color depth",color_depth_image);
            imshow("color depth filtered",color_depth_image_filtered);
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



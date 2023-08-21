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

int main()
{
    // 定义相机0的回调
    cv::Mat color_image, depth_image, irL_image, irR_image;
    bool fs_image_ready = false;
    auto imu_callback = [&](const rs2::frame& frame)
    {
        if(rs2::frameset frames = frame.as<rs2::frameset>())
        {
            // 取得每一张图像
            //rs2::video_frame color_frame = frames.get_color_frame(); // 获取彩色图像数据
            rs2::depth_frame depth_frame = frames.get_depth_frame(); // 获取深度图像数据
            rs2::frame irL_frame = frames.get_infrared_frame(1); // 左红外相机图像
            rs2::frame irR_frame = frames.get_infrared_frame(2); // 右红外相机图像

            //color_image = cv::Mat(Size(640, 480), CV_8UC3, (void*)color_frame.get_data(),Mat::AUTO_STEP); // 彩色图像
            depth_image = cv::Mat(Size(640, 480), CV_16U , (void*)depth_frame.get_data(),Mat::AUTO_STEP); // 深度图像
            irL_image   = cv::Mat(Size(640, 480), CV_8UC1, (void*)irL_frame.get_data()  ,Mat::AUTO_STEP); // 左红外相机图像
            irR_image   = cv::Mat(Size(640, 480), CV_8UC1, (void*)irR_frame.get_data()  ,Mat::AUTO_STEP); // 右红外相机图像
            fs_image_ready = true;
        }
    };

    // 定义相机1的回调
    cv::Mat color_image_1, depth_image_1, irL_image_1, irR_image_1;
    bool fs_image_ready_1 = false;
    auto imu_callback_1 = [&](const rs2::frame& frame)
    {
        if(rs2::frameset frames = frame.as<rs2::frameset>())
        {
            // 取得每一张图像
            //rs2::video_frame color_frame_1 = frames.get_color_frame(); // 获取彩色图像数据
            rs2::depth_frame depth_frame_1 = frames.get_depth_frame(); // 获取深度图像数据
            rs2::frame irL_frame_1 = frames.get_infrared_frame(1); // 左红外相机图像
            rs2::frame irR_frame_1 = frames.get_infrared_frame(2); // 右红外相机图像

            // 图像转Opencv格式
            //color_image_1 = cv::Mat(Size(640, 480), CV_8UC3, (void*)color_frame_1.get_data(),Mat::AUTO_STEP); // 彩色图像
            depth_image_1 = cv::Mat(Size(640, 480), CV_16U , (void*)depth_frame_1.get_data(),Mat::AUTO_STEP); // 深度图像
            irL_image_1   = cv::Mat(Size(640, 480), CV_8UC1, (void*)irL_frame_1.get_data()  ,Mat::AUTO_STEP); // 左红外相机图像
            irR_image_1   = cv::Mat(Size(640, 480), CV_8UC1, (void*)irR_frame_1.get_data()  ,Mat::AUTO_STEP); // 右红外相机图像

            fs_image_ready_1 = true;
        }
    };
    

    bool serial_flag = false;
    rs2::pipeline pipe;
    rs2::config cfg;
    bool serial_flag_1 = false;
    rs2::pipeline pipe_1;
    rs2::config cfg_1;
    {
        std::vector<std::string> serials;
        std::string serial;
        rs2::context ctx;
        rs2::device_list devices = ctx.query_devices();
        if (devices.size() == 0){
            exit(0);
        }

        for (int i = 0; i < devices.size(); i++){
            std::vector<rs2::sensor> sensors = devices[i].query_sensors();
            serial = devices[i].get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
            std::cout << "get serial " << i << " : " << serial << std::endl;

            if(serial == "140122078028"){
                int index = 0;
                // We can now iterate the sensors and print their names
                for (rs2::sensor sensor : sensors){
                    if (sensor.supports(RS2_CAMERA_INFO_NAME)) {
                        ++index;
                        if (index == 1) {
                            sensor.set_option(RS2_OPTION_ENABLE_AUTO_EXPOSURE, 1);
                            // sensor.set_option(RS2_OPTION_AUTO_EXPOSURE_LIMIT,50000);
                            sensor.set_option(RS2_OPTION_EMITTER_ENABLED, 0); // emitter on for depth information
                        }
                    }
                }
                cfg.enable_device(serial);
                // enable stream
                //cfg.enable_stream(RS2_STREAM_COLOR, 640, 480, RS2_FORMAT_BGR8, 60);
                cfg.enable_stream(RS2_STREAM_INFRARED, 1, 640, 480, RS2_FORMAT_Y8, 60);
                cfg.enable_stream(RS2_STREAM_INFRARED, 2, 640, 480, RS2_FORMAT_Y8, 60);
                cfg.enable_stream(RS2_STREAM_DEPTH,640, 480, RS2_FORMAT_Z16, 60);
                pipe.start(cfg, imu_callback);

                serial_flag = true;
                std::cout << "---------- Init 0 camera pipeline succeed ----------" << std::endl;
            }
            if(serial == "152222070849"){
                int index = 0;
                // We can now iterate the sensors and print their names
                for (rs2::sensor sensor : sensors){
                    if (sensor.supports(RS2_CAMERA_INFO_NAME)) {
                        ++index;
                        if (index == 1) {
                            sensor.set_option(RS2_OPTION_ENABLE_AUTO_EXPOSURE, 1);
                            // sensor.set_option(RS2_OPTION_AUTO_EXPOSURE_LIMIT,50000);
                            sensor.set_option(RS2_OPTION_EMITTER_ENABLED, 0); // emitter on for depth information
                        }
                    }
                }
                cfg_1.enable_device(serial);
                // enable stream
                //cfg_1.enable_stream(RS2_STREAM_COLOR, 640, 480, RS2_FORMAT_BGR8, 60);
                cfg_1.enable_stream(RS2_STREAM_INFRARED, 1, 640, 480, RS2_FORMAT_Y8, 60);
                cfg_1.enable_stream(RS2_STREAM_INFRARED, 2, 640, 480, RS2_FORMAT_Y8, 60);
                cfg_1.enable_stream(RS2_STREAM_DEPTH,640, 480, RS2_FORMAT_Z16, 60);
                pipe_1.start(cfg_1, imu_callback_1);

                serial_flag_1 = true;
                std::cout << "---------- Init 1 camera pipeline succeed ----------" << std::endl;
            }
        }
    }



    while (1)
    {
        if(fs_image_ready){
            fs_image_ready = false;
            //imshow("color", color_image);
            imshow("depth", depth_image);
            imshow("irL", irL_image);
            imshow("irR", irR_image);
            waitKey(1);
        }

        if(fs_image_ready_1){
            //imshow("color_1", color_image_1);
            imshow("depth_1", depth_image_1);
            imshow("irL_1", irL_image_1);
            imshow("irR_1", irR_image_1);
            waitKey(1);
        }
    }

    return 0;
}
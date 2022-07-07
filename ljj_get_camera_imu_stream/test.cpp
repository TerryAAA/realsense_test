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
   // 首先实例化pipeline和config
    rs2::pipeline pipe;
    rs2::config cfg;

    // RGB stream
    cfg.enable_stream(RS2_STREAM_COLOR, 640, 480, RS2_FORMAT_BGR8, 30);
    // Depth stream
    cfg.enable_stream(RS2_STREAM_DEPTH, 640, 480, RS2_FORMAT_Z16, 30);
    // IMU stream
    cfg.enable_stream(RS2_STREAM_ACCEL, RS2_FORMAT_MOTION_XYZ32F);
    cfg.enable_stream(RS2_STREAM_GYRO , RS2_FORMAT_MOTION_XYZ32F);
    // IR stream
    cfg.enable_stream(RS2_STREAM_INFRARED, 1, 640, 480, RS2_FORMAT_Y8, 30);
    cfg.enable_stream(RS2_STREAM_INFRARED, 2, 640, 480, RS2_FORMAT_Y8, 30);

    pipe.start(cfg);

    while (1)
    {
        // 等待一帧数据流
        rs2::frameset frames = pipe.wait_for_frames();

        // Get the timestamp of the current frame
        double ts = frames.get_timestamp();

        // 取得每一张图像
        rs2::depth_frame depth_frame = frames.get_depth_frame(); // 获取深度图像数据
        rs2::video_frame color_frame = frames.get_color_frame(); // 获取彩色图像数据
        rs2::frame irL_frame = frames.get_infrared_frame(1); // 左红外相机图像
        rs2::frame irR_frame = frames.get_infrared_frame(2); // 右红外相机图像

        // 图像转Opencv格式
        Mat color_image(Size(640, 480), CV_8UC3, (void*)color_frame.get_data(),Mat::AUTO_STEP); // 彩色图像
        Mat depth_image(Size(640, 480), CV_16U , (void*)depth_frame.get_data(),Mat::AUTO_STEP); // 深度图像
        Mat irL_image  (Size(640, 480), CV_8UC1, (void*)irL_frame.get_data()  ,Mat::AUTO_STEP); // 左红外相机图像
        Mat irR_image  (Size(640, 480), CV_8UC1, (void*)irR_frame.get_data()  ,Mat::AUTO_STEP); // 右红外相机图像
        imshow("color", color_image);
        imshow("depth", depth_image);
        imshow("irL", irL_image);
        imshow("irR", irR_image);
        waitKey(1);

        // Get gyro measures
        rs2::frame f_gyro = frames.first_or_default(RS2_STREAM_GYRO);
        rs2_vector gyro_data = f_gyro.as<rs2::motion_frame>().get_motion_data();
        std::cout << gyro_data << std::endl;

        // Get accelerometer measures
        rs2::frame f_accel = frames.first_or_default(RS2_STREAM_ACCEL);
        rs2_vector accel_data = f_accel.as<rs2::motion_frame>().get_motion_data();
        std::cout << accel_data << std::endl;
    }

    return 0;
}

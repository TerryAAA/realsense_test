/**
 * @file test.cpp
 * align_streams
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


double frames_fs = 0;
rs2::frameset fs_to_align;
int frames_point = 0;
void stream_callback(const rs2::frame& frame){

    static double gyro_fs = 0;
    static double accel_fs = 0;
    std::cout << std::endl;
    std::cout << "frames_fs: " << frames_fs << std::endl;
    std::cout << "gyro_fs: " << gyro_fs << std::endl;
    std::cout << "accel_fs: " << accel_fs << std::endl;
    std::cout << std::endl;

    if(rs2::frameset frames = frame.as<rs2::frameset>()){

        fs_to_align = frames;
        frames_point = 1;

    }else if(rs2::motion_frame m_frame = frame.as<rs2::motion_frame>()){
        if (m_frame.get_profile().stream_name() == "Gyro")
        {
            // Get gyro measures
            rs2_vector gyro_data = m_frame.get_motion_data();
            // std::cout << "gyro_data" << gyro_data << std::endl;

            // 计算、打印帧率
            static double gyro_t_last = 0;
            // Get the timestamp of the current frame
            double gyro_t_now = m_frame.get_timestamp();
            if(gyro_t_last > 0){
                gyro_fs = 1000/(gyro_t_now - gyro_t_last);
            }
            gyro_t_last = gyro_t_now;
        }
        else if (m_frame.get_profile().stream_name() == "Accel")
        {
            // Get accelerometer measures
            rs2_vector accel_data = m_frame.as<rs2::motion_frame>().get_motion_data();
            // std::cout << "accel_data" << accel_data << std::endl;

            // 计算、打印帧率
            static double accel_t_last = 0;
            // Get the timestamp of the current frame
            double accel_t_now = m_frame.get_timestamp();
            if(accel_t_last > 0){
                accel_fs = 1000/(accel_t_now - accel_t_last);
            }
            accel_t_last = accel_t_now;
        }
    }
}


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

    // Create a rs2::align object.
    // rs2::align allows us to perform alignment of depth frames to others frames
    //The "align_to" is the stream type to which we plan to align depth frames.
    rs2::align align(RS2_STREAM_COLOR);
    
    pipe.start(cfg, stream_callback);

    while (1)
    {
        if(frames_point == 1){

            // 计算、打印帧率
            static double frames_t_last = 0;
            // Get the timestamp of the current frame
            double frames_t_now = fs_to_align.get_timestamp();
            if(frames_t_last > 0){
                frames_fs = 1000/(frames_t_now - frames_t_last);
            }
            frames_t_last = frames_t_now;

            // Perform alignment here
            rs2::frameset aligned_frameset = align.process(fs_to_align);

            // 获取对齐后的RGB图像与深度图像
            rs2::video_frame aligned_color_frame = aligned_frameset.get_color_frame();
            rs2::depth_frame aligned_depth_frame = aligned_frameset.get_depth_frame();

            // 取得每一张图像
            rs2::depth_frame depth_frame = fs_to_align.get_depth_frame(); // 获取深度图像数据
            rs2::video_frame color_frame = fs_to_align.get_color_frame(); // 获取彩色图像数据
            rs2::frame irL_frame = fs_to_align.get_infrared_frame(1); // 左红外相机图像
            rs2::frame irR_frame = fs_to_align.get_infrared_frame(2); // 右红外相机图像


            // 图像转Opencv格式
            Mat aligned_color_image(Size(640, 480), CV_8UC3, (void*)aligned_color_frame.get_data(),Mat::AUTO_STEP); // 彩色图像
            Mat aligned_depth_image(Size(640, 480), CV_16U , (void*)aligned_depth_frame.get_data(),Mat::AUTO_STEP); // 深度图像        
            Mat color_image(Size(640, 480), CV_8UC3, (void*)color_frame.get_data(),Mat::AUTO_STEP); // 彩色图像
            Mat depth_image(Size(640, 480), CV_16U , (void*)depth_frame.get_data(),Mat::AUTO_STEP); // 深度图像
            Mat irL_image  (Size(640, 480), CV_8UC1, (void*)irL_frame.get_data()  ,Mat::AUTO_STEP); // 左红外相机图像
            Mat irR_image  (Size(640, 480), CV_8UC1, (void*)irR_frame.get_data()  ,Mat::AUTO_STEP); // 右红外相机图像
            imshow("aligned_color_image", aligned_color_image);
            imshow("aligned_depth_image", aligned_depth_image);
            imshow("color", color_image);
            imshow("depth", depth_image);
            imshow("irL", irL_image);
            imshow("irR", irR_image);
            waitKey(1);
            frames_point = 0;
        }
    }

    return 0;
}

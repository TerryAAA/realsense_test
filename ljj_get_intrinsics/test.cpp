/**
 * @file test.cpp
 * get_intrinsics
 *
 * @author 健宝
 */
#include <librealsense2/rs.hpp>
#include <librealsense2/hpp/rs_internal.hpp>
#include <iostream>



void l_get_intrinsics(const rs2::stream_profile& stream){

    // A sensor's stream (rs2::stream_profile) is in general a stream of data with no specific type.
    // For video streams (streams of images), the sensor that produces the data has a lens and thus has properties such
    //  as a focal point, distortion, and principal point.
    // To get these intrinsics parameters, we need to take a stream and first check if it is a video stream
    if (rs2::video_stream_profile video_stream = stream.as<rs2::video_stream_profile>())
    {
        try
        {
            // 使用get_intrinsics方法获取相机内参
            rs2_intrinsics intrinsics = video_stream.get_intrinsics();
            // 处理一些结果
            auto principal_point = std::make_pair(intrinsics.ppx, intrinsics.ppy);
            auto focal_length = std::make_pair(intrinsics.fx, intrinsics.fy);
            rs2_distortion model = intrinsics.model;

            // 打印内参
            std::cout << "width*height         : " << intrinsics.width << " * " << intrinsics.height << std::endl;
            std::cout << "Principal Point         : " << principal_point.first << ", " << principal_point.second << std::endl;
            std::cout << "Focal Length            : " << focal_length.first << ", " << focal_length.second << std::endl;
            std::cout << "Distortion Model        : " << model << std::endl;
            std::cout << "Distortion Coefficients : [" << intrinsics.coeffs[0] << "," << intrinsics.coeffs[1] << "," <<
            intrinsics.coeffs[2] << "," << intrinsics.coeffs[3] << "," << intrinsics.coeffs[4] << "]" << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cout  << "\033[33m" << "Failed to get intrinsics for the given stream. " << e.what()  << "\033[37m"<< std::endl;
        }
    }
    else if (rs2::motion_stream_profile motion_stream = stream.as<rs2::motion_stream_profile>())
    {
        try
        {
            // 使用get_motion_intrinsics方法获取IMU内参
            rs2_motion_device_intrinsic intrinsics = motion_stream.get_motion_intrinsics();

            // 打印内参
            std::cout << " Scale X      cross axis      cross axis  Bias X \n";
            std::cout << " cross axis    Scale Y        cross axis  Bias Y  \n";
            std::cout << " cross axis    cross axis     Scale Z     Bias Z  \n";
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    std::cout << intrinsics.data[i][j] << "    ";
                }
                std::cout << "\n";
            }

            std::cout << "Variance of noise for X, Y, Z axis \n";
            for (int i = 0; i < 3; i++)
                std::cout << intrinsics.noise_variances[i] << " ";
            std::cout << "\n";

            std::cout << "Variance of bias for X, Y, Z axis \n";
            for (int i = 0; i < 3; i++)
                std::cout << intrinsics.bias_variances[i] << " ";
            std::cout << "\n";
        }
        catch (const std::exception& e)
        {
            std::cout  << "\033[33m" << "Failed to get intrinsics for the given stream. " << e.what()  << "\033[37m" << std::endl;
        }
    }
    else
    {
        std::cerr  << "\033[33m"  << "Given stream profile has no intrinsics data" << "\033[37m" << std::endl;
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


    rs2::pipeline_profile pipe_profile = pipe.start(cfg);

    std::cout << std::endl;
    std::cout << "Get RGB intrinsics :" << std::endl;
    l_get_intrinsics(pipe_profile.get_stream(RS2_STREAM_COLOR));

    std::cout << std::endl;
    std::cout << "Get ACCEL intrinsics :" << std::endl;
    l_get_intrinsics(pipe_profile.get_stream(RS2_STREAM_ACCEL));

    std::cout << std::endl;
    std::cout << "Get GYRO intrinsics :" << std::endl;
    l_get_intrinsics(pipe_profile.get_stream(RS2_STREAM_GYRO));

    std::cout << std::endl;
    std::cout << "Get IR intrinsics :" << std::endl;
    l_get_intrinsics(pipe_profile.get_stream(RS2_STREAM_INFRARED));

    std::cout << std::endl;
    std::cout << "Get DEPTH intrinsics :" << std::endl;
    l_get_intrinsics(pipe_profile.get_stream(RS2_STREAM_DEPTH));
}

#include <iostream>
#include <librealsense2/rs.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
int main(int argc, char * argv[])
{
    // 创建 RealSense 上下文 (context) 对象
    rs2::context ctx;

    // 检测连接的 RealSense 设备
    auto devices = ctx.query_devices();
    if (devices.size() == 0)
    {
        std::cout << "No RealSense device detected." << std::endl;
        return 1;
    }

    // 选择第一个检测到的设备
    rs2::device dev = devices[0];

        dev.hardware_reset();

    // 创建管道 (pipeline) 对象
    rs2::pipeline pipe(ctx);

    // 创建并配置管道配置 (pipeline config) 对象
    rs2::config cfg;

    // 设置管道配置所需的流类型和分辨率
    // 这里配置深度流和彩色流，你可以根据需要进行修改
    cfg.enable_stream(RS2_STREAM_DEPTH, 640, 480, RS2_FORMAT_Z16, 30);
    cfg.enable_stream(RS2_STREAM_COLOR, 640, 480, RS2_FORMAT_BGR8, 30);

    // 启动管道
    try
    {
        // 启动管道
        pipe.start();
    }
    catch (const rs2::error &e)
    {
        // 检查错误消息是否包含 "Device or resource busy"
        std::string error_msg = e.what();
        if (error_msg.find("Device or resource busy") != std::string::npos)
        {
            std::cout << "Error: Device or resource busy" << std::endl;
            // 在此处添加处理代码
        }
        else
        {
            // 其他错误
            std::cout << "Error: " << error_msg << std::endl;
        }
        return 1;
    }

    // 设置 timeout 时间（毫秒）
    unsigned int timeout_ms = 500;

    while (true)
    {
        // 等待管道中的新帧
        rs2::frameset frames;
        try
        {
            frames = pipe.wait_for_frames(timeout_ms);
        }
        catch (const rs2::error & e)
        {
            if (e.get_failed_function() == "rs2_pipeline_wait_for_frames")
            {
                std::cout << "Timeout: No frames received within " << timeout_ms << " ms." << std::endl;
                continue;
            }
            else
            {
                std::cout << "111111111111 " << std::endl;
                throw e;
            }
        }

        // 处理帧数据，例如获取深度和彩色图像
        rs2::frame depth_frame = frames.get_depth_frame();
        // rs2::frame color_frame = frames.get_color_frame();

        // 在此处添加对帧数据的处理代码
        Mat depth_image(Size(640, 480), CV_16U , (void*)depth_frame.get_data(),Mat::AUTO_STEP); // 深度图像
        // Mat irL_image  (Size(640, 480), CV_8UC1, (void*)irL_frame.get_data()  ,Mat::AUTO_STEP); // 左红外相机图像
        // Mat irR_image  (Size(640, 480), CV_8UC1, (void*)irR_frame.get_data()  ,Mat::AUTO_STEP); // 右红外相机图像
        // imshow("color", color_image);
        imshow("depth", depth_image);
        // imshow("irL", irL_image);
        // imshow("irR", irR_image);
        waitKey(1);
    }

    return 0;
}

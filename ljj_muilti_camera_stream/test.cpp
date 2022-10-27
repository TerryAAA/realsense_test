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


int main(int argc, char** argv)
{
    rs2::context ctx;        // Create librealsense context for managing devices

    std::vector<rs2::pipeline> pipelines;

    std::vector<std::string> serials;// Capture serial numbers before opening streaming

    std::cout << "realsense device num : " << ctx.query_devices().size() << std::endl;

    for (auto&& dev : ctx.query_devices())
        serials.push_back(dev.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER));

    // Start a streaming pipe per each connected device
    for (auto&& serial : serials)
    {
        std::cout << "get serial : " << serial << std::endl;
        rs2::pipeline pipe(ctx);

        rs2::config cfg;
        cfg.enable_device(serial);
        cfg.enable_stream(RS2_STREAM_DEPTH, 640, 480, RS2_FORMAT_Z16, 90);
        pipe.start(cfg);
        pipelines.emplace_back(pipe);
    }

    std::cout << "pipelines size " << pipelines.size() << std::endl;


    while(1){
        // // Collect the new frames from all the connected devices
        std::vector<rs2::frameset> new_frames;
        std::vector<double> ts;
        ts.clear();
        std::vector<int64> t1s;
        for (int i = 0; i<pipelines.size(); i++)
        {
            
            // https://www.likecs.com/show-204910874.html?sc=720
            // https://github.com/IntelRealSense/librealsense/issues/2422
            // wait_for_frame / s自然可以与单个管道/ frame_queue对象一起使用。 
            // 如果有多个队列，***使用poll_for_frames确保在等待另一个队列时不会丢失一个队列中的帧。
            


            // rs2::frameset fs;
            // while(!pipelines[i].poll_for_frames(&fs)){}
            // new_frames.push_back(fs);
            // // auto serial = rs2::sensor_from_frame(fs)->get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
            // // std::cout << "serial " << serial << std::endl;
            // int64 t1 = cv::getTickCount();
            // t1s.push_back(t1);
            

            rs2::frameset frameset = pipelines[i].wait_for_frames();
            int64 t1 = cv::getTickCount();
            t1s.push_back(t1);
            new_frames.push_back(frameset);
        }
        std::cout << t1s.size() << "  ts_gap:  " << 1000 * (t1s[1] - t1s[0]) / cv::getTickFrequency() << std::endl;
        // std::cout << "new_frames size " << new_frames.size() << std::endl;



        // Convert the newly-arrived frames to render-friendly format
        std::vector<Mat> depth_images;
        for (const auto& frame : new_frames)
        {
            // Get the serial number of the current frame's device
            auto serial = rs2::sensor_from_frame(frame)->get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
            rs2::depth_frame depth_frame = frame.get_depth_frame(); // 获取深度图像数据
            Mat depth_image(Size(640, 480), CV_16U , (void*)depth_frame.get_data(),Mat::AUTO_STEP); // 深度图像
            
            depth_images.push_back(depth_image);
        }



        for (int i = 0; i<depth_images.size(); i++)
        {
            imshow(serials[i], depth_images[i]);
            waitKey(1);
        }
    }


    return 0;
}
















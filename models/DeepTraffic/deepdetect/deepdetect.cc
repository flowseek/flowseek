#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <tensorflow/lite/interpreter.h>
#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/model.h>
#include <tensorflow/lite/c/common.h>
#include <opencv2/opencv.hpp>
#include <map>
#include <filesystem>

#include "sqlite3.h"

namespace fs = std::filesystem;

// Helper function to read the model file
std::vector<char> ReadFile(const std::string& file_path) {
    std::ifstream file(file_path, std::ios::binary | std::ios::ate);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + file_path);
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        throw std::runtime_error("Failed to read file: " + file_path);
    }
    return buffer;
}

// 定义 12 个类别的名称映射
std::map<int, std::string> dict_12class = {
    {0, "Chat"}, {1, "Email"}, {2, "File"}, {3, "P2p"}, {4, "Streaming"}, {5, "Voip"},
    {6, "Vpn_Chat"}, {7, "Vpn_Email"}, {8, "Vpn_File"}, {9, "Vpn_P2p"}, {10, "Vpn_Streaming"}, {11, "Vpn_Voip"}
};

// 处理单个PNG文件
int ProcessSingleImage(const std::string& image_path, tflite::Interpreter* interpreter) {
    // 检查图像文件是否存在
    if (!fs::exists(image_path)) {
        std::cerr << "Error: Image file not found: " << image_path << std::endl;
        return -1;
    }

    // 从文件名中提取flow_id
    std::string filename = fs::path(image_path).filename().string();
    int flow_id = -1;
    if (filename.rfind("flow_", 0) == 0) {  // 检查是否以flow_开头
        size_t underscore_pos = filename.find('_', 5);  // 查找第二个下划线
        if (underscore_pos != std::string::npos) {
            std::string id_str = filename.substr(5, underscore_pos - 5);
            flow_id = std::stoi(id_str);
        }
    }

    if(flow_id == -1)
        return -1;

    // 使用 OpenCV 加载图像
    cv::Mat image = cv::imread(image_path, cv::IMREAD_GRAYSCALE);
    if (image.empty()) {
        std::cerr << "Error: Failed to load image: " << image_path << std::endl;
        return -1;
    }

    // 调整图像大小到模型输入尺寸 (28x28)
    cv::Mat resized_image;
    cv::resize(image, resized_image, cv::Size(28, 28));

    // 将图像数据转换为浮点型并归一化到 [0, 1]
    resized_image.convertTo(resized_image, CV_32F);
    resized_image = resized_image / 255.0;

    // 将图像数据展平为一维数组
    cv::Mat flat_image = resized_image.reshape(1, 1);
    std::vector<float> normalized_image(flat_image.begin<float>(), flat_image.end<float>());

    // 将数据复制到输入张量
    float* input_data = interpreter->typed_input_tensor<float>(0);
    std::copy(normalized_image.begin(), normalized_image.end(), input_data);

    // 记录推理开始时间
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // 执行模型推理
    if (interpreter->Invoke() != kTfLiteOk) {
        std::cerr << "Failed to invoke interpreter." << std::endl;
        return -1;
    }
    
    // 记录推理结束时间
    auto end_time = std::chrono::high_resolution_clock::now();

    // 计算并输出推理耗时
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    // 获取输出张量数据
    float* output_data = interpreter->typed_output_tensor<float>(0);
    int output_size = interpreter->output_tensor(0)->dims->data[1];

    // 找到概率最大的类别索引
    int predicted_class = std::distance(output_data, std::max_element(output_data, output_data + output_size));
    
    // 输出结果
    std::cout << "Processed: " << image_path << "\n"
              << "Predicted class: " << dict_12class[predicted_class] 
              << " (class " << predicted_class << ") with probability: " 
              << output_data[predicted_class] << "\n"
              << "Inference time: " << duration.count() << " μs\n"
              << "----------------------------------------\n";


    // 如果成功提取到flow_id，则更新数据库
    if (flow_id != -1) {
        sqlite3 *db;
        char *err_msg = 0;
        int rc = sqlite3_open("/tmp/packet_info.db", &db);
        
        if(rc != SQLITE_OK) {
            std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        } else {
            char sql[256];
            snprintf(sql, sizeof(sql),
                    "UPDATE packet_info SET class = '%s', probability = %f "
                    "WHERE flow_id = %d;",
                    dict_12class[predicted_class].c_str(), 
                    output_data[predicted_class],
                    flow_id);
            
            rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
            
            if(rc != SQLITE_OK) {
                std::cerr << "SQL error: " << err_msg << std::endl;
                sqlite3_free(err_msg);
            }
            sqlite3_close(db);
        }
    }

    // 根据预测结果重命名文件
    if (predicted_class != -1) {
        std::string new_filename = dict_12class[predicted_class] + "_" + filename.substr(5);
        fs::path new_path = fs::path(image_path).parent_path() / new_filename;
        
        try {
            fs::rename(image_path, new_path);
            std::cout << "Renamed file to: " << new_path.filename() << "\n";
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Error renaming file: " << e.what() << "\n";
        }
    }

    return predicted_class;
}

// 处理整个目录
void ProcessDirectory(const std::string& dir_path, tflite::Interpreter* interpreter) {
    if (!fs::exists(dir_path)) {
        std::cerr << "Error: Directory not found: " << dir_path << std::endl;
        return;
    }

    std::map<int, int> class_count;
    int total_images = 0;
    double total_time = 0.0;

    // 遍历目录中的所有PNG文件
    for (const auto& entry : fs::directory_iterator(dir_path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".png") {
            std::string filename = entry.path().filename().string();
            
            // 提取 flow_ 开头的文件ID
            if (filename.rfind("flow_", 0) == 0) {  // 检查是否以flow_开头
                size_t underscore_pos = filename.find('_', 5);  // 查找第二个下划线
                size_t dot_pos = filename.find('.');
                if (underscore_pos != std::string::npos && dot_pos != std::string::npos) {
                    std::string id_str = filename.substr(5, underscore_pos - 5);
                    std::cout << "Processing flow file, ID: " << id_str << std::endl;
                }
            }

            auto start_time = std::chrono::high_resolution_clock::now();
            int predicted_class = ProcessSingleImage(entry.path().string(), interpreter);
            auto end_time = std::chrono::high_resolution_clock::now();
            
            if (predicted_class != -1) {
                double duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
                total_time += duration;
                class_count[predicted_class]++;
                total_images++;
            }
        }
    }

    // 输出统计信息
    std::cout << "\nProcessing completed!\n"
              << "Total images processed: " << total_images << "\n"
              << "Total inference time: " << total_time / 1000 << " ms\n"
              << "Average inference time per image: " << total_time / total_images << " μs\n"
              << "Class distribution:\n";
    
    for (const auto& [class_id, count] : class_count) {
        std::cout << dict_12class[class_id] << ": " << count << " images (" 
                  << (count * 100.0 / total_images) << "%)\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <image_path_or_directory>" << std::endl;
        return 1;
    }

    const std::string model_path = "model.tflite"; // TensorFlow Lite 模型路径
    const std::string input_path = argv[1]; // 输入路径（文件或目录）

    // 加载 TensorFlow Lite 模型
    std::vector<char> model_data = ReadFile(model_path);
    auto model = tflite::FlatBufferModel::BuildFromBuffer(model_data.data(), model_data.size());
    if (!model) {
        std::cerr << "Failed to load TFLite model." << std::endl;
        return 1;
    }

    // 创建解释器
    tflite::ops::builtin::BuiltinOpResolver resolver;
    tflite::InterpreterBuilder builder(*model, resolver);
    std::unique_ptr<tflite::Interpreter> interpreter;
    if (builder(&interpreter) != kTfLiteOk) {
        std::cerr << "Failed to create interpreter." << std::endl;
        return 1;
    }

    // 分配张量
    if (interpreter->AllocateTensors() != kTfLiteOk) {
        std::cerr << "Failed to allocate tensors." << std::endl;
        return 1;
    }

    // 判断输入是文件还是目录
    if (fs::is_directory(input_path)) {
        ProcessDirectory(input_path, interpreter.get());
    } else if (fs::is_regular_file(input_path) && input_path.substr(input_path.find_last_of(".") + 1) == "png") {
        ProcessSingleImage(input_path, interpreter.get());
    } else {
        std::cerr << "Invalid input: must be a PNG file or directory containing PNG files." << std::endl;
        return 1;
    }

    return 0;
}

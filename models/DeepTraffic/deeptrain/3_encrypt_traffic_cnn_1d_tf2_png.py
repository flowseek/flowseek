import time
import sys
import os
import numpy as np
import tensorflow as tf

from PIL import Image

# 禁用 GPU
tf.config.set_visible_devices([], 'GPU')

# Configuration
PNG_PATH = sys.argv[1]
CLASS_NUM = int(sys.argv[2])

# Class dictionaries
class_dicts = {
    2: {0:'Novpn', 1:'Vpn'},
    6: {0:'Chat', 1:'Email', 2:'File', 3:'P2p', 4:'Streaming', 5:'Voip'},
    12: {0:'Chat', 1:'Email', 2:'File', 3:'P2p', 4:'Streaming', 5:'Voip',
         6:'Vpn_Chat', 7:'Vpn_Email', 8:'Vpn_File', 9:'Vpn_P2p', 10:'Vpn_Streaming', 11:'Vpn_Voip'}
}


def load_single_png(png_path):
    """加载并预处理 PNG 图像
    
    Args:
        png_path: PNG 图像路径
        
    Returns:
        numpy array: 预处理后的图像数据，形状为 (1, 784, 1)
    """
    # 检查文件是否存在
    if not os.path.exists(png_path):
        raise FileNotFoundError(f"PNG file {png_path} does not exist")
    
    # 打印原始图像信息
    img = Image.open(png_path)
    print(f"\nLoading {png_path}: mode={img.mode}, size={img.size}")
    
    # 确保图像是灰度图并调整为28x28
    if img.mode != 'L':
        print(f"Converting {img.mode} image to grayscale")
        img = img.convert('L')
        
    img = img.resize((28, 28))
    
    # 转换为 numpy 数组
    img_array = np.array(img)
    print(f"Preprocessed Image: min={img_array.min()}, max={img_array.max()}")
    
    # 检查像素值范围是否合理
    if img_array.max() > 255 or img_array.min() < 0:
        raise ValueError(f"Invalid pixel value range: [{img_array.min()}, {img_array.max()}]")
        
    # 归一化到 [0, 1] 范围并 reshape
    img_array = img_array.astype('float32') / 255.0
    img_array = img_array.reshape(1, 784, 1)
    
    # 打印前10个像素值用于调试
    print("First 10 pixels:", np.round(img_array[0, :10, 0], 3))
    
    return img_array

def main():
    # Load single PNG image
    test_image = load_single_png(PNG_PATH)
    
    # Print test_image in hexadecimal with 8-digit precision
    print("Test Image in Hexadecimal:")
    for i in range(784):
        # Convert float to 32-bit integer and format as 0xXXXXXXXX
        float_value = test_image[0, i, 0]
        int_value = np.float32(float_value).view(np.uint32)
        hex_str = f"0x{int_value:08x}"
        print(f"{i:03}: {float_value:.6f} (Hex: {hex_str})")

    # Load existing model
    model_path = os.path.join("model", "model.h5")
    if not os.path.exists(model_path):
        raise FileNotFoundError(f"Model file {model_path} does not exist. Please train the model first.")
    
    print("Loading existing model...")
    model = tf.keras.models.load_model(model_path)
    
    model.summary()

    # Perform inference
    y_pred = model.predict(test_image)
    y_pred_class = np.argmax(y_pred, axis=1)[0]
    
    class_dict = class_dicts.get(CLASS_NUM, {})
    pred_label = class_dict.get(y_pred_class, str(y_pred_class))
    
    # Print inference result
    print(f"Predicted Label: {pred_label}")

if __name__ == "__main__":
    main()

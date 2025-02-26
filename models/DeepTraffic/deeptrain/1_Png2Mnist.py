"""
本脚本用于将PNG格式的手写数字图片转换为MNIST格式的数据集。
MNIST格式是机器学习中广泛使用的手写数字数据集格式。

输入：按类别组织的PNG图片文件夹结构：
    4_Png/Train/0/  - 包含数字0的图片
    4_Png/Train/1/  - 包含数字1的图片
    ...
    4_Png/Test/0/   - 测试集数字0图片
    4_Png/Test/1/   - 测试集数字1图片

输出：MNIST格式的二进制文件，包含：
    - 图片数据（images-idx3-ubyte）
    - 标签数据（labels-idx1-ubyte）
文件会自动进行gzip压缩
"""

import os
import errno
import argparse
from PIL import Image  # 图片处理库
from array import *    # 用于处理二进制数组
from random import shuffle  # 用于打乱数据顺序

def mkdir_p(path):
    """
    创建目录，如果目录已存在则忽略
    python中的mkdir -p功能实现
    """
    try:
        os.makedirs(path)
    except OSError as exc:  # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else:
            raise

# 创建输出目录
mkdir_p('Mnist')

# 解析命令行参数
parser = argparse.ArgumentParser(description='将PNG格式的手写数字图片转换为MNIST格式的数据集')
parser.add_argument('--train', required=True, help='训练集图片文件夹路径')

args = parser.parse_args()

# 收集所有PNG文件路径
FileList = []
for dirname in os.listdir(args.train):
    path = os.path.join(args.train, dirname)
    for filename in os.listdir(path):
        if filename.endswith(".png"):
            FileList.append(os.path.join(args.train, dirname, filename))

# 随机打乱文件顺序
shuffle(FileList)

# 计算测试集大小（训练集的10%）
test_size = int(len(FileList) * 0.1)
test_files = FileList[:test_size]
train_files = FileList[test_size:]

# 定义输入输出路径对应关系
# Names = [文件列表, 输出路径前缀]
Names = [[train_files, 'Mnist/train'], [test_files, 'Mnist/t10k']]

for name in Names:
	# 初始化二进制数组
	data_image = array('B')  # 存储图片像素数据
	data_label = array('B')  # 存储图片标签


	# 处理每个PNG文件
	for filename in name[0]:
		# 使用os.path获取父目录名作为标签
		dirname = os.path.basename(os.path.dirname(filename))
		try:
			label = int(dirname)
		except ValueError:
			print(f"Skipping invalid label directory: {dirname}")
			continue
		
		# 打开图片并获取像素数据
		Im = Image.open(filename)
		pixel = Im.load()
		width, height = Im.size
		
		# 将图片像素按行优先顺序存入数组
		for x in range(0,width):
			for y in range(0,height):
				data_image.append(pixel[y,x])
				
		# 添加对应的标签
		data_label.append(label) # 每个标签占1个无符号字节

	# 将文件数量转换为16进制格式
	hexval = "{0:#0{1}x}".format(len(FileList),6) # 文件数量转为16进制
	hexval = '0x' + hexval[2:].zfill(8)  # 格式化8位16进制数
	
	# 创建标签文件的头信息
	header = array('B')
	header.extend([0,0,8,1])  # 魔数（Magic Number）表明这是标签文件
	# 将文件数量按大端序写入头信息
	header.append(int('0x'+hexval[2:][0:2],16))
	header.append(int('0x'+hexval[2:][2:4],16))
	header.append(int('0x'+hexval[2:][4:6],16))
	header.append(int('0x'+hexval[2:][6:8],16))	
	data_label = header + data_label  # 将头信息与标签数据合并
	# 创建图片文件的头信息
	if max([width,height]) <= 256:
		header.extend([0,0,0,width,0,0,0,height])  # 添加图片尺寸信息
	else:
		raise ValueError('Image exceeds maximum size: 256x256 pixels')

	header[3] = 3 # 修改魔数以表明这是图片文件 (0x00000803)	
	data_image = header + data_image  # 将头信息与图片数据合并
	
	# 写入图片数据文件
	output_file = open(name[1]+'-images-idx3-ubyte', 'wb')
	data_image.tofile(output_file)
	output_file.close()
	
	# 写入标签数据文件
	output_file = open(name[1]+'-labels-idx1-ubyte', 'wb')
	data_label.tofile(output_file)
	output_file.close()

# 对生成的文件进行gzip压缩（MNIST标准格式要求）
for name in Names:
	os.system('gzip '+name[1]+'-images-idx3-ubyte')
	os.system('gzip '+name[1]+'-labels-idx1-ubyte')

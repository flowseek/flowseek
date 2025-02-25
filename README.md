# aifw

Author:  dnsnat 
Page  :  https://space.bilibili.com/317282027
Blog  :  https://blog.dnsnat.com/

#### 介绍



AI防火墙
硬件：复旦微FMQL45T900
![输入图片说明](https://foruda.gitee.com/images/1738312820482474811/4908e768_1626117.png "屏幕截图 2025-01-31 162952.png")
功能：

1. 嵌入式arm linux环境实现tenterflow lite c++推理。
2. 通过fpga加速卷积、池化、激活。
3. 结合ndpi-ntopng实现深度报文识别。
4. 基于深度学习实现加密流量识别。


#### 软件架构
软件架构说明
![输入图片说明](https://foruda.gitee.com/images/1738312890377448933/7f97c0a6_1626117.png "屏幕截图 2025-01-31 164058.png")

#### 安装教程

1. tensorflow

2. opencv

3. 

   

#### 使用说明

1.  windows下VPN-PCAPS-01.zip处理

```bash
cd aifw\2.PreprocessedTools
.\1_Pcap2Session.ps1
.\2_ProcessSession.ps1

python .\3_Session2png.py
python .\4_Png2Mnist.py
```

2. 模型训练

```bash
cd aifw/4.TrainAndTest/1d_cnn_25+3
#python ./encrypt_traffic_cnn_1d_tf2.py  ../../3.PerprocessResults/12class/SessionL7 12  20000
python ./encrypt_traffic_cnn_1d_tf2.py ../../2.PreprocessedTools/5_Mnist 12 2000
Classification Report:
               precision    recall  f1-score   support

         Chat       0.60      0.87      0.71       349
        Email       0.75      0.17      0.28       230
         File       0.92      0.89      0.90       600
          P2p       0.88      0.92      0.90       100
    Streaming       0.67      0.73      0.70       139
         Voip       0.91      0.92      0.91       600
     Vpn_Chat       0.96      0.93      0.94       401
    Vpn_Email       0.92      0.83      0.87        29
     Vpn_File       0.61      0.81      0.70        97
      Vpn_P2p       0.89      0.91      0.90        45
Vpn_Streaming       0.74      0.74      0.74        54
     Vpn_Voip       0.94      0.94      0.94       600

     accuracy                           0.84      3244
    macro avg       0.82      0.81      0.79      3244
 weighted avg       0.86      0.84      0.83      3244
```

3. linux下tensorflow推理 - **嵌入式AI部分**

```bash
cd aifw\5.Invoke
python.exe .\convert.py
./build/minimal /tmp/flow_session/vpn_facebook_audio2.TCP_10.8.0.10_42112_173.252.79.94_443.png 
INFO: Created TensorFlow Lite XNNPACK delegate for CPU.
推理时间: 2686 微秒
Class Chat probability: 0
Class Email probability: 0
Class File probability: 0
Class P2p probability: 0
Class Streaming probability: 3.67928e-36
Class Voip probability: 0
Class Vpn_Chat probability: 4.78836e-24
Class Vpn_Email probability: 0
Class Vpn_File probability: 5.04336e-09
Class Vpn_P2p probability: 0
Class Vpn_Streaming probability: 0
Class Vpn_Voip probability: 1
Predicted class: Vpn_Voip (class 11) with probability: 1
```

4. dpi流量识别后端

```bash
sudo ./ndpiReader -i eth0
   === 数据包信息 [Flow ID: 369] ===                      
   地址: 10.125.114.143:2716 <-> 61.221.110.227:58477                                     
   协议: TLS                                                                               
   类别: Web                                                                               
   主机名: cdnj4209.ww000.xyz                                      
   流量统计:                                                       
   - 上行: 25 包 / 8864 字节                                       
   - 下行: 30 包 / 14052 字节                                      
   时间信息:                                                                               
   - 首次见到: 2025-02-04 20:43:08.004                             
   - 最后见到: 2025-02-04 20:43:09.775                                                     
   风险: Known Proto on Non Std Port,TLS (probably) Not Carrying HTTPS                     
   =======================
```

5. web后端

```bash
./x86_64-pc-linux-gnu-aifw  
   Server running at http://localhost:3000
   [进程 429858] 等待获取信号量...
   [进程 429858] 成功获取信号量
   [进程 429858] 数据库操作完成，准备释放信号量
   [进程 429858] 信号量已释放
```

6. web前端

![](README/屏幕截图_20250204_204734.png)







#### 参与贡献

1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request

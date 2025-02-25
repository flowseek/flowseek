# FlowSeek

--------------------------------------------------------------------------------
[**FlowSeek**][repo] is a multiplatform c/c++ framework providing fast, flexible, and expressive data structures designed to make
working with **online** or **offline** network data easy and intuitive. It aims to be c/c++'s fundamental high-level
building block for doing practical, **real-world** network flow data analysis. Additionally, it has the broader
goal of becoming **a unifying network data analytics framework for researchers** providing data reproducibility
across experiments.


## Main Features

* **Performance:** FlowSeek is designed to be fast: [**AF_PACKET_V3/FANOUT**][packet] on Linux, multiprocessing, native
  [**CFFI based**][cffi] computation engine.
* **Encrypted layer-7 visibility:** FlowSeek deep packet inspection is based on [**nDPI**][ndpi].
  It allows FlowSeek to perform [**reliable**][reliable] encrypted applications identification and metadata
  fingerprinting (e.g. TLS, SSH, DHCP, HTTP).
* **System visibility:** FlowSeek probes the monitored system's kernel to obtain information on open Internet sockets
  and collects guaranteed ground-truth (process name, PID, etc.) at the application level.
* **Deep Learning oriented:** FlowSeek aims to make Deep Learning Approaches for network traffic management
reproducible and deployable. By using FlowSeek as a common framework, researchers ensure that models are trained using
the same feature computation logic, and thus, a fair comparison is possible. Moreover, trained models can be deployed
and evaluated on live networks using [**NFPlugins**][nfplugin].

## How to get it?

Binary installers for the latest released version are available.

```bash
wget flowseek
```

> **Windows Notes**: FlowSeek does not include capture drivers on Windows (license restrictions). It is required to
> install [Npcap drivers][npcap] before installing FlowSeek.
> If Wireshark is already installed on Windows, then Npcap drivers are already installed, and you do not need to perform
> any additional action.

## How to use it?

### Encrypted application identification and metadata extraction

Dealing with a big pcap file and want to aggregate into labeled network flows? **Flowseek** make this path easier in
a few lines:

```python

```

```python

```

### System visibility

FlowSeek probes the monitored system's kernel to obtain information on open Internet sockets and collects guaranteed
ground-truth (process name, PID, etc.) at the application level.

```python

```

```python

```

### Post-mortem statistical flow features extraction

FlowSeek performs 48 post-mortem flow statistical features extraction, which includes detailed TCP flags analysis,
minimum, mean, maximum, and standard deviation of both packet size and inter-arrival time in each direction.

```python

```



### Early statistical flow features extraction

FlowSeek performs early (up to 255 packets) flow statistical features extraction (referred to as SPLT analysis in the
literature). It is summarized as a sequence of these packets' directions, sizes, and inter-arrival times.

```python

```

### Pandas export interface

FlowSeek natively supports Pandas as an export interface.

```python

```



### CSV export interface

FlowSeek natively supports CSV file format as an export interface.

```python

```



### Machine Learning models training and deployment

The following simplistic example demonstrates how to train and deploy a machine-learning approach for traffic
flow categorization.
We want to run a classification of Social Network category flows based on bidirectional_packets and bidirectional_bytes
as input features. For the sake of brevity, we decide to predict only at the flow expiration stage.

#### Training the model

```python

```

#### ML powered streamer on live traffic

```python

```



## Building from sources

git子模块创建
```bash
git submodule add https://github.com/the-tcpdump-group/libpcap.git lib/libpcap 
git submodule add https://github.com/ntop/nDPI.git lib/nDPI 
git submodule add https://github.com/tensorflow/tensorflow.git lib/tensorflow
git submodule status
 2852d15aeee0a493f01b82ac163d7510441bfaac lib/libpcap (libpcap-1.8.1-3760-g2852d15a)
 294b7e18897d4c4cdf2f584dabb87b37015b2337 lib/nDPI (1.7-4946-g294b7e188)
```

git自模块同步
```bash
git submodule update --init --recursive
cd ./lib/libpcap
git checkout 2852d15aeee0a493f01b82ac163d7510441bfaac

cd ./lib/nDPI
git checkout 294b7e18897d4c4cdf2f584dabb87b37015b2337

cd ./lib/tensorflow
git checkout 8a6f519eb79493ccbbee12af45732b19498242da
```

git自模块编译
```bash
cd ./lib/libpcap
./autogen.sh 
./configure --disable-shared --disable-dbus  --without-libnl
make

cd ./lib/nDPI
./autogen.sh --with-only-libndpi
./configure 
make
```



## Contributing

Please read [**Contributing**][contribute] for details on our code of conduct and the process for submitting pull
requests to us.



## Ethics

**FlowSeek** is intended for network data research and forensics. Researchers and network data scientists can use this
framework to build reliable datasets and train and evaluate network-applied machine learning models.
As with any packet monitoring tool, **FlowSeek** could be misused. **Do not run it on any network that you do not own or
administrate**.



## Credits

### 



### Authors

The following people contributed to FlowSeek:

* [**阿威**](mailto:aouinizied@gmail.com): Creator and core developer.

  

### Supporting organizations

The following organizations supported FlowSeek:

* [**ntop**](https://www.ntop.org/): Technical support of [**nDPI**][ndpi] integration.
* [**The Nmap Project**](https://nmap.org): Technical support of [**Npcap**][npcap] integration
  (NPCAP OEM installer on Windows CI).



## Publications that use FlowSeek

* [**A Hierarchical Architecture and Probabilistic Strategy for Collaborative Intrusion Detectionn**](https://ieeexplore.ieee.org/abstract/document/9705027)
* [**Open-Source Framework for Encrypted Internet and Malicious Traffic Classification**](https://arxiv.org/pdf/2206.10144.pdf)
* [**ConFlow: Contrast Network Flow Improving Class-Imbalanced Learning in Network Intrusion Detection**](https://www.researchsquare.com/article/rs-1572776/latest.pdf)
* [**Continual Learning for Anomaly based Network Intrusion Detection**](https://ieeexplore.ieee.org/abstract/document/9668482)
* [**A self-secure system based on software-defined network**](https://www.spiedigitallibrary.org/conference-proceedings-of-spie/12250/122500Z/A-self-secure-system-based-on-software-defined-network/10.1117/12.2639591.short?SSO=1)
* [**Robust Variational Autoencoders and Normalizing Flows for Unsupervised Network Anomaly Detection**](https://hal.archives-ouvertes.fr/hal-03542451/document)
* [**RADON: Robust Autoencoder for Unsupervised Anomaly Detection**](https://ieeexplore.ieee.org/document/9699174)
* [**A Generic Machine Learning Approach for IoT Device Identification**](https://ieeexplore.ieee.org/document/9702983)
* [**Ranking Network Devices for Alarm Prioritisation: Intrusion Detection Case Study**](https://ieeexplore.ieee.org/abstract/document/9559086)
* [**Network Flows-Based Malware Detection Using A Combined Approach of Crawling And Deep Learning**](https://ieeexplore.ieee.org/document/9500920)
* [**Network Intrusion Detection Based on Distributed Trustworthy Artificial Intelligence**](https://repository.dl.itc.u-tokyo.ac.jp/record/2002253/files/48196454.pdf)
* [**Generative Transformer Framework For Network Traffic Generation And Classification**](https://cyberleninka.ru/article/n/generative-transformer-framework-for-network-traffic-generation-and-classification)
* [**Multi-Class Network Traffic Generators and Classifiers Based on Neural Networks**](https://ieeexplore.ieee.org/document/9416067)
* [**Using Embedded Feature Selection and CNN for Classification on CCD-INID-V1 A New IoT Dataset**](https://www.mdpi.com/1424-8220/21/14/4834)
* [**An Approach Based on Knowledge-Defined Networking for Identifying Video Streaming Flows in 5G Networks**](https://latamt.ieeer9.org/index.php/transactions/article/view/5083/1116)
* [**Knowledge Discovery: Can It Shed New Light on Threshold Definition for Heavy‑Hitter Detection?**](https://link.springer.com/content/pdf/10.1007/s10922-021-09593-w.pdf)
* [**Collecting and analyzing Tor exit node traffic**](https://www.diva-portal.org/smash/get/diva2:1575255/FULLTEXT01.pdf)
* [**Analysis and Collection Data from IP Network**](https://sciendo.com/article/10.2478/aei-2022-0013)

## License

This project is licensed under the LGPLv3 License - see the [**License**][license] file for details

[license]: https://github.com/nfstream/nfstream/blob/master/LICENSE
[contribute]: https://nfstream.org/docs/community
[documentation]: https://nfstream.org/
[ndpi]: https://github.com/ntop/nDPI
[npcap]: https://npcap.org
[nfplugin]: https://nfstream.org/docs/api#nfplugin
[reliable]: http://people.ac.upc.edu/pbarlet/papers/ground-truth.pam2014.pdf
[repo]: https://nfstream.org/
[demo]: https://mybinder.org/v2/gh/nfstream/nfstream/master?filepath=demo_notebook.ipynb
[pypy]: https://www.pypy.org/
[cffi]: https://cffi.readthedocs.io/en/latest/goals.html
[sah]:https://raw.githubusercontent.com/nfstream/nfstream/master/assets/sah_logo.png?raw=true
[tuke]:https://raw.githubusercontent.com/nfstream/nfstream/master/assets/tuke_logo.png?raw=true
[ntop]:https://raw.githubusercontent.com/nfstream/nfstream/master/assets/ntop_logo.png?raw=true
[nmap]:https://raw.githubusercontent.com/nfstream/nfstream/master/assets/nmap_logo.png?raw=true
[google]:https://raw.githubusercontent.com/nfstream/nfstream/master/assets/google_logo.png?raw=true
[l]:https://github.com/ryanoasis/nerd-fonts/wiki/screenshots/v1.0.x/linux-pass-sm.png
[m]:https://github.com/ryanoasis/nerd-fonts/wiki/screenshots/v1.0.x/mac-pass-sm.png
[w]:https://github.com/ryanoasis/nerd-fonts/wiki/screenshots/v1.0.x/windows-pass-sm.png
[install]: https://www.nfstream.org/docs/#building-nfstream-from-sources
[doi]: https://doi.org/10.1016/j.comnet.2021.108719
[comnet]: https://www.sciencedirect.com/journal/computer-networks/vol/204/suppl/C
[packet]: https://manned.org/packet.7




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

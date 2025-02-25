
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

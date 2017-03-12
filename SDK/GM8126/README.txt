GM8126交叉编译工具链下载：http://pan.baidu.com/s/1kTxVIq7

1. 将crosstool.tgz到/opt
2. 使用tar -xvzf crosstool.tgz解压到/opt目录
2. 在/etc/profile添加
export PATH="$PATH:/opt/crosstool/arm-none-linux-gnueabi-4.4.0_ARMv5TE/bin"
3. 重新启动linux
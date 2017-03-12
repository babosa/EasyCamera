HI3518交叉编译工具链下载：http://pan.baidu.com/s/1qWBYFCC

1. 将hi3518gcc.tgz到/opt
2. 使用tar -xvzf hi3518gcc.tgz解压到/opt目录
2. 在/etc/profile添加
export PATH="$PATH:/opt/hisi-linux-nptl/arm-hisiv100-linux/bin"
3. 重新启动linux
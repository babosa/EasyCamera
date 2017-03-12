# EasyCamera #

### ！！！EasyCamera是配套EasyDarwin云平台使用的，是平台整体的设备部分，类似于萤石、360水滴、小米、i耳目、Dropcam这种云摄像机方案的模式和流程！ ###


**EasyCamera** 并不是做摄像机硬件方案，或者说EasyCamera是硬件芯片无关的云平台对接方案，我们是在摄像机方案商方案的基础上，通过摄像机方案商提供的SDK在摄像机ARM系统内部与摄像机主服务进行交互，包括实时音视频、云台控制、联动报警等功能，我们在摄像机内部植入EasyCamera程序，一边通过SDK与摄像机主服务交互(获取音视频数据、控制云台、联动报警等交互)，一边与EasyDarwin流媒体云平台(EasyCMS、EasyDarwin、EasyRMS等云服务器)对接，上传数据、接受指令控制，形成一套云摄像机方案.

EasyCamera服务支持Windows平台/ARM摄像机(目前已经有国内多家摄像机方案支持接入，包括雄迈、海芯威视、普顺达等)，支持Android/iOS移动设备(开发中)对接到EasyDarwin开源流媒体云平台，我们定制的摄像机采海思HI3518C/HI3518E、智源GM8126方案，支持RTSP、Onvif、Web管理、配套SDK工具，作为开发辅助的硬件工具，我们提供了全套完备的程序和文档，既可以用于流媒体学习，又可以用于方案移植参考，更可以直接用于项目中，购买最新参考设备可以在：[http://www.easydarwin.org/camera](http://www.easydarwin.org/camera "EasyCamera摄像机购买")，用户也可以将我们参考摄像机的SDK部分替换成自己摄像机的硬件SDK，移植非常方便；

## EasyCamera包括 ##

- **SDK** 摄像机版本及SDK调用示例
- **SRC** EasyCamera开源程序

## 视频教程 ##
EasyCamera开源摄像机解决方案：编译、配置、部署：[http://edu.csdn.net/course/detail/2502/39197](http://edu.csdn.net/course/detail/2502/39197 "EasyCamera")
> BTW：教程第一章里面有具体EasyCamera工作模式的详细说明，请大家看过之后，细细体会！

## 编译和部署方法 ##

### 1、编译EasyCamera最新版本 ###

目前EasyCamera Demo提供了Windows/ARM(GM8126、HI3518C/HI3518E)两个版本！

- Windows版本编译：

可以直接用Visual Studio 2008打开源码文件中的：/EasyDarwin-master/EasyCamera/WinNTSupport/EasyCamera.sln解决方案文件，编译出exe可执行文件EasyCamera.exe；

- ARM版本编译：

这里只说明EasyDarwin开源摄像机的编译方法,其他类型摄像机编译方法类似, 前提是配置相应的交叉编译工具链，我们有两款方案的摄像机，GM8126和HI3518C/HI3518E，工具链分别在/EasyCamera-master/SDK/GM8126/和/EasyCamera-master/SDK/HI3518/，我们这里以安装HI3518C/HI3518E交叉编译工具链为例(HI3518C和HI3518E方案交叉编译工具链相同)：

> “
> 交叉编译工具链可以到[http://pan.baidu.com/s/1qWBYFCC](http://pan.baidu.com/s/1qWBYFCC)下载hi3518gcc.tgz文件，解压hi3518gcc.tgz至Linux开发宿主机的/opt目录，在/etc/profile里设置将交叉编译工具链目录设置到PATH变量，重启完成安装，也可运行source /etc/profile命令使其立即生效。
> 
> - 解压命令：tar zxvf hi3518gcc.tgz -C /opt
> 
> - 在/etc/profile添加：export PATH="$PATH:/opt/hisi-linux-nptl/arm-hisiv100-linux/bin"
> 
> - 立即生效：source /etc/profile
> 
> - 验证PATH：echo $PATH
> “

编译方法,

    cd ./EasyDarwin-master/EasyCamera
    chmod +x ./Buildit
    ./Buildit hisiv100
    cd ./Bin


### 2、配置easycamera.xml ###
EasyCamera主要的几个配置项：

- ***easycms_ip***：EasyCMS服务的IP地址或者域名；

- ***easycms_port***：EasyCMS服务的监听端口；

- ***device_serial***：自定义配置的摄像机序列号，12位字符串(具体规则查看/EasyDarwin/Doc/EasyDarwin Protocol.doc)；

- ***device_name***：摄像机名称；

- ***device_key***：EasyCMS验证密码，默认123456；

- ***device_tag***：摄像机标签，默认dev；

- ***keep_alive_interval***：保活间隔时长，单位为秒，默认30秒；

- ***camera_ip***：本地摄像机地址，例如EasyCamera Windows版本是与硬件分离的，那么具体配置摄像机的ip地址，ARM版本EasyCamera内置于摄像机内部，可以直接配置成127.0.0.1；

- ***camera_port***：摄像机监听端口，默认80，也可以在摄像机Web管理页面重新配置；

- ***camera_user***：摄像机用户名，默认admin;

- ***camera_password***：摄像机密码，默认admin；

- ***camera_stream_type***：默认摄像机传输的码流类型，0表示子码流，1表示主码流；

### 3、运行EasyCamera ###
Windows版本运行(控制台调试运行)：

    EasyCamera.exe -c ./easycamera.xml -d


摄像机内运行：
首先是将ARM程序如何放入摄像机内部，方法见随后的*摄像机操作指南*部分，我们将可执行文件放置于**/mnt/mtd/easycamera/**目录；

调试模式运行（具体配置文件路径根据实际情况设置）,

	cd /mnt/mtd/easycamera/
    ./easycamera -c ./easycamera.xml  -d
守护进程运行,

    ./easycamera -c ./easycamera.xml  &
注：如果xml配置文件路径不能确定，建议最安全的方式就是用全路径，例如 “/mnt/mtd/easycamera/easycamera.xml”，这样在下一次更新服务的时候，配置文件可以保留！

### 4、跟随摄像机系统自启动 ###
需要让EasyCamera程序跟随摄像机系统自动启动，我们需要修改**/mnt/mtd/ipc/allexit.sh**和**/mnt/mtd/ipc/platform.sh**两个启动脚本：

在allexit.sh最后一行加上：

	#! /bin/sh
	...

	OPID=`ps | grep easycamera | awk '{print $1}'`
	kill -9 $OPID

在platform.sh最后一行加上：

	#! /bin/sh
	...

	cd /mnt/mtd/easycamera &&
	./easycamera -c /mnt/mtd/easycamera/easycamera.xml &


### 5、检查EasyCamera是否运行成功 ###
可以通过EasyCamera -d调试模式，查看是否配置成功，也可以到EasyCMS查看设备是否上线；


## 系统架构 ##
![](http://www.easydarwin.org/skin/easydarwin/images/architecture20150825.png)

## 摄像机操作指南 ##

### 1、连接网络 ###
将摄像机通过有线的方式连接到摄像机，路由器需要开启DHCP功能，给摄像机分配到IP地址（如果路由器没有开启DHCP功能，摄像机连接网线后，摄像机的默认IP就是*192.168.1.88*）；

### 2、查找摄像机 ###
打开/EasyCamera-master/SDK/NetLib/bin/HiCamSearcher.exe，搜索摄像机：

![HiCamSearcher](http://www.easydarwin.org/github/images/easycamera/001.png)

### 3、区分摄像机硬件方案 ###

通过浏览器访问摄像机进入Web管理页面，进入设备信息页面，找到“软件版本”或者“固件版本”项，如果版本号以V5打头，那么摄像机是智源GM8126方案，如果版本号以V6打头，那么摄像机是海思HI3518C方案,如果版本号以V7打头，那么摄像机是海思HI3518E方案。


### 4、摄像机开启Telnet服务 ###

通过浏览器访问摄像机进入Web管理页面，进入系统维护页面，在系统升级项中点击浏览找到所提供的升级包（GM8126方案选择/EasyCamera-master/SDK/GM8126/**telnet_8126.pkg**，HI3518C/HI3518E方案选择/EasyCamera-master/SDK/HI3518/**telnet_3518.pkg**），点击确定，等待系统重启。 例如：http://192.168.*.*/web/admin.html

![EasyCamera Telnet](http://www.easydarwin.org/github/images/easycamera/002.png)

### 5、通过Telnet访问摄像机 ###

摄像机开启telnet服务后即可通过telnet 终端进行访问。GM8126方案用户名为：root，密码为空、HI3518C方案用户名为：admin，密码为：HX2105，HI3518E方案用户名为：admin，密码为：2601hx。如下图所示。**摄像机自带的程序与配置位于/mnt/mtd/，请勿删除此目录下任何内容！！！**

![telnet](http://www.easydarwin.org/github/images/easycamera/003.png)

### 6、下载文件到摄像机 ###

可通过ftp协议进行文件传输，摄像机提供ftpget、ftpput命令。用户可以自己的程序下载至/mnt/mtd目录(受嵌入式资源的限制，此款设备用户可支配的空间大约为2M)。

以ftpget命令示例：下载/EasyCamera-master/SDK/Quick Easy FTP Server V4.0.0.exe到Windows上(Linux同理找到相应的ftp服务器运行)，运行Quick Easy FTP Server V4.0.0.exe，设置对应的文件目录和ftp用户名密码:

![pure-ftp](http://easydarwin.org/github/images/easycamera/004.png)

在telnet终端里输入ftpget进行下载：

![download](http://easydarwin.org/github/images/easycamera/005.png)

### 7、摄像机多码流的RTSP地址 ###

摄像机提供1/2/3种码流，RTSP地址分别为：

- 主码流：RTSP://[IP]:[PORT]/11
- 子码流：RTSP://[IP]:[PORT]/12
- 三码流：RTSP://[IP]:[PORT]/13

具体每一个码流的参数细节可在Web管理中进行设置：
![EasyCamera RTSP](http://www.easydarwin.org/github/images/easycamera/008.png)

### 8、摄像机wifi无线连接设置 ###

Web连接到摄像机后，可以通过Web管理界面进行WIFI连接的设置：
![EasyCamera wifi](http://www.easydarwin.org/github/images/easycamera/009.png)


## 摄像机硬件购买 ##
[http://www.easydarwin.org/camera](http://www.easydarwin.org/camera "EasyCamera摄像机购买")

## 获取更多信息 ##

邮件：[support@easydarwin.org](mailto:support@easydarwin.org) 

WEB：[www.EasyDarwin.org](http://www.easydarwin.org)

Copyright &copy; EasyDarwin.org 2012-2017

![EasyDarwin](http://www.easydarwin.org/skin/easydarwin/images/wx_qrcode.jpg)

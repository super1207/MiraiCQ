MiraiCQ-Cli

软件说明：
	1：本软件的编写初衷为兼容酷Q插件，除此之外，还会提供一些额外的功能。
	2：多个插件共用一套消息系统，在收发图片文件时，可以显著节省网络开销。
	3：为网络处理较麻烦的编程语言提供便利的方式来开发聊天插件。
	4：对接`OneBot v11`或`Mirai Http Api`（暂未实现），软件长期可用。

命令行版本的MiraiCQ相比于之前的MFC GUI版本：
	1：更加稳定，高效，占用资源更少。
	2：更合理的图片收发机制。
	3：支持插件函数优先级，事件拦截。
	4：此版本为今后长期优化，维护的版本（之前的MFC GUI版本仍然可用，见其它分支）。

下载地址：
	1：https://github.com/super1207/MiraiCQ/releases
	2：https://github.com/super1207/MiraiCQ/actions

使用方法：
	1：将原酷Q插件的dll和json放入app文件夹。
	2：开启`OneBot`实现端的`websocket`正向连接（详细说明请参照此项目：https://github.com/Mrs4s/go-cqhttp ）。
	3：修改`config\congig.ini`文件中的`ws_url`和`access_token`项。
	4：运行`MiraiCQ.exe`。

运行环境：
	`Windows 7 with sp1`、`Windows Server 2016`或更高版本的Windows系统。
	`wine`（暂未测试）。

编译方法：
	1：安装VS2019，安装时勾选`使用C++的桌面开发`。
	2：clone本项目，使用VS2019打开`MiraiCQ.sln`，直接编译。

开源说明：
	本软件(不包括插件，插件是否免费取决于插件作者)永久免费使用，您可以通过任何合法方式使用本软件。
	本软件使用`GNU Affero General Public License v3.0`开源，若要用于其它用途，请仔细阅读开源协议。
	开源地址：https://github.com/super1207/MiraiCQ

交流与问题反馈：
	1：https://github.com/super1207/MiraiCQ/issues
	2：QQ群：920220179

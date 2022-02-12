MiraiCQ
==

<div align=center>
	<a href="https://imgtu.com/i/HC16MR"><img src="https://s4.ax1x.com/2022/01/30/HC16MR.png" alt="HC16MR.png" border="0" /></a>
</div>

#### 软件说明：

1. 本软件的编写初衷为兼容酷Q插件，除此之外，还会提供一些额外的功能(见[rand_pic](https://github.com/super1207/rand_pic))，来收发原始onebot数据。
2. 对接[OneBot v11](https://github.com/botuniverse/onebot-11)标准，软件长期可用。

#### 特色：

1. 更加稳定，高效，占用资源更少（可执行文件大小不到3M
3. **多进程**框架，每个插件都在独立的进程中运行，瞬间定位问题插件。
3. 简洁明了的操作界面，一看就懂。

#### 下载地址
1. [releases](https://github.com/super1207/MiraiCQ/releases)
2. [actions](https://github.com/super1207/MiraiCQ/actions) (预览版本，可能不够稳定)

#### 使用方法：

1. 将原酷Q插件的**dll**和**json**放入app文件夹(自行创建，或自动创建)。
2. 开启 OneBot v11 实现端的 websocket 正向连接，详细说明请参照此项目：[Mrs4s/go-cqhttp](https://github.com/Mrs4s/go-cqhttp)。
3. 运行 MiraiCQ.exe。

#### 运行环境：
	
Windows 7 with sp1 以及更新的版本，系统语言必须是中文。

#### 编译方法：

1. 安装VS2019，安装时勾选：使用C++的桌面开发。
2. clone本项目，使用VS2019打开 MiraiCQ.sln ，直接编译。

#### 开源说明：
本软件(不包括插件，插件是否免费取决于插件作者)永久免费使用，您可以通过任何合法方式使用本软件。

本软件使用 GNU Affero General Public License v3.0 开源，若要用于其它用途，请仔细阅读开源协议。

开源地址：https://github.com/super1207/MiraiCQ

#### 交流与问题反馈：
1. [issues](https://github.com/super1207/MiraiCQ/issues)
2. QQ群：920220179 (欢迎来玩

#### 常见问题解答：
1. 插件的json文件需要使用 GBK 编码。
2. 某些原酷Q插件会依赖一些酷Q的dll，可以自行将其放入指定目录，一般是bin目录（自行创建）或者MiraiCQ.exe目录（不清楚可以两个目录都放）。
3. 原酷Q中的[dll下载地址](https://super1207.lanzoui.com/iVNkJttuf2f)。
4. MiraiCQ不会支持获取txqq的cookie，csrf token，也不会支持点赞，钱包相关内容。
5. MiraiCQ"暂时"不支持语音收发。

#### 相关项目：

1. [botuniverse/onebot-11](https://github.com/botuniverse/onebot-11)(对接标准)
2. [Mrs4s/go-cqhttp](https://github.com/Mrs4s/go-cqhttp)(一个onebotv11实现)
3. [coolq-http-api](https://github.com/kyubotics/coolq-http-api)(主要测试插件以及部分源码参考)
4. [iTXTech/mirai-native](https://github.com/iTXTech/mirai-native)(相似项目，部分逻辑上参考)
5. [mamoe/mirai](https://github.com/mamoe/mirai)(起名)
6. 此外，其他相关项目在thirdpart目录下可以找到，不一一列举。
		

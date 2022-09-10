MiraiCQ
==

<div align=center>
	<a href="https://imgtu.com/i/OM7U3D"><img src="https://s1.ax1x.com/2022/05/07/OM7U3D.png" alt="OM7U3D.png" border="0" /></a>
</div>

#### 软件说明：

1. 本软件的编写初衷为兼容CQ格式的插件，除此之外，还会提供一些额外的功能(见[rand_pic](https://github.com/super1207/rand_pic))，来收发原始onebot数据。
2. 对接[OneBot v11](https://github.com/botuniverse/onebot-11)标准，软件长期可用。
3. 此项目与[酷Q机器人](https://baike.baidu.com/item/%E9%85%B7Q)**无任何联系**，MiraiCQ同等对待所有支持了onebot接口的平台，并且本身不涉及任何具体聊天平台相关内容。

#### 特色：

1. 更加稳定，高效，占用资源更少（可执行文件大小不到4M
2. **多进程**框架，每个插件都在独立的进程中运行，瞬间定位问题插件。
3. 简洁明了的操作界面，一看就懂。

#### 下载地址
1. [releases](https://github.com/super1207/MiraiCQ/releases)
2. [actions](https://github.com/super1207/MiraiCQ/actions) (预览版本，可能不够稳定，并且需要登录github才能下载)

#### 使用方法：

1. 将原CQ插件的**dll**和**json**放入app文件夹(自行创建，或自动创建)。
2. 开启 OneBot v11 实现端的 websocket 正向连接，详细说明请参照具体实现端的文档。
3. 运行 MiraiCQ.exe。

#### 运行环境：
	
Windows 7 with sp1 以及更新的版本，系统语言必须是中文。

#### 编译方法：

1. 安装VS2022，安装时勾选：使用C++的桌面开发。
2. clone本项目，使用VS2022打开 MiraiCQ.sln ，直接编译。

#### 开源说明：

本软件(不包括插件，插件是否免费取决于插件作者)永久免费使用，您可以通过任何合法方式使用本软件。

本项目的构建过程完全公开透明，为了您的安全，请不要在本说明*下载地址*一节以外的**任何地方**下载此软件。

本软件使用 GNU Affero General Public License v3.0 开源，若要用于其它用途，请仔细阅读开源协议。

唯一的开源地址：https://github.com/super1207/MiraiCQ

#### 交流与问题反馈：
1. [issues](https://github.com/super1207/MiraiCQ/issues)
2. QQ群：920220179 (欢迎来玩

#### 常见问题解答：
1. 某些原CQ插件会依赖一些CQ的dll，可以将其放入bin目录（自行创建）。
2. MiraiCQ不会支持获取任何涉及账号安全的行为，也不会支持钱包转账、红包收发等涉及金钱交易的行为。
3. MiraiCQ"暂时"不支持语音收发。注：您仍然可以使用[**onebot定义**的语音cq码](https://github.com/botuniverse/onebot-11/blob/master/message/segment.md#%E8%AF%AD%E9%9F%B3)来收发语音(需要安装ffmpeg，并且要确保onebot实现所在的电脑可以访问语音文件)。

#### 插件开发：

您可以寻找以前CQ的SDK来开发MiraiCQ的插件。如[kyubotics/cqcppsdk](https://github.com/kyubotics/cqcppsdk)

#### 相关项目：

1. [botuniverse/onebot-11](https://github.com/botuniverse/onebot-11)(对接标准)
2. [Mrs4s/go-cqhttp](https://github.com/Mrs4s/go-cqhttp)(一个onebotv11实现)
3. [coolq-http-api](https://github.com/kyubotics/coolq-http-api)(主要测试插件以及部分源码参考)
4. [iTXTech/mirai-native](https://github.com/iTXTech/mirai-native)(相似项目，部分逻辑上参考)
5. [mamoe/mirai](https://github.com/mamoe/mirai)(起名)
6. 此外，其他相关项目在thirdpart目录下可以找到，不一一列举。


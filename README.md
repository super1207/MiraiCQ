# MiraiCQ
[![QQ 群](https://img.shields.io/badge/qq%E7%BE%A4-920220179-orange.svg)]()
![](https://img.shields.io/badge/OneBot-v11-black?logo=data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAHAAAABwCAMAAADxPgR5AAAAGXRFWHRTb2Z0d2FyZQBBZG9iZSBJbWFnZVJlYWR5ccllPAAAAAxQTFRF////29vbr6+vAAAAk1hCcwAAAAR0Uk5T////AEAqqfQAAAKcSURBVHja7NrbctswDATQXfD//zlpO7FlmwAWIOnOtNaTM5JwDMa8E+PNFz7g3waJ24fviyDPgfhz8fHP39cBcBL9KoJbQUxjA2iYqHL3FAnvzhL4GtVNUcoSZe6eSHizBcK5LL7dBr2AUZlev1ARRHCljzRALIEog6H3U6bCIyqIZdAT0eBuJYaGiJaHSjmkYIZd+qSGWAQnIaz2OArVnX6vrItQvbhZJtVGB5qX9wKqCMkb9W7aexfCO/rwQRBzsDIsYx4AOz0nhAtWu7bqkEQBO0Pr+Ftjt5fFCUEbm0Sbgdu8WSgJ5NgH2iu46R/o1UcBXJsFusWF/QUaz3RwJMEgngfaGGdSxJkE/Yg4lOBryBiMwvAhZrVMUUvwqU7F05b5WLaUIN4M4hRocQQRnEedgsn7TZB3UCpRrIJwQfqvGwsg18EnI2uSVNC8t+0QmMXogvbPg/xk+Mnw/6kW/rraUlvqgmFreAA09xW5t0AFlHrQZ3CsgvZm0FbHNKyBmheBKIF2cCA8A600aHPmFtRB1XvMsJAiza7LpPog0UJwccKdzw8rdf8MyN2ePYF896LC5hTzdZqxb6VNXInaupARLDNBWgI8spq4T0Qb5H4vWfPmHo8OyB1ito+AysNNz0oglj1U955sjUN9d41LnrX2D/u7eRwxyOaOpfyevCWbTgDEoilsOnu7zsKhjRCsnD/QzhdkYLBLXjiK4f3UWmcx2M7PO21CKVTH84638NTplt6JIQH0ZwCNuiWAfvuLhdrcOYPVO9eW3A67l7hZtgaY9GZo9AFc6cryjoeFBIWeU+npnk/nLE0OxCHL1eQsc1IciehjpJv5mqCsjeopaH6r15/MrxNnVhu7tmcslay2gO2Z1QfcfX0JMACG41/u0RrI9QAAAABJRU5ErkJggg==)

![](https://ftp.bmp.ovh/imgs/2020/10/bc4e2057663fb481.png)

## 目的

* 使用C/C++、易语言来写QQ机器人
* 实现方便分享的插件式机器人框架
* 对接OneBotv11标准，使插件长期可用，即使某平台的机器人框架跑路，也不受影响

###  兼容酷Q插件

大多数原来酷Q插件可以直接运行在MiraiCQ上，详见 [兼容细节](https://github.com/super1207/MiraiCQ/blob/main/doc/CQ%E5%85%BC%E5%AE%B9%E7%BB%86%E8%8A%82.md)。 </a> <br />

### 不止是酷Q：可以使用Onebot中描述的所有API，Event

在插件中，可以使用额外的Event，API，来收发 [OneBot](https://github.com/howmanybots/onebot) 原始数据，完成对酷Q之外的功能的使用，详细使用方法参见目录下的 PicHP 插件 (私聊复读)，这也是推荐的使用方式。
代码一览：
```cpp #include "../core/stdafx.h"
#include "../jsoncpp/json.h"
#include "../core/PicHP.h"

int user_event_message_private(const Json::Value & raw_json,const SbotEventMessagePrivate & base)
{
	Json::Value root;
	root["action"] = "send_private_msg";
	Json::Value params;
	params["user_id"] = base.user_id;
	params["message"] = base.message;
	root["params"] = params;
	SBotCore::send_ws(Json::FastWriter().write(root));
	return 0;
}
```

## 如何编译
### 编译MiraiCQ
我使用 VS2008 x86 + boost_1_55_0 ,除此之外无需自己安装其他依赖。<br />
如果你不想亲自编译boost,这里有编译好的版本
[https://boost.teeks99.com/](https://boost.teeks99.com/) <br />
### 编译插件
用自己喜欢的visual studio打开PicHP示例插件即可编译。<br/>
当然，你也可以你也可以选择以前酷Q的sdk来写插件(不推荐)。

## 如何运行
1:首先，需要启动一个实现了 OneBot 标准的框架，例如 [go-cqhttp](https://github.com/Mrs4s/go-cqhttp) <br />
2:在Onebot实现中开启 正向 websocket <br />
3:运行MiraiCQ，将xxx.dll与xxx.json放入自动生成的app文件夹中即可加载。

## 快速体验
此处提供一个快速体验(预览)版本，如果你不想编译MiraiCQ，可以直接从这里下载二进制文件。但是不保证能及时更新: <br />
[fast_try_0_10.zip](https://super1207.lanzoui.com/ig9Unsl2j6b) <br />

注：PicHP是一个私聊复读机的插件示例

## 聊聊其它
的确，这个项目看起来很简陋，实际上也的确如此。项目最开始的目的是复用酷Q插件。但是，我逐渐意识到很多问题:
* 因为不能解包cpk，并且酷Q论坛已经关闭，所以，实际上现存的酷Q插件并不多。
* 酷Q已经停止运行，所以很多运行细节已经很难搞清楚，实现起来难度很大。
* 很多机器人框架出现，带来了很多以前酷Q不具备的好玩的功能，所以，局限在酷Q上很不妥。

为了从一定程度上解决这些问题，我另做了一套插件机制，来完成对Onebot原始功能的调用，但是，这同样问题很多:
* 目前只是简单的在插件中收发网络数据，在每个插件中，都需要构造，解析网络数据，这并不是什么难事，通过SDK可轻易完成，但是，这会带来一些性能损耗。将来，我可能会改善这一情况，但目前我打算先咕着。
* 目前，并没有易语言的SDK，原因很简单，我不会。
* MiraiCQ内部使用多线程(线程池)方式工作，插件多了后资源消耗较大，并且操作系统对一个进程的线程数也是有限制的。我曾想过制作一套异步的插件机制，但是，这对C语言这类语言来说，并不合适。
* 我曾想过做一套类似mirai console的那种指令系统，解决插件之间的依赖问题,目前打算先咕着。

对于界面简陋的问题，这的确是一个值得吐槽的地方。最开始，我并没有制作界面的打算，现在还可以在代码中看到最开始的控制台版本。我想将MiraiCQ通过wine跑在没有界面的linux服务器上，所以，界面是多余的东西。在代码里，界面和功能是分离的，重构界面并不是难事，但是，目前打算咕着。<br />
如果支持其它通讯方式，例如反向websocket，这会使一个MiraiCQ可以连接多个Onebot，实现多个QQ。但是，原酷Q插件中，并没有可以区分不同QQ号的方法。所以，MiraiCQ现在无法支持，将来应该也不会支持。<br />
对于实时日志，MiraiCQ本身不会对此提供支持，在MiraiCQ中，日志的作用定位成辅助分析BUG，在程序闪退时，实时日志起不到任何作用。并且，完全可以由插件来提供实时日志(将来会为插件提供相应接口，目前先咕着)。
## 问题反馈
随意反馈，欢迎交流，通过 github issue 或通过 QQ群(920220179) 均可

## 参考与依赖
感谢他/她们！<br />
[https://github.com/Mrs4s/go-cqhttp](https://github.com/Mrs4s/go-cqhttp) (运行依赖)  <br />
[https://github.com/mamoe/mirai](https://github.com/mamoe/mirai) (起名依赖)  <br />
[https://github.com/howmanybots/onebot](https://github.com/howmanybots/onebot) (教材) <br />
[https://github.com/howmanybots/cqcppsdk](https://github.com/howmanybots/cqcppsdk) (少量源码依赖)  <br />
[https://github.com/zaphoyd/websocketpp](https://github.com/zaphoyd/websocketpp) (抄作业)  <br />
[https://github.com/open-source-parsers/jsoncpp](https://github.com/open-source-parsers/jsoncpp) (抄作业)  <br />
[https://github.com/iTXTech/mirai-native](https://github.com/iTXTech/mirai-native) (相似项目参考)  <br />
[https://www.boost.org/](https://www.boost.org/) (编译依赖) <br />
[https://github.com/richardchien/coolq-http-api](https://github.com/richardchien/coolq-http-api) (插件测试)

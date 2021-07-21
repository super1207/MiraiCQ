# MiraiCQ
[![QQ 群](https://img.shields.io/badge/qq%E7%BE%A4-920220179-orange.svg)]()
![](https://img.shields.io/badge/OneBot-v11-black?logo=data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAHAAAABwCAMAAADxPgR5AAAAGXRFWHRTb2Z0d2FyZQBBZG9iZSBJbWFnZVJlYWR5ccllPAAAAAxQTFRF////29vbr6+vAAAAk1hCcwAAAAR0Uk5T////AEAqqfQAAAKcSURBVHja7NrbctswDATQXfD//zlpO7FlmwAWIOnOtNaTM5JwDMa8E+PNFz7g3waJ24fviyDPgfhz8fHP39cBcBL9KoJbQUxjA2iYqHL3FAnvzhL4GtVNUcoSZe6eSHizBcK5LL7dBr2AUZlev1ARRHCljzRALIEog6H3U6bCIyqIZdAT0eBuJYaGiJaHSjmkYIZd+qSGWAQnIaz2OArVnX6vrItQvbhZJtVGB5qX9wKqCMkb9W7aexfCO/rwQRBzsDIsYx4AOz0nhAtWu7bqkEQBO0Pr+Ftjt5fFCUEbm0Sbgdu8WSgJ5NgH2iu46R/o1UcBXJsFusWF/QUaz3RwJMEgngfaGGdSxJkE/Yg4lOBryBiMwvAhZrVMUUvwqU7F05b5WLaUIN4M4hRocQQRnEedgsn7TZB3UCpRrIJwQfqvGwsg18EnI2uSVNC8t+0QmMXogvbPg/xk+Mnw/6kW/rraUlvqgmFreAA09xW5t0AFlHrQZ3CsgvZm0FbHNKyBmheBKIF2cCA8A600aHPmFtRB1XvMsJAiza7LpPog0UJwccKdzw8rdf8MyN2ePYF896LC5hTzdZqxb6VNXInaupARLDNBWgI8spq4T0Qb5H4vWfPmHo8OyB1ito+AysNNz0oglj1U955sjUN9d41LnrX2D/u7eRwxyOaOpfyevCWbTgDEoilsOnu7zsKhjRCsnD/QzhdkYLBLXjiK4f3UWmcx2M7PO21CKVTH84638NTplt6JIQH0ZwCNuiWAfvuLhdrcOYPVO9eW3A67l7hZtgaY9GZo9AFc6cryjoeFBIWeU+npnk/nLE0OxCHL1eQsc1IciehjpJv5mqCsjeopaH6r15/MrxNnVhu7tmcslay2gO2Z1QfcfX0JMACG41/u0RrI9QAAAABJRU5ErkJggg==)
## 这是什么
MiraiCQ 的目的是实现 [onebot](https://github.com/howmanybots/onebot) 标准向原生酷Q标准（[miraicq标准](https://github.com/super1207/MiraiCQ/blob/main/doc/miraicq_std.md)）的转化，使得原来酷Q的插件得以直接运行。<br />
更重要的是可以使用c++或者易语言来写复读机了 <br />
onebot_websocket_api        <------->    MiraiCQ        <------->    cq_native_dll <br />
MiraiCQ 使用websocket正向连接，与实现了onebot标准的机器人框架对接。
![](https://ftp.bmp.ovh/imgs/2020/10/bc4e2057663fb481.png)

## 如何编译
我使用 VS2008 x86 + boost_1_55_0 ,除此之外无需自己安装其他依赖。<br />
如果你不想亲自编译boost,这里有编译好的版本
[https://boost.teeks99.com/](https://boost.teeks99.com/) <br />
clone本项目，双击test.sln文件，点击生成解决方案即可完成编译。
## 如何运行
首先，需要启动一个实现了 OneBot 标准的框架，例如 [go-cqhttp](https://github.com/Mrs4s/go-cqhttp) <br />
开启 正向 websocket 接口，端口号默认为 6700 <br />
运行上一步骤生成的exe文件，将酷Q的xxx.dll与xxx.json放入自动生成的app文件夹中即可加载 <br />
[json生成工具](https://github.com/super1207/MiraiCQ_Json_Gen)已经可以用啦，快速生成符合MiraiCQ标准的Json文件！
## 快速体验
此处提供一个快速体验(预览)版本，但是不保证能及时更新: <br />
[fast_try_0_6.7z](https://super1207.lanzoui.com/i8bSero37md) <br /><br />
PicHP是一个私聊复读机的插件示例
## 当前缺陷
<1> 目前并没有完整实现了 OneBot 标准的框架 <br />
<2> 目前酷Q已经无法运作了，缺乏对比验证，很多细节全靠记忆 <br />
<3> 到处都是Bug...不过群/私聊复读机终于可以复活了！ <br />
## 问题回馈
随意反馈，[有空]就改，建议自己改，然后发pr
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

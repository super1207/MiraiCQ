# 目前已知的与原酷Q的差异
* MiraiCQ 插件并无优先级概念，与此同时，一个插件也不可以拦截将要传给其他插件的信息。这将会导致原酷Q中用于拦截消息的插件受影响。
* ~~MiraiCQ发送本地图片时，采用[file URI](https://datatracker.ietf.org/doc/html/rfc8089)，而不是像原酷Q那样先将图片放入image文件夹，然后用文件名发送。这将会导致原酷Q中发送本地图片功能受影响。~~ 注：已经兼容
* ~~MiraiCQ中的`CQ_get_image` API，在对接标准OneBot实现的时候，返回的是OneBot实现端的本地图片路径，在与go-cqhttp对接时，将调用出错(go-cqhttp的这个API不符合Onebot标准)。~~ 注：已经兼容
* 同样，收发语音也会存在上述两个问题。
* 原酷Q中，某些插件会复用酷Q的bin文件夹中的一些dll库(比如richardchien的coolq-http-api会使用sqlite.dll)，但是在MiraiCQ中，不存在这些dll。所以在使用插件时，应该手动放入这些dll。
* 某些插件依赖的酷Q中API调用出错时的返回值，但是因为酷Q的API返回值本身就是没有明确定义的，所以MiraiCQ无法与其保持一致。
* 因为各OneBot实现对CQ码进行了一些拓展，所以，插件实际接收到的CQ码可能会与原酷Q有所增改。
* MiraiCQ中，无法获取`cookie`，`csrf token`等信息，也无法使用点赞功能。依赖这些功能的插件可能会受影响。
* MiraiCQ中，因为不使用额外的数据库，所以`CQ_deleteMsg`只能撤回有限时间(目前和消息频率、插件数量有关)内的消息。极端情况下，这类带消息撤回功能的插件会受影响。

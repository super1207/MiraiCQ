// PicHP.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#pragma comment(lib, "CQP.lib")

static __int32 sg_authCode = 0;

extern "C" __int32 __stdcall CQ_sendPrivateMsg(__int32 auth_code, __int64 qq, const char *msg);

/* 接收authCode */
extern "C" __int32 __stdcall Initialize(__int32 authCode)
{
	sg_authCode = authCode;
	return 0;
}

/* 私聊事件 */
extern "C" __int32 __stdcall event_private_message(__int32 sub_type, __int32 msg_id, __int64 from_qq, const char *msg, __int32 font)
{
	CQ_sendPrivateMsg(sg_authCode,from_qq,msg);
	return 0;
}

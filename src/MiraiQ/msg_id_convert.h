#ifndef __MSG_ID_CONVERT_H__LGOOODOYHORUBXTX
#define __MSG_ID_CONVERT_H__LGOOODOYHORUBXTX

//用于转化msgId为正整数

class MsgIdConvert
{
public:
	 __int32 to_cq(__int32 web_msg_id);

	 __int32 to_web(__int32 cq_msg_id);

	 static MsgIdConvert * getInstance();

private:
	MsgIdConvert();
	 
};


#endif
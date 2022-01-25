#pragma once

#ifdef  __cplusplus
extern "C" {
#endif

/* 所有const char * 均不会，也不能为NULL，否则行为未定义 */

/* 
这个函数必须是第一个调用的IPC函数，
mainprocess中 ，uuid传`""`, 
plusproceess中，uuid传plus_uuid
成功返回0，失败返回非0
*/
int IPC_Init(const char* uuid);


/* 调用其它进程的api，tm为超时，单位为ms */
const char* IPC_ApiSend(const char* remote_uuid, const char* msg,int tm);

/* mainproess发送事件 */
void IPC_SendEvent(const char* msg);

/* plusprocess接收事件，此函数会阻塞到接收事件为止 */
const char* IPC_GetEvent(const char* flag);

/* 接收api调用,此函数会阻塞到接收api调用为止为止 */
void IPC_ApiRecv(void((*fun)(const char* sender_uuid, const char* flag, const char* msg)));

/* 用于接收api调用后的返回 */
void IPC_ApiReply(const char* sender_uuid, const char* flag, const char* msg);

/* 用于mainprocess获取main_uuid，然后传给子进程*/
const char* IPC_GetFlag();

#ifdef  __cplusplus
}
#endif


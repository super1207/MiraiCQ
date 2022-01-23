#pragma once

/*
 * 描述：调用Api
 * 参数`to_pid`:：目标进程号
 * 参数`msg`：要发送的数据
 * 返回值：返回调用结果(C语言字符串，无需自己释放)，若超时则返回`""`
*/
const char* IPC_ApiSend(int to_pid, const char* msg);

/*
 * 描述：广播事件
 * 参数`msg`：要发送的数据
*/
void IPC_SendEvent(const char* msg);

/*
 * 描述：接收事件
 * 参数`pid`:：事件广播着的进程号
 * 返回值：返回事件结果(C语言非空字符串，无需自己释放)，若无事件则返回`""`
*/
const char* IPC_GetEvent(int pid);

/*
 * 描述：接收api调用
 * 参数`fun`:：有api调用时的回调函数，参数为api调用者发送的msg,返回值为要返回的字符串，使用IPC_Malloc来分配内存
*/
void IPC_ApiRecv(char* ((*fun)(const char*)));


/*
 * 描述：分配内存，与`IPC_ApiRecv`配合使用
 * 参数`sz`:：要分配的内存大小，同malloc
*/
void* IPC_Malloc(size_t sz);
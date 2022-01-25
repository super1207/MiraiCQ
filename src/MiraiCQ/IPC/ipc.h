#pragma once

const char* IPC_ApiSend(const char* remote, const char* msg,int tm);

void IPC_SendEvent(const char* msg);

const char* IPC_GetEvent(const char* flag);

void IPC_ApiRecv(void((*fun)(const char*, const char*, const char*)));

void IPC_ApiReply(const char* sender, const char* flag, const char* msg);

int IPC_Init(const char* uuid);

const char* IPC_GetFlag();
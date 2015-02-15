/**
 * @file    ipc_unix.h
 * @brief   ipc client and server using STREAM based unix socket
 * @author  Ku Bong Min
 * @date    Fri Jan 11 10:04:03 KST 2008
 *
 *  Header file which implement IPC(Inter Process Call) client based on Unix Socket with STEAM socket.\n
 *  $Id:\n
 *  $HeadURL:\n
 */


#ifndef __IPC_UNIX_H__
#define __IPC_UNIX_H__
#ifdef __cplusplus
extern "C" {
#endif

#define MAXPENDING 10
#define MAX_IPC_PATH 108
#define RPC_USE_SHM 1

#if RPC_USE_SHM
#define RPC_SMEM_KEY 9079
#define RPC_SMEM_SIZE 512*1024
#endif


int IPC_Connect(char *ipcpath, int timeOut);

int IPC_Close(int sock);

int IPC_Send(int sock, void* buf, int len, int timeOut);

int IPC_Recv(int sock, void* buf, int len, int timeOut);

#if RPC_USE_SHM
int IPC_MemPut(const void *src, int n);

int IPC_MemGet(const void *dst, int n);

int IPC_MemGetPtr(const void **pDst);
#endif

int IPC_RunServer(char *ipcpath, int (*clientHander)(int sock));

int IPC_WaitServer(void);

#ifdef __cplusplus
}
#endif
#endif /* __IPC_UNIX_H__ */

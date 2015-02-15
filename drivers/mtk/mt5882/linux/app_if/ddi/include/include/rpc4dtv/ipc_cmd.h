/**
 * @file    ipc_cmd.h
 * @brief   definition of ipc control commands 
 * @author  Ku Bong Min
 * @date    Mon Jan 14 10:50:18 KST 2008
 *
 *  Header file which defines IPC(Inter Process Call) commands betweedn cilent and servert.\n
 *  $Id:\n
 *  $HeadURL:\n
 */

#ifndef __IPC_CMD_H__
#define __IPC_CMD_H___
#ifdef __cplusplus
extern "C" {
#endif
#ifndef _EMUL_WIN
#include <unistd.h>
#else
typedef int pid_t;
#endif

enum RootCmdCode
{
	CMD_C2S_DDI_GPU = 0,
	CMD_C2S_API,
	CMD_S2C_EVENT
};

struct RootCmd
{
	enum RootCmdCode cmd;
	pid_t cpid; //process ID of client
};

#define MAX_MESSAGE_SIZE 4096 

#ifdef __cplusplus
}
#endif
#endif /* __IPC_CMD_H__ */

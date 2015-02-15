#ifdef CC_MTK_LOADER
#ifdef CC_CUSTOMER_CLI
#include "x_drv_cli.h"
#include "x_typedef.h"
#include "loader_if.h"
static INT32 _CmdForceUpgrade(INT32 i4Argc, const CHAR** aszArgv);
const CLI_EXEC_T _arCustomerLoaderCmdTbl[] CLI_MAIN_COMMAND =
{
    {
        "force_upgrade",
        "fu",
        _CmdForceUpgrade,
        NULL,
        "force to upgrade firmware",
		#if defined(CC_CONSOLE_LOCK)
		CLI_SUPERVISOR
		#else
        CLI_GUEST
        #endif
    },
};
static INT32 _CmdForceUpgrade(INT32 i4Argc, const CHAR** aszArgv)
{ 
   LDR_ForceUpgrade(LDR_GetLdrData());
}
#endif
#endif

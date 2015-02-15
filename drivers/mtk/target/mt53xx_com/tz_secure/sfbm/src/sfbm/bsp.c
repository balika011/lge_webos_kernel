#include"drvcust_if.h"

UINT32 BSP_GetFbmMemAddr()
{
    return (DRVCUST_OptGet(eTotalMemSize) - DRVCUST_OptGet(eFbmMemSize));
}

#ifndef _MMU_H
#define _MMU_H

#include "arm/sysregs.h"

#define MM_TYPE_PAGE_TABLE 0x3
#define MM_TYPE_PAGE 0x3
#define MM_TYPE_BLOCK 0x1
#define MM_ACCESS (0x1 << 10)
#define MM_ACCESS_PERMISSION (0x01 << 6)

#define MMU_FLAGS (MM_TYPE_BLOCK | (MT_NORMAL_NC << 2) | MM_ACCESS)
#define MMU_DEVICE_FLAGS (MM_TYPE_BLOCK | (MT_DEVICE_nGnRnE << 2) | MM_ACCESS)
#define MMU_PTE_FLAGS (MM_TYPE_PAGE | (MT_NORMAL_NC << 2) | MM_ACCESS | MM_ACCESS_PERMISSION)

#endif /* _MMU_H */

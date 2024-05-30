#ifndef __EDP_ADAPTER_H__
#define __EDP_ADAPTER_H__

#include "edp_core.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

void EdpInit(struct EdpCntlr *cntlr);
void EdpDeinit(struct EdpCntlr *cntlr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif //__EDP_ADAPTER_H__
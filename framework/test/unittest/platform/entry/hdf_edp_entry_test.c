/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_edp_entry_test.h"
#include "edp_test.h"
#include "hdf_log.h"

#define HDF_LOG_TAG hdf_edp_entry_test

int32_t HdfEdpTestEntry(HdfTestMsg *msg)
{
    if (msg == NULL) {
        HDF_LOGE("%s: msg is null!", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    struct EdpTest *test = EdpTestServiceGet();
    if (test == NULL) {
        HDF_LOGE("%s: test object is null\n", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    
    msg->result = test->doTest(test, msg->subCmd);

    return HDF_SUCCESS;
}

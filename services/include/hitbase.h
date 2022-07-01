#pragma once

#include "hilog/log.h"

#undef LOG_DOMAIN
#undef LOG_TAG
#define LOG_DOMAIN 0xdeadbeef
#define LOG_TAG "HITCamera"

#ifndef __FILENAME__
#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#ifndef DECORATOR_HILOG
#define DECORATOR_HILOG(op, fmt, args...)                                                                        \
    do {                                                                                                         \
        op(LOG_CORE, "{%{public}s()-%{public}s:%{public}d} " fmt, __FUNCTION__, __FILENAME__, __LINE__, ##args); \
    } while (0)
#endif

#ifndef LOGD
#define LOGD(fmt, ...) DECORATOR_HILOG(HILOG_DEBUG, fmt, ##__VA_ARGS__)
#endif
#ifndef LOGE
#define LOGE(fmt, ...) DECORATOR_HILOG(HILOG_ERROR, fmt, ##__VA_ARGS__)
#endif
#ifndef LOGI
#define LOGI(fmt, ...) DECORATOR_HILOG(HILOG_INFO, fmt, ##__VA_ARGS__)
#endif

/*

inline OHOS::HiviewDFX::HiLogLabel labal = {
        .type = LOG_CORE,
        .domain = 0xdeadbeef,
        .tag = "HITCamera"
};

#define LOGD(msg, ...) OHOS::HiviewDFX::HiLog::Debug(labal, "[%{public}s] [line %{public}d]: " msg, __FILE__, __LINE__, ##__VA_ARGS__)
#define LOGI(msg, ...) OHOS::HiviewDFX::HiLog::Info(labal, "[%{public}s] [line %{public}d]: " msg, __FILE__, __LINE__, ##__VA_ARGS__)
#define LOGE(msg, ...) OHOS::HiviewDFX::HiLog::Error(labal, "[%{public}s] [line %{public}d]: " msg, __FILE__, __LINE__, ##__VA_ARGS__)

*/

enum Codes {
    ACTION_CAPTURE,

    FAILED_OCCUPIED = 1000,
    FAILED_OPEN_DEVICE,
    FAILED_QUERY_CAPABILITIES,
    FAILED_SET_FORMAT,
    FAILED_REQUEST_BUFFER,
    FAILED_QUERY_BUFFER,
    FAILED_QUEUE_BUFFER,
    FAILED_START_STREAM,
    FAILED_WAIT_FRAME,
    FAILED_RETRIEVE_FRAME,
    FAILED_STREAM_OFF,
    FAILED_MAP
};

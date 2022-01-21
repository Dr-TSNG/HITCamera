#pragma once

#include "hilog/log.h"

inline OHOS::HiviewDFX::HiLogLabel labal = {
        .type = LOG_CORE,
        .domain = 0xdeadbeef,
        .tag = "HITCamera"
};

#define LOGD(msg, ...) OHOS::HiviewDFX::HiLog::Debug(labal, "[%s] [line %d]: " msg, __FILE__, __LINE__, ##__VA_ARGS__)
#define LOGI(msg, ...) OHOS::HiviewDFX::HiLog::Info(labal, "[%s] [line %d]: " msg, __FILE__, __LINE__, ##__VA_ARGS__)
#define LOGE(msg, ...) OHOS::HiviewDFX::HiLog::Error(labal, "[%s] [line %d]: " msg, __FILE__, __LINE__, ##__VA_ARGS__)

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

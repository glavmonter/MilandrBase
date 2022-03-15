#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "compiler.h"
#include "mdr_log.h"
#include "mdr_log_private.h"


// Maximum time to wait for the mutex in a logging statement.
//
// We don't expect this to happen in most cases, as contention is low. The most likely case is if a
// log function is called from an ISR (technically caller should use the ISR-friendly logging macros but
// possible they use the normal one instead and disable the log type by tag).
#define MAX_MUTEX_WAIT_MS 10
#define MAX_MUTEX_WAIT_TICKS ((MAX_MUTEX_WAIT_MS + portTICK_PERIOD_MS - 1) / portTICK_PERIOD_MS)

static SemaphoreHandle_t s_log_mutex = NULL;
#if (configSUPPORT_STATIC_ALLOCATION == 1)
static StaticSemaphore_t s_log_mutex_static;
#endif

void mdr_log_impl_lock()
{
    if (unlikely(!s_log_mutex)) {
#if (configSUPPORT_STATIC_ALLOCATION == 1)
        s_log_mutex = xSemaphoreCreateMutexStatic(&s_log_mutex_static);
#else
        s_log_mutex = xSemaphoreCreateMutex();
#endif
    }
    if (unlikely(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)) {
        return;
    }
    xSemaphoreTake(s_log_mutex, portMAX_DELAY);
}

bool mdr_log_impl_lock_timeout()
{
    if (unlikely(!s_log_mutex)) {
#if (configSUPPORT_STATIC_ALLOCATION == 1)
        s_log_mutex = xSemaphoreCreateMutexStatic(&s_log_mutex_static);
#else
        s_log_mutex = xSemaphoreCreateMutex();
#endif
    }
    if (unlikely(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)) {
        return true;
    }
    if (xPortIsInsideInterrupt())
        return pdTRUE;
    return xSemaphoreTake(s_log_mutex, MAX_MUTEX_WAIT_TICKS) == pdTRUE;
}

void mdr_log_impl_unlock()
{
    if (unlikely(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)) {
        return;
    }
    if (xPortIsInsideInterrupt())
        return;
    xSemaphoreGive(s_log_mutex);
}

char *mdr_log_system_timestamp()
{
    static char buffer[18] = {0};
//    static _lock_t bufferLock = 0;

    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        uint32_t timestamp = mdr_log_early_timestamp();
        for (uint8_t i = 0; i < sizeof(buffer); i++) {
            if ((timestamp > 0) || (i == 0)) {
                for (uint8_t j = sizeof(buffer) - 1; j > 0; j--) {
                    buffer[j] = buffer[j - 1];
                }
                buffer[0] = (char)(timestamp % 10) + '0';
                timestamp /= 10;
            } else {
                buffer[i] = 0;
                break;
            }
        }
        return buffer;
    } else {
        struct timeval tv;
        struct tm timeinfo;

        gettimeofday(&tv, NULL);
        localtime_r(&tv.tv_sec, &timeinfo);

//        _lock_acquire(&bufferLock); // TODO _lock_acquire
        snprintf(buffer, sizeof(buffer),
                 "%02d:%02d:%02d.%03ld",
                 timeinfo.tm_hour,
                 timeinfo.tm_min,
                 timeinfo.tm_sec,
                 tv.tv_usec / 1000);
//        _lock_release(&bufferLock); // TODO _lock_release

        return buffer;
    }
}

uint32_t mdr_log_timestamp(void)
{
    if (unlikely(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)) {
        return mdr_log_early_timestamp();
    }
    TickType_t tick_count = (xPortIsInsideInterrupt() == pdTRUE) ? xTaskGetTickCountFromISR() : xTaskGetTickCount();
    return tick_count * (1000 / configTICK_RATE_HZ);
}

uint32_t mdr_log_early_timestamp(void)
{
    uint64_t cnt = (uint64_t)DWT->CYCCNT * 1000;
    return cnt / (uint64_t)SystemCoreClock;
}

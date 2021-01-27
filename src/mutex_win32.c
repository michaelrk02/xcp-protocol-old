#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4005)
#endif
#include <windows.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "internal.h"

struct _xcp_mutex_t {
    CRITICAL_SECTION cs;
};

xcp_error_t xcp_mutex_create(xcp_mutex_t *mutex) {
    xcp_error_t err;
    xcp_mutex_t mutex_obj;

    err = xcp_malloc(sizeof(struct _xcp_mutex_t), (void **)&mutex_obj);
    if (XCP_FAILED(err)) {
        return err;
    }

    InitializeCriticalSection(&mutex_obj->cs);
    *mutex = mutex_obj;

    return XCP_S_OK;
}

xcp_error_t xcp_mutex_lock(xcp_mutex_t mutex) {
    if (mutex == NULL) {
        return XCP_E_HANDLE;
    }

    EnterCriticalSection(&mutex->cs);

    return XCP_S_OK;
}

xcp_error_t xcp_mutex_unlock(xcp_mutex_t mutex) {
    if (mutex == NULL) {
        return XCP_E_HANDLE;
    }

    LeaveCriticalSection(&mutex->cs);

    return XCP_S_OK;
}

xcp_error_t xcp_mutex_destroy(xcp_mutex_t mutex) {
    if (mutex != NULL) {
        DeleteCriticalSection(&mutex->cs);
        xcp_free(mutex);
    }
    return XCP_S_OK;
}

#endif

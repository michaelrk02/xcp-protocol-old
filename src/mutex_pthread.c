#ifndef _WIN32 /* assume POSIX */

#include <pthread.h>

#include "internal.h"

struct _xcp_mutex_t {
    pthread_mutex_t m;
};

xcp_error_t xcp_mutex_create(xcp_mutex_t *mutex) {
    xcp_error_t err;
    xcp_mutex_t mutex_obj;

    err = xcp_malloc(sizeof(struct _xcp_mutex_t), (void **)&mutex_obj);
    if (XCP_FAILED(err)) {
        return err;
    }

    pthread_mutex_init(&mutex_obj->m, NULL);
    *mutex = mutex_obj;

    return XCP_S_OK;
}

xcp_error_t xcp_mutex_lock(xcp_mutex_t mutex) {
    if (mutex == NULL) {
        return XCP_E_HANDLE;
    }

    pthread_mutex_lock(&mutex->m);

    return XCP_S_OK;
}

xcp_error_t xcp_mutex_unlock(xcp_mutex_t mutex) {
    if (mutex == NULL) {
        return XCP_E_HANDLE;
    }

    pthread_mutex_unlock(&mutex->m);

    return XCP_S_OK;
}

xcp_error_t xcp_mutex_destroy(xcp_mutex_t mutex) {
    if (mutex != NULL) {
        pthread_mutex_destroy(&mutex->m);
        xcp_free(mutex);
    }
    return XCP_S_OK;
}

#endif

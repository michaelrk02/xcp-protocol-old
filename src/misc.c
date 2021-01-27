#include <stdlib.h>
#include <string.h>

#include "internal.h"

xcp_error_t xcp_malloc(size_t size, void **memory) {
    void *memory_obj;

    if (size == 0) {
        return XCP_E_INVALIDARG;
    }
    if (memory == NULL) {
        return XCP_E_POINTER;
    }

    memory_obj = malloc(size);
    if (memory_obj == NULL) {
        return XCP_E_OUTOFMEMORY;
    }
    memset(memory_obj, 0, size);

    *memory = memory_obj;
    return XCP_S_OK;
}

xcp_error_t xcp_free(void *memory) {
    if (memory != NULL) {
        free(memory);
    }
    return XCP_S_OK;
}

xcp_error_t xcp_str_copy(const char *src, char **dst) {
    xcp_error_t err;
    size_t src_len = strlen(src);
    char *dst_str;

    err = xcp_malloc(src_len + 1, (void **)&dst_str);
    if (XCP_FAILED(err)) {
        return err;
    }
    strcpy(dst_str, src);

    *dst = dst_str;
    return XCP_S_OK;
}

xcp_error_t xcp_str_concat(const char *left, const char *right, char **result) {
    xcp_error_t err;
    size_t left_len = strlen(left);
    size_t right_len = strlen(right);
    size_t result_len = left_len + right_len;
    char *result_str;

    err = xcp_malloc(result_len + 1, (void **)&result_str);
    if (XCP_FAILED(err)) {
        return err;
    }
    strcpy(result_str, left);
    strcat(result_str, right);

    *result = result_str;
    return XCP_S_OK;
}


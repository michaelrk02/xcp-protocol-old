#include <stdio.h>
#include <string.h>

#include "internal.h"

xcp_dispatch_type_t xcp_dispatch_get_type(xcp_dispatch_t dispatch) {
    if (dispatch != NULL) {
        return dispatch->type;
    }
    return xcp_dispatch_type_invalid;
}

xcp_error_t xcp_dispatch_get_error(xcp_dispatch_t dispatch) {
    xcp_error_t err = XCP_E_UNEXPECTED;

    xcp_dispatch_read_scalar(dispatch, &err, sizeof(xcp_error_t));
    return err;
}

xcp_error_t xcp_dispatch_get_bool(xcp_dispatch_t dispatch, xcp_bool_t *value) {
    xcp_error_t err;

    err = xcp_dispatch_read_scalar(dispatch, value, 1);
    if (XCP_FAILED(err)) {
        return err;
    }
    *value = *value != 0;

    return XCP_S_OK;
}

xcp_error_t xcp_dispatch_get_char(xcp_dispatch_t dispatch, char *value) {
    return xcp_dispatch_read_scalar(dispatch, value, sizeof(char));
}

xcp_error_t xcp_dispatch_get_byte(xcp_dispatch_t dispatch, unsigned char *value) {
    return xcp_dispatch_read_scalar(dispatch, value, sizeof(unsigned char));
}

xcp_error_t xcp_dispatch_get_short(xcp_dispatch_t dispatch, short *value) {
    return xcp_dispatch_read_scalar(dispatch, value, sizeof(short));
}

xcp_error_t xcp_dispatch_get_word(xcp_dispatch_t dispatch, unsigned short *value) {
    return xcp_dispatch_read_scalar(dispatch, value, sizeof(unsigned short));
}

xcp_error_t xcp_dispatch_get_int(xcp_dispatch_t dispatch, int *value) {
    return xcp_dispatch_read_scalar(dispatch, value, sizeof(int));
}

xcp_error_t xcp_dispatch_get_dword(xcp_dispatch_t dispatch, unsigned int *value) {
    return xcp_dispatch_read_scalar(dispatch, value, sizeof(unsigned int));
}

xcp_error_t xcp_dispatch_get_long(xcp_dispatch_t dispatch, long long *value) {
    return xcp_dispatch_read_scalar(dispatch, value, sizeof(long long));
}

xcp_error_t xcp_dispatch_get_qword(xcp_dispatch_t dispatch, unsigned long long *value) {
    return xcp_dispatch_read_scalar(dispatch, value, sizeof(unsigned long long));
}

xcp_error_t xcp_dispatch_get_float(xcp_dispatch_t dispatch, float *value) {
    return xcp_dispatch_read_scalar(dispatch, value, sizeof(float));
}

xcp_error_t xcp_dispatch_get_double(xcp_dispatch_t dispatch, double *value) {
    return xcp_dispatch_read_scalar(dispatch, value, sizeof(double));
}

xcp_error_t xcp_dispatch_get_string(xcp_dispatch_t dispatch, char **value) {
    xcp_error_t err;
    unsigned int len;
    char *value_str;

    err = xcp_dispatch_get_dword(dispatch, &len);
    if (XCP_FAILED(err)) {
        return err;
    }

    if (value == NULL) {
        return XCP_E_POINTER;
    }

    err = xcp_malloc((len + 1) * sizeof(char), (void **)&value_str);
    if (XCP_FAILED(err)) {
        return err;
    }
    if (fread(value_str, sizeof(char), len, dispatch->fp) != len) {
        return XCP_E_FAIL;
    }

    *value = value_str;

    return XCP_S_OK;
}

xcp_error_t xcp_dispatch_put_error(xcp_dispatch_t dispatch, xcp_error_t value) {
    return xcp_dispatch_write_scalar(dispatch, &value, sizeof(xcp_error_t));
}

xcp_error_t xcp_dispatch_put_bool(xcp_dispatch_t dispatch, xcp_bool_t value) {
    value = value != 0;
    return xcp_dispatch_write_scalar(dispatch, &value, 1);
}

xcp_error_t xcp_dispatch_put_char(xcp_dispatch_t dispatch, char value) {
    return xcp_dispatch_write_scalar(dispatch, &value, sizeof(char));
}

xcp_error_t xcp_dispatch_put_byte(xcp_dispatch_t dispatch, unsigned char value) {
    return xcp_dispatch_write_scalar(dispatch, &value, sizeof(unsigned char));
}

xcp_error_t xcp_dispatch_put_short(xcp_dispatch_t dispatch, short value) {
    return xcp_dispatch_write_scalar(dispatch, &value, sizeof(short));
}

xcp_error_t xcp_dispatch_put_word(xcp_dispatch_t dispatch, unsigned short value) {
    return xcp_dispatch_write_scalar(dispatch, &value, sizeof(unsigned short));
}

xcp_error_t xcp_dispatch_put_int(xcp_dispatch_t dispatch, int value) {
    return xcp_dispatch_write_scalar(dispatch, &value, sizeof(int));
}

xcp_error_t xcp_dispatch_put_dword(xcp_dispatch_t dispatch, unsigned int value) {
    return xcp_dispatch_write_scalar(dispatch, &value, sizeof(unsigned int));
}

xcp_error_t xcp_dispatch_put_long(xcp_dispatch_t dispatch, long long value) {
    return xcp_dispatch_write_scalar(dispatch, &value, sizeof(long long));
}

xcp_error_t xcp_dispatch_put_qword(xcp_dispatch_t dispatch, unsigned long long value) {
    return xcp_dispatch_write_scalar(dispatch, &value, sizeof(unsigned long long));
}

xcp_error_t xcp_dispatch_put_float(xcp_dispatch_t dispatch, float value) {
    return xcp_dispatch_write_scalar(dispatch, &value, sizeof(float));
}

xcp_error_t xcp_dispatch_put_double(xcp_dispatch_t dispatch, double value) {
    return xcp_dispatch_write_scalar(dispatch, &value, sizeof(double));
}

xcp_error_t xcp_dispatch_put_string(xcp_dispatch_t dispatch, const char *value) {
    xcp_error_t err;
    unsigned int len;

    if (dispatch == NULL) {
        return XCP_E_HANDLE;
    }
    if (value == NULL) {
        return XCP_E_POINTER;
    }

    len = strlen(value);
    err = xcp_dispatch_put_dword(dispatch, len);
    if (XCP_FAILED(err)) {
        return err;
    }
    if (fwrite(value, sizeof(char), len, dispatch->fp) != len) {
        return XCP_E_FAIL;
    }

    return XCP_S_OK;
}

xcp_error_t xcp_dispatch_commit(xcp_dispatch_t dispatch) {
    xcp_error_t err;

    if (dispatch == NULL) {
        return XCP_E_HANDLE;
    }
    if (dispatch->fp == NULL) {
        return XCP_E_POINTER;
    }
    if (!((dispatch->type == xcp_dispatch_type_client_request) || (dispatch->type == xcp_dispatch_type_server_response))) {
        return XCP_E_ACCESSDENIED;
    }

    fflush(dispatch->fp);

    err = xcp_dispatch_end(dispatch, XCP_TRUE);
    if (XCP_FAILED(err)) {
        return err;
    }

    return XCP_S_OK;
}

xcp_error_t xcp_dispatch_close(xcp_dispatch_t dispatch) {
    xcp_error_t err;

    if (dispatch == NULL) {
        return XCP_E_HANDLE;
    }
    if (dispatch->fp == NULL) {
        return XCP_E_POINTER;
    }
    if (!((dispatch->type == xcp_dispatch_type_client_response) || (dispatch->type == xcp_dispatch_type_server_request))) {
        return XCP_E_ACCESSDENIED;
    }

    err = xcp_dispatch_end(dispatch, XCP_FALSE);
    if (XCP_FAILED(err)) {
        return err;
    }

    remove(dispatch->name);
    xcp_dispatch_unlock(dispatch);

    return XCP_S_OK;
}

xcp_error_t xcp_dispatch_wait(xcp_dispatch_t dispatch) {
    xcp_error_t err;
    unsigned int fourcc;
    unsigned long long call_id;

    while (XCP_FAILED(xcp_dispatch_ready(dispatch))) {
    }

    err = xcp_dispatch_begin(dispatch, XCP_TRUE);
    if (XCP_FAILED(err)) {
        return err;
    }

    xcp_dispatch_get_dword(dispatch, &fourcc);
    if (fourcc != XCP_FOURCC) {
        return XCP_E_INVALIDARG;
    }

    xcp_dispatch_get_qword(dispatch, &call_id);
    if (call_id != dispatch->wait_id) {
        return XCP_E_ACCESSDENIED;
    }

    return XCP_S_OK;
}

xcp_error_t xcp_dispatch_create(const char *name, xcp_dispatch_type_t type, xcp_dispatch_t *dispatch) {
    xcp_error_t err;
    xcp_dispatch_t dispatch_obj;

    if (type == xcp_dispatch_type_invalid) {
        return XCP_E_INVALIDARG;
    }
    if ((name == NULL) || (dispatch == NULL)) {
        return XCP_E_POINTER;
    }

    err = xcp_malloc(sizeof(struct _xcp_dispatch_t), (void **)&dispatch_obj);
    if (XCP_FAILED(err)) {
        return err;
    }
    dispatch_obj->type = type;

    err = xcp_str_copy(name, &dispatch_obj->name);
    err = XCP_SUCCEEDED(err) ? xcp_str_concat(name, ".lck", &dispatch_obj->lock_name) : err;

    if (XCP_FAILED(err)) {
        xcp_dispatch_destroy(dispatch_obj);
        return err;
    }

    *dispatch = dispatch_obj;
    return XCP_S_OK;
}

xcp_error_t xcp_dispatch_init(xcp_dispatch_t dispatch) {
    if (dispatch == NULL) {
        return XCP_E_HANDLE;
    }
    if ((dispatch->name == NULL) || (dispatch->lock_name == NULL)) {
        return XCP_E_POINTER;
    }

    remove(dispatch->name);
    remove(dispatch->lock_name);

    return XCP_S_OK;
}

xcp_error_t xcp_dispatch_read_scalar(xcp_dispatch_t dispatch, void *data, size_t size) {
    xcp_error_t err;
    FILE *fp;

    if (dispatch == NULL) {
        return XCP_E_HANDLE;
    }
    if (size == 0) {
        return XCP_E_INVALIDARG;
    }
    if (data == NULL) {
        return XCP_E_POINTER;
    }
    if (dispatch->fp == NULL) {
        return XCP_E_ACCESSDENIED;
    }
    if (fread(data, size, 1, dispatch->fp) != 1) {
        return XCP_E_FAIL;
    }

    return XCP_S_OK;
}

xcp_error_t xcp_dispatch_write_scalar(xcp_dispatch_t dispatch, const void *data, size_t size) {
    xcp_error_t err;

    if (dispatch == NULL) {
        return XCP_E_HANDLE;
    }
    if (size == 0) {
        return XCP_E_INVALIDARG;
    }
    if (data == NULL) {
        return XCP_E_POINTER;
    }
    if (dispatch->fp == NULL) {
        return XCP_E_ACCESSDENIED;
    }
    if (fwrite(data, size, 1, dispatch->fp) != 1) {
        return XCP_E_FAIL;
    }

    return XCP_S_OK;
}

xcp_error_t xcp_dispatch_begin(xcp_dispatch_t dispatch, xcp_bool_t lock) {
    xcp_error_t err;
    const char *mode = NULL;

    if (dispatch == NULL) {
        return XCP_E_HANDLE;
    }
    if (dispatch->fp != NULL) {
        return XCP_E_ABORT;
    }

    switch (dispatch->type) {
    case xcp_dispatch_type_client_response:
    case xcp_dispatch_type_server_request:
        mode = "rb";
        break;
    case xcp_dispatch_type_client_request:
    case xcp_dispatch_type_server_response:
        mode = "wb";
        break;
    }

    err = XCP_S_OK;
    if (lock) {
        err = xcp_dispatch_lock(dispatch);
        if (XCP_FAILED(err)) {
            return err;
        }
    }

    dispatch->fp = fopen(dispatch->name, mode);
    if (dispatch->fp == NULL) {
        return XCP_E_FAIL;
    }

    return XCP_S_OK;
}

xcp_error_t xcp_dispatch_end(xcp_dispatch_t dispatch, xcp_bool_t unlock) {
    if (dispatch == NULL) {
        return XCP_E_HANDLE;
    }
    if (dispatch->fp == NULL) {
        return XCP_E_POINTER;
    }
    fclose(dispatch->fp);
    dispatch->fp = NULL;

    if (unlock) {
        xcp_dispatch_unlock(dispatch);
    }

    return XCP_S_OK;
}

xcp_error_t xcp_dispatch_lock(xcp_dispatch_t dispatch) {
    FILE *lock_fp;
    xcp_bool_t is_used = XCP_FALSE;

    if (dispatch == NULL) {
        return XCP_E_HANDLE;
    }
    do {
        lock_fp = fopen(dispatch->lock_name, "rb");
        is_used = lock_fp != NULL;
        if (is_used) {
            fclose(lock_fp);
            lock_fp = NULL;
        }
    } while (is_used);

    lock_fp = fopen(dispatch->lock_name, "wb");
    if (lock_fp == NULL) {
        return XCP_E_FAIL;
    }
    fclose(lock_fp);

    return XCP_S_OK;
}

xcp_error_t xcp_dispatch_unlock(xcp_dispatch_t dispatch) {
    if (dispatch == NULL) {
        return XCP_E_HANDLE;
    }
    remove(dispatch->lock_name);
    return XCP_S_OK;
}

xcp_error_t xcp_dispatch_ready(xcp_dispatch_t dispatch) {
    FILE *fp;

    if (dispatch == NULL) {
        return XCP_E_HANDLE;
    }

    fp = fopen(dispatch->name, "rb");
    if (fp == NULL) {
        return XCP_E_FAIL;
    }
    fclose(fp);

    return XCP_S_OK;
}

xcp_error_t xcp_dispatch_destroy(xcp_dispatch_t dispatch) {
    if (dispatch != NULL) {
        if (dispatch->fp != NULL) {
            xcp_dispatch_end(dispatch, XCP_TRUE);
        }
        xcp_free(dispatch->name);
        xcp_free(dispatch->lock_name);
    }
    return XCP_S_OK;
}


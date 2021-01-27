#include <time.h>

#include "internal.h"

xcp_error_t xcp_client_connect(const char *address, xcp_client_t *client) {
    xcp_error_t err;
    xcp_client_t client_obj;

    if ((address == NULL) || (client == NULL)) {
        return XCP_E_INVALIDARG;
    }

    err = xcp_malloc(sizeof(struct _xcp_client_t), (void **)&client_obj);
    if (XCP_FAILED(err)) {
        return err;
    }

    err = xcp_str_copy(address, &client_obj->address);
    err = XCP_SUCCEEDED(err) ? xcp_str_concat(address, ".lck", &client_obj->lock_address) : err;
    if (XCP_SUCCEEDED(err)) {
        char *request_name = NULL, *response_name = NULL;

        err = xcp_str_concat(address, ".xcpreq", &request_name);
        err = XCP_SUCCEEDED(err) ? xcp_str_concat(address, ".xcpres", &response_name) : err;
        if (XCP_SUCCEEDED(err)) {
            err = xcp_dispatch_create(request_name, xcp_dispatch_type_client_request, &client_obj->request);
            err = XCP_SUCCEEDED(err) ? xcp_dispatch_create(response_name, xcp_dispatch_type_client_response, &client_obj->response) : err;
        }
        xcp_free(request_name);
        xcp_free(response_name);
    }

    if (XCP_FAILED(err)) {
        xcp_client_close(client_obj);
        return err;
    }

    *client = client_obj;
    return XCP_S_OK;
}

xcp_error_t xcp_client_invoke(xcp_client_t client, xcp_command_t command, xcp_dispatch_t *request, xcp_dispatch_t *response) {
    xcp_error_t err;
    unsigned long long call_id = ((unsigned long long)time(NULL) + clock()) % 1000000000ULL;

    if (client == NULL) {
        return XCP_E_HANDLE;
    }
    if ((request == NULL) || (response == NULL)) {
        return XCP_E_POINTER;
    }

    *request = client->request;
    *response = client->response;

    err = xcp_dispatch_begin(client->request, XCP_TRUE);
    if (XCP_FAILED(err)) {
        return err;
    }

    xcp_dispatch_put_dword(client->request, XCP_FOURCC);
    xcp_dispatch_put_qword(client->request, call_id);
    xcp_dispatch_put_dword(client->request, command);

    client->response->wait_id = call_id;

    return XCP_S_OK;
}

xcp_error_t xcp_client_close(xcp_client_t client) {
    if (client != NULL) {
        xcp_free(client->address);
        xcp_free(client->lock_address);
        xcp_dispatch_destroy(client->request);
        xcp_dispatch_destroy(client->response);
    }
    return XCP_S_OK;
}

xcp_error_t xcp_client_lock(xcp_client_t client) {
    FILE *lock_fp;
    xcp_bool_t is_used;

    if (client == NULL) {
        return XCP_E_HANDLE;
    }
    do {
        lock_fp = fopen(client->lock_address, "rb");
        is_used = lock_fp != NULL;
        if (is_used) {
            fclose(lock_fp);
            lock_fp = NULL;
        }
    } while (is_used);

    lock_fp = fopen(client->lock_address, "wb");
    if (lock_fp == NULL) {
        return XCP_E_FAIL;
    }
    fclose(lock_fp);

    return XCP_S_OK;
}

xcp_error_t xcp_client_unlock(xcp_client_t client) {
    if (client == NULL) {
        return XCP_E_HANDLE;
    }
    remove(client->lock_address);
    return XCP_S_OK;
}


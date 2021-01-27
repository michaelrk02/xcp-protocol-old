#include "internal.h"

xcp_error_t xcp_server_create(const char *address, xcp_interface_router_t router, FILE *logfp, xcp_server_t *server) {
    xcp_server_t server_obj = NULL;
    xcp_error_t err = XCP_S_OK;

    if ((address == NULL) || (router == NULL) || (server == NULL)) {
        return XCP_E_POINTER;
    }

    err = xcp_malloc(sizeof(struct _xcp_server_t), (void **)&server_obj);
    if (XCP_FAILED(err)) {
        return err;
    }

    server_obj->router = router;
    server_obj->logfp = logfp;

    err = xcp_str_copy(address, &server_obj->address);
    err = XCP_SUCCEEDED(err) ? xcp_str_concat(address, ".lck", &server_obj->lock_address) : err;
    if (XCP_SUCCEEDED(err)) {
        char *request_name = NULL, *response_name = NULL;

        err = xcp_str_concat(server_obj->address, ".xcpreq", &request_name);
        err = XCP_SUCCEEDED(err) ? xcp_str_concat(server_obj->address, ".xcpres", &response_name) : err;
        if (XCP_SUCCEEDED(err)) {
            err = xcp_dispatch_create(request_name, xcp_dispatch_type_server_request, &server_obj->request);
            err = XCP_SUCCEEDED(err) ? xcp_dispatch_create(response_name, xcp_dispatch_type_server_response, &server_obj->response) : err;
            err = XCP_SUCCEEDED(err) ? xcp_mutex_create(&server_obj->mutex) : err;
        }
        xcp_free(request_name);
        xcp_free(response_name);
    }

    if (XCP_FAILED(err)) {
        xcp_server_destroy(server_obj);
        return err;
    }

    *server = server_obj;
    return XCP_S_OK;
}

xcp_error_t xcp_server_start(xcp_server_t server) {
    if (server == NULL) {
        return XCP_E_HANDLE;
    }

    remove(server->lock_address);
    xcp_dispatch_init(server->request);
    xcp_dispatch_init(server->response);

    xcp_mutex_lock(server->mutex);
    server->running = XCP_TRUE;
    xcp_mutex_unlock(server->mutex);

    while (XCP_SUCCEEDED(xcp_server_ready(server))) {
        xcp_error_t err = xcp_server_intercept(server);
        if (XCP_FAILED(err)) {
            if (server->logfp != NULL) {
                fprintf(server->logfp, "[XCP] server interception error: 0x%X\n", err);
            }
        }
    }
    fprintf(server->logfp, "[XCP] server stopped\n");
}

xcp_error_t xcp_server_ready(xcp_server_t server) {
    xcp_bool_t running;

    if (server == NULL) {
        return XCP_E_HANDLE;
    }

    xcp_mutex_lock(server->mutex);
    running = server->running;
    xcp_mutex_unlock(server->mutex);

    return running ? XCP_S_OK : XCP_E_FAIL;
}

xcp_error_t xcp_server_intercept(xcp_server_t server) {
    xcp_error_t err;
    xcp_interface_t interface;

    if (server == NULL) {
        return XCP_E_HANDLE;
    }

    while (XCP_SUCCEEDED(xcp_server_ready(server)) && XCP_FAILED(xcp_dispatch_ready(server->request))) {
    }
    if (XCP_FAILED(xcp_server_ready(server))) {
        return XCP_E_ABORT;
    }
    err = xcp_dispatch_begin(server->request, XCP_TRUE);
    if (XCP_FAILED(err)) {
        return err;
    }

    err = xcp_dispatch_begin(server->response, XCP_TRUE);
    if (XCP_SUCCEEDED(err)) {
        unsigned int fourcc;

        xcp_dispatch_get_dword(server->request, &fourcc);
        if (fourcc == XCP_FOURCC) {
            unsigned long long call_id;
            unsigned int command;

            xcp_dispatch_get_qword(server->request, &call_id);
            xcp_dispatch_get_dword(server->request, &command);

            xcp_dispatch_put_dword(server->response, XCP_FOURCC);
            xcp_dispatch_put_qword(server->response, call_id);

            err = server->router(XCP_INTERFACE(command), &interface);
            if (XCP_SUCCEEDED(err)) {
                xcp_message_callback_t callback;

                err = interface->router(XCP_MESSAGE(command), &callback);
                if (XCP_SUCCEEDED(err)) {
                    xcp_dispatch_put_error(server->response, XCP_S_OK);
                    err = callback(server->request, server->response);
                    xcp_dispatch_put_error(server->response, err);
                } else {
                    xcp_dispatch_put_error(server->response, err);
                }
            } else {
                xcp_dispatch_put_error(server->response, err);
            }
        } else {
            xcp_dispatch_put_error(server->response, XCP_E_INVALIDARG);
        }

        err = xcp_dispatch_commit(server->response);
    }

    xcp_dispatch_close(server->request);
    return err;
}

xcp_error_t xcp_server_stop(xcp_server_t server) {
    if (server == NULL) {
        return XCP_E_HANDLE;
    }

    xcp_mutex_lock(server->mutex);
    server->running = XCP_FALSE;
    xcp_mutex_unlock(server->mutex);

    return XCP_S_OK;
}

xcp_error_t xcp_server_destroy(xcp_server_t server) {
    if (server != NULL) {
        xcp_free(server->address);
        xcp_free(server->lock_address);
        xcp_dispatch_destroy(server->request);
        xcp_dispatch_destroy(server->response);
        xcp_mutex_destroy(server->mutex);
        xcp_free(server);
    }
    return XCP_S_OK;
}


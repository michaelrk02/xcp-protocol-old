#ifndef _XCP_INTERNAL_H
#define _XCP_INTERNAL_H

#include <stdio.h>

#include <xcp.h>

#define XCP_FOURCC 0x21504358

typedef struct _xcp_mutex_t *xcp_mutex_t;

struct _xcp_server_t {
    char *address;
    char *lock_address;
    xcp_interface_router_t router;
    FILE *logfp;
    xcp_dispatch_t request;
    xcp_dispatch_t response;
    xcp_bool_t running;
    xcp_mutex_t mutex;
};

struct _xcp_interface_t {
    xcp_message_router_t router;
};

struct _xcp_client_t {
    char *address;
    char *lock_address;
    xcp_dispatch_t request;
    xcp_dispatch_t response;
};

struct _xcp_dispatch_t {
    char *name;
    char *lock_name;
    xcp_dispatch_type_t type;
    FILE *fp;
    unsigned long long wait_id;
};

extern XCPAPI xcp_error_t xcp_str_copy(const char *src, char **dst);
extern XCPAPI xcp_error_t xcp_str_concat(const char *left, const char *right, char **result);

extern XCPAPI xcp_error_t xcp_mutex_create(xcp_mutex_t *mutex);
extern XCPAPI xcp_error_t xcp_mutex_lock(xcp_mutex_t mutex);
extern XCPAPI xcp_error_t xcp_mutex_unlock(xcp_mutex_t mutex);
extern XCPAPI xcp_error_t xcp_mutex_destroy(xcp_mutex_t mutex);

extern XCPAPI xcp_error_t xcp_server_ready(xcp_server_t server);
extern XCPAPI xcp_error_t xcp_server_intercept(xcp_server_t server);

extern XCPAPI xcp_error_t xcp_client_lock(xcp_client_t client);
extern XCPAPI xcp_error_t xcp_client_unlock(xcp_client_t client);

extern XCPAPI xcp_error_t xcp_dispatch_create(const char *name, xcp_dispatch_type_t type, xcp_dispatch_t *dispatch);
extern XCPAPI xcp_error_t xcp_dispatch_init(xcp_dispatch_t dispatch);
extern XCPAPI xcp_error_t xcp_dispatch_read_scalar(xcp_dispatch_t dispatch, void *data, size_t size);
extern XCPAPI xcp_error_t xcp_dispatch_write_scalar(xcp_dispatch_t dispatch, const void *data, size_t size);
extern XCPAPI xcp_error_t xcp_dispatch_begin(xcp_dispatch_t dispatch, xcp_bool_t lock);
extern XCPAPI xcp_error_t xcp_dispatch_end(xcp_dispatch_t dispatch, xcp_bool_t unlock);
extern XCPAPI xcp_error_t xcp_dispatch_lock(xcp_dispatch_t dispatch);
extern XCPAPI xcp_error_t xcp_dispatch_unlock(xcp_dispatch_t dispatch);
extern XCPAPI xcp_error_t xcp_dispatch_ready(xcp_dispatch_t dispatch);
extern XCPAPI xcp_error_t xcp_dispatch_destroy(xcp_dispatch_t dispatch);

#endif

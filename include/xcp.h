/*
** xcp.h : XCP Communication Protocol header file
**
** Copyright (c) 2020, Michael R. Krisnadhi
*/

#ifndef _XCP_H
#define _XCP_H

#include <stdio.h>

#if defined(_MSC_VER)
#define _XCP_IMPORT __declspec(dllimport)
#define _XCP_EXPORT __declspec(dllexport)
#elif defined(_WIN32) && defined(__GNUC__)
#define _XCP_IMPORT __attribute__((dllimport))
#define _XCP_EXPORT __attribute__((dllexport))
#elif defined(__GNUC__)
#define _XCP_IMPORT
#define _XCP_EXPORT __attribute__((visibility("default")))
#else
#error Unsupported compiler. Use GNU C or Microsoft C compiler
#endif

#ifdef XCPBUILD
#define XCPAPI _XCP_EXPORT
#else
#define XCPAPI _XCP_IMPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
** Basic type definitions
*/

typedef unsigned int xcp_error_t;

#define XCP_ERROR(_Severity, _Facility, _Code) \
    ((xcp_error_t)(\
        (((_Severity) & 0xF) << 28) | \
        (((_Facility) & 0xFFF) << 16) | \
        ((_Code) & 0xFFFF) \
    ))

#define XCP_SEVERITY(_Error)    (((_Error) >> 28) & 0xF)
#define XCP_FACILITY(_Error)    (((_Error) >> 16) & 0xFFF)
#define XCP_CODE(_Error)        ((_Error) & 0xFFFF)

#define XCP_SUCCESS             0xC
#define XCP_FAILURE             0xE
#define XCP_SUCCEEDED(_Error)   (XCP_SEVERITY(_Error) == XCP_SUCCESS)
#define XCP_FAILED(_Error)      (XCP_SEVERITY(_Error) == XCP_FAILURE)

#define XCP_F_GENERIC           0x000
#define XCP_S_OK                XCP_ERROR(XCP_SUCCESS, XCP_F_GENERIC, 0x0000)
#define XCP_E_FAIL              XCP_ERROR(XCP_FAILURE, XCP_F_GENERIC, 0x0000)
#define XCP_E_INVALIDARG        XCP_ERROR(XCP_FAILURE, XCP_F_GENERIC, 0x0001)
#define XCP_E_POINTER           XCP_ERROR(XCP_FAILURE, XCP_F_GENERIC, 0x0002)
#define XCP_E_HANDLE            XCP_ERROR(XCP_FAILURE, XCP_F_GENERIC, 0x0003)
#define XCP_E_NOTIMPL           XCP_ERROR(XCP_FAILURE, XCP_F_GENERIC, 0x0004)
#define XCP_E_NOINTERFACE       XCP_ERROR(XCP_FAILURE, XCP_F_GENERIC, 0x0005)
#define XCP_E_ACCESSDENIED      XCP_ERROR(XCP_FAILURE, XCP_F_GENERIC, 0x0006)
#define XCP_E_OUTOFMEMORY       XCP_ERROR(XCP_FAILURE, XCP_F_GENERIC, 0x0007)
#define XCP_E_UNEXPECTED        XCP_ERROR(XCP_FAILURE, XCP_F_GENERIC, 0x0008)
#define XCP_E_ABORT             XCP_ERROR(XCP_FAILURE, XCP_F_GENERIC, 0x0009)

typedef int xcp_bool_t;
#define XCP_TRUE    1
#define XCP_FALSE   0

typedef unsigned int xcp_command_t;

#define XCP_COMMAND(_Interface, _Message) \
    ((xcp_command_t)( \
        (((_Interface) & 0xFFFF) << 16) | \
        ((_Message) & 0xFFFF) \
    ))

#define XCP_INTERFACE(_Command)     (((_Command) >> 16) & 0xFFFF)
#define XCP_MESSAGE(_Command)       ((_Command) & 0xFFFF)

typedef enum {
    xcp_dispatch_type_invalid = -1,
    xcp_dispatch_type_client_request,   /* valid actions: put, commit */
    xcp_dispatch_type_client_response,  /* valid actions: get, close */
    xcp_dispatch_type_server_request,   /* valid actions: get, close */
    xcp_dispatch_type_server_response   /* valid actions: put, commit */
} xcp_dispatch_type_t;

typedef struct _xcp_server_t *xcp_server_t;
typedef struct _xcp_interface_t *xcp_interface_t;
typedef struct _xcp_client_t *xcp_client_t;
typedef struct _xcp_dispatch_t *xcp_dispatch_t;

typedef xcp_error_t (*xcp_message_callback_t)(xcp_dispatch_t request, xcp_dispatch_t response);
typedef xcp_error_t (*xcp_interface_router_t)(unsigned short interface_id, xcp_interface_t *interface);
typedef xcp_error_t (*xcp_message_router_t)(unsigned short message_id, xcp_message_callback_t *callback);

/*******************************************************************************
** Function definitions
*/

extern XCPAPI xcp_error_t xcp_malloc(size_t size, void **memory);
extern XCPAPI xcp_error_t xcp_free(void *memory);

extern XCPAPI xcp_error_t xcp_server_create(const char *address, xcp_interface_router_t router, FILE *logfp, xcp_server_t *server);
extern XCPAPI xcp_error_t xcp_server_start(xcp_server_t server);
extern XCPAPI xcp_error_t xcp_server_stop(xcp_server_t server);
extern XCPAPI xcp_error_t xcp_server_destroy(xcp_server_t server);

extern XCPAPI xcp_error_t xcp_interface_create(xcp_message_router_t router, xcp_interface_t *interface);
extern XCPAPI xcp_error_t xcp_interface_destroy(xcp_interface_t interface);

extern XCPAPI xcp_error_t xcp_client_connect(const char *address, xcp_client_t *client);
extern XCPAPI xcp_error_t xcp_client_invoke(xcp_client_t client, xcp_command_t command, xcp_dispatch_t *request, xcp_dispatch_t *response);
extern XCPAPI xcp_error_t xcp_client_close(xcp_client_t client);

extern XCPAPI xcp_dispatch_type_t xcp_dispatch_get_type(xcp_dispatch_t dispatch);
extern XCPAPI xcp_error_t xcp_dispatch_get_error(xcp_dispatch_t dispatch);
extern XCPAPI xcp_error_t xcp_dispatch_get_bool(xcp_dispatch_t dispatch, xcp_bool_t *value);
extern XCPAPI xcp_error_t xcp_dispatch_get_char(xcp_dispatch_t dispatch, char *value);
extern XCPAPI xcp_error_t xcp_dispatch_get_byte(xcp_dispatch_t dispatch, unsigned char *value);
extern XCPAPI xcp_error_t xcp_dispatch_get_short(xcp_dispatch_t dispatch, short *value);
extern XCPAPI xcp_error_t xcp_dispatch_get_word(xcp_dispatch_t dispatch, unsigned short *value);
extern XCPAPI xcp_error_t xcp_dispatch_get_int(xcp_dispatch_t dispatch, int *value);
extern XCPAPI xcp_error_t xcp_dispatch_get_dword(xcp_dispatch_t dispatch, unsigned int *value);
extern XCPAPI xcp_error_t xcp_dispatch_get_long(xcp_dispatch_t dispatch, long long *value);
extern XCPAPI xcp_error_t xcp_dispatch_get_qword(xcp_dispatch_t dispatch, unsigned long long *value);
extern XCPAPI xcp_error_t xcp_dispatch_get_float(xcp_dispatch_t dispatch, float *value);
extern XCPAPI xcp_error_t xcp_dispatch_get_double(xcp_dispatch_t dispatch, double *value);
extern XCPAPI xcp_error_t xcp_dispatch_get_string(xcp_dispatch_t dispatch, char **value);
extern XCPAPI xcp_error_t xcp_dispatch_put_error(xcp_dispatch_t dispatch, xcp_error_t value);
extern XCPAPI xcp_error_t xcp_dispatch_put_bool(xcp_dispatch_t dispatch, xcp_bool_t value);
extern XCPAPI xcp_error_t xcp_dispatch_put_char(xcp_dispatch_t dispatch, char value);
extern XCPAPI xcp_error_t xcp_dispatch_put_byte(xcp_dispatch_t dispatch, unsigned char value);
extern XCPAPI xcp_error_t xcp_dispatch_put_short(xcp_dispatch_t dispatch, short value);
extern XCPAPI xcp_error_t xcp_dispatch_put_word(xcp_dispatch_t dispatch, unsigned short value);
extern XCPAPI xcp_error_t xcp_dispatch_put_int(xcp_dispatch_t dispatch, int value);
extern XCPAPI xcp_error_t xcp_dispatch_put_dword(xcp_dispatch_t dispatch, unsigned int value);
extern XCPAPI xcp_error_t xcp_dispatch_put_long(xcp_dispatch_t dispatch, long long value);
extern XCPAPI xcp_error_t xcp_dispatch_put_qword(xcp_dispatch_t dispatch, unsigned long long value);
extern XCPAPI xcp_error_t xcp_dispatch_put_float(xcp_dispatch_t dispatch, float value);
extern XCPAPI xcp_error_t xcp_dispatch_put_double(xcp_dispatch_t dispatch, double value);
extern XCPAPI xcp_error_t xcp_dispatch_put_string(xcp_dispatch_t dispatch, const char *value);
extern XCPAPI xcp_error_t xcp_dispatch_commit(xcp_dispatch_t dispatch);
extern XCPAPI xcp_error_t xcp_dispatch_close(xcp_dispatch_t dispatch);
extern XCPAPI xcp_error_t xcp_dispatch_wait(xcp_dispatch_t dispatch);

#ifdef __cplusplus
}
#endif

#endif

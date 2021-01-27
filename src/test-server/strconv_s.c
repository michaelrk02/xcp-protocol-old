/*
** Generated using XCP-IDL compiler. DO NOT EDIT
**
** strconv_s.c: server stub definition for "strconv" XCP module
*/

#define _XCP_SERVER_STUB

#include "strconv.h"

xcp_interface_t __XCP_MODULE_strconv_INTERFACE_StrConv = NULL;

xcp_error_t __XCP_MODULE_strconv_MESSAGE_CALLBACK_StrConv_FromBool(xcp_dispatch_t __request, xcp_dispatch_t __response) {
    xcp_error_t __err;
    xcp_bool_t input;
    char * output;

    xcp_dispatch_get_bool(__request, &input);
    xcp_dispatch_close(__request);

    __err = StrConv_FromBool_s(input, &output);
    xcp_dispatch_put_string(__response, output);
    xcp_free(output);

    return __err;
}

xcp_error_t __XCP_MODULE_strconv_MESSAGE_CALLBACK_StrConv_FromInt(xcp_dispatch_t __request, xcp_dispatch_t __response) {
    xcp_error_t __err;
    int input;
    char * output;

    xcp_dispatch_get_int(__request, &input);
    xcp_dispatch_close(__request);

    __err = StrConv_FromInt_s(input, &output);
    xcp_dispatch_put_string(__response, output);
    xcp_free(output);

    return __err;
}

xcp_error_t __XCP_MODULE_strconv_MESSAGE_CALLBACK_StrConv_FromDouble(xcp_dispatch_t __request, xcp_dispatch_t __response) {
    xcp_error_t __err;
    double input;
    char * output;

    xcp_dispatch_get_double(__request, &input);
    xcp_dispatch_close(__request);

    __err = StrConv_FromDouble_s(input, &output);
    xcp_dispatch_put_string(__response, output);
    xcp_free(output);

    return __err;
}

xcp_error_t __XCP_MODULE_strconv_MESSAGE_CALLBACK_StrConv_ToBool(xcp_dispatch_t __request, xcp_dispatch_t __response) {
    xcp_error_t __err;
    char * input;
    xcp_bool_t output;

    xcp_dispatch_get_string(__request, &input);
    xcp_dispatch_close(__request);

    __err = StrConv_ToBool_s(input, &output);
    xcp_free(input);
    xcp_dispatch_put_bool(__response, output);

    return __err;
}

xcp_error_t __XCP_MODULE_strconv_MESSAGE_CALLBACK_StrConv_ToInt(xcp_dispatch_t __request, xcp_dispatch_t __response) {
    xcp_error_t __err;
    char * input;
    int output;

    xcp_dispatch_get_string(__request, &input);
    xcp_dispatch_close(__request);

    __err = StrConv_ToInt_s(input, &output);
    xcp_free(input);
    xcp_dispatch_put_int(__response, output);

    return __err;
}

xcp_error_t __XCP_MODULE_strconv_MESSAGE_CALLBACK_StrConv_ToDouble(xcp_dispatch_t __request, xcp_dispatch_t __response) {
    xcp_error_t __err;
    char * input;
    double output;

    xcp_dispatch_get_string(__request, &input);
    xcp_dispatch_close(__request);

    __err = StrConv_ToDouble_s(input, &output);
    xcp_free(input);
    xcp_dispatch_put_double(__response, output);

    return __err;
}

xcp_error_t __XCP_MODULE_strconv_MESSAGE_ROUTER_StrConv(unsigned short message_id, xcp_message_callback_t *callback) {
    xcp_message_callback_t callback_func = NULL;

    switch (message_id) {
    case 0:
        callback_func = __XCP_MODULE_strconv_MESSAGE_CALLBACK_StrConv_FromBool;
        break;
    case 1:
        callback_func = __XCP_MODULE_strconv_MESSAGE_CALLBACK_StrConv_FromInt;
        break;
    case 2:
        callback_func = __XCP_MODULE_strconv_MESSAGE_CALLBACK_StrConv_FromDouble;
        break;
    case 3:
        callback_func = __XCP_MODULE_strconv_MESSAGE_CALLBACK_StrConv_ToBool;
        break;
    case 4:
        callback_func = __XCP_MODULE_strconv_MESSAGE_CALLBACK_StrConv_ToInt;
        break;
    case 5:
        callback_func = __XCP_MODULE_strconv_MESSAGE_CALLBACK_StrConv_ToDouble;
        break;
    }
    if (callback_func != NULL) {
        *callback = callback_func;
        return XCP_S_OK;
    }

    return XCP_E_NOTIMPL;
}

xcp_error_t xcp_init_module_strconv(void) {
    xcp_error_t err = XCP_S_OK;

    err = XCP_SUCCEEDED(err) ? xcp_interface_create(__XCP_MODULE_strconv_MESSAGE_ROUTER_StrConv, &__XCP_MODULE_strconv_INTERFACE_StrConv) : err;
    if (XCP_FAILED(err)) {
        xcp_destroy_module_strconv();
        return err;
    }

    return XCP_S_OK;
}

xcp_error_t xcp_destroy_module_strconv(void) {
    xcp_interface_destroy(__XCP_MODULE_strconv_INTERFACE_StrConv);

    return XCP_S_OK;
}

xcp_error_t XCP_MODULE_strconv_INTERFACE_ROUTER(unsigned short interface_id, xcp_interface_t *interface) {
    xcp_interface_t interface_obj = NULL;

    switch (interface_id) {
    case 0:
        interface_obj = __XCP_MODULE_strconv_INTERFACE_StrConv;
        break;
    }
    if (interface_obj != NULL) {
        *interface = interface_obj;
        return XCP_S_OK;
    }

    return XCP_E_NOTIMPL;
}


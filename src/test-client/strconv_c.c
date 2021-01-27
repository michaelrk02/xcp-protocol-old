/*
** Generated using XCP-IDL compiler. DO NOT EDIT
**
** strconv_c.c: client stub definition for "strconv" XCP module
*/

#include "strconv.h"

xcp_error_t StrConv_FromBool(xcp_client_t __client, /* [in] */ xcp_bool_t input, /* [out] */ char * *output) {
    xcp_error_t __err;
    xcp_dispatch_t __request, __response;

    __err = xcp_client_invoke(__client, XCP_COMMAND(0, 0), &__request, &__response);
    if (XCP_FAILED(__err)) {
        return __err;
    }

    xcp_dispatch_put_bool(__request, input);
    __err = xcp_dispatch_commit(__request);
    if (XCP_FAILED(__err)) {
        return __err;
    }

    __err = xcp_dispatch_wait(__response);
    if (XCP_FAILED(__err)) {
        return __err;
    }

    __err = xcp_dispatch_get_error(__response);
    if (XCP_SUCCEEDED(__err)) {
        xcp_dispatch_get_string(__response, output);
        __err = xcp_dispatch_get_error(__response);
    }
    xcp_dispatch_close(__response);

    return __err;
}

xcp_error_t StrConv_FromInt(xcp_client_t __client, /* [in] */ int input, /* [out] */ char * *output) {
    xcp_error_t __err;
    xcp_dispatch_t __request, __response;

    __err = xcp_client_invoke(__client, XCP_COMMAND(0, 1), &__request, &__response);
    if (XCP_FAILED(__err)) {
        return __err;
    }

    xcp_dispatch_put_int(__request, input);
    __err = xcp_dispatch_commit(__request);
    if (XCP_FAILED(__err)) {
        return __err;
    }

    __err = xcp_dispatch_wait(__response);
    if (XCP_FAILED(__err)) {
        return __err;
    }

    __err = xcp_dispatch_get_error(__response);
    if (XCP_SUCCEEDED(__err)) {
        xcp_dispatch_get_string(__response, output);
        __err = xcp_dispatch_get_error(__response);
    }
    xcp_dispatch_close(__response);

    return __err;
}

xcp_error_t StrConv_FromDouble(xcp_client_t __client, /* [in] */ double input, /* [out] */ char * *output) {
    xcp_error_t __err;
    xcp_dispatch_t __request, __response;

    __err = xcp_client_invoke(__client, XCP_COMMAND(0, 2), &__request, &__response);
    if (XCP_FAILED(__err)) {
        return __err;
    }

    xcp_dispatch_put_double(__request, input);
    __err = xcp_dispatch_commit(__request);
    if (XCP_FAILED(__err)) {
        return __err;
    }

    __err = xcp_dispatch_wait(__response);
    if (XCP_FAILED(__err)) {
        return __err;
    }

    __err = xcp_dispatch_get_error(__response);
    if (XCP_SUCCEEDED(__err)) {
        xcp_dispatch_get_string(__response, output);
        __err = xcp_dispatch_get_error(__response);
    }
    xcp_dispatch_close(__response);

    return __err;
}

xcp_error_t StrConv_ToBool(xcp_client_t __client, /* [in] */ const char * input, /* [out] */ xcp_bool_t *output) {
    xcp_error_t __err;
    xcp_dispatch_t __request, __response;

    __err = xcp_client_invoke(__client, XCP_COMMAND(0, 3), &__request, &__response);
    if (XCP_FAILED(__err)) {
        return __err;
    }

    xcp_dispatch_put_string(__request, input);
    __err = xcp_dispatch_commit(__request);
    if (XCP_FAILED(__err)) {
        return __err;
    }

    __err = xcp_dispatch_wait(__response);
    if (XCP_FAILED(__err)) {
        return __err;
    }

    __err = xcp_dispatch_get_error(__response);
    if (XCP_SUCCEEDED(__err)) {
        xcp_dispatch_get_bool(__response, output);
        __err = xcp_dispatch_get_error(__response);
    }
    xcp_dispatch_close(__response);

    return __err;
}

xcp_error_t StrConv_ToInt(xcp_client_t __client, /* [in] */ const char * input, /* [out] */ int *output) {
    xcp_error_t __err;
    xcp_dispatch_t __request, __response;

    __err = xcp_client_invoke(__client, XCP_COMMAND(0, 4), &__request, &__response);
    if (XCP_FAILED(__err)) {
        return __err;
    }

    xcp_dispatch_put_string(__request, input);
    __err = xcp_dispatch_commit(__request);
    if (XCP_FAILED(__err)) {
        return __err;
    }

    __err = xcp_dispatch_wait(__response);
    if (XCP_FAILED(__err)) {
        return __err;
    }

    __err = xcp_dispatch_get_error(__response);
    if (XCP_SUCCEEDED(__err)) {
        xcp_dispatch_get_int(__response, output);
        __err = xcp_dispatch_get_error(__response);
    }
    xcp_dispatch_close(__response);

    return __err;
}

xcp_error_t StrConv_ToDouble(xcp_client_t __client, /* [in] */ const char * input, /* [out] */ double *output) {
    xcp_error_t __err;
    xcp_dispatch_t __request, __response;

    __err = xcp_client_invoke(__client, XCP_COMMAND(0, 5), &__request, &__response);
    if (XCP_FAILED(__err)) {
        return __err;
    }

    xcp_dispatch_put_string(__request, input);
    __err = xcp_dispatch_commit(__request);
    if (XCP_FAILED(__err)) {
        return __err;
    }

    __err = xcp_dispatch_wait(__response);
    if (XCP_FAILED(__err)) {
        return __err;
    }

    __err = xcp_dispatch_get_error(__response);
    if (XCP_SUCCEEDED(__err)) {
        xcp_dispatch_get_double(__response, output);
        __err = xcp_dispatch_get_error(__response);
    }
    xcp_dispatch_close(__response);

    return __err;
}


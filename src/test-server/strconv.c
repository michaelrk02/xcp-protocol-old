#define _XCP_SERVER_STUB

#include <stdio.h>
#include <string.h>

#include "strconv.h"

xcp_error_t StrConv_FromBool_s(/* [in] */ xcp_bool_t input, /* [out] */ char * *output) {
    xcp_error_t err;
    char *buffer;
    unsigned int length = input ? 4 : 5;

    err = xcp_malloc((length + 1) * sizeof(char), (void **)&buffer);
    if (XCP_FAILED(err)) {
        return err;
    }

    strcpy(buffer, input ? "true" : "false");

    *output = buffer;
    return err;
}

xcp_error_t StrConv_FromInt_s(/* [in] */ int input, /* [out] */ char * *output) {
    xcp_error_t err;
    char *buffer;

    err = xcp_malloc(100 * sizeof(char), (void **)&buffer);
    if (XCP_FAILED(err)) {
        return err;
    }

    sprintf(buffer, "%d", input);

    *output = buffer;
    return err;
}

xcp_error_t StrConv_FromDouble_s(/* [in] */ double input, /* [out] */ char * *output) {
    xcp_error_t err;
    char *buffer;

    err = xcp_malloc(100 * sizeof(char), (void **)&buffer);
    if (XCP_FAILED(err)) {
        return err;
    }

    sprintf(buffer, "%f", input);

    *output = buffer;
    return err;
}

xcp_error_t StrConv_ToBool_s(/* [in] */ const char * input, /* [out] */ xcp_bool_t *output) {
    *output = strcmp(input, "true") == 0;
    return XCP_S_OK;
}

xcp_error_t StrConv_ToInt_s(/* [in] */ const char * input, /* [out] */ int *output) {
    sscanf(input, "%d", output);
    return XCP_S_OK;
}

xcp_error_t StrConv_ToDouble_s(/* [in] */ const char * input, /* [out] */ double *output) {
    sscanf(input, "%lf", output);
    return XCP_S_OK;
}


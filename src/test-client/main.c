#include <stdio.h>

#include "strconv.h"

int main(void) {
    xcp_error_t err;
    xcp_client_t client;

    err = xcp_client_connect("strconv", &client);
    if (XCP_SUCCEEDED(err)) {
        xcp_bool_t bool_value = XCP_TRUE;
        int int_value = 47;
        double double_value = 18.7;
        char *buffer;
        char bool_string[] = "false";
        char int_string[] = "74";
        char double_string[] = "78.1";

        err = XCP_S_OK;

        err = XCP_SUCCEEDED(err) ? StrConv_FromBool(client, bool_value, &buffer) : err;
        if (XCP_SUCCEEDED(err)) {
            printf("Boolean string: %s\n", buffer);
            xcp_free(buffer);
        }

        err = XCP_SUCCEEDED(err) ? StrConv_FromInt(client, int_value, &buffer) : err;
        if (XCP_SUCCEEDED(err)) {
            printf("Integer string: %s\n", buffer);
            xcp_free(buffer);
        }

        err = XCP_SUCCEEDED(err) ? StrConv_FromDouble(client, double_value, &buffer) : err;
        if (XCP_SUCCEEDED(err)) {
            printf("Double string: %s\n", buffer);
            xcp_free(buffer);
        }

        err = XCP_SUCCEEDED(err) ? StrConv_ToBool(client, bool_string, &bool_value) : err;
        if (XCP_SUCCEEDED(err)) {
            printf("Bool value: %d\n", bool_value);
        }

        err = XCP_SUCCEEDED(err) ? StrConv_ToInt(client, int_string, &int_value) : err;
        if (XCP_SUCCEEDED(err)) {
            printf("Integer value: %d\n", int_value);
        }

        err = XCP_SUCCEEDED(err) ? StrConv_ToDouble(client, double_string, &double_value) : err;
        if (XCP_SUCCEEDED(err)) {
            printf("Double value: %lf\n", double_value);
        }

        xcp_client_close(client);
    }
    if (XCP_FAILED(err)) {
        printf("E: XCP failure = 0x%X\n", err);
        return 1;
    }

    return 0;
}


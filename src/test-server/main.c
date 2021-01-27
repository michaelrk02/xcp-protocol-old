#define _XCP_SERVER_STUB

#include <signal.h>
#include <stdio.h>

#include "strconv.h"

xcp_server_t g_server;

void stop_server(int sig) {
    xcp_server_stop(g_server);
}

int main() {
    xcp_error_t err;

    signal(SIGINT, stop_server);

    err = xcp_init_module_strconv();
    if (XCP_SUCCEEDED(err)) {

        err = xcp_server_create("strconv", XCP_MODULE_strconv_INTERFACE_ROUTER, stdout, &g_server);
        if (XCP_SUCCEEDED(err)) {
            err = xcp_server_start(g_server);
            xcp_server_destroy(g_server);
        }

        xcp_destroy_module_strconv();
    }

    if (XCP_FAILED(err)) {
        printf("XCP error: 0x%X\n", err);
    }

    return 0;
}


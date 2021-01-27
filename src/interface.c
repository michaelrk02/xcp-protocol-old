#include "internal.h"

xcp_error_t xcp_interface_create(xcp_message_router_t router, xcp_interface_t *interface) {
    xcp_error_t err;
    xcp_interface_t interface_obj;

    if ((router == NULL) || (interface == NULL)) {
        return XCP_E_POINTER;
    }

    err = xcp_malloc(sizeof(struct _xcp_interface_t), (void **)&interface_obj);
    if (XCP_FAILED(err)) {
        return err;
    }
    interface_obj->router = router;

    *interface = interface_obj;
    return XCP_S_OK;
}

xcp_error_t xcp_interface_destroy(xcp_interface_t interface) {
    if (interface != NULL) {
        xcp_free(interface);
    }
    return XCP_S_OK;
}


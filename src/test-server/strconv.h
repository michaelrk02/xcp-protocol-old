/*
** Generated using XCP-IDL compiler. DO NOT EDIT
**
** strconv.h: header file for "strconv" XCP module
*/

#ifndef __XCPMODULE_strconv_H_INCLUDED
#define __XCPMODULE_strconv_H_INCLUDED

#include <xcp.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _XCP_SERVER_STUB
extern xcp_error_t xcp_init_module_strconv(void);
extern xcp_error_t xcp_destroy_module_strconv(void);

extern xcp_error_t XCP_MODULE_strconv_INTERFACE_ROUTER(unsigned short, xcp_interface_t *);
#endif

/*
** StrConv interface
*/

#ifdef _XCP_SERVER_STUB
extern xcp_error_t StrConv_FromBool_s(/* [in] */ xcp_bool_t input, /* [out] */ char * *output);
extern xcp_error_t StrConv_FromInt_s(/* [in] */ int input, /* [out] */ char * *output);
extern xcp_error_t StrConv_FromDouble_s(/* [in] */ double input, /* [out] */ char * *output);
extern xcp_error_t StrConv_ToBool_s(/* [in] */ const char * input, /* [out] */ xcp_bool_t *output);
extern xcp_error_t StrConv_ToInt_s(/* [in] */ const char * input, /* [out] */ int *output);
extern xcp_error_t StrConv_ToDouble_s(/* [in] */ const char * input, /* [out] */ double *output);
#endif

extern xcp_error_t StrConv_FromBool(xcp_client_t client, /* [in] */ xcp_bool_t input, /* [out] */ char * *output);
extern xcp_error_t StrConv_FromInt(xcp_client_t client, /* [in] */ int input, /* [out] */ char * *output);
extern xcp_error_t StrConv_FromDouble(xcp_client_t client, /* [in] */ double input, /* [out] */ char * *output);
extern xcp_error_t StrConv_ToBool(xcp_client_t client, /* [in] */ const char * input, /* [out] */ xcp_bool_t *output);
extern xcp_error_t StrConv_ToInt(xcp_client_t client, /* [in] */ const char * input, /* [out] */ int *output);
extern xcp_error_t StrConv_ToDouble(xcp_client_t client, /* [in] */ const char * input, /* [out] */ double *output);

#ifdef __cplusplus
}
#endif

#endif

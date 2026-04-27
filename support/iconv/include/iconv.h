#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct iconv_descriptor *iconv_t;

  iconv_t iconv_open(const char *tocode, const char *fromcode);

  size_t iconv(iconv_t cd,
               const char **inbuf,
               size_t *inbytesleft,
               char **outbuf,
               size_t *outbytesleft);

  int iconv_close(iconv_t cd);

#ifdef __cplusplus
}
#endif

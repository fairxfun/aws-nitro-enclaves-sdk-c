#ifndef KMSTOOL_DECRYPT_H
#define KMSTOOL_DECRYPT_H

#include "./kmstool.h"

int app_lib_decrypt(
    const struct app_ctx *ctx,
    const struct kmstool_decrypt_params *params,
    unsigned char **plaintext_out,
    unsigned int *plaintext_out_len);

#endif // KMSTOOL_DECRYPT_H
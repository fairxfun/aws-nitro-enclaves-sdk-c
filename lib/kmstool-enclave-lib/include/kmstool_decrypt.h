#ifndef KMSTOOL_DECRYPT_H
#define KMSTOOL_DECRYPT_H

#include "./kmstool.h"

int kmstool_lib_decrypt(
    struct kmstool_lib_ctx *ctx,
    const struct kmstool_decrypt_params *params,
    unsigned char **plaintext_out,
    unsigned int *plaintext_out_len);

#endif // KMSTOOL_DECRYPT_H
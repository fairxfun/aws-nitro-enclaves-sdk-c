#ifndef KMSTOOL_ENCRYP_H
#define KMSTOOL_ENCRYP_H

#include "./kmstool.h"

int kmstool_lib_encrypt(
    struct kmstool_lib_ctx *ctx,
    const struct kmstool_encrypt_params *params,
    unsigned char **ciphertext_out,
    unsigned int *ciphertext_out_len);

#endif // KMSTOOL_ENCRYP_H

#ifndef KMS_TOOL_API_H
#define KMS_TOOL_API_H

#include "./kmstool.h"

int kmstool_lib_init(struct kmstool_lib_ctx *ctx, const struct kmstool_init_params *params);
int kmstool_lib_clean_up(struct kmstool_lib_ctx *ctx);
int kmstool_lib_update_aws_key(struct kmstool_lib_ctx *ctx, const struct kmstool_update_aws_key_params *params);

int kmstool_lib_list_key_policies(
    struct kmstool_lib_ctx *ctx,
    const struct kmstool_list_key_policies_params *params,
    unsigned int *response_json_len,
    unsigned char **response_json_out);

int kmstool_lib_get_key_policy(
    struct kmstool_lib_ctx *ctx,
    const struct kmstool_get_key_policy_params *params,
    unsigned int *response_json_len,
    unsigned char **response_json_out);

int kmstool_lib_encrypt(
    struct kmstool_lib_ctx *ctx,
    const struct kmstool_encrypt_params *params,
    unsigned int *ciphertext_out_len,
    unsigned char **ciphertext_out);

int kmstool_lib_decrypt(
    struct kmstool_lib_ctx *ctx,
    const struct kmstool_decrypt_params *params,
    unsigned int *plaintext_out_len,
    unsigned char **plaintext_out);

int kmstool_lib_get_attestation_document(
    struct kmstool_lib_ctx *ctx,
    unsigned int *response_json_len,
    unsigned char **response_json_out);

#endif // KMS_TOOL_API_H

#include "../include/kmstool_decrypt.h"

/* Decrypt the given ciphertext using KMS and store the result in the plaintext buffer */
static int decrypt_from_kms(
    struct kmstool_lib_ctx *ctx,
    const struct kmstool_decrypt_params *params,
    struct aws_byte_buf *plaintext) {
    ssize_t rc = AWS_OP_ERR;

    log_info("decrypt from kms");

    rc = kms_client_check_and_update(ctx);
    if (rc != KMSTOOL_SUCCESS) {
        log_error("kms client connection is not established");
        return rc;
    }

    struct aws_byte_buf ciphertext = aws_byte_buf_from_array(params->ciphertext, params->ciphertext_len);
    struct aws_string *kms_key_id = aws_string_new_from_c_str(ctx->allocator, params->kms_key_id);
    struct aws_string *kms_algorithm = aws_string_new_from_c_str(ctx->allocator, params->kms_algorithm);

    /* Decrypt the data with KMS using the configured key and algorithm */
    rc = aws_kms_decrypt_blocking(ctx->kms_client, kms_key_id, kms_algorithm, &ciphertext, plaintext);
    aws_byte_buf_clean_up_secure(&ciphertext);
    aws_string_destroy(kms_key_id);
    aws_string_destroy(kms_algorithm);
    if (rc != AWS_OP_SUCCESS) {
        log_error("could not decrypt ciphertext");
        return rc;
    }

    return AWS_OP_SUCCESS;
}

int kmstool_lib_decrypt(
    struct kmstool_lib_ctx *ctx,
    const struct kmstool_decrypt_params *params,
    unsigned char **plaintext_out,
    unsigned int *plaintext_out_len) {
    ssize_t rc = AWS_OP_ERR;

    log_info("decrypt");

    if (params->ciphertext == NULL || params->ciphertext_len == 0) {
        log_error("ciphertext should not be NULL or empty");
        *plaintext_out = NULL;
        *plaintext_out_len = 0;
        return KMSTOOL_ERROR;
    }

    if (params->kms_key_id == NULL || params->kms_algorithm == NULL) {
        log_error("kms key id or algorithm should not be NULL");
        *plaintext_out = NULL;
        *plaintext_out_len = 0;
        return KMSTOOL_ERROR;
    }

    struct aws_byte_buf plaintext_buf = {0};
    rc = decrypt_from_kms(ctx, params, &plaintext_buf);
    if (rc != AWS_OP_SUCCESS) {
        log_error("kms decryption failed");
        *plaintext_out = NULL;
        *plaintext_out_len = 0;
        return rc;
    }

    uint8_t *output = malloc(plaintext_buf.len);
    if (output == NULL) {
        log_error("failed to allocate memory for plaintext output");
        aws_byte_buf_clean_up_secure(&plaintext_buf);
        *plaintext_out = NULL;
        *plaintext_out_len = 0;
        return KMSTOOL_ERROR;
    }

    memcpy(output, plaintext_buf.buffer, plaintext_buf.len);
    *plaintext_out = output;
    *plaintext_out_len = plaintext_buf.len;
    aws_byte_buf_clean_up_secure(&plaintext_buf);
    return KMSTOOL_SUCCESS;
}
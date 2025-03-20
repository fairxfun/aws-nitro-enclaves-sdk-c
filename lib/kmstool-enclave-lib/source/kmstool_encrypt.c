#include "../include/kmstool_encrypt.h"

#define MAX_ENCRYPT_DATA_SIZE 4096

/* Encrypt the given plaintext using KMS and store the result in the ciphertext buffer */
static int encrypt_from_kms(
    struct kmstool_lib_ctx *ctx,
    const struct kmstool_encrypt_params *params,
    struct aws_byte_buf *ciphertext) {
    ssize_t rc = AWS_OP_ERR;

    log_info("encrypt from kms");

    struct aws_byte_buf plaintext = aws_byte_buf_from_array(params->plaintext, params->plaintext_len);
    struct aws_string *kms_key_id = aws_string_new_from_c_str(ctx->allocator, params->kms_key_id);

    /* Encrypt the data with KMS using the configured key and algorithm */
    rc = aws_kms_encrypt_blocking(ctx->kms_client, kms_key_id, &plaintext, ciphertext);
    aws_byte_buf_clean_up_secure(&plaintext);
    aws_string_destroy(kms_key_id);
    if (rc != AWS_OP_SUCCESS) {
        log_error("could not encrypt plaintext");
        return rc;
    }

    return AWS_OP_SUCCESS;
}

int kmstool_lib_encrypt(
    struct kmstool_lib_ctx *ctx,
    const struct kmstool_encrypt_params *params,
    unsigned char **ciphertext_out,
    unsigned int *ciphertext_out_len) {
    ssize_t rc = AWS_OP_ERR;

    log_info("encrypt");
    rc = kms_client_check_and_update(ctx);
    if (rc != KMSTOOL_SUCCESS) {
        log_error("kms client connection is not established");
        return rc;
    }

    if (params->plaintext == NULL || params->plaintext_len == 0) {
        log_error("plaintext should not be NULL or empty");
        *ciphertext_out = NULL;
        *ciphertext_out_len = 0;
        return KMSTOOL_ERROR;
    }

    if (params->kms_key_id == NULL) {
        log_error("kms key id or algorithm should not be NULL");
        *ciphertext_out = NULL;
        *ciphertext_out_len = 0;
        return KMSTOOL_ERROR;
    }

    if (params->plaintext_len > MAX_ENCRYPT_DATA_SIZE) {
        log_error("plaintext too large");
        *ciphertext_out = NULL;
        *ciphertext_out_len = 0;
        return KMSTOOL_ERROR;
    }

    struct aws_byte_buf ciphertext_buf = {0};
    rc = encrypt_from_kms(ctx, params, &ciphertext_buf);
    if (rc != AWS_OP_SUCCESS) {
        log_error("kms encryption failed");
        *ciphertext_out = NULL;
        *ciphertext_out_len = 0;
        return rc;
    }

    uint8_t *output = malloc(ciphertext_buf.len);
    if (output == NULL) {
        log_error("failed to allocate memory for ciphertext output");
        aws_byte_buf_clean_up_secure(&ciphertext_buf);
        *ciphertext_out = NULL;
        *ciphertext_out_len = 0;
        return KMSTOOL_ERROR;
    }

    memcpy(output, ciphertext_buf.buffer, ciphertext_buf.len);
    *ciphertext_out = output;
    *ciphertext_out_len = ciphertext_buf.len;
    aws_byte_buf_clean_up_secure(&ciphertext_buf);
    return KMSTOOL_SUCCESS;
}

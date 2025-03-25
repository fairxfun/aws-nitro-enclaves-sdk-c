#include "../include/kmstool_api.h"

static int get_attestation_document(
    struct kmstool_lib_ctx *ctx,
    struct aws_byte_buf *response) {
    
    log_info("attestation document");

    ssize_t rc = kms_client_check_and_update(ctx);
    if (rc != KMSTOOL_SUCCESS) {
        log_error("kms client connection is not established");
        return rc;
    }
    
    rc = aws_attestation_request(ctx->allocator, 
        ctx->kms_client->keypair,
        response);
    if (rc != AWS_OP_SUCCESS) {
        log_error("failed to get attestation document");
        return KMSTOOL_ERROR;
    }

    return KMSTOOL_SUCCESS;
}

int kmstool_lib_get_attestation_document(
    struct kmstool_lib_ctx *ctx,
    unsigned int *response_len,
    unsigned char **response_out) {
    log_info("get attestation document");

    ssize_t rc = kms_client_check_and_update(ctx);
    if (rc != KMSTOOL_SUCCESS) {
        log_error("kms client connection is not established");
        return rc;
    }

    struct aws_byte_buf response_buf = {0};
    rc = get_attestation_document(ctx, &response_buf);
    if (rc != AWS_OP_SUCCESS) {
        log_error("failed to get attestation document");
        return KMSTOOL_ERROR;
    }

    uint8_t *output = malloc(response_buf.len);
    if (output == NULL) {
        log_error("failed to allocate memory for attestation document output");
        return KMSTOOL_ERROR;
    }

    memcpy(output, response_buf.buffer, response_buf.len);
    *response_out = output;
    *response_len = response_buf.len;
    return KMSTOOL_SUCCESS;
}

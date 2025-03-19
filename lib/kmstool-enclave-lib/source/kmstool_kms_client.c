#include "../include/kmstool.h"
#include <unistd.h>

/* Initialize KMS client with AWS credentials and vsock endpoint configuration */
int kms_client_init(struct app_ctx *ctx) {
    log_info("initializing kms client");

    if (ctx->kms_client != NULL || ctx->aws_credentials != NULL) {
        log_error("kms client has already been initialized");
        return AWS_OP_SUCCESS;
    }

    /* Configure vsock endpoint for parent enclave communication */
    struct aws_socket_endpoint endpoint = {.address = DEFAULT_PARENT_CID, .port = ctx->proxy_port};
    struct aws_nitro_enclaves_kms_client_configuration configuration = {
        .allocator = ctx->allocator, .endpoint = &endpoint, .domain = AWS_SOCKET_VSOCK, .region = ctx->aws_region};

    /* Create AWS credentials and KMS client */
    struct aws_credentials *new_credentials = aws_credentials_new(
        ctx->allocator,
        aws_byte_cursor_from_c_str((const char *)ctx->aws_access_key_id->bytes),
        aws_byte_cursor_from_c_str((const char *)ctx->aws_secret_access_key->bytes),
        aws_byte_cursor_from_c_str((const char *)ctx->aws_session_token->bytes),
        UINT64_MAX);
    configuration.credentials = new_credentials;

    ctx->kms_client = aws_nitro_enclaves_kms_client_new(&configuration);
    if (ctx->kms_client == NULL) {
        log_error("failed to create KMS client by nitro");
        aws_credentials_release(new_credentials);
        new_credentials = NULL;
        return AWS_OP_ERR;
    }

    ctx->aws_credentials = new_credentials;
    return AWS_OP_SUCCESS;
}

int kms_client_destroy(struct app_ctx *ctx) {
    log_info("destroying kms client");

    if (ctx->kms_client != NULL) {
        aws_nitro_enclaves_kms_client_destroy(ctx->kms_client);
        ctx->kms_client = NULL;
    }

    if (ctx->aws_credentials != NULL) {
        aws_credentials_release(ctx->aws_credentials);
        ctx->aws_credentials = NULL;
    }

    return KMSTOOL_SUCCESS;
}

int kms_client_update(struct app_ctx *ctx) {
    log_info("update kms client");

    ssize_t rc = kms_client_destroy(ctx);
    if (rc != KMSTOOL_SUCCESS) {
        log_error("failed to destroy kms client");
        return KMSTOOL_ERROR;
    }

    rc = kms_client_init(ctx);
    if (rc != AWS_OP_SUCCESS) {
        log_error("failed to initialize kms client");
        return KMSTOOL_ERROR;
    }

    return KMSTOOL_SUCCESS;
}

int kms_client_check_and_update(struct app_ctx *ctx) {
    log_info("kms client check and update");

    if (ctx->kms_client->rest_client->connection != NULL) {
        log_info("kms client connection is established, no need to update");
        return KMSTOOL_SUCCESS;
    }

    return kms_client_update(ctx);
}

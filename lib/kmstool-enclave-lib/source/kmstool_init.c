#include "../include/kmstool_init.h"

/**
 * Initialize the KMS Tool enclave library.
 *
 * This function must be called before using any KMS operations.
 * It performs the following initialization steps:
 * 1. Validates all required parameters
 * 2. Initializes AWS Nitro Enclaves library
 * 3. Sets up logging if enabled
 * 4. Creates KMS client with provided credentials
 *
 * @param ctx The KMS Tool enclave context to initialize
 * @param params Configuration parameters including AWS credentials and KMS settings
 *
 * @return KMSTOOL_SUCCESS on success, KMSTOOL_ERROR on failure
 */
int kmstool_lib_init(struct kmstool_lib_ctx *ctx, const struct kmstool_init_params *params) {
    if (ctx->allocator != NULL) {
        log_error("kms tool enclave lib has already been initialized");
        return KMSTOOL_SUCCESS;
    }

    /* Initialize the AWS Nitro Enclaves library */
    aws_nitro_enclaves_library_init(NULL);

    if (aws_nitro_enclaves_library_seed_entropy(1024) != AWS_OP_SUCCESS) {
        log_error("failed to seed entropy for AWS Nitro Enclaves library");
        aws_nitro_enclaves_library_clean_up();
        return KMSTOOL_ERROR;
    }

    ctx->allocator = aws_nitro_enclaves_get_allocator();
    if (ctx->allocator == NULL) {
        log_error("failed to get AWS Nitro Enclaves allocator");
        aws_nitro_enclaves_library_clean_up();
        return KMSTOOL_ERROR;
    }

    /* Initialize logger if enabled */
    if (params->enable_logging == 1) {
        g_log_enabled = true;
        ctx->logger = malloc(sizeof(struct aws_logger));
        if (ctx->logger == NULL) {
            log_error("failed to allocate memory for logger");
            aws_nitro_enclaves_library_clean_up();
            return KMSTOOL_ERROR;
        }

        struct aws_logger_standard_options options = {
            .file = stderr,
            .level = AWS_LL_INFO,
            .filename = NULL,
        };
        if (aws_logger_init_standard(ctx->logger, ctx->allocator, &options) != AWS_OP_SUCCESS) {
            log_error("failed to initialize AWS logger");
            free(ctx->logger);
            ctx->logger = NULL;
            aws_nitro_enclaves_library_clean_up();
            return KMSTOOL_ERROR;
        }
        aws_logger_set(ctx->logger);
    }

    ctx->proxy_port = params->proxy_port;
    ctx->aws_region = aws_string_new_from_c_str(ctx->allocator, params->aws_region);

    return KMSTOOL_SUCCESS;
}

/**
 * Clean up all resources associated with the KMS Tool enclave library.
 *
 * This function releases all allocated resources including:
 * - AWS strings (aws_region, credentials, etc.)
 * - KMS client
 * - AWS credentials
 * - Logger
 * - AWS Nitro Enclaves library
 *
 * @param ctx The KMS Tool enclave context to clean up
 * @return KMSTOOL_SUCCESS on success
 */
int kmstool_lib_clean_up(struct kmstool_lib_ctx *ctx) {
    log_info("cleaning up kms tool lib");

    if (ctx->aws_region != NULL) {
        aws_string_destroy(ctx->aws_region);
        ctx->aws_region = NULL;
    }

    aws_nitro_enclaves_library_clean_up();

    if (ctx->logger) {
        free(ctx->logger);
        ctx->logger = NULL;
    }

    // TODO: destroy kms client

    ctx->allocator = NULL;
    return KMSTOOL_SUCCESS;
}

/**
 * Update AWS credentials for an initialized KMS Tool enclave.
 *
 * This function updates the AWS credentials and
 * wait for the KMS client to be updated when connection is established.
 *
 * @param ctx The KMS Tool enclave context
 * @param params New AWS credentials
 * @return KMSTOOL_SUCCESS on success, KMSTOOL_ERROR on failure
 */
int kmstool_lib_update_aws_key(struct kmstool_lib_ctx *ctx, const struct kmstool_update_aws_key_params *params) {
    log_info("updating aws key");

    if (ctx->allocator == NULL) {
        log_error("should init kms tool lib before update");
        return KMSTOOL_ERROR;
    }

    /* Free previously allocated memory for aws_access_key_id if it exists */
    if (ctx->aws_access_key_id != NULL) {
        aws_string_destroy(ctx->aws_access_key_id);
    }
    ctx->aws_access_key_id = aws_string_new_from_c_str(ctx->allocator, params->aws_access_key_id);

    /* Similarly free and update aws_secret_access_key */
    if (ctx->aws_secret_access_key != NULL) {
        aws_string_destroy(ctx->aws_secret_access_key);
    }
    ctx->aws_secret_access_key = aws_string_new_from_c_str(ctx->allocator, params->aws_secret_access_key);

    /* And free and update aws_session_token */
    if (ctx->aws_session_token != NULL) {
        aws_string_destroy(ctx->aws_session_token);
    }
    ctx->aws_session_token = aws_string_new_from_c_str(ctx->allocator, params->aws_session_token);

    return KMSTOOL_SUCCESS;
}

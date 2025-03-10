#include "../include/kmstool_init.h"

/* Default parent CID for vsock communication with the parent enclave */
#define DEFAULT_PARENT_CID "3"

/* Initialize app context with the provided parameters */
static void app_ctx_init_with_params(struct app_ctx *ctx, const struct kmstool_init_params *params) {
    ctx->proxy_port = params->proxy_port;
    ctx->aws_region = aws_string_new_from_c_str(ctx->allocator, params->aws_region);
    ctx->aws_access_key_id = aws_string_new_from_c_str(ctx->allocator, params->aws_access_key_id);
    ctx->aws_secret_access_key = aws_string_new_from_c_str(ctx->allocator, params->aws_secret_access_key);
    ctx->aws_session_token = aws_string_new_from_c_str(ctx->allocator, params->aws_session_token);
    ctx->kms_key_id = aws_string_new_from_c_str(ctx->allocator, params->kms_key_id);
    ctx->kms_encryption_algorithm = aws_string_new_from_c_str(ctx->allocator, params->kms_encryption_algorithm);
}

/* Initialize KMS client with AWS credentials and vsock endpoint configuration */
static int kms_client_init(struct app_ctx *ctx) {
    if (ctx->kms_client != NULL || ctx->aws_credentials != NULL) {
        fprintf(stderr, "Error: KMS client has already been initialized\n");
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
        fprintf(stderr, "Error: Failed to create KMS client: %s\n", aws_error_str(aws_last_error()));
        aws_credentials_release(new_credentials);
        new_credentials = NULL;
        return AWS_OP_ERR;
    }

    ctx->aws_credentials = new_credentials;
    return AWS_OP_SUCCESS;
}

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
int app_lib_init(struct app_ctx *ctx, const struct kmstool_init_params *params) {
    if (ctx->allocator != NULL || ctx->kms_client != NULL) {
        fprintf(stderr, "KMS tool enclave lib has already been initialized\n");
        return KMSTOOL_SUCCESS;
    }

    /* Validate required parameters */
    if (params->aws_region == NULL) {
        fprintf(stderr, "Error: AWS region parameter is NULL\n");
        return KMSTOOL_ERROR;
    }

    if (params->aws_access_key_id == NULL) {
        fprintf(stderr, "Error: AWS access key ID parameter is NULL\n");
        return KMSTOOL_ERROR;
    }

    if (params->aws_secret_access_key == NULL) {
        fprintf(stderr, "Error: AWS secret access key parameter is NULL\n");
        return KMSTOOL_ERROR;
    }

    if (params->aws_session_token == NULL) {
        fprintf(stderr, "Error: AWS session token parameter is NULL\n");
        return KMSTOOL_ERROR;
    }

    if (params->kms_key_id == NULL) {
        fprintf(stderr, "Error: KMS key ID parameter is NULL\n");
        return KMSTOOL_ERROR;
    }

    if (params->kms_encryption_algorithm == NULL) {
        fprintf(stderr, "Error: KMS algorithm parameter is NULL\n");
        return KMSTOOL_ERROR;
    }

    /* Initialize the AWS Nitro Enclaves library */
    aws_nitro_enclaves_library_init(NULL);

    if (aws_nitro_enclaves_library_seed_entropy(1024) != AWS_OP_SUCCESS) {
        fprintf(stderr, "Error: Failed to seed entropy for AWS Nitro Enclaves library\n");
        aws_nitro_enclaves_library_clean_up();
        return KMSTOOL_ERROR;
    }

    ctx->allocator = aws_nitro_enclaves_get_allocator();
    if (ctx->allocator == NULL) {
        fprintf(stderr, "Error: Failed to get AWS Nitro Enclaves allocator\n");
        aws_nitro_enclaves_library_clean_up();
        return KMSTOOL_ERROR;
    }

    /* Initialize logger if enabled */
    if (params->enable_logging == 1) {
        ctx->logger = malloc(sizeof(struct aws_logger));
        if (ctx->logger == NULL) {
            fprintf(stderr, "Error: Failed to allocate memory for logger\n");
            aws_nitro_enclaves_library_clean_up();
            return KMSTOOL_ERROR;
        }
        struct aws_logger_standard_options options = {
            .file = stderr,
            .level = AWS_LL_INFO,
            .filename = NULL,
        };
        if (aws_logger_init_standard(ctx->logger, ctx->allocator, &options) != AWS_OP_SUCCESS) {
            fprintf(stderr, "Error: Failed to initialize AWS logger\n");
            free(ctx->logger);
            ctx->logger = NULL;
            aws_nitro_enclaves_library_clean_up();
            return KMSTOOL_ERROR;
        }
        aws_logger_set(ctx->logger);
    }

    /* Initialize context with provided parameters */
    app_ctx_init_with_params(ctx, params);

    /* Initialize KMS client */
    ssize_t rc = kms_client_init(ctx);
    if (rc != AWS_OP_SUCCESS) {
        fprintf(stderr, "Error: Failed to initialize KMS client: %s\n", aws_error_str(aws_last_error()));
        app_lib_clean_up(ctx);
        return KMSTOOL_ERROR;
    }

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
int app_lib_clean_up(struct app_ctx *ctx) {
    if (ctx->aws_region) {
        aws_string_destroy(ctx->aws_region);
        ctx->aws_region = NULL;
    }

    if (ctx->aws_access_key_id) {
        aws_string_destroy(ctx->aws_access_key_id);
        ctx->aws_access_key_id = NULL;
    }

    if (ctx->aws_secret_access_key) {
        aws_string_destroy(ctx->aws_secret_access_key);
        ctx->aws_secret_access_key = NULL;
    }

    if (ctx->aws_session_token) {
        aws_string_destroy(ctx->aws_session_token);
        ctx->aws_session_token = NULL;
    }

    if (ctx->kms_key_id) {
        aws_string_destroy(ctx->kms_key_id);
        ctx->kms_key_id = NULL;
    }

    if (ctx->kms_encryption_algorithm) {
        aws_string_destroy(ctx->kms_encryption_algorithm);
        ctx->kms_encryption_algorithm = NULL;
    }

    if (ctx->kms_client != NULL) {
        aws_nitro_enclaves_kms_client_destroy(ctx->kms_client);
        ctx->kms_client = NULL;
    }

    if (ctx->aws_credentials != NULL) {
        aws_credentials_release(ctx->aws_credentials);
        ctx->aws_credentials = NULL;
    }

    aws_nitro_enclaves_library_clean_up();

    if (ctx->logger) {
        free(ctx->logger);
        ctx->logger = NULL;
    }

    ctx->allocator = NULL;
    return KMSTOOL_SUCCESS;
}

/**
 * Update AWS credentials for an initialized KMS Tool enclave.
 *
 * This function updates the AWS credentials and reinitializes the KMS client.
 * The enclave must be initialized before calling this function.
 *
 * @param ctx The KMS Tool enclave context
 * @param params New AWS credentials
 * @return KMSTOOL_SUCCESS on success, KMSTOOL_ERROR on failure
 */
int app_lib_update_aws_key(struct app_ctx *ctx, const struct kmstool_update_aws_key_params *params) {
    if (ctx->allocator == NULL) {
        fprintf(stderr, "should init kms tool lib before update\n");
        return KMSTOOL_ERROR;
    }

    /* Free previously allocated memory for aws_access_key_id if it exists */
    if (ctx->aws_access_key_id) {
        aws_string_destroy(ctx->aws_access_key_id);
    }
    ctx->aws_access_key_id = aws_string_new_from_c_str(ctx->allocator, params->aws_access_key_id);

    /* Similarly free and update aws_secret_access_key */
    if (ctx->aws_secret_access_key) {
        aws_string_destroy(ctx->aws_secret_access_key);
    }
    ctx->aws_secret_access_key = aws_string_new_from_c_str(ctx->allocator, params->aws_secret_access_key);

    /* And free and update aws_session_token */
    if (ctx->aws_session_token) {
        aws_string_destroy(ctx->aws_session_token);
    }
    ctx->aws_session_token = aws_string_new_from_c_str(ctx->allocator, params->aws_session_token);

    if (ctx->kms_client != NULL) {
        aws_nitro_enclaves_kms_client_destroy(ctx->kms_client);
        ctx->kms_client = NULL;
    }

    if (ctx->aws_credentials != NULL) {
        aws_credentials_release(ctx->aws_credentials);
        ctx->aws_credentials = NULL;
    }

    ssize_t rc = AWS_OP_ERR;
    rc = kms_client_init(ctx);
    if (rc != AWS_OP_SUCCESS) {
        fprintf(stderr, "failed to update kms client \n");
        return KMSTOOL_ERROR;
    }

    return KMSTOOL_SUCCESS;
}

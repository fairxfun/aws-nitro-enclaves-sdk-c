#include "../include/kmstool_api.h"

int query_key_policies_from_kms(
    struct kmstool_lib_ctx *ctx,
    const struct kmstool_list_key_policies_params *params,
    struct aws_byte_buf *key_policies_json) {
    ssize_t rc = AWS_OP_ERR;

    log_info("querying key policies from kms");
    
    rc = kms_client_check_and_update(ctx);
    if (rc != KMSTOOL_SUCCESS) {
        log_error("kms client connection is not established");
        return rc;
    }

    struct aws_string *key_id = aws_string_new_from_c_str(ctx->allocator, params->key_id);
    struct aws_string *marker = NULL;
    if (params->marker != NULL) {
        marker = aws_string_new_from_c_str(ctx->allocator, params->marker);
    }

    rc = aws_kms_list_key_policies_blocking(ctx->kms_client, key_id, params->limit, marker, key_policies_json);
    aws_string_destroy(key_id);
    if (marker != NULL) {
        aws_string_destroy(marker);
    }

    if (rc != AWS_OP_SUCCESS) {
        log_error("could not list key policies");
        return rc;
    }

    return AWS_OP_SUCCESS;
}

int kmstool_lib_list_key_policies(
    struct kmstool_lib_ctx *ctx,
    const struct kmstool_list_key_policies_params *params,
    unsigned int *response_json_len,
    unsigned char **response_json_out) {

    log_info("listing key policies");

    if (params->key_id == NULL) {
        log_error("key_id is NULL");
        return KMSTOOL_ERROR;
    }

    if (params->limit < 1 || params->limit > 1000) {
        log_error("limit is out of range");
        return KMSTOOL_ERROR;
    }

    struct aws_byte_buf key_policies_json = {0};
    ssize_t rc = query_key_policies_from_kms(ctx, params, &key_policies_json);
    if (rc != AWS_OP_SUCCESS) {
        log_error("could not list key policies");
        return rc;
    }

    uint8_t *output = malloc(key_policies_json.len);
    if (output == NULL) {
        log_error("failed to allocate memory for key policies output");
        aws_byte_buf_clean_up_secure(&key_policies_json);
        *response_json_out = NULL;
        *response_json_len = 0;
        return KMSTOOL_ERROR;
    }

    memcpy(output, key_policies_json.buffer, key_policies_json.len);
    *response_json_out = output;
    *response_json_len = key_policies_json.len;
    aws_byte_buf_clean_up_secure(&key_policies_json);

    return KMSTOOL_SUCCESS;
}

int query_key_policy_from_kms(
    struct kmstool_lib_ctx *ctx,
    const struct kmstool_get_key_policy_params *params,
    struct aws_byte_buf *key_policy_json) {
    ssize_t rc = AWS_OP_ERR;

    log_info("querying key policy from kms");

    rc = kms_client_check_and_update(ctx);
    if (rc != KMSTOOL_SUCCESS) {
        log_error("kms client connection is not established");
        return rc;
    }

    struct aws_string *key_id = aws_string_new_from_c_str(ctx->allocator, params->key_id);
    struct aws_string *policy_name = aws_string_new_from_c_str(ctx->allocator, params->policy_name);

    rc = aws_kms_get_key_policy_blocking(ctx->kms_client, key_id, policy_name, key_policy_json);
    aws_string_destroy(key_id);
    aws_string_destroy(policy_name);

    if (rc != AWS_OP_SUCCESS) {
        log_error("could not get key policy");
        return rc;
    }

    return AWS_OP_SUCCESS;
}

int kmstool_lib_get_key_policy(
    struct kmstool_lib_ctx *ctx,
    const struct kmstool_get_key_policy_params *params,
    unsigned int *response_json_len,
    unsigned char **response_json_out) {

    log_info("getting key policy");

    if (params->key_id == NULL) {   
        log_error("key_id is NULL");
        return KMSTOOL_ERROR;
    }

    if (params->policy_name == NULL) {
        log_error("policy_name is NULL");
        return KMSTOOL_ERROR;
    }

    struct aws_byte_buf key_policy_json = {0};
    ssize_t rc = query_key_policy_from_kms(ctx, params, &key_policy_json);
    if (rc != AWS_OP_SUCCESS) {
        log_error("could not get key policy");
        return rc;
    }
    
    uint8_t *output = malloc(key_policy_json.len);
    if (output == NULL) {
        log_error("failed to allocate memory for key policy output");
        aws_byte_buf_clean_up_secure(&key_policy_json);
        *response_json_out = NULL;
        *response_json_len = 0;
        return KMSTOOL_ERROR;
    }

    memcpy(output, key_policy_json.buffer, key_policy_json.len);
    *response_json_out = output;
    *response_json_len = key_policy_json.len;
    aws_byte_buf_clean_up_secure(&key_policy_json);

    return KMSTOOL_SUCCESS;
}
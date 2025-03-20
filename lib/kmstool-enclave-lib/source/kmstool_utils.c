#include "../include/kmstool.h"

bool g_log_enabled = false;

/* Encode the given text buffer to base64 and store it in text_b64 */
int encode_b64(const struct kmstool_lib_ctx *ctx, const struct aws_byte_buf *text, struct aws_byte_buf *text_b64) {
    log_info("encoding text to base64");

    ssize_t rc = AWS_OP_ERR;
    size_t text_b64_len;

    aws_base64_compute_encoded_len(text->len, &text_b64_len);
    rc = aws_byte_buf_init(text_b64, ctx->allocator, text_b64_len + 1);
    if (rc != AWS_OP_SUCCESS) {
        log_error("memory allocation error");
        return rc;
    }

    struct aws_byte_cursor text_cursor = aws_byte_cursor_from_buf(text);
    rc = aws_base64_encode(&text_cursor, text_b64);
    if (rc != AWS_OP_SUCCESS) {
        aws_byte_buf_clean_up_secure(text_b64);
        log_error("base64 encoding error");
        return rc;
    }

    aws_byte_buf_append_null_terminator(text_b64);
    return AWS_OP_SUCCESS;
}

/* Decord the given text buffer from base64 and store it in text */
int decode_b64(const struct kmstool_lib_ctx *ctx, const struct aws_string *text_b64, struct aws_byte_buf *text) {
    log_info("decoding text from base64");

    ssize_t rc = AWS_OP_ERR;
    size_t text_len;

    struct aws_byte_cursor text_b64_cursor = aws_byte_cursor_from_c_str((const char *)text_b64->bytes);
    rc = aws_base64_compute_decoded_len(&text_b64_cursor, &text_len);
    if (rc != AWS_OP_SUCCESS) {
        log_error("text not a base64 string");
        return rc;
    }

    rc = aws_byte_buf_init(text, ctx->allocator, text_len);
    if (rc != AWS_OP_SUCCESS) {
        log_error("failed init ciphertext");
        return rc;
    }

    rc = aws_base64_decode(&text_b64_cursor, text);
    if (rc != AWS_OP_SUCCESS) {
        aws_byte_buf_clean_up_secure(text);
        log_error("ciphertext not a base64 string");
        return rc;
    }

    return AWS_OP_SUCCESS;
}

inline void log_info(const char *message) {
    if (g_log_enabled) {
        fprintf(stderr, "kmstool lib info: %s\n", message);
    }
}

inline void log_error(const char *message) {
    fprintf(stderr, "kmstool lib error: %s\n", message);
}
#ifndef KMSTOOL_UTILS_H
#define KMSTOOL_UTILS_H

extern bool g_log_enabled;

int encode_b64(const struct app_ctx *ctx, const struct aws_byte_buf *text, struct aws_byte_buf *text_b64);
int decode_b64(const struct app_ctx *ctx, const struct aws_string *text_b64, struct aws_byte_buf *text);
void log_info(const char *message);
void log_error(const char *message);

#endif // KMSTOOL_UTILS_H

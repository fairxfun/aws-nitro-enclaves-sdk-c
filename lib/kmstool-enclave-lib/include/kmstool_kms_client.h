#ifndef KMSTOOL_KMS_CLIENT_H
#define KMSTOOL_KMS_CLIENT_H

#include "./kmstool_type.h"

int kms_client_init(struct app_ctx *ctx);
int kms_client_destroy(struct app_ctx *ctx);
int kms_client_update(struct app_ctx *ctx);
int kms_client_check_and_update(struct app_ctx *ctx);

#endif // KMSTOOL_KMS_CLIENT_H

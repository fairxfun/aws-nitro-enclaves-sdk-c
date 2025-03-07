#ifndef KMSTOOL_INIT_H
#define KMSTOOL_INIT_H

#include "./kmstool.h"

int app_lib_init(struct app_ctx *ctx, const struct kmstool_init_params *params);
int app_lib_clean_up(struct app_ctx *ctx);
int app_lib_update_aws_key(struct app_ctx *ctx, const struct kmstool_update_aws_key_params *params);

#endif // KMSTOOL_INIT_H
#ifndef KMSTOOL_INIT_H
#define KMSTOOL_INIT_H

#include "./kmstool.h"

int kmstool_lib_init(struct kmstool_lib_ctx *ctx, const struct kmstool_init_params *params);
int kmstool_lib_clean_up(struct kmstool_lib_ctx *ctx);
int kmstool_lib_update_aws_key(struct kmstool_lib_ctx *ctx, const struct kmstool_update_aws_key_params *params);

#endif // KMSTOOL_INIT_H
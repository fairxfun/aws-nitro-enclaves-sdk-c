# Copyright 2021 Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
ARG BASE_IMAGE=public.ecr.aws/amazonlinux/amazonlinux:2023

FROM $BASE_IMAGE as VAULTRON_ENCLAVE

#RUN yum install -y ca-certificates && update-ca-trust

RUN mkdir /fairx

#RUN rpm -e python python-libs python-urlgrabber python2-rpm pygpgme pyliblzma python-iniparse pyxattr python-pycurl amazon-linux-extras yum yum-metadata-parser yum-plugin-ovl yum-plugin-priorities
COPY ./libnsm.so /usr/lib64/libnsm.so
COPY ./libkmstool-enclave-lib.so /usr/lib64/libkmstool-enclave-lib.so
COPY ./kmstool_enclave_lib_test /fairx/kmstool_enclave_lib_test

WORKDIR /fairx
CMD ["/fairx/kmstool_enclave_lib_test"]

#include "kmstool_enclave_lib.h"
#include "include/kmstool_api.h"

/**
 * @file kmstool_enclave_lib.c
 * @brief Implementation of the AWS KMS Tool Enclave Library
 *
 * This file implements the public interface for the AWS KMS Tool Enclave Library.
 * It provides a secure way to perform KMS operations within Nitro Enclaves by
 * managing a global application context and delegating operations to the
 * appropriate internal modules.
 */

/* Global static application context for managing KMS operations */
static struct kmstool_lib_ctx g_ctx = {0};

/* Define API export macro for different platforms */
#ifdef _WIN32
#    define API_EXPORT __declspec(dllexport)
#else
#    define API_EXPORT __attribute__((visibility("default")))
#endif

/**
 * @brief Initialize the KMS Tool enclave
 *
 * Initializes the global application context with the provided parameters
 * and sets up all necessary resources for KMS operations.
 *
 * @param params Configuration parameters including AWS credentials and settings
 * @return KMSTOOL_SUCCESS on success, KMSTOOL_ERROR on failure
 */
API_EXPORT int kmstool_enclave_init(const struct kmstool_init_params *params) {
    return kmstool_lib_init(&g_ctx, params);
}

/**
 * @brief Stop and clean up the KMS Tool enclave
 *
 * Releases all resources associated with the global application context
 * and performs necessary cleanup operations.
 *
 * @return KMSTOOL_SUCCESS on success, KMSTOOL_ERROR on failure
 */
API_EXPORT int kmstool_enclave_stop() {
    return kmstool_lib_clean_up(&g_ctx);
}

/**
 * @brief Update AWS credentials
 *
 * Updates the AWS credentials in the global application context and
 * reinitializes the KMS client with the new credentials.
 *
 * @param params New AWS credentials to use
 * @return KMSTOOL_SUCCESS on success, KMSTOOL_ERROR on failure
 */
API_EXPORT int kmstool_enclave_update_aws_key(const struct kmstool_update_aws_key_params *params) {
    return kmstool_lib_update_aws_key(&g_ctx, params);
}

/**     
 * @brief List key policies for a KMS key
 *
 * This function lists the key policies for a specified KMS key.
 *
 * @param params Pointer to the parameters for the ListKeyPolicies operation
 * @param response_json_out Pointer to store the raw JSON response
 * @param response_json_len Pointer to store the length of the raw JSON response
 * @return KMSTOOL_SUCCESS on success, KMSTOOL_ERROR on failure
 */
API_EXPORT int kmstool_enclave_list_key_policies(
    const struct kmstool_list_key_policies_params *params,
    unsigned int *response_json_len,
    unsigned char **response_json_out) {
    return kmstool_lib_list_key_policies(&g_ctx, params, response_json_len, response_json_out);
}

/**
 * @brief Get key policy for a KMS key
 *
 * This function gets the key policy for a specified KMS key.
 *
 * @param params Pointer to the parameters for the GetKeyPolicy operation
 * @param response_json_out Pointer to store the raw JSON response
 * @param response_json_len Pointer to store the length of the raw JSON response
 * @return KMSTOOL_SUCCESS on success, KMSTOOL_ERROR on failure
 */
API_EXPORT int kmstool_enclave_get_key_policy(
    const struct kmstool_get_key_policy_params *params,
    unsigned int *response_json_len,
    unsigned char **response_json_out) {
    return kmstool_lib_get_key_policy(&g_ctx, params, response_json_len, response_json_out);
}

/**
 * @brief Encrypt data using KMS
 *
 * Encrypts the provided plaintext using the configured KMS key and algorithm.
 * The encrypted data is allocated and must be freed by the caller.
 *
 * @param params Encryption parameters including plaintext data
 * @param ciphertext_out Pointer to store the encrypted data
 * @param ciphertext_out_len Pointer to store the length of encrypted data
 * @return KMSTOOL_SUCCESS on success, KMSTOOL_ERROR on failure
 */
API_EXPORT int kmstool_enclave_encrypt(
    const struct kmstool_encrypt_params *params,
    unsigned int *ciphertext_out_len,
    unsigned char **ciphertext_out) {
    return kmstool_lib_encrypt(&g_ctx, params, ciphertext_out_len, ciphertext_out);
}

/**
 * @brief Decrypt data using KMS
 *
 * Decrypts the provided ciphertext using the configured KMS key and algorithm.
 * The decrypted data is allocated and must be freed by the caller.
 *
 * @param params Decryption parameters including ciphertext data
 * @param plaintext_out Pointer to store the decrypted data
 * @param plaintext_out_len Pointer to store the length of decrypted data
 * @return KMSTOOL_SUCCESS on success, KMSTOOL_ERROR on failure
 */
API_EXPORT int kmstool_enclave_decrypt(
    const struct kmstool_decrypt_params *params,
    unsigned int *plaintext_out_len,
    unsigned char **plaintext_out) {
    return kmstool_lib_decrypt(&g_ctx, params, plaintext_out_len, plaintext_out);
}

/**
 * @brief Get attestation document for the enclave
 *
 * This function gets the attestation document for the enclave.
 */
API_EXPORT int kmstool_enclave_get_attestation_document(unsigned int *response_json_len, unsigned char **response_json_out) {
    return kmstool_lib_get_attestation_document(&g_ctx, response_json_len, response_json_out);
}

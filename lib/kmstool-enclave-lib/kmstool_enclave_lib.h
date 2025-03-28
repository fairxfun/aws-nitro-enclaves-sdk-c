#ifndef KMSTOOL_ENCLAVE_LIB_H
#define KMSTOOL_ENCLAVE_LIB_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file kmstool_enclave_lib.h
 * @brief Public interface for the AWS KMS Tool Enclave Library
 *
 * This library provides a secure interface for AWS KMS operations within Nitro Enclaves.
 * It handles encryption, decryption, and key management operations using AWS KMS,
 * while maintaining the security guarantees provided by the enclave environment.
 */

/**
 * @brief Status codes for KMS Tool operations
 *
 * These status codes indicate the result of KMS Tool operations.
 * All functions in this library return one of these values.
 */
enum KMSTOOL_STATUS {
    KMSTOOL_ERROR = -1,  /* Operation failed */
    KMSTOOL_SUCCESS = 0, /* Operation succeeded */
};

/**
 * @brief Initialization parameters for the KMS Tool enclave
 *
 * This structure contains all the necessary parameters to initialize
 * the KMS Tool enclave, including AWS credentials, region settings,
 * and encryption parameters.
 */
struct kmstool_init_params {
    const char *aws_region;            /* AWS region for KMS operations */
    const unsigned int enable_logging; /* Enable logging if set to 1 */
    const unsigned int proxy_port;     /* vsock port on which vsock-proxy is available in parent */
};

/**
 * @brief Parameters for updating AWS credentials
 *
 * This structure contains the parameters needed to update the AWS credentials
 * for an already initialized KMS Tool enclave.
 */
struct kmstool_update_aws_key_params {
    const char *aws_access_key_id;     /* New AWS access key ID */
    const char *aws_secret_access_key; /* New AWS secret access key */
    const char *aws_session_token;     /* New AWS session token */
};

/**
 * Parameters for the ListKeyPolicies operation.
 */
struct kmstool_list_key_policies_params {
    /** The identifier of the KMS key to list policies for */
    const char *key_id;
    /** Maximum number of items to return (-1 for no limit) */
    unsigned int limit;
    /** Pagination token from a previous request (can be NULL) */
    const char *marker;
};

struct kmstool_get_key_policy_params {
    const char *key_id;
    const char *policy_name;
};

/**
 * @brief Parameters for encryption operation
 *
 * This structure contains the data to be encrypted using KMS.
 */
struct kmstool_encrypt_params {
    const char *kms_key_id;           /* KMS key ID to use for operations */
    const unsigned char *plaintext;   /* Data to encrypt */
    const unsigned int plaintext_len; /* Length of data to encrypt */
};

/**
 * @brief Parameters for decryption operation
 *
 * This structure contains the data to be decrypted using KMS.
 */
struct kmstool_decrypt_params {
    const char *kms_key_id;            /* KMS key ID to use for operations */
    const char *kms_algorithm;         /* KMS encryption algorithm to use */
    const unsigned char *ciphertext;   /* Data to decrypt */
    const unsigned int ciphertext_len; /* Length of data to decrypt */
};

/**
 * @brief Initialize the KMS Tool enclave with the given parameters
 *
 * This function must be called before performing any KMS operations.
 * It sets up the AWS credentials, KMS client, and other necessary resources.
 *
 * @param params Pointer to initialization parameters
 * @return KMSTOOL_SUCCESS on success, KMSTOOL_ERROR on failure
 */
int kmstool_enclave_init(const struct kmstool_init_params *params);

/**
 * @brief Clean up and stop the KMS Tool enclave
 *
 * This function releases all resources associated with the KMS Tool enclave.
 * It should be called when the enclave is no longer needed.
 *
 * @return KMSTOOL_SUCCESS on success, KMSTOOL_ERROR on failure
 */
int kmstool_enclave_stop(void);

/**
 * @brief Update AWS credentials for the KMS Tool enclave
 *
 * This function updates the AWS credentials used by the KMS Tool enclave.
 * The enclave must be initialized before calling this function.
 *
 * @param params Pointer to the new AWS credentials
 * @return KMSTOOL_SUCCESS on success, KMSTOOL_ERROR on failure
 */
int kmstool_enclave_update_aws_key(const struct kmstool_update_aws_key_params *params);

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
int kmstool_enclave_list_key_policies(
    const struct kmstool_list_key_policies_params *params,
    unsigned int *response_json_len,
    unsigned char **response_json_out);

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
int kmstool_enclave_get_key_policy(
    const struct kmstool_get_key_policy_params *params,
    unsigned int *response_json_len,
    unsigned char **response_json_out);

/**
 * @brief Encrypt data using KMS
 *
 * This function encrypts the provided plaintext using the configured KMS key
 * and encryption algorithm.
 *
 * @param params Pointer to encryption parameters
 * @param ciphertext_out Pointer to store the encrypted data (caller must free)
 * @param ciphertext_out_len Pointer to store the length of encrypted data
 * @return KMSTOOL_SUCCESS on success, KMSTOOL_ERROR on failure
 */
int kmstool_enclave_encrypt(
    const struct kmstool_encrypt_params *params,
    unsigned int *ciphertext_out_len,
    unsigned char **ciphertext_out);

/**
 * @brief Decrypt data using KMS
 *
 * This function decrypts the provided ciphertext using the configured KMS key
 * and encryption algorithm.
 *
 * @param params Pointer to decryption parameters
 * @param plaintext_out Pointer to store the decrypted data (caller must free)
 * @param plaintext_out_len Pointer to store the length of decrypted data
 * @return KMSTOOL_SUCCESS on success, KMSTOOL_ERROR on failure
 */
int kmstool_enclave_decrypt(
    const struct kmstool_decrypt_params *params,
    unsigned int *plaintext_out_len,
    unsigned char **plaintext_out);

/**
 * @brief Get attestation document for the enclave
 *
 * This function gets the attestation document for the enclave.
 *
 * @param response_json_len Pointer to store the length of the raw JSON response
 * @param response_json_out Pointer to store the raw JSON response
 */
int kmstool_enclave_get_attestation_document(unsigned int *response_json_len, unsigned char **response_json_out);

#ifdef __cplusplus
}
#endif

#endif /* KMSTOOL_ENCLAVE_LIB_H */
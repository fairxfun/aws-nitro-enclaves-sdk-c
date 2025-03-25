# GetKeyPolicy

## Overview
The `GetKeyPolicy` operation retrieves a key policy for the specified KMS key. This operation is useful for viewing the permissions and access controls associated with a KMS key.

**Cross-account use**: No. You cannot perform this operation on a KMS key in a different AWS account.

**Required permissions**: kms:GetKeyPolicy (key policy)

**Related operations**: PutKeyPolicy

**Eventual consistency**: The AWS KMS API follows an eventual consistency model.

## AWS Documentation
[GetKeyPolicy - AWS Key Management Service](https://docs.aws.amazon.com/kms/latest/APIReference/API_GetKeyPolicy.html)

## Function Signature
```c
int aws_kms_get_key_policy_blocking(
    struct aws_nitro_enclaves_kms_client *client,
    const struct aws_string *key_id,
    const struct aws_string *policy_name,
    struct aws_byte_buf *response_json);
```

## Parameters
- `client` (in): The KMS client instance.
- `key_id` (in): The identifier of the KMS key. This can be the key ID or the Amazon Resource Name (ARN) of the key.
  - Key ID format: `1234abcd-12ab-34cd-56ef-1234567890ab`
  - Key ARN format: `arn:aws:kms:us-east-2:111122223333:key/1234abcd-12ab-34cd-56ef-1234567890ab`
  - Length constraints: Minimum length of 1, maximum length of 2048
- `policy_name` (in): The name of the key policy. The only valid value is "default".
  - Length constraints: Minimum length of 1, maximum length of 128
  - Pattern: `[\w]+`
- `response_json` (out): A buffer to store the JSON response from the KMS service.

## Return Value
- Returns `AWS_OP_SUCCESS` on success.
- Returns `AWS_OP_ERR` on failure.

## Example Usage
```c
struct aws_byte_buf response_json;
aws_byte_buf_init(&response_json, allocator, 1024);

struct aws_string *key_id = aws_string_new_from_c_str(allocator, "1234abcd-12ab-34cd-56ef-1234567890ab");
struct aws_string *policy_name = aws_string_new_from_c_str(allocator, "default");

int rc = aws_kms_get_key_policy_blocking(client, key_id, policy_name, &response_json);
if (rc == AWS_OP_SUCCESS) {
    // Process the response JSON
    printf("Policy: %s\n", aws_string_c_str(response_json));
}

aws_byte_buf_clean_up(&response_json);
aws_string_destroy(key_id);
aws_string_destroy(policy_name);
```

## Response Format
The response is a JSON object containing the key policy. The policy is a JSON document that defines the permissions for the KMS key.

Example response:
```json
{
    "Policy": "{\n    \"Version\": \"2012-10-17\",\n    \"Id\": \"key-default-1\",\n    \"Statement\": [ {\n        \"Sid\": \"Enable IAM User Permissions\",\n        \"Effect\": \"Allow\",\n        \"Principal\": {\n            \"AWS\": \"arn:aws:iam::111122223333:root\"\n        },\n        \"Action\": \"kms:*\",\n        \"Resource\": \"*\"\n    } ]\n}",
    "PolicyName": "default"
}
```

## Error Handling
The function will return `AWS_OP_ERR` in the following cases:
- `DependencyTimeoutException`: The system timed out while trying to fulfill the request. You can retry the request.
- `InvalidArnException`: The request was rejected because a specified ARN, or an ARN in a key policy, is not valid.
- `KMSInternalException`: The request was rejected because an internal exception occurred. The request can be retried.
- `KMSInvalidStateException`: The request was rejected because the state of the specified resource is not valid for this request.
- `NotFoundException`: The request was rejected because the specified entity or resource could not be found.

## Notes
- The policy name must be "default" as this is the only supported value.
- The key ID can be provided in either the key ID format or the ARN format.
- The response JSON buffer must be initialized before calling this function.
- The caller is responsible for cleaning up the response buffer and any allocated strings.
- The policy document in the response is a JSON string that needs to be parsed separately. 
# ListKeyPolicies

Gets the names of the key policies that are attached to a KMS key. This operation returns the raw JSON response from AWS KMS, which you can then parse as needed.

## Function Overview

```c
int aws_kms_list_key_policies_blocking(
    struct aws_nitro_enclaves_kms_client *client,
    const struct aws_string *key_id,
    int limit,
    const struct aws_string *marker,
    struct aws_byte_buf *response_json);
```

This function makes a blocking call to the AWS KMS ListKeyPolicies API and returns the raw JSON response. The caller is responsible for parsing the JSON response as needed.

### Parameters

- `client`: The KMS client to use
- `key_id`: The identifier of the key to list policies for
- `limit`: Maximum number of items to return (-1 for no limit)
- `marker`: Pagination token from a previous request (NULL if first request)
- `response_json`: Will receive the raw JSON response string

### Return Value

- `AWS_OP_SUCCESS`: Call succeeded, `response_json` contains the raw response
- `AWS_OP_ERR`: Call failed

### Memory Management

The caller must call `aws_string_destroy` on `*response_json` when done with it.

### Example Usage

```c
struct aws_string *key_id = aws_string_new_from_c_str(allocator, 
    "1234abcd-12ab-34cd-56ef-1234567890ab");
struct aws_byte_buf *response_json = NULL;

int result = aws_kms_list_key_policies_blocking(
    client, key_id, 100, NULL, &response_json);

aws_string_destroy(key_id);
```

## API Details

For more information about key policies in AWS KMS, see [Key Policies](https://docs.aws.amazon.com/kms/latest/developerguide/key-policies.html) in the AWS KMS Developer Guide.

**Cross-account use**: No. You cannot perform this operation on a KMS key in a different AWS account.

Required permissions: [kms:ListKeyPolicies](https://docs.aws.amazon.com/kms/latest/developerguide/kms-api-permissions-reference.html) (key policy)

## Request Format

The request will be sent as JSON with this structure:

```json
{
   "KeyId": "string",
   "Limit": number,
   "Marker": "string"
}
```

### Request Parameters

#### KeyId (required)

The identifier of the KMS key. Can be:
- Key ID: `1234abcd-12ab-34cd-56ef-1234567890ab`
- Key ARN: `arn:aws:kms:us-east-2:111122223333:key/1234abcd-12ab-34cd-56ef-1234567890ab`

#### Limit (optional)

Maximum number of items to return (1-1000). Defaults to 100 if not specified.

#### Marker (optional)

Pagination token from a previous request's `NextMarker` field.

## Response Format

The function returns the raw JSON response string, which has this structure:

```json
{
   "NextMarker": "string",
   "PolicyNames": [ "string" ],
   "Truncated": boolean
}
```

### Response Fields

#### PolicyNames

Array of policy names. Currently, the only valid value is `"default"`.

#### Truncated

Boolean indicating if there are more results. If true, use the `NextMarker` value in a subsequent request.

#### NextMarker

Present only if `Truncated` is true. Use this value in the `marker` parameter of a subsequent request to get the next page of results.

## Error Handling

The function returns `AWS_OP_ERR` on failure. Common errors include:

- InvalidArnException (400): Invalid key ARN
- NotFoundException (400): Key not found
- KMSInvalidStateException (400): Key state is not valid for the operation
- KMSInternalException (500): Internal service error
- DependencyTimeoutException (500): Request timed out

For details, see [Common Errors](https://docs.aws.amazon.com/kms/latest/APIReference/CommonErrors.html) in the AWS KMS API Reference.

**Related operations:**
* [GetKeyPolicy](https://docs.aws.amazon.com/kms/latest/APIReference/API_GetKeyPolicy.html)
* [PutKeyPolicy](https://docs.aws.amazon.com/kms/latest/APIReference/API_PutKeyPolicy.html)

For the complete API reference, see [ListKeyPolicies](https://docs.aws.amazon.com/kms/latest/APIReference/API_ListKeyPolicies.html) in the *AWS Key Management Service API Reference*.

## Request Syntax
```json
{
   "KeyId": "string",
   "Limit": number,
   "Marker": "string"
}
```

## Request Parameters

For information about the parameters that are common to all actions, see [Common Parameters](https://docs.aws.amazon.com/kms/latest/APIReference/CommonParameters.html).

The request accepts the following data in JSON format.
> **Note**
>
> In the following list, the required parameters are described first.

#### KeyId

Gets the names of key policies for the specified KMS key.

Specify the key ID or key ARN of the KMS key.

For example:
* Key ID: `1234abcd-12ab-34cd-56ef-1234567890ab`
* Key ARN: `arn:aws:kms:us-east-2:111122223333:key/1234abcd-12ab-34cd-56ef-1234567890ab`

To get the key ID and key ARN for a KMS key, use [ListKeys](https://docs.aws.amazon.com/kms/latest/APIReference/API_ListKeys.html) or [DescribeKey](https://docs.aws.amazon.com/kms/latest/APIReference/API_DescribeKey.html).

Type: String

Length Constraints: Minimum length of 1. Maximum length of 2048.

Required: Yes

#### Limit

Use this parameter to specify the maximum number of items to return. When this value is present, AWS KMS does not return more than the specified number of items, but it might return fewer.

This value is optional. If you include a value, it must be between 1 and 1000, inclusive. If you do not include a value, it defaults to 100.

Note: Only one policy can be attached to a key.

Type: Integer

Valid Range: Minimum value of 1. Maximum value of 1000.

Required: No

#### Marker

Use this parameter in a subsequent request after you receive a response with truncated results. Set it to the value of `NextMarker` from the truncated response you just received.

Type: String

Length Constraints: Minimum length of 1. Maximum length of 1024.

Pattern: `[\u0020-\u00FF]*`

Required: No

## Response Syntax
```json
{
   "NextMarker": "string",
   "PolicyNames": [ "string" ],
   "Truncated": boolean
}
```

## Response Elements

If the action is successful, the service sends back an HTTP 200 response.

The following data is returned in JSON format by the service.

#### NextMarker

When `Truncated` is true, this element is present and contains the value to use for the `Marker` parameter in a subsequent request.

Type: String

Length Constraints: Minimum length of 1. Maximum length of 1024.

Pattern: `[\u0020-\u00FF]*`

#### PolicyNames

A list of key policy names. The only valid value is `default`.

Type: Array of strings

Length Constraints: Minimum length of 1. Maximum length of 128.

Pattern: `[\w]+`

#### Truncated

A flag that indicates whether there are more items in the list. When this value is true, the list in this response is truncated. To get more items, pass the value of the `NextMarker` element in this response to the `Marker` parameter in a subsequent request.

Type: Boolean

## Errors

For information about the errors that are common to all actions, see [Common Errors](https://docs.aws.amazon.com/kms/latest/APIReference/CommonErrors.html).

#### DependencyTimeoutException

The system timed out while trying to fulfill the request. You can retry the request.

HTTP Status Code: 500

#### InvalidArnException

The request was rejected because a specified ARN, or an ARN in a key policy, is not valid.

HTTP Status Code: 400

#### KMSInternalException

The request was rejected because an internal exception occurred. The request can be retried.

HTTP Status Code: 500

#### KMSInvalidStateException

The request was rejected because the state of the specified resource is not valid for this request.

This exceptions means one of the following:
* The key state of the KMS key is not compatible with the operation.
* For cryptographic operations on KMS keys in custom key stores, this exception represents a general failure with many possible causes.

HTTP Status Code: 400

#### NotFoundException

The request was rejected because the specified entity or resource could not be found.

HTTP Status Code: 400

## Examples

The following examples are formatted for legibility.

### Example Request

This example illustrates one usage of ListKeyPolicies.
```http
POST / HTTP/1.1
Host: kms.us-east-2.amazonaws.com
Content-Length: 49
X-Amz-Target: TrentService.ListKeyPolicies
X-Amz-Date: 20161206T235923Z
Content-Type: application/x-amz-json-1.1
Authorization: AWS4-HMAC-SHA256\
 Credential=AKIAI44QH8DHBEXAMPLE/20161206/us-east-2/kms/aws4_request,\
 SignedHeaders=content-type;host;x-amz-date;x-amz-target,\
 Signature=82fe067c53d0dfff36793b8b6ef2d82d8adf0f1c05016bf4b4d6c50563ec7033

{
    "KeyId": "1234abcd-12ab-34cd-56ef-1234567890ab"
}
```

### Example Response

This example illustrates one usage of ListKeyPolicies.
```http
HTTP/1.1 200 OK
Server: Server
Date: Tue, 06 Dec 2016 23:59:24 GMT
Content-Type: application/x-amz-json-1.1
Content-Length: 45
Connection: keep-alive
x-amzn-RequestId: 036f8e4b-bc10-11e6-b60b-ffb5eb2d1d15

{
    "PolicyNames": ["default"],
    "Truncated": false
}
``` 
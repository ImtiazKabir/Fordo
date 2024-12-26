#include "http_status.h"

#include "imlib/imstdinc.h"

static int const http_status_codes[] = {
    /* 1xx Informational */
    100, 101, 102,
    
    /* 2xx Success */
    200, 201, 202, 203, 204, 205, 206,
    
    /* 3xx Redirection */
    300, 301, 302, 303, 304, 307, 308,
    
    /* 4xx Client Error */
    400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417, 418, 422, 425, 426, 428, 429, 431,
    
    /* 5xx Server Error */
    500, 501, 502, 503, 504, 505, 507, 508, 510, 511,

    /* Default */
    500
};

static char const *const http_status_messages[] = {
    /* 1xx Informational */
    "Continue", "Switching Protocols", "Processing",
    
    /* 2xx Success */
    "OK", "Created", "Accepted", "Non-Authoritative Information", "No Content", "Reset Content", "Partial Content",
    
    /* 3xx Redirection */
    "Multiple Choices", "Moved Permanently", "Found", "See Other", "Not Modified", "Temporary Redirect", "Permanent Redirect",
    
    /* 4xx Client Error */
    "Bad Request", "Unauthorized", "Payment Required", "Forbidden", "Not Found", "Method Not Allowed", "Not Acceptable",
    "Proxy Authentication Required", "Request Timeout", "Conflict", "Gone", "Length Required", "Precondition Failed",
    "Payload Too Large", "URI Too Long", "Unsupported Media Type", "Range Not Satisfiable", "Expectation Failed",
    "I'm a teapot", "Unprocessable Entity", "Too Early", "Upgrade Required", "Precondition Required", "Too Many Requests",
    "Request Header Fields Too Large",
    
    /* 5xx Server Error */
    "Internal Server Error", "Not Implemented", "Bad Gateway", "Service Unavailable", "Gateway Timeout",
    "HTTP Version Not Supported", "Insufficient Storage", "Loop Detected", "Not Extended", "Network Authentication Required",

    /* Default */
    "Internal Server Error"
};

static char const *const http_status[] = {
    /* 1xx Informational */
    "100 Continue", "101 Switching Protocols", "102 Processing",
    
    /* 2xx Success */
    "200 OK", "201 Created", "202 Accepted", "203 Non-Authoritative Information", "204 No Content", "205 Reset Content", "206 Partial Content",
    
    /* 3xx Redirection */
    "300 Multiple Choices", "301 Moved Permanently", "302 Found", "303 See Other", "304 Not Modified", "307 Temporary Redirect", "308 Permanent Redirect",
    
    /* 4xx Client Error */
    "400 Bad Request", "401 Unauthorized", "402 Payment Required", "403 Forbidden", "404 Not Found", "405 Method Not Allowed", "406 Not Acceptable",
    "407 Proxy Authentication Required", "408 Request Timeout", "409 Conflict", "410 Gone", "411 Length Required", "412 Precondition Failed",
    "413 Payload Too Large", "414 URI Too Long", "415 Unsupported Media Type", "416 Range Not Satisfiable", "417 Expectation Failed",
    "418 I'm a teapot", "422 Unprocessable Entity", "425 Too Early", "426 Upgrade Required", "428 Precondition Required", "429 Too Many Requests",
    "431 Request Header Fields Too Large",
    
    /* 5xx Server Error */
    "500 Internal Server Error", "501 Not Implemented", "502 Bad Gateway", "503 Service Unavailable", "504 Gateway Timeout",
    "505 HTTP Version Not Supported", "507 Insufficient Storage", "508 Loop Detected", "510 Not Extended", "511 Network Authentication Required",

    /* Default */
    "500 Internal Server Error"
};


PUBLIC int GetHttpStatusCode(register enum HttpStatusCode const status) {
  if (status > HTTP_STATUS_UNKNOWN) {
    return http_status_codes[HTTP_STATUS_UNKNOWN];
  }
  return http_status_codes[status];
}

PUBLIC char const *GetHttpStatusMessage(register enum HttpStatusCode const status) {
  if (status > HTTP_STATUS_UNKNOWN) {
    return http_status_messages[HTTP_STATUS_UNKNOWN];
  }
  return http_status_messages[status];
}


PUBLIC char const *GetHttpStatus(register enum HttpStatusCode const status) {
  if (status > HTTP_STATUS_UNKNOWN) {
    return http_status[HTTP_STATUS_UNKNOWN];
  }
  return http_status[status];
}


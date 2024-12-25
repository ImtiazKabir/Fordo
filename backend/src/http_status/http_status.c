#include "http_status.h"

int const httpStatusCodes[] = {
    /* 1xx Informational */
    100, 101, 102,
    
    /* 2xx Success */
    200, 201, 202, 203, 204, 205, 206,
    
    /* 3xx Redirection */
    300, 301, 302, 303, 304, 307, 308,
    
    /* 4xx Client Error */
    400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417, 418, 422, 425, 426, 428, 429, 431,
    
    /* 5xx Server Error */
    500, 501, 502, 503, 504, 505, 507, 508, 510, 511
};

char const *const httpStatusMessages[] = {
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
    "HTTP Version Not Supported", "Insufficient Storage", "Loop Detected", "Not Extended", "Network Authentication Required"
};

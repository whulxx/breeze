#include "http.h"
#include "common.h"
#include "stacktrace.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>


char* test_request = 
    "GET /home/hello.do?id=1001&name=hello HTTP/1.1\r\n"
    "Host: www.javaeye.com\r\n"
    "Connection: keep-alive\r\n"
    "Referer: http://www.javaeye.com/\r\n"
    "Cache-Control: max-age=0\r\n"
    "If-Modified-Since: Sat, 15 Dec 2007 04:04:14 GMT\r\n"
    "If-None-Match: \"3922745954\"\r\n"
    "Accept: */*\r\n"
    "User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US) AppleWebKit/534.15 (KHTML, like Gecko) Chrome/10.0.612.1 Safari/534.15\r\n"
    "Accept-Encoding: gzip,deflate,sdch\r\n"
    "Accept-Language: zh-CN,zh;q=0.8\r\n"
    "Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n"
    "Cookie: lzstat_uv=39255935923757216993|1146165@1270080@1632275@1145010@0; remember_me=yes; login_token=MTAyMjExXzJkZjY4ZGIyNDIzZTdjMTE4YmM5OTU0OTQ2MTU0N2Fh%0A; _javaeye3_session_=BAh7BzoMdXNlcl9pZGkDQ48BOg9zZXNzaW9uX2lkIiVjMDdlNjJmZGJiNTFhZjhhYmU5Yjk4NjE1Y2ZjODBhOQ%3D%3D--b9945c8e50a5eba75e75c92b2c92e5a3a86f1000\r\n\r\n";

char* test_request_2 = 
    "GET /home/hello.do?id=1001&name=hello HTTP/1.1\r\n"
    "Host: www.javaeye.com\r\n"
    "Connection: keep-alive\r\n"
    "Referer: http://www.javaeye.com/\r\n"
    "Cache-Control: max-age=0\r\n"
    "If-Modified-Since: Sat, 15 Dec 2007 04:04:14 GMT\r\n"
    "If-None-Match: \"3922745954\"\r\n"
    "Accept: */*\r\n"
    "User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US) AppleWebKit/534.15 (KHTML, like Gecko) Chrome/10.0.612.1 Safari/534.15\r\n"
    "Accept-Encoding: gzip,deflate,sdch\r\n"
    "Accept-Language: zh-CN,zh;q=0.8\r\n"
    "Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n"
    "Cookie: lzstat_uv=39255935923757216993|1146165@1270080@1632275@1145010@0; remember_me=yes; login_token=MTAyMjExXzJkZjY4ZGIyNDIzZTdjMTE4YmM5OTU0OTQ2MTU0N2Fh%0A; _javaeye3_session_=BAh7BzoMdXNlcl9pZGkDQ48BOg9zZXNzaW9uX2lkIiVjMDdlNjJmZGJiNTFhZjhhYmU5Yjk4NjE1Y2ZjODBhOQ%3D%3D--b9945c8e50a5eba75e75c92b2c92e5a3a86f1000\r\n\r\n"
    "GET /home/hello.do?id=1001&name=hello HTTP/1.1\r\n"; // This line is the start of another request

char* invalid_req = 
    "GET /home/hello.do?id=1001&name=hello HTTP/1.9\r\n"
    "Host: www.javaeye.com\r\n"
    "Connection: keep-alive\r\n"
    "Referer: http://www.javaeye.com/\r\n"
    "Cache-Control: max-age=0\r\n"
    "If-Modified-Since: Sat, 15 Dec 2007 04:04:14 GMT\r\n"
    "If-None-Match: \"3922745954\"\r\n"
    "Accept: */*\r\n"
    "User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US) AppleWebKit/534.15 (KHTML, like Gecko) Chrome/10.0.612.1 Safari/534.15\r\n"
    "Accept-Encoding: gzip,deflate,sdch\r\n"
    "Accept-Language: zh-CN,zh;q=0.8\r\n"
    "Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n"
    "Cookie: lzstat_uv=39255935923757216993|1146165@1270080@1632275@1145010@0; remember_me=yes; login_token=MTAyMjExXzJkZjY4ZGIyNDIzZTdjMTE4YmM5OTU0OTQ2MTU0N2Fh%0A; _javaeye3_session_=BAh7BzoMdXNlcl9pZGkDQ48BOg9zZXNzaW9uX2lkIiVjMDdlNjJmZGJiNTFhZjhhYmU5Yjk4NjE1Y2ZjODBhOQ%3D%3D--b9945c8e50a5eba75e75c92b2c92e5a3a86f1000\r\n\r\n";

const http_header_t     expected_headers[] = {
    {"Host", "www.javaeye.com"},
    {"Connection", "keep-alive"},
    {"Referer", "http://www.javaeye.com/"},
    {"Cache-Control", "max-age=0"},
    {"If-Modified-Since", "Sat, 15 Dec 2007 04:04:14 GMT"},
    {"If-None-Match", "\"3922745954\""},
    {"Accept", "*/*"},
    {"User-Agent", "Mozilla/5.0 (X11; U; Linux i686; en-US) AppleWebKit/534.15 (KHTML, like Gecko) Chrome/10.0.612.1 Safari/534.15"},
    {"Accept-Encoding", "gzip,deflate,sdch"},
    {"Accept-Language", "zh-CN,zh;q=0.8"},
    {"Accept-Charset", "GBK,utf-8;q=0.7,*;q=0.3"},
    {"Cookie", "lzstat_uv=39255935923757216993|1146165@1270080@1632275@1145010@0; remember_me=yes; login_token=MTAyMjExXzJkZjY4ZGIyNDIzZTdjMTE4YmM5OTU0OTQ2MTU0N2Fh%0A; _javaeye3_session_=BAh7BzoMdXNlcl9pZGkDQ48BOg9zZXNzaW9uX2lkIiVjMDdlNjJmZGJiNTFhZjhhYmU5Yjk4NjE1Y2ZjODBhOQ%3D%3D--b9945c8e50a5eba75e75c92b2c92e5a3a86f1000"}
};

void dump_request(request_t *req);
void assert_headers(request_t *req);

void test_parse_once() {
    request_t  *req;
    size_t     req_size, consumed_size;
    int        rc;

    req = request_create();
    assert(req != NULL);
    printf("\n\nTesting parsing all in one time\n");
    req_size = strlen(test_request);
    rc = request_parse_headers(req, test_request, req_size, &consumed_size);
    printf("Request size: %zu, consumed size: %zu, return status: %d\n", req_size, consumed_size, rc);
    dump_request(req);
    assert(rc == STATUS_COMPLETE);
    assert(consumed_size == req_size);
    assert_headers(req);
    assert(request_destroy(req) == 0);
}


void test_parse_once_with_extra_data() {
    request_t  *req;
    size_t     req_size, consumed_size;
    int        rc;    

    req = request_create();
    assert(req != NULL);
    printf("\n\nTesting parsing all in one time with extra data left\n");
    req_size = strlen(test_request_2);
    rc = request_parse_headers(req, test_request_2, req_size, &consumed_size);
    printf("Request size: %zu, consumed size: %zu, return status: %d\n", req_size, consumed_size, rc);
    dump_request(req);
    assert(rc == STATUS_COMPLETE);
    assert(consumed_size == req_size - strlen("GET /home/hello.do?id=1001&name=hello HTTP/1.1\r\n"));
    assert_headers(req);
    assert(request_destroy(req) == 0);
}

void test_parse_multiple_times() {
    request_t  *req;
    size_t     req_size, consumed_size;
    int        rc;
    size_t     part1_size;
    char       *data;

    req = request_create();
    assert(req != NULL);
    printf("\n\nTesting parsing all in multiple time\n");
    data = test_request;
    req_size = strlen(data);
    part1_size = req_size / 2;

    // Incomplete request
    rc = request_parse_headers(req, data, part1_size, &consumed_size);
    printf("First Time: Request size: %zu, consumed size: %zu, return status: %d\n",
           req_size, consumed_size, rc);
    dump_request(req);
    assert(rc == STATUS_CONTINUE);
    assert(consumed_size == part1_size);
    assert(request_destroy(req) == 0);
}


void test_parse_invalid_version() {
    request_t *req;
    int     req_size, rc;
    size_t  consumed_size;

    req = request_create();
    assert(req != NULL);
    req_size = strlen(invalid_req);

    printf("\n\nTesting invalid HTTP version\n");
    rc = request_parse_headers(req, invalid_req, req_size, &consumed_size);
    dump_request(req);
    assert(rc == STATUS_ERROR);
    assert(request_destroy(req) == 0);
}

void dump_request(request_t *req) {
    int i;

    printf("--------- Parser State -----------------------\n");
    printf("Method: %s\n", req->method);
    printf("Path: %s\n", req->path);
    printf("Query String: %s\n", req->query_str);
    printf("HTTP Version: %d\n", req->version);
    printf("Header count: %zu\n", req->header_count);
    printf("Headers: \n");
    printf("------------\n");

    for (i = 0; i < req->header_count; i++) {
        printf("\r%s: %s\n", req->headers[i].name, req->headers[i].value);
    }

    printf("----------------------------------------------\n");
}

void assert_headers(request_t *req) {
    int expected_header_size, i;

    expected_header_size = sizeof(expected_headers) / sizeof(http_header_t);

    printf("Expecting %d headers\n", expected_header_size);
    assert(expected_header_size == req->header_count);
    printf("Checked\n");

    for (i = 0; i < req->header_count; i++) {
        assert(strcmp(expected_headers[i].name,  req->headers[i].name) == 0);
        assert(strcmp(expected_headers[i].value, req->headers[i].value) == 0);
    }
}

int main(int argc, const char *argv[])
{
    print_stacktrace_on_error();
    //bzero(&request, sizeof(request_t));
    test_parse_once();
    test_parse_once_with_extra_data();
    test_parse_multiple_times();
    test_parse_invalid_version();
    return 0;
}

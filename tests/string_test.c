//#include <alloca.h>
//#include <stddef.h>
//#include <stdbool.h>
//#include <stdio.h>
//#include <string.h>
//
//static inline char char_value(const char* ptr)
//{
//    return *ptr;
//}
//
//static inline bool is_null(const char* str)
//{
//    return (str == NULL);
//}
//
//static inline bool is_empty_value(const char c)
//{
//    return (c == '\0');
//}
//
//static inline bool is_empty(const char* str)
//{
//    return is_empty_value(char_value(str));
//}
//
//static inline bool is_null_or_empty(const char* str)
//{
//    return (is_null(str) || is_empty(str));
//}
//
//static inline bool is_compare_str(const char* s1, const char* s2, const size_t len)
//{
//    if (is_null_or_empty(s1) || is_null_or_empty(s2)) {
//        return false;
//    }
//
//    if (len <= 0) {
//        return false;
//    }
//
//    for (size_t i = 0; i < len; i++) {
//        if (s1[i] != s2[i]) {
//            return false;
//        }
//    }
//
//    return true;
//}
//
//static inline int search_str(const char* base, const size_t base_len, const char* target, const size_t target_len)
//{
//    if (is_null_or_empty(base) || base_len <= 0 || target_len > base_len) {
//        return -1;
//    }
//
//    if (is_null_or_empty(target) || target_len <= 0) {
//        return -1;
//    }
//
//    for (size_t base_pos = 0; base_pos <= (base_len - target_len); base_pos++) {
//        if (is_compare_str(&base[base_pos], target, target_len)) {
//            return base_pos;
//        }
//    }
//
//    return -1;
//}
//
//static inline bool is_contain_str(const char* base, const size_t base_len, const char* target, const size_t target_len)
//{
//    return (search_str(base, base_len, target, target_len) != -1);
//}
//
//static inline bool is_white_space(const char c)
//{
//    return (c == ' ') ||
//           (c == '\t') ||
//           (c == '\r') ||
//           (c == '\n') ||
//           (c == '\v') ||
//           (c == '\f');
//}
//
//static inline bool is_line_break(const char c)
//{
//    return (c == '\r') ||
//           (c == '\n');
//}
//
//static inline bool is_utf8_white_space(const char* str)
//{
//    // UTF-8: 0xE3 0x80 0x80
//    return (str[0] == '\xE3') &&
//           (str[1] == '\x80') &&
//           (str[2] == '\x80');
//}
//
//static inline int skip_white_space(const char* buffer, const size_t buffer_size)
//{
//    if (buffer == NULL | buffer_size <= 0) {
//        return -1;
//    }
//
//    size_t pos = 0;
//    while (!is_empty(&buffer[pos]) && pos < buffer_size) {
//        char current = char_value(&buffer[pos]);
//        if (is_white_space(current)) {
//            pos++;
//            continue;
//        }
//
//        if (pos + 2 < buffer_size) {
//            if (is_utf8_white_space(&buffer[pos])) {
//                pos += 3;
//                continue;
//            }
//        }
//
//        return pos;
//    }
//
//    return pos;
//}
//
//static inline int skip_word(const char* buffer, const size_t buffer_size)
//{
//    for (size_t i = 0; i < buffer_size; i++) {
//        if (is_white_space(buffer[i])) {
//            return i;
//        }
//
//        if (is_empty_value(buffer[i])) {
//            return i;
//        }
//
//        if (i + 2 >= buffer_size) {
//            continue;
//        }
//
//        if (is_utf8_white_space(&buffer[i])) {
//            return i;
//        }
//    }
//
//    return buffer_size;
//}
//
//static inline int skip_next_line(const char* buffer, const size_t buffer_len)
//{
//    if (buffer == NULL || buffer_len < 2) {
//        return 0;
//    }
//
//    size_t buffer_pos = 0;
//    while (buffer[buffer_pos] != '\0' && buffer_pos < buffer_len - 1) {
//        char current = buffer[buffer_pos];
//        char next    = buffer[buffer_pos + 1];
//
//        if (current == '\r' && next == '\n') {
//            return buffer_pos + 2;
//        }
//
//        if (current == '\n') {
//            return buffer_pos + 1;
//        }
//
//        if (next == '\n') {
//            return buffer_pos + 2;
//        }
//
//        buffer_pos++;
//    }
//
//    return buffer_pos;
//}
//
//static inline int skip_token(const char* buffer, const size_t buffer_size, const char token)
//{
//    for (size_t i = 0; i < buffer_size; i++) {
//        if (buffer[i] == token) {
//            return i;
//        }
//    }
//
//    return -1;
//}

//typedef struct {
//    char* key;
//    char* value;
//} HTTPRequestHeaderLine, *PHTTPRequestHeaderLine;
//
//static inline bool extract_http_request_header(const char* restrict buffer, const size_t buffer_size, PHTTPRequestHeaderLine restrict line)
//{
//    size_t remain_buffer_size = buffer_size;
//
//    char* key_start = (char *)buffer;
//    int   key_len = skip_token(buffer, remain_buffer_size, ':');
//    if (key_len == -1) {
//        return false;
//    }
//
//    buffer             += key_len + 1;
//    remain_buffer_size -= key_len + 1;
//
//    size_t new_pos = skip_white_space(buffer, remain_buffer_size);
//    buffer             += new_pos;
//    remain_buffer_size -= new_pos;
//
//    char* value_start = (char *)buffer;
//    int   value_len   = skip_token(buffer, remain_buffer_size, '\r');
//    if (value_len == -1) {
//        return false;
//    }
//
//    memcpy(line->key, key_start, key_len);
//    memcpy(line->value, value_start, value_len);
//    line->key[key_len]     = '\0';
//    line->value[value_len] = '\0';
//    return true;
//}
//
//static inline bool parse_http_request_header(const char* restrict buffer, const size_t buffer_size, const size_t header_size, restrict PHTTPRequestHeaderLine lines)
//{
//    if (
//        is_null(buffer) ||
//        is_null((char*)lines) ||
//        buffer_size <= 2 ||
//        header_size <= 0) {
//        perror("Invalid parameter");
//        return false;
//    }
//
//    for (size_t i = 0; i < header_size; i++) {
//        if (is_null(lines[i].key) || is_null(lines[i].value)) {
//            perror("header parameter(key, value) is not allocated");
//            return false;
//        }
//    }
//
//    size_t buffer_pos         = 0;
//    size_t header_pos         = 0;
//    size_t remain_buffer_size = buffer_size;
//
//    while ((buffer_pos < buffer_size) || (header_pos < header_size)) {
//        const char*            current_buffer = &buffer[buffer_pos];
//        PHTTPRequestHeaderLine current_line   = &lines[header_pos];
//
//        if (extract_http_request_header(current_buffer, remain_buffer_size, current_line) == false) {
//            perror("header extract error");
//            return false;
//        }
//
//        size_t new_pos = skip_next_line(current_buffer, remain_buffer_size);
//        remain_buffer_size -= new_pos;
//        buffer_pos         += new_pos;
//        header_pos++;
//    }
//
//    if (header_pos <= 0) {
//        return false;
//    }
//
//    return true;
//}

//#include <string.h>
//#include <assert.h>
//#include <stdlib.h>
//#include <sys/mman.h>
//#include <unistd.h>
//void test_search_str()
//{
//    // search_str
//    assert(search_str("hello, world\nworld\n", 19, "world", 5) == 7);
//    assert(search_str("world",                 5,  "world", 5) == 0);
//    assert(search_str("worl",                  4,  "world", 5) == -1);
//    assert(search_str("hello, world\n",        12, ""     , 1) == -1);
//    assert(search_str("hello, world\n",        12, "world", 0) == -1);
//    assert(search_str(NULL,                    3,  "world", 5) == -1);
//    assert(search_str("hello, world\n",        0,  "world", 5) == -1);
//}
//
//void test_parse_request_header()
//{
//    // parse_http_request_header
//    int count = 6;
//    char*  buffer[] = {
//        "Key: Value\r\n",
//        "Key: Value",
//        "",
//        NULL,
//        "Key: Value\r\n",
//        "Key: Value\r\nKey2: Value2\r\n",
//    };
//    size_t header_size[] = {
//        1,
//        1,
//        1,
//        1,
//        0,
//        2,
//    };
//    bool expected[] = {
//        true,
//        false,
//        false,
//        false,
//        false,
//        true,
//    };
//
//    #define memory_alloc_header(header2, header_size2)  \
//        for (size_t header_count = 0; header_count < header_size2; header_count++) { \
//            header2[header_count].key   = alloca(128); \
//            header2[header_count].value = alloca(128); \
//        }
//
//    for (int i = 0; i < count; i++) {
//        printf("test [%d]\n", i+1);
//        fflush(stdout);
//        HTTPRequestHeaderLine* header =
//            (HTTPRequestHeaderLine*)alloca(sizeof(HTTPRequestHeaderLine) * header_size[i]);
//
//        memory_alloc_header(header, header_size[i]);
//        assert(
//            parse_http_request_header(buffer[i],
//            (buffer[i] == NULL) ? 1 : strlen(buffer[i]),
//            header_size[i],
//            header) == expected[i]);
//        fflush(stdout);
//        //assert(strcmp(header[0].key, "Key") == 0);
//        //assert(strcmp(header[0].value, "Value") == 0);
//    }
//}
//
//#define STK_SIZE 7*1024*1024
//void stack_init(void)
//{
//  char x[STK_SIZE];
//  mlock(x, sizeof(x));
//  memset(x, 0x00, sizeof(x));
//}
//
//void stack_check(void)
//{
//  char x[STK_SIZE];
//  int i;
//  for (i = 0; i < STK_SIZE; i++) {
//      if (x[i] != 0x00) {
//          break;
//      }
//  }
//
//    printf("\n stack size: %d\n",  STK_SIZE - i);
//    fflush(stdout);
//}
//
//
//#define memory_alloc_header(header2, header_size2)  \
//    for (size_t header_count = 0; header_count < header_size2; header_count++) { \
//        header2[header_count].key   = alloca(128); \
//        header2[header_count].value = alloca(128); \
//    }
//
//void measure_stack_usage() {
//    char*  buffer =
//        "Key: Value\r\n"
//        "Key2: Value2\r\n"
//        "Key3: Value3\r\n"
//        "Key4: Value4\r\n";
//    size_t header_size = 4;
//
//        HTTPRequestHeaderLine* header =
//            (HTTPRequestHeaderLine*)alloca(sizeof(HTTPRequestHeaderLine) * header_size);
//        memory_alloc_header(header, header_size);
//        size_t buffer_len = strlen(buffer);
//
//        stack_init();
//        bool ret =
//            parse_http_request_header(
//                buffer,
//                buffer_len,
//                header_size,
//                header
//            );
//        stack_check();
//        assert(ret == true);
//        assert(strcmp(header[0].key, "Key") == 0);
//        assert(strcmp(header[0].value, "Value") == 0);
//        assert(strcmp(header[1].key, "Key2") == 0);
//        assert(strcmp(header[1].value, "Value2") == 0);
//}
//
//int main(void)
//{
//    measure_stack_usage();
//    //CALLGRIND_START_INSTRUMENTATION;
//    //test_parse_request_header();
//    //CALLGRIND_STOP_INSTRUMENTATION;
//    //CALLGRIND_DUMP_STATS;
//    return 0;
//}

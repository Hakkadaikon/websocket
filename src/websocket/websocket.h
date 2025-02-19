#ifndef _NOSTR_WEBSOCKET_H_
#define _NOSTR_WEBSOCKET_H_

/** @file  websocket.h
 *
 * @brief Parses each parameter of a websocket frame stored in network byte order.
 * @see RFC6455 (https://datatracker.ietf.org/doc/html/rfc6455)
 */
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1  // Standard output.
#endif

#ifndef STDERR_FILENO
#define STDERR_FILENO 2  // Standard error output.
#endif

/**
 * @enum WebSocketOpCode
 * @brief WebSocket frame type
 */
typedef enum {
    WEBSOCKET_OP_CODE_TEXT   = 0x1,  ///< When this frame is received, Invokes the user callback.
    WEBSOCKET_OP_CODE_BINARY = 0x2,  ///< When this frame is received, Invokes the user callback.
    WEBSOCKET_OP_CODE_CLOSE  = 0x8,  ///< When this frame is received, closes the client socket.
    WEBSOCKET_OP_CODE_PING   = 0x9,  ///< When this frame is received, this server replies with a Pong.
    WEBSOCKET_OP_CODE_PONG   = 0xA,  ///< Pong frame.
} WebSocketOpCode;

/**
 * @brief Error code when calling a WebSocket function
 */
typedef enum {
    WEBSOCKET_ERRORCODE_FATAL_ERROR        = -3,  ///< When this code is received, This server will terminate processing.
    WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR = -2,  ///< When this code is received, This server closes the client socket.
    WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR  = -1,  ///< When this code is received, This server will not proceed to the next epoll loop and will continue.
    WEBSOCKET_ERRORCODE_NONE               = 0,   ///< Normal code. No action.
} WebSocketErrorCode;

/**
 * @brief Result of parsing the WebSocket frame
 */
typedef struct _WebSocketFrame {
    uint8_t         fin;
    uint8_t         rsv1;
    uint8_t         rsv2;
    uint8_t         rsv3;
    WebSocketOpCode opcode;
    uint8_t         mask;
    uint8_t         payload_len;
    uint64_t        ext_payload_len;
    uint8_t         masking_key[4];
    char*           payload;
} WebSocketFrame, *PWebSocketFrame;

/**
 * @brief User callback that is called when a WebSocket frame is received.
 */
typedef void (*PWebSocketReceiveCallback)(
    const int       client_sock,      ///< @param[in]     client_sock     Client socket that sent the data
    PWebSocketFrame frame,            ///< @param[in]     frame           Parsed websocket frame
    const size_t    buffer_capacity,  ///< @param[in]     buffer_capacity Response_buffer capacity.
    char*           response_buffer   ///< @param[in/out] response_buffer This buffer must be used to create the return frame.
);

/**
 * @brief User callback to be called when connection is established
 */
typedef void (*PWebSocketConnectCallback)(
    const int client_sock  ///< @param[in] client_sock Client socket to connect
);

/**
 * @ brief User callback that is called when a client is disconnected.
 *
 * When this callback is called, the connection has already been disconnected,
 * so BSD socket functions cannot be called using the client_sock argument.
 * It is passed to identify the client and release resources if they have been allocated.
 */
typedef void (*PWebSocketDisconnectCallback)(
    const int client_sock  ///< @param[in] client_sock Client socket to disconnect
);

/**
 * @brief User callback list to pass to the WebSocket library.
 */
typedef struct {
    PWebSocketReceiveCallback    receive_callback;     ///< @see PWebSocketReceiveCallback
    PWebSocketConnectCallback    connect_callback;     ///< @see PWebSocketConnectCallback
    PWebSocketDisconnectCallback disconnect_callback;  ///< @see PWebSocketDisconnectCallback
} WebSocketCallbacks, *PWebSocketCallbacks;

/**
 * @brief Arguments to pass to websocket_init()
 */
typedef struct {
    int port_num;  ///< WebSocket port number
    int backlog;   ///< Listen queue size
} WebSocketInitArgs, *PWebSocketInitArgs;

/**
 * @brief Arguments to pass to websocket_loop()
 */
typedef struct {
    int                server_sock;      ///< Socket descriptor obtained by websocket_server_init() function
    size_t             buffer_capacity;  ///< Capacity of the send and receive buffer for one client.
    WebSocketCallbacks callbacks;        ///< @see WebSocketCallBacks
} WebSocketLoopArgs, *PWebSocketLoopArgs;

/**
 * @brief Parse raw data in network byte order into a websocket flame structure
 *
 * @param[in]  raw         raw data (network byte order)
 * @param[in]  frame_size  Size of webSocket frame
 * @param[out] frame       Output destination of parsed frame
 *
 * @return true: Parse was successful / false: Failed parse
 */
bool parse_websocket_frame(const char* raw, const size_t frame_size, PWebSocketFrame frame);

/**
 * @brief Creates raw data to send back to the client
 *
 * @param[in]  frame    Transmitted frame before it becomes raw data
 * @param[in]  capacity Raw data capacity
 * @param[out] raw      Raw data frame to be returned
 *
 * @return Size of raw data. If parsing fails, 0 is returned.
 */
size_t create_websocket_frame(PWebSocketFrame frame, const size_t capacity, char* raw);

/**
 * @brief Initialize a WebSocket server. socket listen and register signal handler.
 *
 * @param[in] port_num Listening port number
 * @param[in] backlog  Listen queue size
 *
 * @return Positive value: Server socket descriptor / Negative value: WebSocket error code
 * @see WebSocketErrorCode
 */
int websocket_server_init(const PWebSocketInitArgs args);

/**
 * @brief Wrapper for the BSD socket send() API.
 *
 * @param[in] sock_fd     Destination socket descriptor
 * @param[in] buffer_size Buffer size
 * @param[in] buffer      Buffer that stores the transmission data
 *
 * @return WebSocket error code
 * @see WebSocketErrorCode
 */
int websocket_send(const int sock_fd, const size_t buffer_size, const char* buffer);

/**
 * @brief Wrapper for the BSD socket recv() API.
 *
 * @param[in]  sock_fd  Destination socket descriptor
 * @param[in]  capacity Receive buffer capacity
 * @param[out] buffer   Receive buffer. User must allocate the amount specified by the capacity variable.
 *
 * @return Positive value: Receive data size / Negative value: WebSocket error code
 * @see WebSocketErrorCode
 */
ssize_t websocket_recv(const int sock_fd, const size_t capacity, char* buffer);

#ifndef __APPLE__
/**
 * @brief Wrapper for the BSD socket recv() API.
 *
 * @param[in]  sock_fd       Destination socket descriptor
 * @param[in]  capacity      Receive buffer capacity
 * @param[in]  num_of_buffer Number of receive buffer
 * @param[out] buffers       Receive buffers. User must allocate the amount specified by the capacity x num_of_buffer.
 *
 * @return Positive value: Receive data count / Negative value: WebSocket error code
 * @see WebSocketErrorCode
 */
ssize_t websocket_recvmmsg(const int sock_fd, const size_t capacity, const int num_of_buffer, char** buffers);
#endif

/**
 * @brief Wrapper for the BSD socket close() API
 *
 * @param[in] sock_fd Socket descriptor
 *
 * @return WebSocket error code
 * @see WebSocketErrorCode
 */
int websocket_close(const int sock_fd);

/**
 * @brief Enter the receive loop from the client.
 *        This function will block until it is sent a SIGHUP/SIGINT/SIGTERM signal or detects a FATAL ERROR internally.
 *
 * @param[in] args @see WebSocketLoopArgs
 *
 * @return true: success / false: failure
 */
bool websocket_server_loop(PWebSocketLoopArgs args);

void log_dump_local(const int fd, const char* str);
void var_dump_local(const int fd, const char* str, const int value);
void str_dump_local(const int fd, const char* str, const char* value);

#if !defined(LOG_LEVEL_DEBUG) && !defined(LOG_LEVEL_INFO) && !defined(LOG_LEVEL_ERROR)
#define LOG_LEVEL_ERROR
#endif

#ifdef LOG_LEVEL_DEBUG
#define hex_dump(data, size) hex_dump_local(data, size)
#define log_dump(fd, str) log_dump_local(fd, str)
#define log_debug(str) log_dump(STDOUT_FILENO, str)
#define log_info(str) log_dump(STDOUT_FILENO, str)
#define log_error(str) log_dump(STDERR_FILENO, str)
#define var_dump(fd, str, value) var_dump_local(fd, str, value)
#define var_debug(str, value) var_dump(STDOUT_FILENO, str, value)
#define var_info(str, value) var_dump(STDOUT_FILENO, str, value)
#define var_error(str, value) var_dump(STDERR_FILENO, str, value)
#define str_debug(str, value) str_dump_local(STDOUT_FILENO, str, value)
#define str_info(str, value) str_dump_local(STDOUT_FILENO, str, value)
#define str_error(str, value) str_dump_local(STDERR_FILENO, str, value)

#elif defined(LOG_LEVEL_INFO)
#define hex_dump(data, size) hex_dump_local(data, size)
#define log_dump(fd, str) log_dump_local(fd, str)
#define log_debug(str)
#define log_info(str) log_dump(STDOUT_FILENO, str)
#define log_error(str) log_dump(STDERR_FILENO, str)
#define var_dump(fd, str, value) var_dump_local(fd, str, value)
#define var_debug(str, value)
#define var_info(str, value) var_dump(STDOUT_FILENO, str, value)
#define var_error(str, value) var_dump(STDERR_FILENO, str, value)
#define str_debug(str, value)
#define str_info(str, value) str_dump_local(STDOUT_FILENO, str, value)
#define str_error(str, value) str_dump_local(STDERR_FILENO, str, value)

#elif defined(LOG_LEVEL_ERROR)
#define hex_dump(data, size) hex_dump_local(data, size)
#define log_dump(fd, str) log_dump_local(fd, str)
#define log_debug(str)
#define log_info(str)
#define log_error(str) log_dump(STDERR_FILENO, str)
#define var_dump(fd, str, value) var_dump_local(fd, str, value)
#define var_debug(str, value)
#define var_info(str, value)
#define var_error(str, value) var_dump(STDERR_FILENO, str, value)
#define str_debug(str, value)
#define str_info(str, value)
#define str_error(str, value) str_dump_local(STDERR_FILENO, str, value)

#elif
#define hex_dump(data, size)
#define log_dump(fd, str)
#define log_debug(str)
#define log_info(str)
#define log_error(str)
#define var_dump(fd, str, value)
#define var_debug(str, value)
#define var_info(str, value)
#define var_error(str, value)
#define str_debug(str, value)
#define str_info(str, value)
#define str_error(str, value)
#endif

#endif

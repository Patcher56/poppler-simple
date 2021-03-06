#include "MemoryStream.h"

inline OFFSET_TYPE pow2roundup(OFFSET_TYPE x) {
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x |= x >> 32;
    return x+1;
}

inline SSIZE_TYPE memory_stream_write(void *cookie, const char *buf, SIZE_TYPE size) {
    return ((Cookie*) cookie)->write(buf, size);
}

inline int memory_stream_close(void *cookie) {
    return ((Cookie*) cookie)->close();
}

inline SSIZE_TYPE Cookie::write(const char *buf, SIZE_TYPE size) {
    return stream->write(buf, size);
}

inline int Cookie::close() {
    return stream->close();
}

FILE* MemoryStream::open() {
#ifdef __linux
    cookie_io_functions_t funcs = {NULL, memory_stream_write, NULL, memory_stream_close};
    return fopencookie((void*) cookie, "wb", funcs);
#elif __APPLE__
    return funopen((void*) cookie, NULL, memory_stream_write, NULL, memory_stream_close);
#endif
}

SSIZE_TYPE MemoryStream::write(const char *buf, SIZE_TYPE size) {
    if (((OFFSET_TYPE)(offset + size)) > buffer_len) {
        buffer_len = pow2roundup(offset + size);
        buffer = (char*) realloc(buffer, buffer_len);
    }
    if (! buffer) {
        return 0;
    }
    memcpy(buffer + offset, buf, size);
    offset += size;
    return size;
}

int MemoryStream::close() {
    return 0;
}

#include "buffer.h"
#include "dbg.h"

Buffer *
buffer_alloc(int initial_size)
{
    // TODO: handle memory allocation problems
    Buffer *buf     = malloc(sizeof(Buffer));
    buf->contents   = calloc(1, initial_size * sizeof(char));
    buf->bytes_used = 0;
    buf->total_size = initial_size;

    return buf;
}

int
buffer_strlen(Buffer *buf)
{
    return buf->bytes_used;
}

void
buffer_free(Buffer *buf)
{
    free(buf->contents);
    free(buf);
}

int
buffer_has_space(Buffer *buf, int desired_length)
{
    return desired_length <= (buf->total_size - desired_length);
}

int
buffer_grow(Buffer *buf, int minimum_size)
{
    int factor = buf->total_size;

    if (factor < minimum_size) {
        factor = minimum_size;
    }

    int new_size = factor * 2;

    char *tmp = realloc(buf->contents, new_size * sizeof(char));
    if (tmp == NULL) { return -1; }

    buf->contents   = tmp;
    buf->total_size = new_size;

    return 0;
}

void
buffer_cat(Buffer *buf, char *append, int length)
{
    int i               = 0;
    int bytes_copied    = 0;
    int buffer_position = 0;

    for (i = 0; i < length; i++) {
        if (append[i] == '\0') { break; }

        buffer_position = buf->bytes_used + i;
        *(buf->contents + buffer_position) = append[i];

        bytes_copied++;
    }

    buf->bytes_used += bytes_copied;
    *(buf->contents + buf->bytes_used) = '\0';
}

int
buffer_append(Buffer *buf, char *append, int length)
{
    int status         = 0;
    int desired_length = length + 1; // Space for NUL byte

    if (!buffer_has_space(buf, desired_length)) {
        status = buffer_grow(buf, desired_length);
        if (status == -1) { return -1; }
    }

    buffer_cat(buf, append, length);

    return 0;
}

int
buffer_appendf(Buffer *buf, const char *format, ...)
{
    char *tmp = NULL;
    int bytes_written, status;

    va_list argp;
    va_start(argp, format);

    bytes_written = vasprintf(&tmp, format, argp);
    jump_unless(bytes_written >= 0);

    va_end(argp);

    status = buffer_append(buf, tmp, bytes_written);
    jump_unless(status == 0);

    free(tmp);

    return 0;
error:
    if (tmp != NULL) { free(tmp); }
    return -1;
}
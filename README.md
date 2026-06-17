# libbuffer

This is a simple C library that implements a growable buffer.

## Status: unmaintained — known broken for binary data

This library has known correctness and safety bugs and is **not recommended for
new use**. It is being archived. The issues below were surfaced while hardening a
downstream consumer (`bode`) and were reviewed but intentionally left unfixed.

File/line references are against commit `14cc4dc` (`src/buffer.c`).

### Critical

1. **Cannot store binary data — truncates at embedded NUL** (`src/buffer.c:78-79`)
   `buffer_cat` is given an explicit `length` but breaks out of its copy loop at
   the first `\0` byte. Any payload containing NUL (e.g. PNG/JPEG/GIF) is silently
   truncated. Fix would be to `memcpy` exactly `length` bytes.

2. **`buffer_to_s` truncates at NUL** (`src/buffer.c:167-168`)
   `strncpy` stops at the first `\0` in the source. Note that `buffer_to_s`
   returns a bare `char *` with no length, so it is fundamentally a C-string
   accessor; binary callers should read `buf->contents` / `buf->bytes_used`
   directly rather than rely on this function.

3. **Allocation error path frees an uninitialized pointer** (`src/buffer.c:7-21`)
   `buf` is allocated with `malloc` (not zeroed). If the subsequent `calloc`
   fails, the error path calls `buffer_free(buf)`, which runs
   `free(buf->contents)` on uninitialized memory — crash / undefined behavior.
   Confirmed under AddressSanitizer.

4. **Integer overflow in size math** (`src/buffer.c:8, 56, 60, 95`)
   Sizes are tracked as `int` throughout. A large or attacker-controlled length
   can overflow during the size calculations and produce a too-small allocation,
   leading to a heap overflow on the following copy. Sizes should be `size_t`
   with an explicit overflow check in `buffer_grow`.

### Minor

- `buffer_appendf` / `buffer_nappendf` skip `va_end` on the error path
  (`src/buffer.c:118-121`, `147-148`) — undefined to leave a `va_list` unclosed.
- `src/buffer.h:5` includes `<strings.h>` but the code uses `<string.h>`
  functions (`strncpy`/`strlen`); it compiles only by luck.
- `buffer_nappendf` computes `printf_length` as `int` from a `size_t length`
  (`src/buffer.c:137-138`) — same overflow family as #4.
- Append functions take `char *` where they should take `const char *`.
- `buffer_strlen` return type and `Buffer.total_size` / `bytes_used` are `int`;
  they should be `size_t`.

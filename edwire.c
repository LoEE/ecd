#include <fixint.h>
#include <stddef.h>
#include <assert.h>

#define NOINLINE __attribute__((__noinline__))

NOINLINE u32 zigzag32enc (s32 n)
{
  return (n << 1) ^ (n >> 31);
}

NOINLINE u64 zigzag64enc (s64 n)
{
  return (n << 1) ^ (n >> 31);
}

NOINLINE s32 zigzag32dec (u32 n)
{
  return (n >> 1) ^ -(n & 1);
}

NOINLINE s64 zigzag64dec (u64 n)
{
  return (n >> 1) ^ -(n & 1);
}

NOINLINE size_t writevarint32 (u8 *s, u32 n)
{
  u8 *x = s;
  *x = n;
  while(n >>= 7) {
    *x++ |= 0x80;
    *x = n;
  }
  x++;
  return x - s;
}

NOINLINE size_t writevarint64 (u8 *s, u64 n)
{
  u8 *x = s;
  *x = n;
  while(n >>= 7) {
    *x++ |= 0x80;
    *x = n;
  }
  x++;
  return x - s;
}

NOINLINE size_t readvarint32 (u8 *s, size_t n, u32 *v)
{
  assert(n != 0);
  u32 r = 0;
  int i = 0;
  do {
    r = r | (*s & 0x7f) << (i * 7); i++;
  } while (*s++ & 0x80 && --n);
  *v = r;
  return i;
}

NOINLINE size_t readvarint64 (u8 *s, size_t n, u64 *v)
{
  assert(n != 0);
  u64 r = 0;
  int i = 0;
  do {
    r = r | (*s & 0x7f) << (i * 7); i++;
  } while (*s++ & 0x80 && --n);
  *v = r;
  return i;
}

#include <stdio.h>
#include "common.h"

void test_zigzag () {
  static const struct { s32 in; u32 exp; } test[] = {
    {0, 0},
    {-1, 1},
    {1, 2},
    {-2, 3},
    { 2147483647, 4294967294},
    {-2147483648, 4294967295},
  };

  for (int i = 0; i < lengthof(test); i++) {
    s32 in = test[i].in;
    u32 exp = test[i].exp;

    u32 out = zigzag32enc(in);
    if (out == exp) {
      fprintf (stderr, "enc32: %d -> %u (OK)\n", in, out);
    } else {
      fprintf (stderr, "enc32: %d -> %u (FAIL: %u)\n", in, out, exp);
    }
    s32 back = zigzag32dec(out);
    if (back == in) {
      fprintf (stderr, "dec32: %u -> %d (OK)\n", out, back);
    } else {
      fprintf (stderr, "dec32: %u -> %d (FAIL: %d)\n", out, back, in);
    }
  }
}

int byte_diff (const void *_a, size_t n, const void *_b)
{
  const u8 *a = _a, *b = _b;
  while (n--) {
    if (*a != *b) return *a++ - *b++;
  }
  return 0;
}

void test_writevarint32 (void)
{
  static const struct { u8 s[10]; size_t len; u32 val; } test[] = {
    {{0x00}      , 1, 0},
    {{0x01}      , 1, 1},
    {{0x7f}      , 1, 127},
    {{0xa2, 0x74}, 2, (0x22 << 0) | (0x74 << 7)},          // 14882
    {{0xbe, 0xf7, 0x92, 0x84, 0x0b}, 5,                    // 2961488830
      (0x3e << 0) | (0x77 << 7) | (0x12 << 14) | (0x04 << 21) |
        ((u32)(0x0b) << 28)},
  };
  for (int i = 0; i < lengthof(test); i++) {
    u8 out[10];
    size_t len = writevarint32 (out, test[i].val);
    if (len != test[i].len) {
      fprintf (stderr, "writevarint32: error: invalid length for: %u (got %zu, expected %zu)\n",
          test[i].val, len, test[i].len);
      break;
    }
    if (byte_diff (out, len, test[i].s)) {
      fprintf (stderr, "writevarint32: error: invalid encoded data for: %u\n", test[i].val);
      break;
    }
    fprintf (stderr, "writevarint32: %u PASSED\n", test[i].val);
    u32 in;
    len = readvarint32 (out, sizeof(out), &in);
    if (len != test[i].len) {
      fprintf (stderr, "readvarint32: error: invalid length for: %u (got %zu, expected %zu)\n",
          test[i].val, len, test[i].len);
      break;
    }
    if (in != test[i].val) {
      fprintf (stderr, "readvarint32: error: invalid value (got %u, expected %u)\n",
          in, test[i].val);
      break;
    }
    fprintf (stderr, "readvarint32: %u PASSED\n", test[i].val);
  }
}

int main (void)
{
  test_zigzag();
  test_writevarint32();
  return 0;
}

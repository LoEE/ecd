typedef uint8_t u8;
typedef int8_t s8;
typedef uint32_t u32;
typedef int32_t s32;

u32 zigzag32enc (s32 n);
s32 zigzag32dec (u32 n);
size_t writevarint32 (u8 *s, u32 n);
size_t readvarint32 (u8 *s, size_t n, u32 *v);

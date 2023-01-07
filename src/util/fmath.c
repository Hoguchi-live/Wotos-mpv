#include "fmath.h"
#include "../util/util.h"

int ivec3scmp(ivec3s a, ivec3s b) {
    return memcmp(&a, &b, sizeof(ivec3s));
}

s64 ivec3shash(ivec3s v) {
    s64 h = 0;
    for(int i = 0; i < 3; i++) {
        h ^= v.raw[i] + 0x9e3779b9 + (h << 6) + (h >> 2);
    }
    return h;
}

// finds the smallest possible t such that s + t * ds is an integer
static vec3s intbound(vec3s s, vec3s ds) {
    vec3s v;
    #if defined(__clang__)
    #pragma clang loop unroll_count(3)
    #elif defined(__GNUC__)
    #pragma GCC unroll 3
    #endif
    for (size_t i = 0; i < 3; i++) {
        v.raw[i] = (ds.raw[i] > 0 ? (ceilf(s.raw[i]) - s.raw[i]) : (s.raw[i] - floorf(s.raw[i]))) / fabsf(ds.raw[i]);
    }
    return v;
}

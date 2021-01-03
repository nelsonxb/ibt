#pragma once

#include <sys/types.h>
#include <stdint.h>
#include <stdarg.h>

struct svec {
    size_t len, cap;
    uint64_t alocv[2];
    char *vec[];
};

inline size_t sv_len(struct svec *sv)
{
    return sv == (struct svec*)0 ? 0 : sv->len;
}

struct svec *sv_make(size_t cap);
struct svec *sv_append(struct svec *sv, char *str);
struct svec *sv_appendv(struct svec *sv, ...);
struct svec *sv_append_own(struct svec *sv, char *str);
struct svec *sv_append_copy(struct svec *sv, const char *str);
struct svec *sv_append_va(struct svec *sv, va_list va);
void sv_clear(struct svec *sv);

inline void fv_set(uint64_t fv[], int n, int i)
{
    int k = i / 64;
    int s = i - k;
    if (k < n)
        fv[k] |= 1ull << s;
}

inline void fv_unset(uint64_t fv[], int n, int i)
{
    int k = i / 64;
    int s = i - k;
    if (k < n)
        fv[k] &= ~(1ull << s);
}

inline int fv_get(uint64_t fv[], int n, int i)
{
    int k = i / 64;
    int s = i - k;
    if (k < n)
        return (fv[k] >> s) & 1;
    return 0;
}

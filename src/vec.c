#include <stdlib.h>
#include <string.h>

#include "vec.h"


extern size_t sv_len(struct svec *sv);

struct svec *sv_make(size_t cap)
{
    struct svec *sv = malloc(
        sizeof(struct svec) + (cap * sizeof(char *))
    );
    if (sv == NULL) {
        return sv;
    }

    sv->len = 0;
    sv->cap = cap;
    sv->alocv[0] = 0;
    sv->alocv[1] = 0;
    return sv;
}


struct svec *sv_append(struct svec *sv, char *str)
{
    static const size_t cap_inc = 12;

    if (sv == NULL) {
        sv = sv_make(cap_inc);
    }

    if (sv->len == sv->cap) {
        size_t newcap = sv->cap + cap_inc;
        struct svec *newsv = realloc(sv,
            sizeof(struct svec) + (newcap * sizeof(char *))
        );

        if (newsv == NULL) {
            // O NO
            return sv;
        }

        sv = newsv;
        sv->cap = newcap;
    }

    sv->vec[sv->len] = str;
    sv->len += 1;
    return sv;
}


struct svec *sv_appendv(struct svec *sv, ...)
{
    va_list va;
    va_start(va, sv);
    sv = sv_append_va(sv, va);
    va_end(va);
    return sv;
}


struct svec *sv_append_own(struct svec *sv, char *str)
{
    size_t i = sv->len;
    sv = sv_append(sv, str);
    if (sv->len > i) {
        fv_set(sv->alocv, 2, i);
    }

    return sv;
}


struct svec *sv_append_copy(struct svec *sv, const char *str)
{
    char *copy = NULL;

    if (str != NULL) {
        size_t slen = strlen(str);
        copy = malloc(slen+1);
        strncpy(copy, str, slen+1);
    }

    return sv_append_own(sv, copy);
}


struct svec *sv_append_va(struct svec *sv, va_list va)
{
    char *arg;
    while ((arg = va_arg(va, char *)) != NULL) {
        sv = sv_append(sv, arg);
    }

    return sv;
}


void sv_clear(struct svec *sv)
{
    for (size_t i = 0; i < sv->len; i += 1) {
        if (fv_get(sv->alocv, 2, i)) {
            free(sv->vec[i]);
        }
    }

    free(sv);
}


extern void fv_set(uint64_t fv[], int n, int i);
extern void fv_unset(uint64_t fv[], int n, int i);
extern int fv_get(uint64_t fv[], int n, int i);

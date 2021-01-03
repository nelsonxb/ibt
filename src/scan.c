#include <stddef.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#include "scan.h"

static int isreg(DIR *dir, struct dirent *entry)
{
#ifdef DT_REG
    return entry->d_type == DT_REG;
#else
    int fdd = dirfd(dir);
    if (fdd < 0) {
        return 0;
    }

    int fdf = openat(fdd, entry->d_name, 0);
    if (fdf < 0) {
        return 0;
    }

    struct stat stats;
    if (fstat(fdf, &stats)) {
        close(fdf);
        return 0;
    }

    close(fdf);
    return S_ISREG(stats.st_mode);
#endif
}

static int extkind(const char *name, enum ibt_file *kind);

int ibt_scan_sources(DIR *srcdir, struct ibt_scan_file *scan)
{
    if (scan == NULL) {
        return -1;
    }

    do {
        errno = 0;
        struct dirent *entry = readdir(srcdir);
        if (entry == NULL) {
            if (errno == 0) {
                break;
            }

            return -1;
        }

        if (isreg(srcdir, entry) && extkind(entry->d_name, &scan->kind)) {
            scan->name = entry->d_name;
            return 1;
        } else if (errno) {
            return -1;
        }

    } while (1);

    return 0;
}

static int extkind(const char *name, enum ibt_file *kind)
{
    size_t nl = strlen(name);

    if (strcmp(&name[nl-2], ".c") == 0)
    {
        *kind = IBT_FILE_C_SOURCE;
        return 1;
    }
    else if (strcmp(&name[nl-2], ".h") == 0)
    {
        *kind = IBT_FILE_C_HEADER;
        return 1;
    }
    else if (strcmp(&name[nl-3], ".cc") == 0
        || strcmp(&name[nl-4], ".c++") == 0
        || strcmp(&name[nl-4], ".cxx") == 0
        || strcmp(&name[nl-4], ".cpp") == 0)
    {
        *kind = IBT_FILE_CXX_SOURCE;
        return 1;
    }
    else if (strcmp(&name[nl-3], ".hh") == 0
        || strcmp(&name[nl-4], ".h++") == 0
        || strcmp(&name[nl-4], ".hxx") == 0
        || strcmp(&name[nl-4], ".hpp") == 0)
    {
        *kind = IBT_FILE_CXX_HEADER;
        return 1;
    }

    return 0;
}

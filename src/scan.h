#pragma once

#include <sys/types.h>
#include <dirent.h>

enum ibt_file {
    IBT_FILE_C_SOURCE = 0,
    IBT_FILE_C_HEADER = 0 | 1,
    IBT_FILE_CXX_SOURCE = 2,
    IBT_FILE_CXX_HEADER = 2 | 1,
};

struct ibt_scan_file {
    enum ibt_file kind;
    const char *name;
};

int ibt_scan_sources(DIR *srcdir, struct ibt_scan_file *scan);

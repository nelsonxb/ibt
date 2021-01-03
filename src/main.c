#include <sys/stat.h>
#include <sys/types.h>
#include "stdio.h"
#include <errno.h>
#include <err.h>
#include <string.h>

#include "vec.h"
#include "scan.h"
#include "exec.h"


int main(int argc, char *argv[])
{
    DIR *src = opendir("src");
    if (src == NULL) {
        err(1, "opendir src");
    }

    DIR *build;
_openbuild:
    build = opendir("build");
    if (build == NULL) {
        if (errno == ENOENT) {
            if (mkdir("build", 0777)) {
                err(1, "mkdir build");
            }

            goto _openbuild;
        }
        err(1, "opendir build");
    }

    int buildfd = dirfd(build);

    struct svec *ldargv = sv_appendv(NULL, "gcc", "-o", "ibt", NULL);
    enum ibt_file ldkind = IBT_FILE_C_SOURCE;
    int status, ok = 1;
    for (struct ibt_scan_file scan; (status = ibt_scan_sources(src, &scan)) > 0; ) {
        if (scan.kind & 1) {
            continue;
        }

        char cpath[PATH_MAX];
        strcpy(cpath, "../src/");
        strcpy(&cpath[7], scan.name);

        char opath[PATH_MAX];
        strcpy(opath, scan.name);

        char *c = strchr(opath, '.');
        if (c == NULL) {
            if ((c = strchr(opath, '\0')) == NULL) {
                continue;
            }
        }

        strcpy(c, ".o");
        ldargv = sv_append_copy(ldargv, opath);

        char *cc = "gcc", *cstd = "-std=gnu18";
        if (scan.kind & 2) {
            ldkind = IBT_FILE_CXX_SOURCE;
            cc = "g++";
            cstd = "-std=c++17";
        }

        int code = ibt_pexec(buildfd, cc, "-Wall", "-Werror", cstd,
            "-c", "-o", opath, cpath,
            NULL);
        if (code != 0) {
            ok = 0;
        }
    }

    if (status < 0) {
        err(1, "scanning src");
    }

    if (!ok) {
        return 1;
    }

    if (ldkind == IBT_FILE_CXX_SOURCE) {
        ldargv->vec[0] = "g++";
    }

    ldargv = sv_append(ldargv, NULL);
    ibt_pexecv(buildfd, ldargv->vec);

    return 0;
}

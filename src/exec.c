#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <err.h>
#include <stdio.h>

#include "exec.h"
#include "vec.h"


int ibt_exec(int workdir, char *progname, ...)
{
    va_list vl;
    va_start(vl, progname);
    struct svec *argsv = sv_append(NULL, progname);
    argsv = sv_append_va(argsv, vl);
    va_end(vl);
    argsv = sv_append(argsv, NULL);

    int code = ibt_execv(workdir, &argsv->vec[0]);
    free(argsv);

    return code;
}


int ibt_pexec(int workdir, char *progname, ...)
{
    va_list vl;
    va_start(vl, progname);
    struct svec *argsv = sv_append(NULL, progname);
    argsv = sv_append_va(argsv, vl);
    va_end(vl);
    argsv = sv_append(argsv, NULL);

    int code = ibt_pexecv(workdir, &argsv->vec[0]);
    free(argsv);

    return code;
}


int ibt_execv(int workdir, char *const argv[])
{
    pid_t cpid = fork();
    if (cpid == 0) {
        if (workdir > 0 && fchdir(workdir)) {
            err(-1, "failed to chdir");
        }

        execvp(argv[0], argv);
        // Exec failed!
        err(-1, "failed to exec %s", argv[0]);

    } else if (cpid < 0) {
        return -1;
    }

    // Left in the parent process with a valid child process.
    // Wait for process to exit
    int code;
    waitpid(cpid, &code, 0);

    return code;
}


int ibt_pexecv(int workdir, char *const argv[])
{
    printf("%s", argv[0]);
    for (size_t i = 1; argv[i] != NULL; i += 1) {
        printf(" %s", argv[i]);
    }

    printf("\n");

    return ibt_execv(workdir, argv);
}

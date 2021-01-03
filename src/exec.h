#pragma once

int ibt_exec(int workdir, char *progname, ...);
int ibt_pexec(int workdir, char *progname, ...);
int ibt_execv(int workdir, char *const argv[]);
int ibt_pexecv(int workdir, char *const argv[]);

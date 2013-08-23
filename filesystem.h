#include "file-system-internals.h"

void mkfs(Filesystem *files);
int touch(Filesystem *files, const char arg[]);
int mkdir(Filesystem *files, const char arg[]);
int cd(Filesystem *files, const char arg[]);
int ls(Filesystem files, const char arg[]);
void pwd(Filesystem files);
void rmfs(Filesystem *files);
int rm(Filesystem *files, const char arg[]);
int re_name(Filesystem *files, const char arg1[], const char arg2[]);

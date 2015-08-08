/*
 * ©2015 Lambda Project, Quinn Evans <heddwch@lambdaos.org>
 * See COPYING for more information
 */

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sysexits.h>
#include <sys/types.h>
#include <sys/stat.h>

extern char *optarg;
extern int optind;

void usage(char*[]);
int ensure_db_root(char*);

int main(int argc, char *argv[]) {
  char db_root[0x100];
  db_root[0]='\0';
  
  int opt;
  while((opt=getopt(argc, argv, "")) != -1) {
    switch(opt) {
    case '?':
      usage(argv);
    }
  }
  if(optind == (argc - 1))
    strncpy(db_root, argv[optind], sizeof(db_root) - 2);
  else if(optind == argc)
    strncpy(db_root, getenv("HOME"), sizeof(db_root) - 2);
  else usage(argv);
  db_root[sizeof(db_root) - 1] = '\0';

  if(ensure_db_root(db_root))
    err(EX_DATAERR, "chdir to %s failed", db_root);

  return 0;
}
  
void usage(char *argv[]) {
  errx(EX_USAGE, "usage: %s [db_root]", argv[0]);
  return;
}

int ensure_db_root(char *db_root)
{
  struct stat sb;
  if(stat(db_root, &sb))
    err(EX_DATAERR, "can't stat db_root: %s", db_root);
  if(!S_ISDIR(sb.st_mode))
    errx(EX_DATAERR, "db_root not a directory: %s", db_root);

  return chdir(db_root);
}

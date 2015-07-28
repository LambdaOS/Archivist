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

void usage();
int find_or_create_db_root(char*);

int main(int argc, char *argv[]) {
  int tmp;
  char db_root[0x100];
  db_root[0]='\0';
  
  int opt;
  while((opt=getopt(argc, argv, "d:")) > 0) {
    switch(opt) {
    case 'd':
      strncpy(db_root, optarg, sizeof(db_root) - 1);
      break;
    case '?':
      usage(argv);
    }
  }
  if(!strlen(db_root)) {
    strncpy(db_root, getenv("HOME"), sizeof(db_root) - 1);
    strncat(db_root, "/Archivist", sizeof(db_root) - 1 - strlen(db_root));
  }

  if(find_or_create_db_root(db_root))
    err(EX_DATAERR, "chdir failed to db_root: %s", db_root);

  return 0;
}
  
void usage(char *argv[]) {
  errx(EX_USAGE, "usage: %s [-d db_root]\n", argv[0]);
  return;
}

int find_or_create_db_root(char *db_root)
{
  struct stat sb;
  if(stat(db_root, &sb)) {
    if(errno == ENOENT) {
      if(mkdir(db_root, S_IRWXU))
	err(EX_CANTCREAT, "can't create db_root: %s", db_root);
      return find_or_create_db_root(db_root);
    }
    else
      err(EX_DATAERR, "can't stat db_root: %s", db_root);
  }
  if(!S_ISDIR(sb.st_mode))
    errx(EX_DATAERR, "db_root not a directory: %s", db_root);

  return chdir(db_root);
}

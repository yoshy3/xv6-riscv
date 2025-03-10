#include "../kernel/types.h"
#include "../kernel/fcntl.h"
#include "../kernel/fs.h"
#include "../kernel/stat.h"
#include "user.h"

#define NULL   ((void*)0)
#define FALSE  (0)
#define TRUE   (1)

#define PATH_SEPARATOR   "/"

static int getcwd(char* resultPath);
static char* goUp(int ino, char* ancestorPath, char* resultPath);
static int dirlookup(int fd, int ino, char* p);

int main(int argc, char *argv[]) {
  char resultPath[512];
  if (getcwd(resultPath))
    printf("%s\n", resultPath);
  else
    printf("pwd failed");
  exit(0);
}

static int getcwd(char* resultPath) {
  resultPath[0] = '\0';

  char ancestorPath[512];
  strcpy(ancestorPath, ".");

  struct stat st;
  if (stat(ancestorPath, &st) < 0)
    return FALSE;

  char* p = goUp(st.ino, ancestorPath, resultPath);
  if (p == NULL)
    return FALSE;
  if (resultPath[0] == '\0')
    strcpy(resultPath, PATH_SEPARATOR);
  return TRUE;
}

static char* goUp(int ino, char* ancestorPath, char* resultPath) {
  strcpy(ancestorPath + strlen(ancestorPath), PATH_SEPARATOR "..");
  struct stat st;
  if (stat(ancestorPath, &st) < 0)
    return NULL;

  if (st.ino == ino) {
    // No parent directory exists: must be the root.
    return resultPath;
  }

  char* foundPath = NULL;
  int fd = open(ancestorPath, O_RDONLY);
  if (fd >= 0) {
    char* p = goUp(st.ino, ancestorPath, resultPath);
    if (p != NULL) {
      strcpy(p, PATH_SEPARATOR);
      p += sizeof(PATH_SEPARATOR) - 1;

      // Find current directory.
      if (dirlookup(fd, ino, p))
        foundPath = p + strlen(p);
    }
    close(fd);
  }
  return foundPath;
}

// @param fd   file descriptor for a directory.
// @param ino  target inode number.
// @param p    [out] file name (part of absPath), overwritten by the file name of the ino.
static int dirlookup(int fd, int ino, char* p) {
  struct dirent de;
  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0)
      continue;
    if (de.inum == ino) {
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = '\0';
      return TRUE;
    }
  }
  return FALSE;
}




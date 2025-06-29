#include "../include/paths.h"

// expand home path, addresses file, etc.
char *getHomePath() {
  const char *home = getenv("HOME");
  if (!home) {
    struct passwd *pw = getpwuid(getuid());
    home = pw->pw_dir;
  }

  char *path = malloc(strlen(home) + 32);
  sprintf(path, "%s/.config/sshbook/book", home);

  char dir[2048];
  strcpy(dir, path);
  *strrchr(dir, '/') = 0;
  mkdir(dir, 0755);

  char *sub = strrchr(dir, '/');
  if (sub) {
    mkdir(dir, 0755);
  }
  return path;
}

/*
char *getDescPath() {
  const char *home = getenv("HOME");
  if (!home) {
    struct passwd *pw = getpwuid(getuid());
    home = pw->pw_dir;
  }

  char *path = malloc(strlen(home) + 32);
  sprintf(path, "%s/.config/sshbook/desc", home);

  char dir[2048];
  strcpy(dir, path);
  *strrchr(dir, '/') = 0;
  mkdir(dir, 0755);

  char *sub = strrchr(dir, '/');
  if (sub) {
    mkdir(dir, 0755);
  }
  return path;
}
*/
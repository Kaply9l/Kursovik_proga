#include "hash.h"
#include "stat.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int list_dir(char *dir, int parent);
void chapath(char *path);
int save_info(char *file, char **output);

int id = 1;

FILE *file = NULL;

int main(int argc, char **argv) {
  if ((argc < 5) || (argc > 6)) {
    fprintf(stderr, "Please specify the file names!\n");
    fprintf(stderr, "Basic usage: integrctrl –s –f data path\n");
    fprintf(stderr, "Basic usage: integrctrl –s -r –f data path\n");
    fprintf(stderr, "Basic usage: integrctrl –c –f data path\n");
    return EXIT_FAILURE;
  }

  int key, mode = 0, recursive = 0, file_count = 0;
  char *data = NULL;
  char *path = NULL;

  // opterr = 0; // запретить вывод ошибок от getopt()

  // s-Save integrity info; c-check integrity info
  while ((key = getopt(argc, argv, "scrf:")) != -1) {
    switch (key) {
    default: {
      fprintf(stderr, "Basic usage: integrctrl –s –f data path\n");
      fprintf(stderr, "Basic usage: integrctrl –s -r –f data path\n");
      fprintf(stderr, "Basic usage: integrctrl –c –f data path\n");
      return 4;
      break;
    }
    case 's': {
      if (mode != 0) {
        fprintf(stderr,
                "You're using keys -s and -c together or more than one\n");
        return 5;
      }
      mode++;
      break;
    }
    case 'c': {
      if (mode != 0) {
        fprintf(stderr,
                "You're using keys -s and -c together or more than one\n");
        return 5;
      }
      mode = mode + 2;
    }
    case 'r': {
      if (recursive != 0) {
        fprintf(stderr, "You use -r more than one time\n");
        return 5;
      }
      recursive++;
      break;
    }
    case 'f': {
      if (file_count != 0) {
        fprintf(stderr, "You use -f more than one time\n");
        return 5;
      }
      if ((strcmp(optarg, "-f") == 0) || (strcmp(argv[optind], "-f") == 0)) {
        fprintf(stderr, "You use -f more than one time\n");
        return 5;
      }
      if ((strcmp(optarg, "-r") == 0) || (strcmp(argv[optind], "-r") == 0)) {
        fprintf(stderr, "Value is key\n");
        return 6;
      }
      if ((strcmp(optarg, "-s") == 0) || (strcmp(argv[optind], "-s") == 0)) {
        fprintf(stderr, "Value is key\n");
        return 6;
      }
      if ((strcmp(optarg, "-c") == 0) || (strcmp(argv[optind], "-c") == 0)) {
        fprintf(stderr, "Value is key\n");
        return 6;
      }
      if (!argv[optind]) {
        fprintf(stderr, "Could not find the path directory\n");
        return 6;
      }
      data = optarg;
      path = argv[optind];
      break;
    }
    }
  }

  // printf("%s\n%s\n", data, path);
  if (mode == 1) {
    // printf("mode %s\n", "Save information");
  } else if (mode == 2) {
    // printf("mode %s\n", "Check infromation");
  } else {
    fprintf(stderr, "Incorrect mode\n");
    return 8;
  }
  file = fopen(data, "w");

  chapath(path);
  printf("Directory %s\n", path);
  fprintf(file, "%d %s %s %d\n", id, path, "dir", 0);
  id++;
  if (list_dir(path, (id - 1)) == 69) {
    printf("Oops\n");
    return 69;
  }

  fclose(file);
  return 0;
}

int list_dir(char *path, int parent) {
  int n;
  DIR *dir;             // Директория
  struct dirent *entry; // Элемент директории
  char new_path[PATH_MAX];
  dir = opendir(path);
  if (!dir) {
    fprintf(stderr, "Incorrect path of directory\n");
    return 69;
  }

  while ((entry = readdir(dir)) != NULL) {
    if ((entry->d_type == 4) &&
        (strcmp(entry->d_name, ".") != 0) && // If this's directory
        (strcmp(entry->d_name, "..")) != 0) {

      n = strlen(path) - 1;
      while (path[n] == '/') { // Delete slashes
        path[n] = '\0';
        n--;
      }

      snprintf(new_path, PATH_MAX, "%s/%s", path,
               entry->d_name); // Create the path for the directory
      printf("Directory %d %s\n", parent, new_path);
      fprintf(file, "%d %s %s %d\n", id, entry->d_name, "dir", parent);
      id++;
      list_dir(new_path, (id - 1)); // Read new the path
    } else if ((strcmp(entry->d_name, ".") !=
                0) && // Do something if it is not dot or directory
               (strcmp(entry->d_name, "..")) != 0) {
      printf("File %d %s hash\n", parent, entry->d_name);
      fprintf(file, "%d %s %s %d hash\n", id, entry->d_name, "file", parent);
      id++;
    }
  }

  closedir(dir);
}
int save_info(char *file, char **output) {}
void chapath(char *path) {
  int i = 0;

  if (strcmp(path, "./") == 0)
    return;
  if (strcmp(path, "/") == 0)
    return;
  if (path[0] == '.') {
    while (path[i + 2] != '\0') {
      path[i] = path[i + 2];
      i++;
    }
    path[i] = '\0';
  }
}

#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/unistd.h>
#include <sys/stat.h> 
#include <stdlib.h>
#include <string.h>

static int z = 0;

void DFS(const char *st, int deep, const char *file) { // st – стартовый узел
  int r; 
  DIR * hw_pointer;
  struct dirent *entry;
  struct stat *buf;
  z++;
  printf("Ищем %s в директории %s\n", file, st);
  
  if (z <= deep) { 
    if ((hw_pointer = opendir(st)) == NULL) {
      fprintf(stderr, "Error\n");
      perror("Your error");
      exit(EXIT_FAILURE);
    }

    while ( (entry = readdir(hw_pointer)) != NULL) {
      printf("%ld - %s [%d] %d\n",
      entry->d_ino, entry->d_name, entry->d_type, entry->d_reclen);
    
      if (!strcmp(entry->d_name, file)) {
        printf("Нашли %s в директории %s\n", file, st);
        exit(EXIT_SUCCESS);    
      }
      
      if (entry->d_type == 4) {
        char new_dir[100];
        strcpy(new_dir, st);
        strcat(new_dir, "/");
        strcat(new_dir, entry->d_name); 
        DFS(new_dir, deep, file);
      }
    }
    closedir(hw_pointer);
  }
    z--;
}


int main(int argc, char** argv) {
  if(argc != 4) {
    fprintf(stderr, "Usage: %s ДИРЕКТОРИЯ ГЛУБИНА_ПОИСКА ИМЯ_ФАЙЛА\n", argv[0]);
    exit(EXIT_FAILURE); 
  }

  int deep1 = atoi(argv[2]);

/*
 * #define MAX_FILE_NAME 100
 * char dir_name[MAX_FILE_NAME];
 *
 * Не используйте в коде магических чисел.
 * И зачем вы объявили переменную как static?
 * http://stackoverflow.com/questions/572547/what-does-static-mean-in-a-c-program 
 * 
 * В вашем случае static лишнее.
 */
 
  static char dir_name[100];
  static char file_name[100];
  
  strcpy(dir_name, argv[1]);
  strcpy(file_name, argv[3]);

  DFS(dir_name, deep1, file_name);

return 0;
}

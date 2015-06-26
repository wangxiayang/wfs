#include <stdio.h>
#include <string.h>

int main() {
  char *p = "aabbccdd";
  char a[10];
  int i = 0;
  for (;i < 10; i++) {
    a[i] = 'a';
  }
  strcpy(a, p);
  printf("version=%d %s\n", strlen(p), a);
  return 0;
}

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>

//char* strs[] = { "abcd", "efgd", "ijk" }; 

int main() { 
//  int fd = open ("/dev/ttyACM0", O_RDWR); 
  FILE* f = fopen("/dev/ttyACM0", "r"); 
//  assert(fd != -1); 
  assert(f); 
  uint32_t counter;
  int shift = 16;
  int buf[200]; 
  assert(200 == fread(&buf, 4, 200, f));
  for (int i = 0; i < 10000; ++i) {
    uint32_t buf;
    assert(1 == fread(&buf, 4, 1, f));
    uint32_t nextCounter = buf >> shift;
    if (i != 0 && i != 1) 
    //if (counter + 1 != nextCounter) printf("oblom %i %i \n", counter, nextCounter); // 
      assert(counter + 1 == nextCounter); 
//    if (i == 1 && counter + 1 != nextCounter) shift = 16 - shift; 
    counter = nextCounter;
    printf("%08x\n", buf); 
  }
/*  for (int i = 0; i < 3; ++i) { 
    assert(3 == write(fd, strs[i], 3)); 
    char buf[4]; 
    assert(3 == read(fd, buf, 3)); 
    buf[3] = 0; 
    printf("%s\n", buf); 
  }*/
  assert(0 == fclose(f)); 
  return 0;
}


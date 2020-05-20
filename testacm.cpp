#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <cstring>
#include <cerrno>
#include <QSerialPort>

//char* strs[] = { "abcd", "efgd", "ijk" }; 

#include <string>
#include <vector>

using namespace std;

using cmd = vector<uint8_t>; 

cmd makeReadReg(uint8_t reg) { 
  cmd buf { (2 << 1) | 1, reg }; 
  return buf;
}

cmd makeGetEncoderValue(uint8_t n) { return makeReadReg(0x30 + n); } 


int main() { 

  QSerialPort p("/dev/ttyACM0");
  p.open(QIODevice::ReadWrite);

  for (int i = 0; i < 1000000; ++i) { 
    auto c { makeGetEncoderValue(i % 4) }; 
    assert(c.size() == p.write(reinterpret_cast<char*>(c.data()), c.size())); 
    assert(p.waitForBytesWritten(100)); 
    int value = 0xcacacaca; 
    assert(p.waitForReadyRead(-1)); 
    assert(4 == p.read(reinterpret_cast<char*>(&value), 4)); 
    assert(value == (i / 4)); 
//    fprintf(stderr, "%08x\n", value);
  }

  return 0; 
}


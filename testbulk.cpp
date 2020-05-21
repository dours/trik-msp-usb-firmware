
#include <libusb-1.0/libusb.h>
#include <cstdio>
#include <unistd.h>
#include <cassert>

int main() { 
  libusb_context* context;
  libusb_init(&context); 
  libusb_device_handle* handle = libusb_open_device_with_vid_pid(context, 0x2047, 0x0301);
  assert(handle); 
  assert(0 == libusb_claim_interface(handle, 0));
  for (int iteration = 0; iteration < 1000; ++iteration) { 
    int transferred = -1; 
    unsigned short buf[32]; 
    int error = libusb_bulk_transfer(handle, 0x81, (unsigned char*)buf, 64, &transferred, 100); 
    if (0 != error) fprintf(stderr, "error = %i\n", error); 
    assert(0 == error); 
    assert(64 == transferred);
#if 1
    printf("N%i  ", iteration);
    for (int j = 0; j < 11; ++j) printf("%04x  ", buf[j]); 
    printf("\n"); 
#else
    uint16_t value = (int(buf[1]) << 8) | buf[0];
    if (iteration == 0) iteration = value; 
    assert(value == (iteration & 0xffff)); 
#endif
  }
}


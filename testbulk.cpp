
#include <libusb.h>

int main() { 

  libusb_device** list;
  assert(0 == libusb_get_device_list(&context, &list));


  libusb_device_handle* handle; 
  assert(0 == libusb_open(device, &handle)); 

  while (1) { 
    int transferred = -1; 
    int error = libusb_bulk_transfer(handle, 0x81, buf, 64, &transferred, 10); 
    assert(64 == transferred);
    assert(0 == error); 


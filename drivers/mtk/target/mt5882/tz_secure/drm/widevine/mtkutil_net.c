#include "type_c.h"

union {
  uint32_t array[2];
  int64_t number;
} mixed;


//ntohl
uint32_t net_to_host(uint32_t net_uint32)
{
    unsigned char *addr = (unsigned char*) &net_uint32;
    return ((unsigned int)((*addr))) << 24 | ((unsigned int)(*(addr+1))) << 16 | ((unsigned int)(*(addr+2))) << 8 | ((unsigned int)(*(addr+3))) ; 
}

//htonl

uint32_t htonl(uint32_t net_uint32)
{
    unsigned char *addr = (unsigned char*) &net_uint32;
    return ((unsigned int)((*addr))) << 24 | ((unsigned int)(*(addr+1))) << 16 | ((unsigned int)(*(addr+2))) << 8 | ((unsigned int)(*(addr+3))) ; 
}

//htonll64
int64_t htonll64(int64_t x) {  // Convert to big endian (network-byte-order)

  uint32_t temp;
  
  mixed.number = 1;
  if (mixed.array[0] == 1) {
    mixed.number = x;  // Little Endian.
    temp = mixed.array[0];
    mixed.array[0] = htonl(mixed.array[1]);
    mixed.array[1] = htonl(temp);
    return mixed.number;
  } else {
    return x;  // Big Endian.
  }
}

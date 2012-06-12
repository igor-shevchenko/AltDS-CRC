#ifndef __CRC_H__
#define __CRC_H__

void crc_init();
unsigned long crc_cycle(unsigned long crc32, unsigned char* buf, unsigned int cnt);

#endif
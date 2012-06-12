#include "crc.h"

unsigned long crc_table[256];

void crc_init(){
    unsigned long t;
    int i,j;
    for(i=0;i<256;i++){
        t=i;
        for(j=8;j>0;j--){
            if(t&1)
                t=(t>>1)^0xedb88320;
            else
                t>>=1;
        }
        crc_table[i] = t;
    }
}

unsigned long crc_cycle(unsigned long crc32, unsigned char *buf, unsigned int cnt){
    crc32=~crc32;
    unsigned int i;
    for(i=0;i<cnt;i++){
        crc32=crc_table[(crc32^buf[i])&0xff]^(crc32>>8);
    }
    crc32=~crc32;
    return crc32;
}
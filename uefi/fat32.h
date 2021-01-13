#ifndef __FAT_32_H__
#define __FAT_32_H__

#include <stdint.h>

struct __attribute__((__packed__)) Fat32MBR
{
    uint8_t jumpCode[3];
    uint8_t oemName[8];
    uint16_t bytesPerSector;
    uint8_t sectorsPerCluster;
    uint16_t reseveSectors;
    uint8_t fats;
    uint16_t direcotriesInRoot;
    uint16_t totalSectors16;
    uint8_t media;
    uint16_t fatSize16;
    uint16_t sectorsPerTruck;
    uint32_t hiddenSectors;
    uint32_t totalSectors32;
    uint32_t fatSize32;
    uint16_t flag;
    uint16_t version;
    uint32_t rootClusterPosition;
    uint16_t fsinfoPosition;
    


};


#endif  /* __FAT_32_H__ */

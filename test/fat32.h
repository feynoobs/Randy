#ifndef __FAT_32_H__
#define __FAT_32_H__

#include <stdint.h>
#include <dirent.h>
typedef struct __attribute__((__packed__)) tagFat32MBR
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
    uint16_t backupPosition;
    uint8_t reserved1[12];
    uint8_t driveNumber;
    uint8_t reserved2;
    uint8_t signature;
    uint32_t volumeID;
    uint8_t volumeLabel[11];
    uint8_t fileSystemType[8];
    uint8_t bootCode[420];
    uint16_t sign;
} Fat32MBR;

typedef struct tagDIR
{
    uint64_t sector;
    uint64_t posision;
} DIR;


#endif  /* __FAT_32_H__ */

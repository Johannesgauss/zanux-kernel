#include "util/alias.h"

#define FAT_SECTOR_SIZE

struct fat_bpb {
	uint8_t jmp[3];
	uint64_t oem;
	uint16_t bytes_per_sector;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sectors;
	uint8_t fat_num;
	uint16_t rootdir_entry_num;
	uint16_t total_sectors;
	uint8_t media_descriptor_type;
	uint16_t sectors_per_fat;
	uint16_t sectors_per_track;
	uint16_t heads_num;
	uint32_t hidden_sectors_num;
	uint32_t large_sector_count;
} __attribute__((packed));

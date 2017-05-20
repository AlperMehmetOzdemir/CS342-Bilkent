/*
 * Celik Koseoglu
 * CS342 Bilkent - Project 5
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>

#define SECTORSIZE 512   //bytes
#define BLOCKSIZE  4096  //bytes - do not change this value

char disk_name[48];
int disk_fd;

uint8_t volumesector[SECTORSIZE];

//First sector of the FAT file system contains the master boot record.
//The master boot record contains information about the volume.
//The first thing in this sector is the jmp instruction for x86 machines.
//This instruction basically allows the machine to boot.

//The following struct is derived from OSDevWiki's FAT entry.
//BPB stands for BIOS Parameter Block
//use of uints may look confusing but makes it platform independent... (can run on both x86 and x64)
struct BIOSParameterBlock {
    uint8_t BPB_jmp_instruction[3];
    uint8_t BPB_OEM_name[8];
    uint16_t BPB_bytes_per_sector;
    uint8_t BPB_sectors_per_cluster;
    uint16_t BPB_number_of_reserved_sectors; //relocated sector count from the SMART status probably reads if any of these blocks are occupied
    uint8_t BPB_number_of_file_allocation_tables;
    uint16_t BPB_number_of_directory_entries;
    uint16_t BPB_number_of_sectors; //if this value returns 0, then there are more than 65535 sectors
    uint8_t BPB_media_decriptor_type;
    uint16_t BPB_number_of_sectors_per_file_allocation_table; //this applies to fat12/fat16 only. 0 otherwise
    uint16_t BPB_number_of_sectors_per_track;
    uint16_t BPB_number_of_disk_heads_or_sides; //should apply for spinning hard disk drives only
    uint32_t BPB_number_of_hidden_sectors;
    uint32_t BPB_large_amount_sector_on_media; //this field is set if there are more than 65535 sectors

    //the rest applies for extended boot records
    uint32_t BPB_sectors_per_fat; //size of fat in sectors
    uint16_t BPB_flags;
    uint16_t BPB_fat_version_number; //high byte s the major, low byte is the minor version
    uint32_t BPB_cluster_number_of_root_directory; //usually set to 2
    uint16_t BPB_sector_number_of_FSInfo_struct;
    uint16_t BPB_sector_number_of_backup_boot_sector;
    uint8_t BPB_reserved[12]; //should all be set to zero when volume is formatted
    uint8_t BPB_drive_number; //0x00 for floppy disks, 0x80 for hard disks
    uint8_t BPB_flags_in_windows_NT;
    uint8_t BPB_signature;
    uint32_t BPB_volume_serial_number; //hardware specific. used to tracking volumes between computers
    uint8_t BPB_volume_label_string[11];
    uint8_t BPB_system_identifier_string[8]; //Always says "FAT32 ". Useless...
    uint8_t BPB_boot_code[420];
    uint16_t BPB_bootable_partition_signature; //used for sanity checks. Has to be 0xaa55 for the drive to be bootable.
} __attribute__((packed));

//The directory structure of the FAT filesystem. Create date and create time resides in the reserved block. Check microsoft's documentation.
struct DIRStructure {
    uint8_t DIR_file_name[8];
    uint8_t DIR_extension[3];
    uint8_t DIR_attributes;
    uint8_t DIR_reserved[10];
    uint16_t DIR_modify_time;
    uint16_t DIR_modify_date;
    uint16_t DIR_starting_cluster;
    uint32_t DIR_file_size;
} __attribute__((packed));

//given in the project skeleton
int get_sector(uint8_t *buf, int snum) {
    off_t offset;
    int n;
    offset = snum * SECTORSIZE;
    lseek(disk_fd, offset, SEEK_SET);
    n = read(disk_fd, buf, SECTORSIZE);
    if (n == SECTORSIZE)
        return (0);
    else {
        printf("sector number %d invalid or read error.\n", snum);
        exit(1);
    }
}

//given in the project skeleton
void print_sector(uint8_t *s) {
    int i;

    for (i = 0; i < SECTORSIZE; ++i) {
        printf("%02x ", (uint8_t) s[i]);
        if ((i + 1) % 16 == 0)
            printf("\n");
    }
    printf("\n");
}

//convert lowercase string to uppercase string
void upper_string(char *s) {
    int c = 0;

    while (s[c] != '\0') {
        if (s[c] >= 'a' && s[c] <= 'z') {
            s[c] = s[c] - 32;
        }
        c++;
    }
}

//basic str_replace function, taken from: http://www.binarytides.com/str_replace-for-c/
char *str_replace(char *search, char *replace, char *subject) {
    char *p = NULL, *old = NULL, *new_subject = NULL;
    int c = 0, search_size;

    search_size = strlen(search);

    //Count how many occurences
    for (p = strstr(subject, search); p != NULL; p = strstr(p + search_size, search)) {
        c++;
    }

    //Final size
    c = (strlen(replace) - search_size) * c + strlen(subject);

    //New subject with new size
    new_subject = malloc(c);

    //Set it to blank
    strcpy(new_subject, "");

    //The start position
    old = subject;

    for (p = strstr(subject, search); p != NULL; p = strstr(p + search_size, search)) {
        //move ahead and copy some text from original subject , from a certain position
        strncpy(new_subject + strlen(new_subject), old, p - old);

        //move ahead and copy the replacement text
        strcpy(new_subject + strlen(new_subject), replace);

        //The new start position after this search match
        old = p + search_size;
    }

    //Copy the part after the last search match
    strcpy(new_subject + strlen(new_subject), old);

    return new_subject;
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("wrong usage\n");
        exit(1);
    }

    strcpy(disk_name, argv[1]);

    disk_fd = open(disk_name, O_RDWR);
    if (disk_fd < 0) {
        printf("could not open the disk image\n");
        exit(1);
    }

    struct BIOSParameterBlock bootEntry;
    struct DIRStructure dirEntry;

    get_sector(volumesector, 0);

    //get the first sector of the drive. It is always the MasterBootRecord.
    memcpy(&bootEntry, volumesector, sizeof(struct BIOSParameterBlock));

    int FAT1_sector_offset = bootEntry.BPB_number_of_reserved_sectors;
    //int FAT2_sector_offset = bootEntry.BPB_number_of_reserved_sectors + bootEntry.BPB_sectors_per_fat; //in case you need it...
    int data_sector_offset = bootEntry.BPB_number_of_reserved_sectors + 2 * bootEntry.BPB_sectors_per_fat;

    if (strcmp(argv[3], "volumeinfo") == 0) {
        printf("Name = %s\n", bootEntry.BPB_OEM_name);
        printf("Bytes per Sector = %d\n", bootEntry.BPB_bytes_per_sector);
        printf("Sectors per Cluster = %d\n", bootEntry.BPB_sectors_per_cluster);
        printf("Reserved Sector Count = %d\n", bootEntry.BPB_number_of_reserved_sectors);
        printf("Number of FATs = %d\n", bootEntry.BPB_number_of_file_allocation_tables);
        printf("Size of FAT = %d bytes\n", bootEntry.BPB_sectors_per_fat);
        printf("Number of Directory Entries = %d\n", bootEntry.BPB_number_of_directory_entries);
        printf("Partition Signature for Sanity Check = %04X\n", bootEntry.BPB_bootable_partition_signature);
        printf("FAT Sector 1 offset: %d, FAT Sector 2 offset: %d\n", bootEntry.BPB_number_of_reserved_sectors, bootEntry.BPB_number_of_reserved_sectors + bootEntry.BPB_sectors_per_fat);
        printf("You can print any other information from the BIOSParameterBlock struct. Check code...\n");
    }

    else if (strcmp(argv[3], "rootdir") == 0) {
        //project specification says that there will be no subdirectories and no more than 50 files. The root cluster will span through only one cluster which is cluster 2.
        for (int i = 0; i < bootEntry.BPB_sectors_per_cluster; i++) {
            get_sector(volumesector, data_sector_offset + i);
            //a sector can only contain 16 entries. 512 bytes per sector / 32 bytes per file entry
            for (int j = 0; j < bootEntry.BPB_bytes_per_sector / sizeof(struct DIRStructure); j++) {
                memcpy(&dirEntry, volumesector + j * sizeof(struct DIRStructure), sizeof(struct DIRStructure));
                if (dirEntry.DIR_file_name[0] != 0x00 && dirEntry.DIR_file_name[0] != 0xe5) {

                    if (dirEntry.DIR_extension[0] == ' ')
                        printf("directory = %.8s\n", dirEntry.DIR_file_name);
                    else {
                        printf("filename = %.8s\n", dirEntry.DIR_file_name);
                        printf("extension = %s\n", dirEntry.DIR_extension);
                    }
                    printf("size = %d\n", dirEntry.DIR_file_size == -1 ? 0 : dirEntry.DIR_file_size);
                    printf("attributes = %d\n", dirEntry.DIR_attributes);
                    //from right to left, first 5 bits represent the date, next four the month, next 7 is years since 1980. Read more from: https://technet.microsoft.com/en-us/library/cc938438.aspx
                    printf("modified at %d : %d : %d\n", dirEntry.DIR_modify_date & 0x001f, (dirEntry.DIR_modify_date & 0x01ff) >> 5, 1980 + ((dirEntry.DIR_modify_date & 0xfd00) >> 9));
                    printf("--\n");
                }
            }
        }
    }

    else if (strcmp(argv[3], "blocks") == 0) {

        //convert the specified filename to uppercase letters for comparison later.
        upper_string(argv[4]);
        printf("Searching for %s...\n", argv[4]);

        //flag to respond the user if the specified file is not found
        short found = 0;

        //project specification says that there will be no subdirectories and no more than 50 files. The root cluster will span through only one cluster which is cluster 2.
        for (int i = 0; i < bootEntry.BPB_sectors_per_cluster; i++) {
            get_sector(volumesector, data_sector_offset + i);
            //a sector can only contain 16 entries. 512 bytes per sector / 32 bytes per file entry
            for (int j = 0; j < bootEntry.BPB_bytes_per_sector / sizeof(struct DIRStructure); j++) {
                memcpy(&dirEntry, volumesector + j * sizeof(struct DIRStructure), sizeof(struct DIRStructure));
                if (dirEntry.DIR_file_name[0] != 0x00 && dirEntry.DIR_file_name[0] != 0xe5) {
                    if (strcmp(str_replace(".", "", argv[4]), str_replace(" ", "", dirEntry.DIR_file_name)) == 0) {

                        found = 1; //we found the file. no need to tell the user the file is not found...

                        //first cluster location of the file
                        uint32_t cluster_chain = dirEntry.DIR_starting_cluster;

                        if (cluster_chain == 0) {
                            printf("File is empty. No blocks...\n");
                            break;
                        }

                        //while the cluster is still pointing to another cluster...
                        while (cluster_chain < 0xFFFFFF0) {

                            printf("Cluster at block = %d\n", cluster_chain);
                            uint32_t sector_offset = cluster_chain / 128; //there are 128 cluster chains per sector. 512 bytes per sector / 4 bytes per cluster chain...
                            cluster_chain = cluster_chain - sector_offset * 128; //our cluster chain offset for the current sector

                            if (sector_offset > bootEntry.BPB_sectors_per_cluster) { //in the case that a cluster chain is bad and pointing to outside the FAT
                                printf("Filesystem corrupted. Cluster chain pointing outside file allocation table\n");
                                break;
                            } else {
                                get_sector(volumesector, FAT1_sector_offset + sector_offset);
                                memcpy(&cluster_chain, volumesector + cluster_chain * sizeof(uint32_t), sizeof(uint32_t));
                            }
                        }
                    }
                }
            }
        }

        if (!found)
            printf("File not found!");

    }

    close(disk_fd);

    return (0);
}
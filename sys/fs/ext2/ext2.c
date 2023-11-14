#include "fs/ext2/ext2.h"

#include "config.h"
#include "dev/disk/disk.h"
#include "errno.h"
#include "kernel.h"
#include "libk/kheap.h"

#include <stdint.h>

#define EXT2_MAGIC_NUMBER 0xEF53
#define EXT2_SUPERBLOCK_LBA_START (2048 + 2) // 100MB offset + 2 sectors
#define EXT2_SUPERBLOCK_SECTOR_SIZE 2

#define bytes_to_sectors(bytes) (bytes / LATTE_SECTOR_SIZE)


struct superblock {
    uint32_t    s_inodes_count;             /* Inodes count */
    uint32_t    s_blocks_count;             /* Blocks count */
    uint32_t    s_r_blocks_count;           /* Reserved blocks count */
    uint32_t    s_free_blocks_count;        /* Free blocks count */
    uint32_t    s_free_inodes_count;        /* Free inodes count */
    uint32_t    s_first_data_block;         /* First data block */
    uint32_t    s_log_block_size;           /* Base 2 log of block size */
    uint32_t    s_log_frag_size;            /* Base 2 log of fragment size*/
    uint32_t    s_blocks_per_group;         /* Number of blocks per group */
    uint32_t    s_frags_per_group;          /* Number of fragments per group*/
    uint32_t    s_inodes_per_group;         /* Number of inodes per group */
    uint32_t    s_mtime;                    /* Mount time */
    uint32_t    s_wtime;                    /* Write time */
    uint16_t    s_mnt_count;                /* Mount count */
    uint16_t    s_max_mnt_count;            /* Maximum mount count */
    uint16_t    s_magic;                    /* Magic signature */
    uint16_t    s_state;                    /* File system state */
    uint16_t    s_errors;                   /* Behavior when detecting errors */
    uint16_t    s_minor_rev_level;          /* Minor revision level */
    uint32_t    s_lastcheck;                /* Time of last check */
    uint32_t    s_checkinterval;            /* Maximum time between checks */
    uint32_t    s_creator_os;               /* OS */
    uint32_t    s_rev_level;                /* Revision level */
    uint16_t    s_def_resuid;               /* Default uid for reserved blocks */
    uint16_t    s_def_resgid;               /* Default gid for reserved blocks */
    /* EXT2_DYNAMIC_REV Specific*/
    uint32_t    s_first_ino;                /* First non-reserved inode */
    uint16_t    s_inode_size;               /* Size of inode structure */
    uint16_t    s_block_group_nr;           /* Block group number of this superblock */
    uint32_t    s_feature_compat;           /* Compatible feature set */
    uint32_t    s_feature_incompat;         /* Incompatible feature set */
    uint32_t    s_feature_ro_compat;        /* Readonly-compatible feature set */
    uint8_t     s_uuid[16];                 /* 128-bit uuid for volume */
    char        s_volume_name[16];          /* Volume name */
    char        s_last_mounted[64];         /* Directory where last mounted */
    uint32_t    s_algo_bitmap;              /* For compression */
    /* Performance Hints */
    uint8_t     s_prealloc_blocks;          /* Number of blocks to preallocate for regular files */
    uint8_t     s_prealloc_dir_blocks;      /* Number of blocks to preallocate for directories */
    uint8_t     s_padding1[2];
    /* Journaling Support */
    uint8_t     s_journal_uuid;             /* UUID of journal superblock */
    uint32_t    s_jounal_inum;              /* Inode number of journal file */
    uint32_t    s_jounal_dev;               /* Device number of journal file */
    uint32_t    s_last_orphan;              /* Start of list of inodes to delete */
    /* Directory Indexing Support */
    uint32_t    s_hash_seed[4];             /* HTREE hash seed */
    uint8_t     s_def_hash_version;         /* Default hash version to use */
    uint8_t     s_padding2[3];
    /* Other Options */
    uint32_t    s_default_mount_options;
    uint32_t    s_first_mega_bg;            /* First metablock block group */
    uint8_t     s_reserved[760];
} __attribute((packed));

struct block_group_descriptor {
    uint32_t    bg_block_bitmap;            /* Blocks bitmap block */
    uint32_t    bg_inode_bitmap;            /* Inodes bitmap block */
    uint32_t    bg_inode_table;             /* Inodes table block */
    uint16_t    bg_free_blocks_count;       /* Free blocks count */
    uint16_t    bg_free_inodes_count;       /* Free inodes count */
    uint16_t    bg_used_dirs_count;         /* Directories count */
    uint16_t    bg_pad;
    uint8_t     bg_reserved[12];
} __attribute((packed));

struct inode_table {
    uint16_t    i_mode;                     /* File mode */
    uint16_t    i_uid;                      /* Low 16 bits of owner uid */
    uint32_t    i_size;                     /* Size in bytes */
    uint32_t    i_atime;                    /* Access time */
    uint32_t    i_ctime;                    /* Creation time */
    uint32_t    i_mtime;                    /* Modification time */
    uint32_t    i_dtime;                    /* Delete time */
    uint16_t    i_gid;                      /* Low 16 bits of gid */
    uint16_t    i_links_count;              /* Links count */
    uint32_t    i_blocks;                   /* Blocks count */
    uint32_t    i_flags;                    /* File flags */
    uint32_t    i_osd1;                     /* OS dependent 1 */
    uint32_t    i_block[15];                /* Pointers to blocks */
    uint32_t    i_generation;               /* File version */
    uint32_t    i_file_acl;                 /* File ACL */
    uint32_t    i_dir_acl;                  /* Directory ACL */
    uint32_t    i_faddr;                    /* Fragment address */
    uint32_t    i_osd2[3];                  /* OS dependent 2 */
} __attribute((packed));

struct ext2_private {
    struct superblock superblock;
};

int
ext2_resolve(struct disk *disk)
{
    struct ext2_private *private = kzalloc(sizeof(struct ext2_private));
    if (!private) {
        return -ENOMEM;
    }

    int res = disk->read_sectors(disk, EXT2_SUPERBLOCK_LBA_START, EXT2_SUPERBLOCK_SECTOR_SIZE, &private->superblock);
    if (res < 0) {
        kfree(private);
        return -EIO;
    }

    if (private->superblock.s_magic != EXT2_MAGIC_NUMBER) {
        kfree(private);
        return -EIO;
    }

    disk->fs_private = private;

    return 0;
}

void*
ext2_open(struct disk *disk, struct path *path, FILE_MODE mode)
{
    return 0;
}

int
ext2_close(void *private)
{
    return 0;
}

int
ext2_read(struct disk *disk, void *private, uint32_t size, uint32_t count, char *buf)
{
    return 0;
}

int
ext2_write(struct disk *disk, void *private, uint32_t size, uint32_t count, const char *buf)
{
    return 0;
}

int
ext2_seek(void *private, uint32_t offset, FILE_SEEK_MODE seek_mode)
{
    return 0;
}

int
ext2_stat(struct disk *disk, void *private, struct file_stat *stat)
{
    return 0;
}

struct filesystem*
ext2_init()
{
    struct filesystem *fs = kzalloc(sizeof(struct filesystem));
    if (!fs) {
        panic("Unable to initialize ext2 filesystem");
    }

    fs->open = ext2_open;
    fs->close = ext2_close;
    fs->read = ext2_read;
    fs->write = ext2_write;
    fs->stat = ext2_stat;
    fs->seek = ext2_seek;
    fs->resolve = ext2_resolve;

    return fs;
}

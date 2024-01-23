#ifndef EXT2_COMMON_H
#define EXT2_COMMON_H

#include <stdint.h>

#define EXT2_MAGIC_NUMBER      0xEF53
#define EXT2_FS_START          (1024 * 1024) // 1MB
#define EXT2_SUPERBLOCK_OFFSET 1024
#define EXT2_SUPERBLOCK_SIZE   1024
#define EXT2_ROOT_DIR_INODE    2

#define EXT2_FT_UNKNOWN  0
#define EXT2_FT_REG_FILE 1
#define EXT2_FT_DIR      2
#define EXT2_FT_CHRDEV   3
#define EXT2_FT_BLKDEV   4
#define EXT2_FT_FIFO     5
#define EXT2_FT_SOCK     6
#define EXT2_FT_SYMLINK  7

#define EXT2_S_IFSOCK 0xC000
#define EXT2_S_IFLNK  0xA000
#define EXT2_S_IFREG  0x8000
#define EXT2_S_IFBLK  0x6000
#define EXT2_S_IFDIR  0x4000
#define EXT2_S_IFCHR  0x2000
#define EXT2_S_IFIFO  0x1000

#define EXT2_S_ISUID 0x0800
#define EXT2_S_ISGID 0x0400
#define EXT2_S_ISVXT 0x0200

#define EXT2_S_IRUSR 0x0100
#define EXT2_S_IWUSR 0x0080
#define EXT2_S_IXUSR 0x0040
#define EXT2_S_IRGRP 0x0020
#define EXT2_S_IWGRP 0x0010
#define EXT2_S_IXGRP 0x0008
#define EXT2_S_IROTH 0x0004
#define EXT2_S_IWOTH 0x0002
#define EXT2_S_IXOTH 0x0001

struct ext2_superblock {
    uint32_t s_inodes_count;      /* Inodes count */
    uint32_t s_blocks_count;      /* Blocks count */
    uint32_t s_r_blocks_count;    /* Reserved blocks count */
    uint32_t s_free_blocks_count; /* Free blocks count */
    uint32_t s_free_inodes_count; /* Free inodes count */
    uint32_t s_first_data_block;  /* First data block */
    uint32_t s_log_block_size;    /* Base 2 log of block size */
    uint32_t s_log_frag_size;     /* Base 2 log of fragment size*/
    uint32_t s_blocks_per_group;  /* Number of blocks per group */
    uint32_t s_frags_per_group;   /* Number of fragments per group*/
    uint32_t s_inodes_per_group;  /* Number of inodes per group */
    uint32_t s_mtime;             /* Mount time */
    uint32_t s_wtime;             /* Write time */
    uint16_t s_mnt_count;         /* Mount count */
    uint16_t s_max_mnt_count;     /* Maximum mount count */
    uint16_t s_magic;             /* Magic signature */
    uint16_t s_state;             /* File system state */
    uint16_t s_errors;            /* Behavior when detecting errors */
    uint16_t s_minor_rev_level;   /* Minor revision level */
    uint32_t s_lastcheck;         /* Time of last check */
    uint32_t s_checkinterval;     /* Maximum time between checks */
    uint32_t s_creator_os;        /* OS */
    uint32_t s_rev_level;         /* Revision level */
    uint16_t s_def_resuid;        /* Default uid for reserved blocks */
    uint16_t s_def_resgid;        /* Default gid for reserved blocks */
    /* EXT2_DYNAMIC_REV Specific*/
    uint32_t s_first_ino;         /* First non-reserved inode */
    uint16_t s_inode_size;        /* Size of inode structure */
    uint16_t s_block_group_nr;    /* Block group number of this superblock */
    uint32_t s_feature_compat;    /* Compatible feature set */
    uint32_t s_feature_incompat;  /* Incompatible feature set */
    uint32_t s_feature_ro_compat; /* Readonly-compatible feature set */
    uint8_t s_uuid[16];           /* 128-bit uuid for volume */
    char s_volume_name[16];       /* Volume name */
    char s_last_mounted[64];      /* Directory where last mounted */
    uint32_t s_algo_bitmap;       /* For compression */
    /* Performance Hints */
    uint8_t s_prealloc_blocks;     /* Number of blocks to preallocate for regular files */
    uint8_t s_prealloc_dir_blocks; /* Number of blocks to preallocate for directories */
    uint8_t s_padding1[2];
    /* Journaling Support */
    uint8_t s_journal_uuid[16]; /* UUID of journal superblock */
    uint32_t s_jounal_inum;     /* Inode number of journal file */
    uint32_t s_jounal_dev;      /* Device number of journal file */
    uint32_t s_last_orphan;     /* Start of list of inodes to delete */
    /* Directory Indexing Support */
    uint32_t s_hash_seed[4];    /* HTREE hash seed */
    uint8_t s_def_hash_version; /* Default hash version to use */
    uint8_t s_padding2[3];
    /* Other Options */
    uint32_t s_default_mount_options;
    uint32_t s_first_mega_bg; /* First metablock block group */
    uint8_t s_reserved[760];
} __attribute((packed));

struct ext2_block_group_descriptor {
    uint32_t bg_block_bitmap;      /* Blocks bitmap block */
    uint32_t bg_inode_bitmap;      /* Inodes bitmap block */
    uint32_t bg_inode_table;       /* Inodes table block */
    uint16_t bg_free_blocks_count; /* Free blocks count */
    uint16_t bg_free_inodes_count; /* Free inodes count */
    uint16_t bg_used_dirs_count;   /* Directories count */
    uint16_t bg_pad;
    uint8_t bg_reserved[12];
} __attribute((packed));

struct ext2_directory_entry {
    uint32_t inode;    /* Inode number */
    uint16_t rec_len;  /* Directory entry length */
    uint8_t name_len;  /* Name length */
    uint8_t file_type; /* File type */
    char name[256];    /* File name, up to 255 characters, plus null terminator */
} __attribute((packed));

struct ext2_inode {
    uint16_t i_mode;        /* File mode */
    uint16_t i_uid;         /* Low 16 bits of owner uid */
    uint32_t i_size;        /* Size in bytes */
    uint32_t i_atime;       /* Access time */
    uint32_t i_ctime;       /* Creation time */
    uint32_t i_mtime;       /* Modification time */
    uint32_t i_dtime;       /* Delete time */
    uint16_t i_gid;         /* Low 16 bits of gid */
    uint16_t i_links_count; /* Links count */
    uint32_t i_blocks;      /* Blocks count - 512 bytes sectors not fs blocks */
    uint32_t i_flags;       /* File flags */
    uint32_t i_osd1;        /* OS dependent 1 */
    uint32_t i_block[15];   /* Pointers to blocks */
    uint32_t i_generation;  /* File version */
    uint32_t i_file_acl;    /* File ACL */
    uint32_t i_dir_acl;     /* Directory ACL */
    uint32_t i_faddr;       /* Fragment address */
    uint32_t i_osd2[3];     /* OS dependent 2 */
} __attribute((packed));

/**
 * @brief Ext2 private filesystem data structure
 *
 */
struct ext2_private {
    // Copy of the superblock
    struct ext2_superblock superblock;

    // Block size
    uint32_t block_size;

    // Pointer to a buffered reader for the filesystem
    struct bufferedreader *reader;
};

/**
 * @brief Ext2 private file descriptor structure
 *
 */
struct ext2_descriptor_private {
    // Pointer to the inode
    struct ext2_inode *inode;

    // Block offset into the file
    int blk_offset;

    // Byte offset into the file
    int byte_offset;
};

#endif
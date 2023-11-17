#include "fs/ext2/inode.h"

#include "config.h"
#include "dev/disk/buffer/bufferedreader.h"
#include "fs/ext2/common.h"
#include "errno.h"
#include "libk/kheap.h"
#include "libk/memory.h"

struct block_iterator {
    const struct inode *inode;
    uint32_t block_size;
    int block_idx;
    int dbl_block_idx;
    int tpl_block_idx;
    uint32_t *indirect_blocks;
    uint32_t *dbl_indirect_blocks;
    uint32_t *tpl_indirect_blocks;
};

static int
block_iterator_init(struct block_iterator *iter, struct ext2_private *fs_private, const struct inode *inode)
{
    iter->inode = inode;
    iter->block_size = fs_private->block_size;
    iter->block_idx = 0;
    iter->dbl_block_idx = 0;
    iter->tpl_block_idx = 0;
    iter->indirect_blocks = 0;
    iter->dbl_indirect_blocks = 0;
    iter->tpl_indirect_blocks = 0;

    return 0;
}

static int
block_iterator_skip_blocks(struct block_iterator *iter, uint32_t offset)
{
    if (offset > 12) {
        return -EIO; // Only support reading direct block pointers for now
    }

    iter->block_idx = offset;
    return 0;
}

static int
block_iterator_iterate_tpl(struct block_iterator *iter, struct ext2_private *fs_private)
{
    // TODO
    return 0;
}

static int
block_iterator_iterate_dbl(struct block_iterator *iter, struct ext2_private *fs_private)
{
    // TODO
    return 0;
}


static int
block_iterator_iterate_indirect(struct block_iterator *iter, struct ext2_private *fs_private)
{
    // TODO
    return 0;
}

static int
block_iterator_convert_to_indirect(struct block_iterator *iter, struct ext2_private *fs_private)
{
    // TODO
    return 0;
}

static int
block_iterator_iterate(struct block_iterator *iter, struct ext2_private *fs_private)
{
    if (iter->tpl_indirect_blocks) {
        return block_iterator_iterate_tpl(iter, fs_private);
    }

    if (iter->dbl_indirect_blocks) {
        return block_iterator_iterate_dbl(iter, fs_private);
    }

    if (iter->indirect_blocks) {
        return block_iterator_iterate_indirect(iter, fs_private);
    }

    iter->block_idx++;
    if (iter->block_idx == 12) {
        block_iterator_convert_to_indirect(iter, fs_private);
    }

    return 0;
}

static int
block_iterator_block_no(struct block_iterator *iter){
    return iter->inode->i_block[iter->block_idx];
}

static int
ext2_read_block(struct block_iterator *iter, struct ext2_private *fs_private, char *out)
{
    int block_no = block_iterator_block_no(iter);
    int data_offset = EXT2_FS_START + (block_no * fs_private->block_size);

    bufferedreader_seek(fs_private->reader, data_offset);
    return bufferedreader_read(fs_private->reader, out, fs_private->block_size);
}

static int
inode_to_block_group(struct ext2_private *fs_private, uint32_t inode)
{
    return (inode - 1) / fs_private->superblock.s_inodes_per_group;
}

static int
ext2_read_block_group_desc(struct block_group_descriptor **desc_out, struct ext2_private *fs_private, int block_group)
{
    int bg_desc_tbl_start_blk = 1;
    if (fs_private->block_size <= 1024) {
        bg_desc_tbl_start_blk = 2;
    }

    int bg_desc_tbl_start = EXT2_FS_START + (bg_desc_tbl_start_blk * fs_private->block_size);
    int bg_desc_start = bg_desc_tbl_start + (block_group * sizeof(struct block_group_descriptor));

    struct block_group_descriptor *desc = kzalloc(sizeof(struct block_group_descriptor));
    if (!desc) {
        return -ENOMEM;
    }

    bufferedreader_seek(fs_private->reader, bg_desc_start);
    int res = bufferedreader_read(fs_private->reader, desc, sizeof(struct block_group_descriptor));
    if (res < 0) {
        kfree(desc);
        return res;
    }

    *desc_out = desc;
    return 0;
}

static int
ext2_read_inode_from_tbl(struct inode **inode_out, struct ext2_private* fs_private, uint32_t inode_tbl, uint32_t inode_no)
{
    int inode_tbl_start = EXT2_FS_START + (inode_tbl * fs_private->block_size);
    int inode_start = inode_tbl_start + ((inode_no - 1) * fs_private->superblock.s_inode_size);

    struct inode *inode = kzalloc(sizeof(struct inode));
    if (!inode) {
        return -ENOMEM;
    }

    bufferedreader_seek(fs_private->reader, inode_start);
    int res = bufferedreader_read(fs_private->reader, inode, sizeof(struct inode));
    if (res < 0) {
        kfree(inode);
        return res;
    }

    *inode_out = inode;
    return 0;
}



int
ext2_read_inode(struct inode **inode_out, struct disk *disk, struct ext2_private *fs_private, uint32_t inode_no)
{
    int block_group = inode_to_block_group(fs_private, inode_no);

    struct block_group_descriptor *desc;
    int res = ext2_read_block_group_desc(&desc, fs_private, block_group);
    if (res < 0) {
        return res;
    }

    struct inode *inode;
    res = ext2_read_inode_from_tbl(&inode, fs_private, desc->bg_inode_table, inode_no);
    if (res < 0) {
        return res;
    }

    *inode_out = inode;
    return 0;
}

int
ext2_read_inode_data(struct ext2_private *fs_private, const struct inode *inode, char *out, size_t count, unsigned int blk_offset)
{
    struct block_iterator iter;
    block_iterator_init(&iter, fs_private, inode);
    block_iterator_skip_blocks(&iter, blk_offset);

    char buf[fs_private->block_size];
    int bytes_read = 0;
    int bytes_remaining = count;

    while (bytes_read < bytes_remaining) {
        ext2_read_block(&iter, fs_private, buf);
        
        int bytes_to_copy = fs_private->block_size;
        if (bytes_remaining < bytes_to_copy) {
            bytes_to_copy = bytes_remaining;
        }

        memcpy(out + bytes_read, buf, bytes_to_copy);
        bytes_read += bytes_to_copy;
        bytes_remaining -= bytes_read;

        block_iterator_iterate(&iter, fs_private);
    }

    return bytes_read;
}
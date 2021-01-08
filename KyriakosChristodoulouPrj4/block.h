#ifndef BLOCK_H
#define BLOCK_H

#define BLOCK_SIZE 512
#define MAXCHILDREN ((BLOCK_SIZE-6*sizeof(int)-sizeof(struct stat)-128*sizeof(char))/sizeof(int))

typedef struct HeaderBlock {
    int firstblocknum;
    int gzip_enabled;
    int blocksCntr;
    char padding[BLOCK_SIZE - sizeof(firstblocknum) - sizeof(gzip_enabled) -sizeof(blocksCntr)];
} HeaderBlock;

typedef struct MetadataBlock {
    char d_name[128];
    struct stat mybuf;
    int isdirectory;
    int parentblocknum;
    int nextblocknum;
    // directory:
    int children;
    int childrenblocknum[MAXCHILDREN];
    // file:
    int datablocknum;
    int bytesOfLast;
} MetadataBlock;

typedef struct DataBlock {
    char bytes[BLOCK_SIZE];
} DataBlock;


#endif /* BLOCK_H */

#ifndef BLOCKHANDLER_H
#define BLOCKHANDLER_H

#include "block.h"

class BlockHandler {
public:
    BlockHandler(char * filename, bool gzip_enabled);
    ~BlockHandler();
    FILE* retFD(){return fd;};
private:
    char * filename;
    bool gzip_enabled;
    FILE* fd;
};

#endif /* BLOCKHANDLER_H */

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <error.h>
#include <errno.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "BlockHandler.h"

using namespace std;

BlockHandler::BlockHandler(char * filename, bool gzip_enabled) : filename(filename), gzip_enabled(gzip_enabled) {
    fd = fopen(filename,"w+");
    if (fd == NULL) {
        perror("creating");
        exit(1);
    }
}

BlockHandler::~BlockHandler() {
    if (fd != NULL) {
        fclose(fd);
    }
}

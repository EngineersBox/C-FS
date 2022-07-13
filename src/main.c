#define ENABLE_LOGGING
#define LOG_DATETIME_PREFIX
#include "logging/logging.h"
LOGS_DIR("/mnt/e/C-FS/logs");

#include <stdlib.h>
#include "fs/cfs.h"

int main(int argc, char* argv[]) {
    INFO("Example log entry");
    return 0;
}
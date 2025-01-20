#include "shared_memory.hpp"
#include <cstdio>
#include <cstdlib>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#define GET_PID() GetCurrentProcessId()
#else
#include <unistd.h>
#define GET_PID() getpid()
#endif

namespace {
    const char *SHARED_MEMORY_NAME = "/shared_counter";
    const size_t SHARED_MEMORY_SIZE = sizeof(int);
}

static int *sharedCounter = nullptr;
static int shm_fd = -1;

void init_shared_memory() {
    shm_fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Failed to open shared memory");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd, SHARED_MEMORY_SIZE) == -1) {
        perror("Failed to set size of shared memory");
        exit(EXIT_FAILURE);
    }

    void *memory = mmap(nullptr, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (memory == MAP_FAILED) {
        perror("Failed to map shared memory");
        exit(EXIT_FAILURE);
    }

    sharedCounter = static_cast<int *>(memory);
    *sharedCounter = 0;
}

void cleanupSharedMemory() {
    if (munmap(sharedCounter, SHARED_MEMORY_SIZE) == -1) {
        perror("Failed to unmap shared memory");
    }
    if (shm_unlink(SHARED_MEMORY_NAME) == -1) {
        perror("Failed to unlink shared memory");
    }
}

int *getSharedCounter() {
    return sharedCounter;
}

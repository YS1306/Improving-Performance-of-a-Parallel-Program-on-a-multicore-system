#include <pthread.h>
#include <sys/mman.h>
#define KiB (1024lu)
#define MiB (1024 * KiB)
#define GiB (1024 * MiB)
#define PAGE (4 * KiB)
#define LARGE_PAGE (2 * MiB)
#define TOTAL_SIZE (1 * MiB)

// implemented by the library
extern "C" void performant_thread_run(uint32_t threadIdx, uint8_t *data_addr, int8_t *data);

// implemented in main.cpp
extern "C" void work_run();
extern "C" int work_init();
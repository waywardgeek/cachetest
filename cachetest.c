#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void time_ms(struct timespec *start, struct timespec *end, double *ms)
{
    double start_ms, end_ms;

    start_ms = start->tv_sec * 1000.0 + start->tv_nsec / (1000.0 * 1000);
    end_ms   = end->tv_sec * 1000.0 + end->tv_nsec / (1000.0 * 1000);

    *ms = end_ms - start_ms;
}

// Perform random 8 byte reads and writes throughout 2^memSize memory, for 2^totalData
static void randomReadWrite(uint64_t *mem, uint32_t memSize, uint32_t totalData) {
    struct timespec start, end;
    double ms;
    uint32_t data = 12345;
    uint32_t address = 1234;
    uint32_t memWords = 1U << (memSize - 2);
    uint32_t dataWords = 1U << (totalData - 2);
    clock_gettime(CLOCK_MONOTONIC, &start);
    for(uint32_t i = 0; i < dataWords; i++) {
        address += 104729;
        uint32_t index = address & (memWords-1);
        data += mem[index] + address;
        mem[index] = data;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_ms(&start, &end, &ms);
    printf("Access time for random read then write to %u memory is %gns\n", 1 << memSize, 1000000*ms/dataWords);
}

int main() {
    uint64_t *mem = malloc(1ULL << 30);

    // 16KiB random read/write test
    randomReadWrite(mem, 14, 30);
    // 128KiB random read/write test
    randomReadWrite(mem, 17, 30);
    // 4MiB random read/write test
    randomReadWrite(mem, 24, 30);
    // 1GiB random read/write test
    randomReadWrite(mem, 30, 30);
    return 0;
}

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static inline uint32_t fastrand() { 
	static uint32_t seed = 12345;
    //seed = (214013U*seed+2531011U); 
    //return (seed>>16)&0x7FFF; 
	seed *= 3;
	return seed;
}

static void time_ms(struct timespec *start, struct timespec *end, double *ms)
{
    double start_ms, end_ms;

    start_ms = start->tv_sec * 1000.0 + start->tv_nsec / (1000.0 * 1000);
    end_ms   = end->tv_sec * 1000.0 + end->tv_nsec / (1000.0 * 1000);

    *ms = end_ms - start_ms;
}

// Perform random writes throughout memory.
static void randomWrite(__uint128_t *mem, uint32_t memExp, uint32_t dataExp, uint32_t blockExp) {
    struct timespec start, end;
    double ms;
    __uint128_t data = 12345;
    uint32_t address = 1;
	uint32_t blockWords = 1 << blockExp;
    uint32_t memBlocks = 1U << (memExp - 4 - blockExp);
    uint32_t dataBlocks = 1U << (dataExp - 4 - blockExp);
    clock_gettime(CLOCK_MONOTONIC, &start);
    for(uint32_t i = 0; i < dataBlocks; i++) {
		address += fastrand();
        uint32_t index = (address & (memBlocks-1)) << blockExp;
        for(uint32_t j = 0; j < blockWords; j++) {
			data += address;
			mem[index + j] = data;
		}
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_ms(&start, &end, &ms);
	double nsPerBlock = 1000000*ms/dataBlocks;
	double nsPerByte = nsPerBlock/(16*blockWords);
    printf("For random writes to %u memory: %gns per block, or %gns per byte\n", 1 << memExp, nsPerBlock, nsPerByte);
}

// Perform random reads throughout memory.
static void randomRead(__uint128_t *mem, uint32_t memExp, uint32_t dataExp, uint32_t blockExp) {
    struct timespec start, end;
    double ms;
    __uint128_t data = 12345;
    uint32_t address = 1234;
	uint32_t blockWords = 1 << blockExp;
    uint32_t memBlocks = 1U << (memExp - 4 - blockExp);
    uint32_t dataBlocks = 1U << (dataExp - 4 - blockExp);
    clock_gettime(CLOCK_MONOTONIC, &start);
    uint32_t index = 0;
    for(uint32_t i = 0; i < dataBlocks; i++) {
        address += fastrand();
        index = (address & (memBlocks-1)) << blockExp;
        for(uint32_t j = 0; j < blockWords; j++) {
			data += mem[index + j];
		}
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_ms(&start, &end, &ms);
	mem[index] = data; // Needed so the optimizer doesn't just drop the loop
	double nsPerBlock = 1000000*ms/dataBlocks;
	double nsPerByte = nsPerBlock/(16*blockWords);
    printf("For random reads from %u memory: %gns per block, or %gns per byte\n", 1 << memExp, nsPerBlock, nsPerByte);
}

// Perform random read then writes throughout memory.
static void randomReadWrite(__uint128_t *mem, uint32_t memExp, uint32_t dataExp, uint32_t blockExp) {
    struct timespec start, end;
    double ms;
    __uint128_t data = 12345;
    uint32_t address = 1234;
	uint32_t blockWords = 1 << blockExp;
    uint32_t memBlocks = 1U << (memExp - 4 - blockExp);
    uint32_t dataBlocks = 1U << (dataExp - 4 - blockExp);
    clock_gettime(CLOCK_MONOTONIC, &start);
    for(uint32_t i = 0; i < dataBlocks; i++) {
        address += fastrand();
        uint32_t index = (address & (memBlocks-1)) << blockExp;
        for(uint32_t j = 0; j < blockWords; j++) {
			data += mem[index + j] + address;
			mem[index + j] = data;
		}
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_ms(&start, &end, &ms);
	double nsPerBlock = 1000000*ms/dataBlocks;
	double nsPerByte = nsPerBlock/(16*blockWords);
    printf("For readom read-then-write from %u memory: %gns per block, or %gns per byte\n", 1 << memExp, nsPerBlock, nsPerByte);
}

int main() {
    __uint128_t *mem = calloc(1ULL << 30, 1);
	uint32_t blockExp = BLOCK_EXP; // Define this in Makefile to set memory size.
    printf("Access times for random read then write of %u byte blocks\n", 1 << (blockExp + 4));
    // 16KiB random read/write test - 1/2 my L1 cache
    randomWrite(mem, 14, 30, blockExp);
    randomRead(mem, 14, 30, blockExp);
    randomReadWrite(mem, 14, 30, blockExp);
	printf("\n");
    // 128KiB random read/write test - 1/2 my L2 cache
    randomWrite(mem, 17, 30, blockExp);
    randomRead(mem, 17, 30, blockExp);
    randomReadWrite(mem, 17, 30, blockExp);
	printf("\n");
    // 4MiB random read/write test - 1/2 my L3 cache
    randomWrite(mem, 22, 30, blockExp);
    randomRead(mem, 22, 30, blockExp);
    randomReadWrite(mem, 22, 30, blockExp);
	printf("\n");
    // 1GiB random read/write test - blow out of cache
    randomWrite(mem, 30, 30, blockExp);
    randomRead(mem, 30, 30, blockExp);
    randomReadWrite(mem, 30, 30, blockExp);
    return 0;
}

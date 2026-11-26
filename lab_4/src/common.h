#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define SHM_FILE "/tmp/lab4_shm_8"

#define MAX_NUMS 1024

typedef struct {
    volatile int ready; // родитель выставил данные
    volatile int done; // ребёнок закончил
    volatile int finish;
    volatile int zero_error;
    int count;
    int nums[MAX_NUMS];
    int result;
} SharedData;

#endif
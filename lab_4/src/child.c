#include "common.h"

int main(void)
{
    int fd = open(SHM_FILE, O_RDWR);
    if (fd == -1) {
        perror("open shm");
        return 1;
    }

    SharedData *shm = mmap(NULL, sizeof(SharedData),
    PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    while (!shm->finish) {
        if (!shm->ready) {
            usleep(1000);
            continue;
        }

        int zero_error = 0;
        for (int i = 1; i < shm->count; i++) {
            if (shm->nums[i] == 0) {
                zero_error = 1;
                break;
            }
        }

        if (zero_error) {
            shm->zero_error = 1;
            shm->done = 1;
            shm->ready = 0;
            break;
        }

        int result = shm->nums[0];
        for (int i = 1; i < shm->count; i++) {
            result /= shm->nums[i];
        }

        shm->result = result;
        shm->zero_error = 0;
        shm->done = 1;
        shm->ready = 0;
    }

    munmap(shm, sizeof(SharedData));
    close(shm_fd);
    return 0;
}
#include "common.h"

int parse_line(char *line, int *nums, int *count)
{
    *count = 0;
    char *token = strtok(line, " \t\r\n");
    while (token != NULL) {
        char *endptr;
        long val = strtol(token, &endptr, 10);
        if (*endptr != '\0' || endptr == token) {
            fprintf(stderr, "Invalid number: %s\n", token);
            return -1;
        }
        nums[(*count)++] = (int)val;
        token = strtok(NULL, " \t\r\n");
    }
    if (*count >= 2) return 1;
    return 0;
}

int main(void) {
    char path[256];
    printf("Input path: ");
    fflush(stdout);
    if (scanf("%255s", path) != 1) {
        fprintf(stderr, "Invalid path\n");
        return 1;
    }

    FILE *input = fopen(path, "r"); // файл
    if (!input) {
        perror("fopen input");
        return 1;
    }

    int fd = open(SHM_FILE, O_CREAT | O_RDWR | O_TRUNC, 0666); // дескриптор
    if (fd == -1) {
        perror("open shm");
        fclose(input);
        return 1;
    }

    if (ftruncate(fd, sizeof(SharedData)) == -1) { // выделяет память
        perror("ftruncate");
        close(fd);
        fclose(input);
        return 1;
    }

    SharedData *shm = mmap(NULL, sizeof(SharedData), 
    PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED) {
        perror("mmap");
        close(fd);
        fclose(input);
        return 1;
    }
    memset((void *)shm, 0, sizeof(SharedData));

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        munmap(shm, sizeof(SharedData));
        close(fd);
        fclose(input);
        return 1;
    }

    if (pid == 0) {
        // Ребёнок
        close(fd);
        fclose(input);
        execl("./child", "child", NULL);
        perror("execl");
        exit(1);
    }

    // Родитель
    char line[4096];
    int exit_code = 0;

    while (fgets(line, sizeof(line), input)) {
        int nums[MAX_NUMS];
        int count = 0;

        if (parse_line(line, nums, &count) == 0) {
            fprintf(stderr, "Skip invalid line\n");
            continue;
        }

        // Ждём, пока ребёнок закончит
        while (shm->ready || shm->done) {
            usleep(1000);
        }

        if (shm->zero_error) {
            break;
        }

        shm->count = count;
        memcpy(shm->nums, nums, sizeof(int) * (size_t)count);
        shm->result = 0;
        shm->zero_error = 0;
        shm->done = 0;
        shm->ready = 1;

        while (!shm->done) {
            usleep(1000);
        }

        if (shm->zero_error) {
            fprintf(stderr, "Error: Division by zero\n");
            exit_code = 2;
            break;
        }

        printf("Result: %d\n", shm->result);
        shm->ready = 0;
        shm->done = 0;
    }

    shm->finish = 1;
    usleep(10000);
    waitpid(pid, NULL, 0);

    munmap(shm, sizeof(SharedData));
    close(fd);
    fclose(input);
    unlink(SHM_FILE);

    return exit_code;
}
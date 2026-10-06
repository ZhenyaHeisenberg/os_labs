#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>  

int main() {
    char path[256];
    printf("Input path: ");
    if (scanf("%s", path) != 1) {
        printf("Invalid path (%s)", path);
        return 1;
    }

    int file = open(path, O_RDONLY);
    if (file == -1) {
        perror("Open error");
        return 1;
    }

    int fd[2];
    if (pipe(fd) == -1) {
        perror("Ошибка pipe");
        return 1;
    }
    // fd[0] - чтение
    // fd[1] - запись

    pid_t pid = fork(); // Дочерний процесс
    if (pid == -1) {
        perror("Ошибка fork");
        return 1;
    }
    if (pid == 0) { // Ребенок
        // Чтение не нужно
        close(fd[0]);

        // child читает из file
        if (dup2(file, 0) == -1) { // 0 - stdin; теперь то что в файле - просто ввод для ребенка
            printf("dup2 stdin"); exit(1);
        }

        // child пишет в пайп
        if (dup2(fd[1], 1) == -1) { // 1 - stdout; теперь запись в канале это вывод
            printf("dup2 stdout"); exit(1);
        }

        // Ненужные дескрипторы
        close(file);
        close(fd[1]);

        // запуск ребенка
        execl("./child", "child", NULL);

        printf("Ошибка exec");
        exit(1);

    } else { // Родитель

        close(fd[1]);   // fd[1] - запись
        close(file);

        // Читаем из канала то, что пишет ребенок
        char buffer[256];
        int n;
        while ((n = read(fd[0], buffer, sizeof(buffer))) > 0) {
            buffer[n] = '\0';
            printf("%s", buffer);
        }


        close(fd[0]);
        wait(NULL); // Ждем завершения ребенка
    }

    return 0;
}

// wsl -d Ubuntu

// /mnt/c/Users/admin/Desktop/os_labs/lab_2
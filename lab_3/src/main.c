#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <gmp.h>

typedef struct {
    const char* filename;
    int         max_threads;
    long        memory_limit_mb;
} ProcessArgs;

CRITICAL_SECTION   g_cs;
mpz_t              g_sum;
unsigned long long g_count;

typedef struct { const char* p; size_t len; } Chunk;

DWORD WINAPI worker(LPVOID arg) {
    Chunk* c = (Chunk*)arg;
    mpz_t local, num;
    mpz_init(local);
    mpz_init(num);
    unsigned long long cnt = 0;

    const char* p = c->p;
    const char* end = c->p + c->len;
    while (p < end) {
        const char* nl = (const char*)memchr(p, '\n', (size_t)(end - p));
        size_t len = nl ? (size_t)(nl - p) : (size_t)(end - p);
        if (len > 0 && p[len - 1] == '\r') len--;

        if (len > 0 && len < 256) {
            char buf[256];
            memcpy(buf, p, len);
            buf[len] = '\0';
            if (mpz_set_str(num, buf, 16) == 0) { mpz_add(local, local, num); cnt++; }
        }
        if (!nl) break;
        p = nl + 1;
    }

    EnterCriticalSection(&g_cs);
    mpz_add(g_sum, g_sum, local);
    g_count += cnt;
    LeaveCriticalSection(&g_cs);

    mpz_clear(num);
    mpz_clear(local);
    return 0;
}

void set_memory_limit(long mb) {
    if (mb <= 0) return;
    HANDLE h = CreateJobObjectA(NULL, NULL);
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION j;
    memset(&j, 0, sizeof(j));
    j.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_PROCESS_MEMORY;
    j.ProcessMemoryLimit = (SIZE_T)mb * 1024 * 1024;
    SetInformationJobObject(h, JobObjectExtendedLimitInformation, &j, sizeof(j));
    if (!AssignProcessToJobObject(h, GetCurrentProcess()))
        fprintf(stderr, "Job limit not applied: %lu\n", GetLastError());
}

int process(const ProcessArgs* args) {
    set_memory_limit(args->memory_limit_mb);

    int T = args->max_threads < 1 ? 1 : args->max_threads;

    // FILE
    FILE* f = fopen(args->filename, "rb");
    if (!f) { fprintf(stderr, "open failed: %s\n", args->filename); return 1; }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* buf = (char*)malloc((size_t)size + 1);
    if (!buf) { fclose(f); return 1; }
    fread(buf, 1, (size_t)size, f);
    buf[size] = '\0';
    fclose(f);

    InitializeCriticalSection(&g_cs);
    mpz_init_set_ui(g_sum, 0);
    g_count = 0;

    //  нарезаем на куски. Всего кусков = T
    int nchunks = T;
    Chunk* chunks = (Chunk*)malloc(sizeof(Chunk) * nchunks);
    HANDLE* th    = (HANDLE*)malloc(sizeof(HANDLE) * nchunks);

    long target = size / nchunks; // байт на кусок
    const char* p = buf;
    for (int i = 0; i < nchunks; i++) {
        const char* end;
        if (i == nchunks - 1) {
            end = buf + size;
        } else {
            end = p + target;
            // сдвигаем конец до \n
            while (end < buf + size && *end != '\n') end++;
            if (end < buf + size) end++;
        }
        chunks[i].p = p;
        chunks[i].len = (size_t)(end - p);
        p = end;
    }


    for (int k = 0; k < T; k++) {
        th[k] = CreateThread(NULL, 0, worker, &chunks[k], 0, NULL);
    }
    WaitForMultipleObjects(T, th, TRUE, INFINITE);
    for (int k = 0; k < T; k++) CloseHandle(th[k]);

    if (g_count > 0) { // если что-то прочитали, записываем в global
        mpz_t avg;
        mpz_init(avg);
        mpz_tdiv_q_ui(avg, g_sum, g_count);
        gmp_printf("Average: %Zd\n", avg);
        mpz_clear(avg);
    } else {
        printf("Average: %d\n", 0);
    }

    mpz_clear(g_sum);
    DeleteCriticalSection(&g_cs);
    free(th); free(chunks); free(buf);
    return 0;
}


double now(void) {
    LARGE_INTEGER fr; int inited = 0;
    if (!inited) { QueryPerformanceFrequency(&fr); inited = 1; }
    LARGE_INTEGER t; QueryPerformanceCounter(&t);
    return (double)t.QuadPart / (double)fr.QuadPart;
}

void run_benchmark(const ProcessArgs* base, int t_min, int t_max) {
    printf("\n%-6s %-12s %-12s %-12s\n\n", "T", "time(s)", "speedup", "efficiency");
    double t1 = 0;
    for (int T = t_min; T <= t_max; T += 1) {
        ProcessArgs a = *base; a.max_threads = T; a.memory_limit_mb = 0;
        double t0 = now();
        int rc = process(&a);
        double dt = now() - t0;
        if (rc) { printf("T=%d failed\n", T); continue; }
        if (T == t_min) t1 = dt;
        double s = dt > 0 ? t1 / dt : 0;
        double e = s / T;
        printf("%-6d %-12.6f %-12.4f %-12.4f\n\n", T, dt, s, e);
    }
}

int main(void) {
    const char* file   = "data.txt";
    int         t_min;
    int         t_max;
    long        mem_mb;

    printf("min threads: ");
    scanf("%d", &t_min);

    printf("\nmax threads: ");
    scanf("%d", &t_max);

    printf("\nmemory (mb): ");
    scanf("%ld", &mem_mb);

    ProcessArgs base = { file, t_min, mem_mb };

    if (t_min == t_max) {
        double t0 = now();
        process(&base);
        double t1 = now();
        printf("\nTime(s): %f\n", t1-t0);
        return 0;
    }
    run_benchmark(&base, t_min, t_max);
    return 0;
}

// cd /c/Users/admin/Desktop/os_labs/os_labs/lab_3

// # удалить старый кэш (он от другого пути)
// rm -rf build

// # сконфигурировать заново
// cmake -S . -B build -G "MinGW Makefiles"

// # собрать
// cmake --build build
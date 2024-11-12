#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/time.h> // Для gettimeofday
#include <time.h>

// Callback для обработки данных, получаемых из запроса
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    // Игнорируем полученные данные
    return size * nmemb; 
}

// Функция сравнения для qsort
int compare(const void *a, const void *b) {
    return (*(long long *)a - *(long long *)b);
}

// Функция для вычисления медианы
long long median(long long *arr, size_t len) {
    qsort(arr, len, sizeof(long long), compare);
    if (len % 2 == 0) {
        return (arr[len / 2 - 1] + arr[len / 2]) / 2;
    }
    return arr[len / 2];
}

int main() {
    CURL *curl;
    CURLcode res;
    long long times[10];
    int repeats = 10;

    // Инициализация CURL
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "CURL initialization failed!\n");
        return 1;
    }

    // Установка URL
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.bybit.com/v2/public/time");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

    // Прогревочный запрос
    printf("Skipping first request (warm-up)...\n");
    curl_easy_perform(curl);

    // Основные запросы
    for (int i = 0; i < repeats; ++i) {
        struct timeval start, end; // Структуры для хранения времени

        gettimeofday(&start, NULL); // Начало отсчета времени

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "Request %d failed: %s\n", i + 1, curl_easy_strerror(res));
            continue;
        }

        gettimeofday(&end, NULL); // Конец отсчета времени

        // Вычисление времени в миллисекундах
        long long duration = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
        times[i] = duration;

        printf("Request %d time: %lld ms\n", i + 1, duration);
    }

    // Вычисление минимального, максимального и медианного времени
    long long minTime = times[0];
    long long maxTime = times[0];
    for (int i = 0; i < repeats; i++) {
        if (times[i] < minTime) {
            minTime = times[i];
        }
        if (times[i] > maxTime) {
            maxTime = times[i];
        }
    }
    long long medianTime = median(times, repeats);

    printf("\nResults over %d requests (excluding warm-up):\n", repeats);
    printf("Min time: %lld ms\n", minTime);
    printf("Max time: %lld ms\n", maxTime);
    printf("Median time: %lld ms\n", medianTime);

    // Очистка ресурсов
    curl_easy_cleanup(curl);
    return 0;
}

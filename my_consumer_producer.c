#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 5
#define MESSAGE_FILE "message.txt"

char buffer[BUFFER_SIZE];
int buffer_head = 0;
int buffer_tail = 0;
int buffer_count = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffer_not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer_not_empty = PTHREAD_COND_INITIALIZER;

void *producer(void *arg) {
    FILE *message_file = fopen(MESSAGE_FILE, "r");
    char c;
    while ((c = fgetc(message_file)) != EOF) {
        pthread_mutex_lock(&mutex);
        while (buffer_count == BUFFER_SIZE) {
            pthread_cond_wait(&buffer_not_full, &mutex);
        }
        buffer[buffer_tail] = c;
        buffer_tail = (buffer_tail + 1) % BUFFER_SIZE;
        buffer_count++;
        pthread_cond_signal(&buffer_not_empty);
        pthread_mutex_unlock(&mutex);
    }
    pthread_mutex_lock(&mutex);
    while (buffer_count > 0) {
        pthread_cond_wait(&buffer_not_full, &mutex);
    }
    pthread_cond_signal(&buffer_not_empty);
    pthread_mutex_unlock(&mutex);
    fclose(message_file);
    return NULL;
}

void *consumer(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        while (buffer_count == 0) {
            pthread_cond_wait(&buffer_not_empty, &mutex);
        }
        putchar(buffer[buffer_head]);
        buffer_head = (buffer_head + 1) % BUFFER_SIZE;
        buffer_count--;
        pthread_cond_signal(&buffer_not_full);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t producer_thread, consumer_thread;
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);
    pthread_join(producer_thread, NULL);
    pthread_cancel(consumer_thread);
    pthread_join(consumer_thread, NULL);
    return 0;
}

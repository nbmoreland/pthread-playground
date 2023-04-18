#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define MAX 1024
#define NUM_THREADS 4

int total = 0;
int n1, n2;
char *s1, *s2;
pthread_mutex_t lock;

int readf(FILE *fp) {
    if ((fp = fopen("string.txt", "r")) == NULL) {
        printf("ERROR: can't open string.txt! \n");
        return 0;
    }
    s1 = (char *) malloc(sizeof(char) * MAX);
    if (s1 == NULL) {
        printf("ERROR: Out of memory! \n");
        return -1;
    }
    s2 = (char *) malloc(sizeof(char) * MAX);
    if (s2 == NULL) {
        printf("ERROR: Out of memory! \n");
        return -1;
    }
    /* reads s1 and s2 from the file */
    s1 = fgets(s1, MAX, fp);
    s2 = fgets(s2, MAX, fp);
    n1 = strlen(s1); /* length of s1 */
    n2 = strlen(s2) - 1; /* length of s2 */
    if (s1 == NULL || s2 == NULL || n1 < n2) /* when error exit */
        return -1;
    return 1;
}

void *num_substring(void *arg) {
    int tid = *(int*)arg;
    int count = 0;
    for (int i = tid; i <= n1 - n2; i += NUM_THREADS) {
        int j, k;
        for (j = i, k = 0; k < n2; j++, k++) {
            if (*(s1+j) != *(s2+k)) {
                break;
            }
        }
        if (k == n2) {
            count++;
        }
    }
    pthread_mutex_lock(&lock);
    total += count;
    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    int rc;
    FILE *fp;
    if (readf(fp) < 0) {
        return -1;
    }
    pthread_mutex_init(&lock, NULL);
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        rc = pthread_create(&threads[i], NULL, num_substring, &thread_ids[i]);
        if (rc) {
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("The number of substrings is: %d\n", total);
    pthread_mutex_destroy(&lock);
    return 1;
}

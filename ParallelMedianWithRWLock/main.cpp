//
//  main.cpp
//  ParallelMedianWithRWLock
//
//  Created by Руслан on 13.12.15.
//  Copyright © 2015 MIPT. All rights reserved.
//

#include <iostream>
#include <pthread.h>
using std::cout;

#define NUMBER_OF_THREADS 4

int reader_count, writer_count;
pthread_mutex_t reader_count_mutex, writer_count_mutex, reader_enters_mutex, readTry, resource;
int global;

void* reader(void* args){
    pthread_mutex_lock(&reader_enters_mutex);
    pthread_mutex_lock(&readTry);
    pthread_mutex_lock(&reader_count_mutex);
    reader_count ++;
    if (reader_count == 1) {
        pthread_mutex_lock(&resource);
    }
    pthread_mutex_unlock(&reader_count_mutex);
    pthread_mutex_unlock(&readTry);
    pthread_mutex_unlock(&reader_enters_mutex);
    //CRITICAL SECTION
    cout << global << '\n';
    pthread_mutex_lock(&reader_count_mutex);
    reader_count --;
    if (reader_count == 0) {
        pthread_mutex_unlock(&resource);
    }
    pthread_mutex_unlock(&reader_count_mutex);
    return NULL;
}

void* writer (void* args){
    pthread_mutex_lock(&writer_count_mutex);
    writer_count++;
    if (writer_count == 1) {
        pthread_mutex_lock(&readTry);
    }
    pthread_mutex_unlock(&writer_count_mutex);
    pthread_mutex_lock(&resource);
    global ++;
    pthread_mutex_unlock(&resource);
    pthread_mutex_lock(&writer_count_mutex);
    writer_count--;
    if (writer_count == 0) {
        pthread_mutex_unlock(&readTry);
    }
    pthread_mutex_unlock(&writer_count_mutex);
    return NULL;
}

int main(int argc, const char * argv[]) {
    global = 0;
    pthread_t threads[2*NUMBER_OF_THREADS];
    pthread_mutex_init(&reader_count_mutex, NULL);
    pthread_mutex_init(&writer_count_mutex, NULL);
    pthread_mutex_init(&reader_enters_mutex, NULL);
    pthread_mutex_init(&readTry, NULL);
    pthread_mutex_init(&resource, NULL);
    reader_count = 0;
    writer_count = 0;
    for(int i = 0; i < NUMBER_OF_THREADS; i++){
        pthread_create(&threads[i], NULL, &reader, NULL);
        pthread_create(&threads[NUMBER_OF_THREADS + i], NULL, &writer, NULL);
    }
    for (int i = 0; i < 2 * NUMBER_OF_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    std::cout << "Result: " << global <<'\n';
    return 0;
}

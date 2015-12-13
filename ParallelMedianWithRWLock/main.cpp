//
//  main.cpp
//  ParallelMedianWithRWLock
//
//  Created by Руслан on 13.12.15.
//  Copyright © 2015 MIPT. All rights reserved.
//

#include <iostream>
#include <vector>
#include <pthread.h>
using std::cout;

#define NUMBER_OF_THREADS 100
#define SIZE_OF_CHUNK 10

int reader_count, writer_count;
pthread_mutex_t reader_count_mutex, writer_count_mutex, reader_enters_mutex, readTry, resource;
int global;
std::vector<int> data;

struct chunk_of_data{
    int begin_index;
    int end_index;
};

int calculate_median (std::vector<int> data){
    int median;
    size_t size = data.size();
    
    sort(data.begin(), data.end());
    
    if (size  % 2 == 0)
    {
        median = (data[size / 2 - 1] + data[size / 2]) / 2;
    }
    else
    {
        median = data[size / 2];
    }
    return median;

}

void* median_thread_func(void* args){
    chunk_of_data chunk = *(chunk_of_data*) args;
    int begin = chunk.begin_index;
    int end = chunk.end_index;
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
    std::vector<int>::const_iterator first = data.begin() + begin;
    std::vector<int>::const_iterator last = data.begin() + end;
    std::vector<int> newVec(first, last);
    int median = calculate_median(newVec);
    
    pthread_mutex_lock(&reader_count_mutex);
    reader_count --;
    if (reader_count == 0) {
        pthread_mutex_unlock(&resource);
    }
    pthread_mutex_unlock(&reader_count_mutex);
    pthread_mutex_lock(&writer_count_mutex);
    writer_count++;
    if (writer_count == 1) {
        pthread_mutex_lock(&readTry);
    }
    pthread_mutex_unlock(&writer_count_mutex);
    pthread_mutex_lock(&resource);
    
    //CRITICAL SECTION
    int element = (begin + end)/2;
    std::vector<int>::iterator it = data.begin() + element;
    *it = median;
    
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
    for (int i = 0; i <= NUMBER_OF_THREADS; i++) {
        data.push_back(rand()%10);
    }
    global = 0;
    pthread_t threads[NUMBER_OF_THREADS];
    pthread_mutex_init(&reader_count_mutex, NULL);
    pthread_mutex_init(&writer_count_mutex, NULL);
    pthread_mutex_init(&reader_enters_mutex, NULL);
    pthread_mutex_init(&readTry, NULL);
    pthread_mutex_init(&resource, NULL);
    reader_count = 0;
    writer_count = 0;
    for(int i = 0; i < NUMBER_OF_THREADS - SIZE_OF_CHUNK; i++){
        chunk_of_data* chunk = new chunk_of_data;
        chunk->begin_index = i;
        chunk->end_index = SIZE_OF_CHUNK + i;
        pthread_create(&threads[i], NULL, &median_thread_func, chunk);
    }
    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    for (int n:data){
        cout << n << ' ';
    }
    cout << '\n';
    return 0;
}

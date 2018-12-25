#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdio>
using namespace std;

int num = 0;
pthread_mutex_t lock;


void *handler1(void *arg){
    sleep(1);
    int i = *(int*)arg;
    cout << "---------" << i << endl;
    int n = 0;
    while(n++ < 100000000) {
        pthread_mutex_lock(&lock);
        num ++;
        //cout << i << "`s num = " << num << endl;
        pthread_mutex_unlock(&lock);
    }
}

void *handler2(void *arg){
    sleep(1);
    int i = *(int*)arg;
    cout << "---------" << i << endl;
    int n = 0;
    while(n++ < 100000000) {
        __sync_fetch_and_add(&num, 1);
        //cout << i << "`s num = " << num << endl;
    }
}

void *handler3(void *arg){
    sleep(1);
    int i = *(int*)arg;
    cout << "---------" << i << endl;
    int n = 0;
    while(n++ < 100000000) {
        num ++;
        //cout << i << "`s num = " << num << endl;
    }
}

int main(int argc, char const *argv[])
{
    pthread_mutex_init(&lock, NULL);
    pthread_t tid[3];
    int ret, i;
    int a[3] = {1, 2, 3};
    for(i=0; i<3; i++){
        ret = pthread_create(&tid[i], NULL, handler3, &a[i]);
        if(ret != 0){
            perror("pthread_create");
            return -1;
        }
        cout << "number : " << i+1 << endl;
    }
    for(i=0; i<3; i++){
        pthread_join(tid[i], NULL);
        cout << "number : " << i+1 << " destoried" << endl;
    }
    cout << "final number = " << num << endl;
    pthread_mutex_destroy(&lock);
    return 0;
}

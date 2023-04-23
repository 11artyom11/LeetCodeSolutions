#define O2_COUNT 5

#include <iostream>
#include <condition_variable>
#include <functional>
#include <thread>
#include <unistd.h>
#include <mutex>
#include <vector>

using std::cerr;
using std::cin;
using std::condition_variable;
using std::function;
using std::thread;
using std::mutex;
using std::unique_lock;
using std::vector;

condition_variable  cv;
mutex               m;
unsigned int hydrogen_count = 0;
unsigned int oxygen_count = 0;


void release_hydrogen(void)
{
    cerr << "H - ";
}

void release_oxygen(void)
{
    cerr << "O +  ";
}

void hydrogen(function<void()> release_hydrogen)
{
    unique_lock lk(m);
    // sleep(1);
    cv.wait(lk, [](){return hydrogen_count < 2;});
    release_hydrogen();
    hydrogen_count+=1;
    lk.unlock();
    cv.notify_all();
}

void oxygen(function<void()> release_oxygen)
{
    unique_lock lk(m);
    // sleep(1);
    cv.wait(lk, [](){return hydrogen_count == 2;});
    release_oxygen();
    hydrogen_count = 0;
    oxygen_count+=1;
    lk.unlock();
    cv.notify_all();
}

int main()
{
    vector<thread> hydrogen_threads;
    vector<thread> oxygen_threads;

    for (size_t i = 0; i < O2_COUNT; i++){
        oxygen_threads.emplace_back(oxygen, release_oxygen);
    }    

    for (size_t i = 0; i < 2 * O2_COUNT; i++){
        hydrogen_threads.emplace_back(hydrogen, release_hydrogen);
    }

    for (auto& th : oxygen_threads)  {th.join();}
    for (auto& th : hydrogen_threads){th.join();}
    return 0;
}
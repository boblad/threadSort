#include <mutex>
#include <pthread.h>
#include <stdio.h>
#include <ctype.h>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <random>
#include <cmath>
#include <chrono>
#include <string>
#include <queue>
#include <cstdlib>
#include <algorithm>


const int THREAD_COUNT = 8;
const int MAXSIZE = 10;
const int VECTOR_SIZE = 100000;
  std::vector<int> v2(VECTOR_SIZE);

class TSQ
{
public:
static std::mutex m;
  std::queue<int> the_queue;
  void push(int& data, int maxSize, int name){
    std::lock_guard<std::mutex> guard(m);
    if(the_queue.size() < maxSize){
    the_queue.push(data);
    std::cout << "Producer thread named " << name << " produced " << data << std::endl;
    }
  }
  bool empty() const
  {
    std::lock_guard<std::mutex> guard(m);
    return the_queue.empty();
  }
  int front(){
    std::lock_guard<std::mutex> guard(m);
    return the_queue.front();
  }
  int size(){
    std::lock_guard<std::mutex> guard(m);
    return the_queue.size();
  }
  int pop(int name){
    // boost::mutex::scoped_lock lock(the_mutex);
    std::lock_guard<std::mutex> guard(m);
    if(!the_queue.empty()){
      int value = the_queue.front();
      the_queue.pop();
      std::cout << "Consumer thread named " << name << " consumed " << value << std::endl;
      return value;
    }
    else{
      return -1;
    }
  }
};


void producer_thread(TSQ* the_queue, int name, int maxSize){
  int i = 0;
  while(1){
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    if(the_queue->size() < maxSize){
    if(i >= VECTOR_SIZE){
      throw std::runtime_error("");
    }
    int random = 0;
    the_queue->push(random, maxSize, name);
    i++;
    }
  }
}

void consumer_thread(TSQ* the_queue, int name){
  while(1){
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    if(!the_queue->empty()){
      the_queue->pop(name);
    }
  }
}

void quickSort(int left, int right) {
      int i = left, j = right;
      int tmp;
      int pivot = v2[(left + right) / 2];
      while (i <= j) {
            while (v2[i] < pivot)
                  i++;
            while (v2[j] > pivot)
                  j--;
            if (i <= j) {
                  tmp = v2[i];
                  v2[i] = v2[j];
                  v2[j] = tmp;
                  i++;
                  j--;
            }
      };
      if (left < j)
            quickSort(left, j);
      if (i < right)
            quickSort(i, right);
}


template<class Iter>
void merge_sort(Iter first, Iter last)
{
    if (last - first > 1) {
        Iter middle = first + (last - first) / 2;
        merge_sort(first, middle);
        merge_sort(middle, last);
        std::inplace_merge(first, middle, last);
    }
}

void stats(float serial, float parallel){
  float speedUp = serial/parallel;
  float efficiency = speedUp/THREAD_COUNT;
  std::cout << "Speed Up = " << speedUp << std::endl;
  std::cout << "Efficiency = " << efficiency << std::endl;

}
bool found = false;

void linearSearch(int start, int stop, int value){
  for(int i = start; i < stop && found == false; i++){
    if(v2[i] == value){
      found = true;
      std::cout << "found it" << std::endl;
    }
  }
}

std::mutex TSQ::m;

int main(){
  TSQ the_queue;
  std::thread t[THREAD_COUNT];
  std::thread t2[THREAD_COUNT];

  std::vector<int> v(VECTOR_SIZE);

  int breakUp = VECTOR_SIZE/THREAD_COUNT;

  for(int i = 0; i < VECTOR_SIZE; i++){
    int temp = rand() % 100;
    v[i] = temp;
    v2[i] = temp;
    //std::cout<< "v[" << i <<"] = " << v[i] << std::endl;
  }

  int finding = rand() % VECTOR_SIZE;
  auto startTime3 = std::chrono::steady_clock::now();
  std::find(std::begin(v2), std::end(v2), v2[finding]);
  auto endTime3 = std::chrono::steady_clock::now();
  auto duration3 = endTime3 - startTime3;
  auto sec3 = std::chrono::duration_cast<std::chrono::duration<float> >(duration3);

  auto startTime4 = std::chrono::steady_clock::now();
  for(int i = 0; i < THREAD_COUNT; i++){
    int start = i * breakUp;
    if(start != 0){
      start = start + 1;
    }
    int end = (i + 1) * breakUp;
    int value = v2[finding];
    t2[i] = std::thread(linearSearch, start, end, value);
  }

  for(int i = 0; i < THREAD_COUNT; i++){
    t2[i].join();
  }
  auto endTime4 = std::chrono::steady_clock::now();
  auto duration4 = endTime4 - startTime4;
  auto sec4 = std::chrono::duration_cast<std::chrono::duration<float> >(duration4);

  auto startTime = std::chrono::steady_clock::now();
  std::sort(std::begin(v), std::end(v));
  auto endTime = std::chrono::steady_clock::now();
  auto duration = endTime - startTime;
  auto sec = std::chrono::duration_cast<std::chrono::duration<float> >(duration);

  auto startTime2 = std::chrono::steady_clock::now();
  for(int i = 0; i < THREAD_COUNT; i++){
    int start = i * breakUp;
    if(start != 0){
      start = start + 1;
    }
    int end = (i + 1) * breakUp;
    t[i] = std::thread(quickSort, start, end);
  }

  for(int i = 0; i < THREAD_COUNT; i++){
    t[i].join();
  }
  merge_sort(v2.begin(), v2.end());
  auto endTime2 = std::chrono::steady_clock::now();
  auto duration2 = endTime2 - startTime2;
  auto sec2 = std::chrono::duration_cast<std::chrono::duration<float> >(duration2);
  // for(int i = 0; i < VECTOR_SIZE; i++){
  //   std::cout<< "v2[" << i <<"] = " << v2[i] << std::endl;
  // }
  std::cout << "std::sort with " << VECTOR_SIZE << " elements took " << sec.count() << " seconds." << std::endl;
  std::cout << "sorting with a threaded quicksort of size " << VECTOR_SIZE << " elements took " << sec2.count() << " seconds with " << THREAD_COUNT << " threads" << std::endl;
  std::cout << "std::find of size " << VECTOR_SIZE << " elements took " << sec3.count() << " seconds." << std::endl;
  std::cout << "finding with a threaded linear find of " << VECTOR_SIZE << " elements took " << sec4.count() << " seconds with " << THREAD_COUNT << " threads" << std::endl;
  std::cout << "Sorting Stats" << std::endl;
  stats(sec.count(), sec2.count());
  std::cout << "Finding Stats" << std::endl;
  stats(sec3.count(), sec4.count());
  std::cout << "To change Array Size, change the value of VECTOR_SIZE on line 19" << std::endl;
  std::cout << "To change Prove array is sorted uncomment out lines 162 - 164" << std::endl;
}

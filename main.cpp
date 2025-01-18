#include <thread>
#include <vector>
#include <iostream>
#include "performant_thread.h"
#include <stdint.h>
using namespace std;

// Struct representing sensor readings
struct SensorReadings {
    int8_t temperature;
    char padding1[63]; // added padding of 63 char variable of 1 bytes each
    int8_t humidity;
    char padding2[63]; // added padding of 63 char variable of 1 bytes each 
    int8_t pressure;
    char padding3[63]; // added padding of 63 char variable of 1 bytes each
    int8_t light;
    char padding4[63]; // added padding of 63 char variable of 1 bytes each
    int8_t co2;
    char padding5[63]; // added padding of 63 char variable of 1 bytes each
    int8_t aqi;
    char padding6[63]; // added padding of 63 char variable of 1 bytes each
};

struct thread_info
{
  void (*thread_func)(uint32_t, uint8_t *, int8_t *) = nullptr;
  uint32_t threadIdx = -1;
  uint8_t *data_addr = nullptr;
  int8_t *data = nullptr;
};

std::vector<thread_info> _threads;

SensorReadings readings;

int work_init()
{
  std::cout << "Allocating & Initializing Memory" << std::endl;
  std::vector<thread_info> threads;

  int thread_count = 6;
  for (int i = 0; i < thread_count; i++)
  {
    thread_info thread_info;
    thread_info.thread_func = performant_thread_run;
    thread_info.threadIdx = i;

    // actual code used collected raw data but we replaced it with random
    // data for the sake of privacy and simplicity.
    uint8_t *thread_data = (uint8_t *)malloc(TOTAL_SIZE);
    madvise(thread_data, TOTAL_SIZE, MADV_HUGEPAGE);



    for (long unsigned int j = 0; j < TOTAL_SIZE; j++)
    {
      thread_data[j] = rand() % 256;
    }
    thread_info.data_addr = thread_data;
    switch(i) {
      case 0:
      thread_info.data = &readings.temperature;
      break;
      case 1:
      thread_info.data = &readings.humidity;
      break;
      case 2:
      thread_info.data = &readings.pressure;
      break;
      case 3:
      thread_info.data = &readings.light;
      break;
      case 4:
      thread_info.data = &readings.co2;
      break;
      case 5:
      thread_info.data = &readings.aqi;
      break;
      default:
        perror("Error, no need of this thread");
    }
    _threads.push_back(thread_info);
    
  }

  return _threads.size();
}

void work_run()
{
  std::cout << "Starting Threads for Running" << std::endl;
  std::vector<std::thread> threads;

  for (long unsigned int i = 0; i < _threads.size(); i++)
  {
    threads.push_back(std::thread(_threads[i].thread_func, _threads[i].threadIdx, _threads[i].data_addr, _threads[i].data));
  }


  for (long unsigned int i = 0; i < threads.size(); i++)
  {
    threads[i].join();
  }
  return;
}
int main(void)
{

  work_init();
  work_run();

  std::cout << "Final Sensor Readings:\n";
  std::cout << "Temperature: " << static_cast<int>(readings.temperature) << "\n";
  std::cout << "Humidity: " << static_cast<int>(readings.humidity) << "\n";
  std::cout << "Pressure: " << static_cast<int>(readings.pressure) << "\n";
  std::cout << "Light: " << static_cast<int>(readings.light) << "\n";
  std::cout << "CO2: " << static_cast<int>(readings.co2) << "\n";
  std::cout << "AQI: " << static_cast<int>(readings.aqi) << "\n";

}

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

#include "metrics_collector.h"

std::unique_ptr<MetricsCollector> CreateCollector();

int main()
{
    auto collector = CreateCollector();

    while (true)
    {
        float cpu = collector->GetCpuUsage();
        float mem = collector->GetMemoryUsage();

        std::cout << "CPU Usage: " << cpu << "%" << std::endl;
        std::cout << "Memory Usage: " << mem << "%" << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return 0;
}

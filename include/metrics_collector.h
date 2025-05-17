#ifndef METRICS_COLLECTOR_H
#define METRICS_COLLECTOR_H

class MetricsCollector
{
public:
    virtual ~MetricsCollector() = default;

    virtual float GetCpuUsage() = 0;

    virtual float GetMemoryUsage() = 0;
};

#endif
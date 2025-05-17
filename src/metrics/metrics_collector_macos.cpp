#include "metrics_collector.h"
#include <mach/mach.h>
#include <sys/sysctl.h>

class MacOSMetricsCollector : public MetricsCollector
{
    float GetCpuUsage() override
    {
        host_cpu_load_info_data_t cpuinfo;
        mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
        kern_return_t kr = host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, (host_info_t)&cpuinfo, &count);
        if (kr != KERN_SUCCESS)
            return -1.0f;

        static uint64_t prevTotalTicks = 0, prevIdleTicks = 0;

        uint64_t idleTicks = cpuinfo.cpu_ticks[CPU_STATE_IDLE];
        uint64_t totalTicks = 0;
        for (int i = 0; i < CPU_STATE_MAX; i++)
            totalTicks += cpuinfo.cpu_ticks[i];

        uint64_t totalDiff = totalTicks - prevTotalTicks;
        uint64_t idleDiff = idleTicks - prevIdleTicks;

        prevTotalTicks = totalTicks;
        prevIdleTicks = idleTicks; // since they are static they keep their value between function calls

        if (totalDiff == 0)
            return 0.0f;
        return (float)(totalDiff - idleDiff) * 100.0f / totalDiff;
    }

    float GetMemoryUsage() override
    {
        mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
        vm_statistics_data_t vmstat;
        if (host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t)&vmstat, &count) != KERN_SUCCESS)
            return -1.0f;

        int64_t pageSize;
        size_t size = sizeof(pageSize); // sysctl needs a pointer to a size_t, we can't just int64_t
        sysctlbyname("hw.pagesize", &pageSize, &size, nullptr, 0);

        int64_t free = vmstat.free_count * pageSize;
        int64_t active = vmstat.active_count * pageSize;
        int64_t inactive = vmstat.inactive_count * pageSize;
        int64_t wired = vmstat.wire_count * pageSize;

        int64_t used = active + inactive + wired;
        int64_t total = used + free;

        if (total == 0)
            return 0.0f;
        return (float)used * 100.0f / total;
    }
};

#include <memory>
std::unique_ptr<MetricsCollector> CreateCollector()
{
    return std::make_unique<MacOSMetricsCollector>();
}
#include "system_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include "cJSON.h"

// 读取 CPU 温度
static float get_cpu_temperature() {
    FILE *temp_file = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    if (temp_file == NULL) {
        return 0.0;
    }
    
    int temp;
    fscanf(temp_file, "%d", &temp);
    fclose(temp_file);
    
    return temp / 1000.0; // 转换为摄氏度
}

// 读取 CPU 频率
static int get_cpu_frequency() {
    FILE *freq_file = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq", "r");
    if (freq_file == NULL) {
        return 0;
    }
    
    int freq;
    fscanf(freq_file, "%d", &freq);
    fclose(freq_file);
    
    return freq / 1000; // 转换为 MHz
}

// 获取 CPU 使用率
static float get_cpu_usage() {
    static unsigned long long prev_idle = 0, prev_total = 0;
    unsigned long long user, nice, system, idle, iowait, irq, softirq;
    
    FILE *stat_file = fopen("/proc/stat", "r");
    if (stat_file == NULL) {
        return 0.0;
    }
    
    fscanf(stat_file, "cpu %llu %llu %llu %llu %llu %llu %llu",
           &user, &nice, &system, &idle, &iowait, &irq, &softirq);
    fclose(stat_file);
    
    unsigned long long total = user + nice + system + idle + iowait + irq + softirq;
    unsigned long long current_idle = idle + iowait;
    
    unsigned long long total_delta = total - prev_total;
    unsigned long long idle_delta = current_idle - prev_idle;
    
    prev_total = total;
    prev_idle = current_idle;
    
    if (total_delta == 0) {
        return 0.0;
    }
    
    return 100.0 * (1.0 - ((float)idle_delta / total_delta));
}

// 获取网络接口状态
static cJSON* get_network_status() {
    cJSON *network_array = cJSON_CreateArray();
    
    FILE *net_file = fopen("/proc/net/dev", "r");
    if (net_file == NULL) {
        return network_array;
    }
    
    char line[256];
    // 跳过前两行头部
    fgets(line, sizeof(line), net_file);
    fgets(line, sizeof(line), net_file);
    
    while (fgets(line, sizeof(line), net_file)) {
        char iface[32];
        unsigned long long rx_bytes, tx_bytes;
        sscanf(line, "%s %llu %*u %*u %*u %*u %*u %*u %*u %llu",
               iface, &rx_bytes, &tx_bytes);
        
        // 移除接口名称末尾的冒号
        char *colon = strchr(iface, ':');
        if (colon) *colon = '\0';
        
        // 跳过 lo 接口
        if (strcmp(iface, "lo") == 0) continue;
        
        cJSON *iface_obj = cJSON_CreateObject();
        cJSON_AddStringToObject(iface_obj, "interface", iface);
        cJSON_AddStringToObject(iface_obj, "status", "up"); // 简化处理
        cJSON_AddStringToObject(iface_obj, "rx_speed", "0 Mbps"); // 简化处理
        cJSON_AddStringToObject(iface_obj, "tx_speed", "0 Mbps"); // 简化处理
        
        cJSON_AddItemToArray(network_array, iface_obj);
    }
    
    fclose(net_file);
    return network_array;
}

void handle_system_status(struct mg_connection *c) {
    struct sysinfo si;
    if (sysinfo(&si) != 0) {
        mg_http_reply(c, 500, "", "{\"error\":\"Failed to get system info\"}");
        return;
    }
    
    // 创建 JSON 响应
    cJSON *response = cJSON_CreateObject();
    
    // CPU 状态
    cJSON *cpu = cJSON_CreateObject();
    cJSON_AddNumberToObject(cpu, "usage", get_cpu_usage());
    cJSON_AddNumberToObject(cpu, "temperature", get_cpu_temperature());
    cJSON_AddNumberToObject(cpu, "frequency", get_cpu_frequency());
    cJSON_AddItemToObject(response, "cpu", cpu);
    
    // 内存状态
    cJSON *memory = cJSON_CreateObject();
    unsigned long total_ram = si.totalram * si.mem_unit;
    unsigned long used_ram = (si.totalram - si.freeram) * si.mem_unit;
    float mem_usage = (float)used_ram / total_ram * 100;
    
    cJSON_AddNumberToObject(memory, "total", (double)total_ram);
    cJSON_AddNumberToObject(memory, "used", (double)used_ram);
    cJSON_AddNumberToObject(memory, "usage", mem_usage);
    cJSON_AddItemToObject(response, "memory", memory);
    
    // 网络状态
    cJSON_AddItemToObject(response, "network", get_network_status());
    
    // 获取 JSON 字符串
    char *json_str = cJSON_Print(response);
    
    // 发送响应
    mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", json_str);
    
    cJSON_free(json_str);
    cJSON_Delete(response);
} 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cJSON.h"
#include <sqlite3.h>
#include "deep_learning_handler.h"

// 配置文件路径
#define CONFIG_FILE "config/deep_learning.json"
#define DB_PATH "/tmp/packet_info.db"
#define DB_BUSY_TIMEOUT 5000  // 数据库忙等待超时时间(毫秒)
#define MAX_RETRIES 3        // 最大重试次数

char* get_deep_learning_settings(void) {
    FILE* fp = fopen(CONFIG_FILE, "r");
    if (!fp) {
        // 如果文件不存在，返回默认配置
        cJSON* config = cJSON_CreateObject();
        
        // 基础配置
        cJSON* basic = cJSON_CreateObject();
        cJSON_AddBoolToObject(basic, "enabled", 1);
        cJSON_AddBoolToObject(basic, "fpgaEnabled", 0);
        cJSON_AddNumberToObject(basic, "maxConcurrent", 10);
        cJSON_AddItemToObject(config, "basic", basic);
        
        // 检测配置
        cJSON* detection = cJSON_CreateObject();
        cJSON_AddBoolToObject(detection, "maliciousTraffic", 1);
        cJSON_AddBoolToObject(detection, "abnormalBehavior", 1);
        cJSON_AddBoolToObject(detection, "unknownThreats", 1);
        cJSON_AddNumberToObject(detection, "threshold", 80);
        cJSON_AddItemToObject(config, "detection", detection);
        
        // 模型配置
        cJSON* model = cJSON_CreateObject();
        cJSON_AddStringToObject(model, "updateCycle", "weekly");
        cJSON_AddBoolToObject(model, "autoUpdate", 1);
        cJSON_AddStringToObject(model, "currentVersion", "v1.0.0");
        cJSON_AddItemToObject(config, "model", model);
        
        // 日志配置
        cJSON* log = cJSON_CreateObject();
        cJSON_AddStringToObject(log, "level", "info");
        cJSON_AddNumberToObject(log, "retentionDays", 30);
        cJSON_AddItemToObject(config, "log", log);
        
        char* json_str = cJSON_Print(config);
        char* result = strdup(json_str);
        
        cJSON_Delete(config);
        free(json_str);
        return result;
    }

    // 读取文件内容
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char* buffer = malloc(size + 1);
    fread(buffer, 1, size, fp);
    buffer[size] = '\0';
    
    fclose(fp);
    return buffer;
}

int update_deep_learning_settings(const char* settings) {
    // 验证 JSON 格式
    cJSON* config = cJSON_Parse(settings);
    if (!config) {
        return 0;
    }
    
    // 保存到文件
    FILE* fp = fopen(CONFIG_FILE, "w");
    if (!fp) {
        cJSON_Delete(config);
        return 0;
    }
    
    fputs(settings, fp);
    fclose(fp);
    
    cJSON_Delete(config);
    return 1;
}

int clear_deep_learning_database(void) {
    sqlite3 *db;
    char *err_msg = NULL;
    int rc;
    int retry_count = 0;
    
    // 检查数据库文件是否存在
    if (access(DB_PATH, F_OK) != 0) {
        fprintf(stderr, "Database file does not exist: %s\n", DB_PATH);
        return 0;
    }
    
    // 检查数据库文件权限
    if (access(DB_PATH, W_OK) != 0) {
        fprintf(stderr, "Database file is not writable: %s\n", DB_PATH);
        return 0;
    }
    
    // 打开数据库连接
    while (retry_count < MAX_RETRIES) {
        rc = sqlite3_open_v2(DB_PATH, &db, 
                           SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                           NULL);
        if (rc == SQLITE_OK) {
            break;
        }
        fprintf(stderr, "Cannot open database (attempt %d/%d): %s\n", 
                retry_count + 1, MAX_RETRIES, sqlite3_errmsg(db));
        sqlite3_close(db);
        usleep(100000);  // 等待100ms
        retry_count++;
    }
    
    if (retry_count >= MAX_RETRIES) {
        return 0;
    }
    
    // 设置数据库忙等待超时
    sqlite3_busy_timeout(db, DB_BUSY_TIMEOUT);
    
    // 清空数据表
    const char *sql = "DELETE FROM packet_info";
    retry_count = 0;
    while (retry_count < MAX_RETRIES) {
        rc = sqlite3_exec(db, sql, NULL, NULL, &err_msg);
        if (rc == SQLITE_OK) {
            break;
        }
        fprintf(stderr, "Failed to clear table (attempt %d/%d): %s\n", 
                retry_count + 1, MAX_RETRIES, err_msg);
        sqlite3_free(err_msg);
        usleep(100000);
        retry_count++;
    }
    
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return 0;
    }
    
    // 清理并关闭数据库连接
    sqlite3_close(db);
    return 1;
} 

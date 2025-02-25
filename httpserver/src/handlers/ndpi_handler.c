#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <errno.h>
#include "cJSON.h"
#include "ndpi_handler.h"
#include <sqlite3.h>

#define MAX_PROTOCOLS 100
#define SEM_WAIT_TIMEOUT 5  // 5秒超时
#define DB_BUSY_TIMEOUT 5000  // 数据库忙等待超时时间(毫秒)
#define MAX_RETRIES 3        // 最大重试次数

// NDPI 协议统计结构
typedef struct {
    char protocol[32];
    unsigned long packets;
    unsigned long bytes;
    unsigned long flows;
    time_t last_seen;
} ndpi_protocol_stat;

// 全局统计数据
static struct {
    ndpi_protocol_stat protocols[MAX_PROTOCOLS];
    int protocol_count;
} ndpi_stats = {
    .protocols = {
        {"HTTP", 1234567, 98765432, 4321, 0},
        {"HTTPS", 7654321, 87654321, 3214, 0},
        {"DNS", 456789, 3456789, 1234, 0},
        {"SSH", 123456, 2345678, 567, 0},
        {"FTP", 34567, 1234567, 123, 0},
        {"SMTP", 23456, 987654, 89, 0},
        {"POP3", 12345, 876543, 67, 0},
        {"IMAP", 9876, 765432, 45, 0},
        {"NTP", 8765, 654321, 34, 0},
        {"SNMP", 7654, 543210, 23, 0}
    },
    .protocol_count = 10
};

/* 定义信号量名称 */
#define SEM_NAME "/ndpi_stats_sem"

/* 定义静态信号量指针 */
static sem_t *stats_sem = NULL;

/* 初始化信号量 */
static int init_semaphore() {
    // 先尝试解除已存在的同名信号量
    sem_unlink(SEM_NAME);

    stats_sem = sem_open(SEM_NAME, O_CREAT, 0644, 1);
    if(stats_sem == SEM_FAILED) {
        fprintf(stderr, "Failed to create semaphore: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

void format_time_str(char *str, size_t str_size, u_int64_t ms_time) {
    struct tm time_tm;
    time_t sec = ms_time / 1000;
    localtime_r(&sec, &time_tm);
    snprintf(str, str_size, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
            time_tm.tm_year + 1900, time_tm.tm_mon + 1, time_tm.tm_mday,
            time_tm.tm_hour, time_tm.tm_min, time_tm.tm_sec,
            (int)(ms_time % 1000));
}

char* get_ndpi_stats(int page, int page_size) {
    // 确保信号量已初始化
    if(!stats_sem && init_semaphore() != 0) {
        printf("[进程 %d] 初始化信号量失败\n", getpid());
        return NULL;
    }

    printf("[进程 %d] 等待获取信号量...\n", getpid());

    // 使用超时版本的信号量等待
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += SEM_WAIT_TIMEOUT;

    int wait_result;
    while((wait_result = sem_timedwait(stats_sem, &ts)) == -1 && errno == EINTR) {
        // 如果是被信号中断，继续等待
        continue;
    }

    if(wait_result == -1) {
        if(errno == ETIMEDOUT) {
            printf("[进程 %d] 等待信号量超时\n", getpid());
        } else {
            printf("[进程 %d] 获取信号量失败: %s\n", getpid(), strerror(errno));
        }
        return NULL;
    }

    printf("[进程 %d] 成功获取信号量\n", getpid());

    sqlite3 *db;
    sqlite3_stmt *stmt;
    cJSON *root = cJSON_CreateObject();
    cJSON *protocols = cJSON_CreateArray();
    int rc;
    
    // 打开数据库连接
    rc = sqlite3_open("/tmp/packet_info.db", &db);
    if(rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        printf("[进程 %d] 数据库打开失败，释放信号量\n", getpid());
        sem_post(stats_sem);
        return NULL;
    }
    
    // 设置数据库忙等待超时
    sqlite3_busy_timeout(db, DB_BUSY_TIMEOUT);
    
    // 计算偏移量
    int offset = (page - 1) * page_size;
    
    // 准备SQL语句 - 使用LIMIT和OFFSET进行分页
    const char *sql = "SELECT * FROM packet_info ORDER BY flow_id DESC LIMIT ? OFFSET ?";
    
    int retry_count = 0;
    while (retry_count < MAX_RETRIES) {
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
        if (rc == SQLITE_BUSY || rc == SQLITE_LOCKED) {
            printf("[进程 %d] 数据库忙，等待后重试 (尝试 %d/%d)\n", 
                   getpid(), retry_count + 1, MAX_RETRIES);
            sqlite3_close(db);
            usleep(100000);  // 等待100ms
            rc = sqlite3_open("../7.nDPI/detect_service/packet_info.db", &db);
            if (rc) break;
            sqlite3_busy_timeout(db, DB_BUSY_TIMEOUT);
            retry_count++;
            continue;
        }
        break;
    }
    
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        printf("[进程 %d] SQL准备失败，释放信号量\n", getpid());
        sem_post(stats_sem);
        return NULL;
    }
    
    // 绑定参数
    sqlite3_bind_int(stmt, 1, page_size);
    sqlite3_bind_int(stmt, 2, offset);
    
    // 遍历结果
    while(sqlite3_step(stmt) == SQLITE_ROW) {
        cJSON *row = cJSON_CreateObject();
        
        cJSON_AddNumberToObject(row, "flow_id", sqlite3_column_int(stmt, 0));
        cJSON_AddStringToObject(row, "src_ip", (const char*)sqlite3_column_text(stmt, 1));
        cJSON_AddNumberToObject(row, "src_port", sqlite3_column_int(stmt, 2));
        cJSON_AddStringToObject(row, "dst_ip", (const char*)sqlite3_column_text(stmt, 3));
        cJSON_AddNumberToObject(row, "dst_port", sqlite3_column_int(stmt, 4));
        cJSON_AddStringToObject(row, "protocol", (const char*)sqlite3_column_text(stmt, 5));
        cJSON_AddStringToObject(row, "category", (const char*)sqlite3_column_text(stmt, 6));
        cJSON_AddStringToObject(row, "hostname", (const char*)sqlite3_column_text(stmt, 7));
        cJSON_AddNumberToObject(row, "up_packets", sqlite3_column_int(stmt, 8));
        cJSON_AddNumberToObject(row, "up_bytes", sqlite3_column_int(stmt, 9));
        cJSON_AddNumberToObject(row, "down_packets", sqlite3_column_int(stmt, 10));
        cJSON_AddNumberToObject(row, "down_bytes", sqlite3_column_int(stmt, 11));
        //cJSON_AddNumberToObject(row, "first_seen", sqlite3_column_int(stmt, 12));
        //cJSON_AddNumberToObject(row, "last_seen", sqlite3_column_int(stmt, 13));

        const char first_seen_val[128] = {0};
        const char last_seen_val[128] = {0};

        format_time_str(first_seen_val, sizeof(first_seen_val), sqlite3_column_int64(stmt, 12));
        format_time_str(last_seen_val, sizeof(last_seen_val), sqlite3_column_int64(stmt, 13));

        cJSON_AddStringToObject(row, "first_seen", (const char*)first_seen_val);
        cJSON_AddStringToObject(row, "last_seen", (const char*)last_seen_val);


        cJSON_AddStringToObject(row, "risks", (const char*)sqlite3_column_text(stmt, 14));
        
        // 添加class字段处理
        const char *class_val = (const char*)sqlite3_column_text(stmt, 15);
        cJSON_AddStringToObject(row, "class", class_val ? class_val : "unknown");
        
        // 添加probability字段处理
        double probability = sqlite3_column_double(stmt, 16);
        cJSON_AddNumberToObject(row, "probability", probability);
        
        cJSON_AddItemToArray(protocols, row);
    }
    
    // 获取总记录数
    sqlite3_stmt *count_stmt;
    const char *count_sql = "SELECT COUNT(*) FROM packet_info";
    retry_count = 0;
    while (retry_count < MAX_RETRIES) {
        rc = sqlite3_prepare_v2(db, count_sql, -1, &count_stmt, 0);
        if (rc == SQLITE_BUSY || rc == SQLITE_LOCKED) {
            printf("[进程 %d] 数据库忙，等待后重试计数查询 (尝试 %d/%d)\n", 
                   getpid(), retry_count + 1, MAX_RETRIES);
            usleep(100000);  // 等待100ms
            retry_count++;
            continue;
        }
        break;
    }
    
    int total = 0;
    if(rc == SQLITE_OK && sqlite3_step(count_stmt) == SQLITE_ROW) {
        total = sqlite3_column_int(count_stmt, 0);
    }
    
    cJSON_AddItemToObject(root, "protocols", protocols);
    cJSON_AddNumberToObject(root, "total", total);
    cJSON_AddNumberToObject(root, "page", page);
    cJSON_AddNumberToObject(root, "pageSize", page_size);
    
    // 清理
    sqlite3_finalize(stmt);
    sqlite3_finalize(count_stmt);
    sqlite3_close(db);
    
    printf("[进程 %d] 数据库操作完成，准备释放信号量\n", getpid());
    // 释放信号量
    sem_post(stats_sem);
    printf("[进程 %d] 信号量已释放\n", getpid());
    
    char *json_str = cJSON_Print(root);
    cJSON_Delete(root);
    
    return json_str;
}

/* 清理信号量 */
static void cleanup_semaphore() {
    if(stats_sem != NULL) {
        // 在关闭前尝试获取信号量，以确保没有其他进程在使用
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += 1;  // 等待1秒
        
        if(sem_timedwait(stats_sem, &ts) == 0) {
            sem_post(stats_sem);  // 如果获取成功，立即释放
        }

        sem_close(stats_sem);
        sem_unlink(SEM_NAME);
        stats_sem = NULL;
        printf("信号量已清理\n");
    }
}

// 在程序退出时调用此函数清理资源
void cleanup_ndpi_handler() {
    cleanup_semaphore();
} 

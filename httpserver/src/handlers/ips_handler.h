#ifndef IPS_HANDLER_H
#define IPS_HANDLER_H

#include <stdbool.h>
#include "mongoose.h"
#include "cJSON.h"

#define MAX_RULES 100

// 获取IPS设置
char* get_ips_settings(void);

// 更新IPS设置
bool update_ips_settings(const char* settings_json);

#endif 
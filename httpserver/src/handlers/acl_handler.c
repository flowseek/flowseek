#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "acl_handler.h"

#define MAX_RULES 100

static struct {
    struct {
        int id;
        char source_ip[32];
        char dest_ip[32];
        char protocol[16];
        char action[16];
        int priority;
        bool enabled;
    } rules[MAX_RULES];
    int rule_count;
} acl_settings = {
    .rule_count = 0  // 初始化为空
};

char* get_acl_rules(void) {
    cJSON *root = cJSON_CreateArray();
    
    // 添加所有规则
    for (int i = 0; i < acl_settings.rule_count; i++) {
        cJSON *rule = cJSON_CreateObject();
        cJSON_AddNumberToObject(rule, "id", acl_settings.rules[i].id);
        cJSON_AddStringToObject(rule, "sourceIP", acl_settings.rules[i].source_ip);
        cJSON_AddStringToObject(rule, "destIP", acl_settings.rules[i].dest_ip);
        cJSON_AddStringToObject(rule, "protocol", acl_settings.rules[i].protocol);
        cJSON_AddStringToObject(rule, "action", acl_settings.rules[i].action);
        cJSON_AddNumberToObject(rule, "priority", acl_settings.rules[i].priority);
        cJSON_AddBoolToObject(rule, "enabled", acl_settings.rules[i].enabled);
        cJSON_AddItemToArray(root, rule);
    }
    
    // 生成JSON字符串
    char *json_str = cJSON_Print(root);
    cJSON_Delete(root);
    
    return json_str;
}

bool add_acl_rule(const char* rule_json) {
    printf("Received rule JSON: %s\n", rule_json);
    if (acl_settings.rule_count >= MAX_RULES) return false;
    
    cJSON *root = cJSON_Parse(rule_json);
    if (!root) {
        printf("Failed to parse JSON\n");
        return false;
    }
    
    int idx = acl_settings.rule_count;
    cJSON *id = cJSON_GetObjectItem(root, "id");
    acl_settings.rules[idx].id = id ? id->valueint : (idx + 1);
    
    cJSON *action = cJSON_GetObjectItem(root, "action");
    cJSON *source_ip = cJSON_GetObjectItem(root, "sourceIP");
    cJSON *dest_ip = cJSON_GetObjectItem(root, "destIP");
    cJSON *protocol = cJSON_GetObjectItem(root, "protocol");
    cJSON *priority = cJSON_GetObjectItem(root, "priority");
    cJSON *enabled = cJSON_GetObjectItem(root, "enabled");
    
    printf("Parsed fields - action: %s, sourceIP: %s, destIP: %s, protocol: %s, priority: %d\n",
           action ? cJSON_GetStringValue(action) : "null",
           source_ip ? cJSON_GetStringValue(source_ip) : "null",
           dest_ip ? cJSON_GetStringValue(dest_ip) : "null",
           protocol ? cJSON_GetStringValue(protocol) : "null",
           priority ? priority->valueint : -1);
    
    if (!action || !source_ip || !dest_ip || !protocol || !priority) {
        printf("Missing required fields\n");
        cJSON_Delete(root);
        return false;
    }
    
    // 确保字符串不为空
    const char *action_str = cJSON_GetStringValue(action);
    const char *source_ip_str = cJSON_GetStringValue(source_ip);
    const char *dest_ip_str = cJSON_GetStringValue(dest_ip);
    const char *protocol_str = cJSON_GetStringValue(protocol);
    
    if (!action_str || !source_ip_str || !dest_ip_str || !protocol_str) {
        printf("One or more fields are empty\n");
        cJSON_Delete(root);
        return false;
    }
    
    strncpy(acl_settings.rules[idx].source_ip, source_ip_str, sizeof(acl_settings.rules[idx].source_ip) - 1);
    strncpy(acl_settings.rules[idx].dest_ip, dest_ip_str, sizeof(acl_settings.rules[idx].dest_ip) - 1);
    strncpy(acl_settings.rules[idx].protocol, protocol_str, sizeof(acl_settings.rules[idx].protocol) - 1);
    strncpy(acl_settings.rules[idx].action, action_str, sizeof(acl_settings.rules[idx].action) - 1);
    
    // 确保字符串正确终止
    acl_settings.rules[idx].source_ip[sizeof(acl_settings.rules[idx].source_ip) - 1] = '\0';
    acl_settings.rules[idx].dest_ip[sizeof(acl_settings.rules[idx].dest_ip) - 1] = '\0';
    acl_settings.rules[idx].protocol[sizeof(acl_settings.rules[idx].protocol) - 1] = '\0';
    acl_settings.rules[idx].action[sizeof(acl_settings.rules[idx].action) - 1] = '\0';
    
    acl_settings.rules[idx].priority = priority->valueint;
    acl_settings.rules[idx].enabled = enabled ? cJSON_IsTrue(enabled) : true;
    
    acl_settings.rule_count++;
    cJSON_Delete(root);
    printf("Rule added successfully. Total rules: %d\n", acl_settings.rule_count);
    return true;
}

bool update_acl_rule(int id, const char* rule_json) {
    for (int i = 0; i < acl_settings.rule_count; i++) {
        if (acl_settings.rules[i].id == id) {
            cJSON *root = cJSON_Parse(rule_json);
            if (!root) return false;
            
            cJSON *action = cJSON_GetObjectItem(root, "action");
            cJSON *source_ip = cJSON_GetObjectItem(root, "sourceIP");
            cJSON *dest_ip = cJSON_GetObjectItem(root, "destIP");
            cJSON *protocol = cJSON_GetObjectItem(root, "protocol");
            cJSON *enabled = cJSON_GetObjectItem(root, "enabled");
            
            if (!action || !source_ip || !dest_ip || !protocol) {
                cJSON_Delete(root);
                return false;
            }
            
            strncpy(acl_settings.rules[i].source_ip, cJSON_GetStringValue(source_ip), sizeof(acl_settings.rules[i].source_ip) - 1);
            strncpy(acl_settings.rules[i].dest_ip, cJSON_GetStringValue(dest_ip), sizeof(acl_settings.rules[i].dest_ip) - 1);
            strncpy(acl_settings.rules[i].protocol, cJSON_GetStringValue(protocol), sizeof(acl_settings.rules[i].protocol) - 1);
            if (enabled) {
                acl_settings.rules[i].enabled = cJSON_IsTrue(enabled);
            }
            
            cJSON_Delete(root);
            return true;
        }
    }
    return false;
}

bool delete_acl_rule(int id) {
    for (int i = 0; i < acl_settings.rule_count; i++) {
        if (acl_settings.rules[i].id == id) {
            // 将后面的规则向前移动
            for (int j = i; j < acl_settings.rule_count - 1; j++) {
                acl_settings.rules[j] = acl_settings.rules[j + 1];
            }
            acl_settings.rule_count--;
            return true;
        }
    }
    return false;
} 
#ifndef ACL_HANDLER_H
#define ACL_HANDLER_H

#include <stdbool.h>
#include "cJSON.h"

#define MAX_RULES 100

// 获取所有ACL规则
char* get_acl_rules(void);

// 添加新规则
bool add_acl_rule(const char* rule_json);

// 更新规则
bool update_acl_rule(int id, const char* rule_json);

// 删除规则
bool delete_acl_rule(int id);

#endif 
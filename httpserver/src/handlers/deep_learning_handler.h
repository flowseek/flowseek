#ifndef DEEP_LEARNING_HANDLER_H
#define DEEP_LEARNING_HANDLER_H

// 获取深度学习配置
char* get_deep_learning_settings(void);

// 更新深度学习配置
int update_deep_learning_settings(const char* settings);

// 清除深度学习数据库
int clear_deep_learning_database(void);

#endif 
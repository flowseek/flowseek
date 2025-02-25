#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mongoose.h"
#include "handlers/acl_handler.h"
#include "handlers/ips_handler.h"
#include "handlers/system_handler.h"
#include "handlers/ndpi_handler.h"
#include "handlers/deep_learning_handler.h"

#define PORT "3000"

static void fn(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;
        struct mg_str uri = hm->uri;
        
        // 处理OPTIONS请求
        if (mg_match(hm->uri, mg_str("/api/**"), NULL) &&
            mg_strcmp(hm->method, mg_str("OPTIONS")) == 0) {
            mg_http_reply(c, 200,
                        "Access-Control-Allow-Origin: *\r\n"
                        "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
                        "Access-Control-Allow-Headers: *\r\n",
                        "");
            return;
        }

        // 处理静态资源
        if (mg_match(hm->uri, mg_str("/assets/**"), NULL) ||
            mg_match(hm->uri, mg_str("/dist/**"), NULL) ||
            mg_match(hm->uri, mg_str("/components/**"), NULL)) {
            char path[256] = "frontend";
            const struct mg_str uri_str = hm->uri;
            memcpy(path + strlen(path), uri_str.buf, uri_str.len);
            path[strlen(path) + uri_str.len] = '\0';
            
            printf("Serving static file: %s\n", path);
            
            const char* mime_type = "text/plain";
            if (strstr(path, ".js")) mime_type = "application/javascript";
            else if (strstr(path, ".mjs")) mime_type = "application/javascript";
            else if (strstr(path, ".css")) mime_type = "text/css";
            else if (strstr(path, ".html")) mime_type = "text/html";
            else if (strstr(path, ".vue")) 
            {
                mime_type = "application/javascript";  // 修改 .vue 文件的 MIME 类型
                printf("Serving static file: %s\n", mime_type);
            }
            else if (strstr(path, ".png")) mime_type = "image/png";
            else if (strstr(path, ".jpg") || strstr(path, ".jpeg")) mime_type = "image/jpeg";
            
            struct mg_http_serve_opts opts = {
                .mime_types = mime_type,
                .root_dir = ".",
                .extra_headers = "Access-Control-Allow-Origin: *\r\n"
                                "Cache-Control: no-cache\r\n"
            };
            mg_http_serve_file(c, hm, path, &opts);
            return;
        }

        // 获取ACL规则列表
        if (mg_match(hm->uri, mg_str("/api/acl/rules"), NULL)) {
            if (mg_strcmp(hm->method, mg_str("GET")) == 0) {
                char* rules = get_acl_rules();
                mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", rules);
                free(rules);
                return;
            }
            
            if (mg_strcmp(hm->method, mg_str("POST")) == 0) {
                const struct mg_str body_str = hm->body;
                char* body = malloc(body_str.len + 1);
                memcpy(body, body_str.buf, body_str.len);
                body[body_str.len] = '\0';

                if (add_acl_rule(body)) {
                    mg_http_reply(c, 201, "", "{\"message\":\"Rule created\"}");
                } else {
                    mg_http_reply(c, 400, "", "{\"error\":\"Invalid request\"}");
                }
                free(body);
                return;
            }
            
            if (mg_strcmp(hm->method, mg_str("PUT")) == 0) {
                // 提取规则ID
                char id_str[32];
                mg_http_get_var(&hm->query, "id", id_str, sizeof(id_str));
                int id = atoi(id_str);

                const struct mg_str body_str = hm->body;
                char* body = malloc(body_str.len + 1);
                memcpy(body, body_str.buf, body_str.len);
                body[body_str.len] = '\0';

                if (update_acl_rule(id, body)) {
                    mg_http_reply(c, 200, "", "{\"message\":\"Rule updated\"}");
                } else {
                    mg_http_reply(c, 404, "", "{\"error\":\"Rule not found\"}");
                }
                free(body);
                return;
            }
            
            if (mg_strcmp(hm->method, mg_str("DELETE")) == 0) {
                // 从URL中获取规则ID
                struct mg_str *rule_id_str = mg_http_get_header(hm, "X-Rule-Id");
                if (rule_id_str == NULL) {
                    mg_http_reply(c, 400, "", "{\"error\":\"Missing rule ID\"}");
                    return;
                }
                
                int rule_id = atoi(rule_id_str->buf);
                if (delete_acl_rule(rule_id)) {
                    mg_http_reply(c, 200, "", "{\"message\":\"Rule deleted\"}");
                } else {
                    mg_http_reply(c, 404, "", "{\"error\":\"Rule not found\"}");
                }
                return;
            }
        }

        // 获取IPS设置
        if (mg_match(hm->uri, mg_str("/api/ips/settings"), NULL) &&
            mg_strcmp(hm->method, mg_str("GET")) == 0) {
            char* settings = get_ips_settings();
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", settings);
            free(settings);
            return;
        }

        // 更新IPS设置
        if (mg_match(hm->uri, mg_str("/api/ips/settings"), NULL) &&
            mg_strcmp(hm->method, mg_str("PUT")) == 0) {
            const struct mg_str body_str = hm->body;
            char* body = malloc(body_str.len + 1);
            memcpy(body, body_str.buf, body_str.len);
            body[body_str.len] = '\0';

            if (update_ips_settings(body)) {
                mg_http_reply(c, 200, "", "{\"message\":\"Settings updated\"}");
            } else {
                mg_http_reply(c, 400, "", "{\"error\":\"Invalid settings\"}");
            }
            free(body);
            return;
        }

        // 获取系统状态
        if (mg_match(hm->uri, mg_str("/api/system/status"), NULL) &&
            mg_strcmp(hm->method, mg_str("GET")) == 0) {
            handle_system_status(c);
            return;
        }

        // NDPI 统计信息
        if (mg_match(hm->uri, mg_str("/api/ndpi/stats"), NULL)) {
            // 获取分页参数
            char page_str[8] = "1", page_size_str[8] = "10";
            mg_http_get_var(&hm->query, "page", page_str, sizeof(page_str));
            mg_http_get_var(&hm->query, "pageSize", page_size_str, sizeof(page_size_str));
            
            int page = atoi(page_str);
            int page_size = atoi(page_size_str);
            
            // 参数验证
            if (page < 1) page = 1;
            if (page_size < 1) page_size = 10;
            if (page_size > 100) page_size = 100;
            
            char *json = get_ndpi_stats(page, page_size);
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s\n", json);
            free(json);
            return;
        }

        // 获取深度学习配置
        if (mg_match(hm->uri, mg_str("/api/deep-learning/settings"), NULL)) {
            if (mg_strcmp(hm->method, mg_str("GET")) == 0) {
                char* settings = get_deep_learning_settings();
                mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", settings);
                free(settings);
                return;
            }
            
            if (mg_strcmp(hm->method, mg_str("PUT")) == 0) {
                const struct mg_str body_str = hm->body;
                char* body = malloc(body_str.len + 1);
                memcpy(body, body_str.buf, body_str.len);
                body[body_str.len] = '\0';

                if (update_deep_learning_settings(body)) {
                    mg_http_reply(c, 200, "", "{\"message\":\"Settings updated\"}");
                } else {
                    mg_http_reply(c, 400, "", "{\"error\":\"Invalid settings\"}");
                }
                free(body);
                return;
            }
        }
        
        // 清除深度学习数据库
        if (mg_match(hm->uri, mg_str("/api/deep-learning/clear-database"), NULL) &&
            mg_strcmp(hm->method, mg_str("POST")) == 0) {
            if (clear_deep_learning_database()) {
                mg_http_reply(c, 200, "", "{\"message\":\"Database cleared\"}");
            } else {
                mg_http_reply(c, 500, "", "{\"error\":\"Failed to clear database\"}");
            }
            return;
        }

        // 处理 API 请求
        if (mg_match(hm->uri, mg_str("/api/**"), NULL)) {
            // ... API 处理代码保持不变 ...
            return;
        }

        // 所有其他请求返回 index.html
        printf("Serving index.html for route: %.*s\n", (int)uri.len, uri.buf);
        struct mg_http_serve_opts opts = {
            .mime_types = "text/html",
            .root_dir = ".",
            .extra_headers = "Cache-Control: no-cache\r\n"
        };
        mg_http_serve_file(c, hm, "frontend/index.html", &opts);
    }
}

int main(void) {
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
    
    char listen_addr[64];
    snprintf(listen_addr, sizeof(listen_addr), "http://0.0.0.0:%s", PORT);
    mg_http_listen(&mgr, listen_addr, fn, NULL);
    
    printf("Server running at http://localhost:%s\n", PORT);

    // 事件循环
    for (;;) mg_mgr_poll(&mgr, 1000);
    
    mg_mgr_free(&mgr);
    return 0;
} 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "ips_handler.h"

static struct {
    bool fragment_attack;
    bool illegal_ip;
    bool tcp_syn_frag;
    bool illegal_udp_check;
    bool illegal_tcp_flags;
    bool layer2_unknown;
    bool layer3_unknown;
    bool short_fragment;
    bool long_icmp;
    bool ip_option;
    bool icmp_fragment;
    bool tcp_fin_fragment;
    bool land_attack;
    bool illegal_layer3;
    bool illegal_layer4;
    bool winnuke;
    bool ping_of_death;
    bool anti_source_route;
} ips_settings = {
    .fragment_attack = false,
    .illegal_ip = false,
    .tcp_syn_frag = false,
    .illegal_udp_check = false,
    .illegal_tcp_flags = false,
    .layer2_unknown = false,
    .layer3_unknown = false,
    .short_fragment = false,
    .long_icmp = false,
    .ip_option = false,
    .icmp_fragment = false,
    .tcp_fin_fragment = false,
    .land_attack = false,
    .illegal_layer3 = false,
    .illegal_layer4 = false,
    .winnuke = false,
    .ping_of_death = false,
    .anti_source_route = false
};

char* get_ips_settings(void) {
    cJSON *root = cJSON_CreateObject();
    
    cJSON_AddBoolToObject(root, "fragment_attack", ips_settings.fragment_attack);
    cJSON_AddBoolToObject(root, "illegal_ip", ips_settings.illegal_ip);
    cJSON_AddBoolToObject(root, "tcp_syn_frag", ips_settings.tcp_syn_frag);
    cJSON_AddBoolToObject(root, "illegal_udp_check", ips_settings.illegal_udp_check);
    cJSON_AddBoolToObject(root, "illegal_tcp_flags", ips_settings.illegal_tcp_flags);
    cJSON_AddBoolToObject(root, "layer2_unknown", ips_settings.layer2_unknown);
    cJSON_AddBoolToObject(root, "layer3_unknown", ips_settings.layer3_unknown);
    cJSON_AddBoolToObject(root, "short_fragment", ips_settings.short_fragment);
    cJSON_AddBoolToObject(root, "long_icmp", ips_settings.long_icmp);
    cJSON_AddBoolToObject(root, "ip_option", ips_settings.ip_option);
    cJSON_AddBoolToObject(root, "icmp_fragment", ips_settings.icmp_fragment);
    cJSON_AddBoolToObject(root, "tcp_fin_fragment", ips_settings.tcp_fin_fragment);
    cJSON_AddBoolToObject(root, "land_attack", ips_settings.land_attack);
    cJSON_AddBoolToObject(root, "illegal_layer3", ips_settings.illegal_layer3);
    cJSON_AddBoolToObject(root, "illegal_layer4", ips_settings.illegal_layer4);
    cJSON_AddBoolToObject(root, "winnuke", ips_settings.winnuke);
    cJSON_AddBoolToObject(root, "ping_of_death", ips_settings.ping_of_death);
    cJSON_AddBoolToObject(root, "anti_source_route", ips_settings.anti_source_route);
    
    char *json_str = cJSON_Print(root);
    
    cJSON_Delete(root);
    
    return json_str;
}

bool update_ips_settings(const char* json_str) {
    printf("Received JSON: %s\n", json_str);
    cJSON *root = cJSON_Parse(json_str);
    
    if (root == NULL) {
        printf("Failed to parse JSON: %s\n", cJSON_GetErrorPtr());
        return false;
    }
    
    cJSON *value;
    value = cJSON_GetObjectItem(root, "fragment_attack");
    if (value) ips_settings.fragment_attack = cJSON_IsTrue(value);
    value = cJSON_GetObjectItem(root, "illegal_ip");
    if (value) ips_settings.illegal_ip = cJSON_IsTrue(value);
    value = cJSON_GetObjectItem(root, "tcp_syn_frag");
    if (value) ips_settings.tcp_syn_frag = cJSON_IsTrue(value);
    value = cJSON_GetObjectItem(root, "illegal_udp_check");
    if (value) ips_settings.illegal_udp_check = cJSON_IsTrue(value);
    value = cJSON_GetObjectItem(root, "illegal_tcp_flags");
    if (value) ips_settings.illegal_tcp_flags = cJSON_IsTrue(value);
    value = cJSON_GetObjectItem(root, "layer2_unknown");
    if (value) ips_settings.layer2_unknown = cJSON_IsTrue(value);
    value = cJSON_GetObjectItem(root, "layer3_unknown");
    if (value) ips_settings.layer3_unknown = cJSON_IsTrue(value);
    value = cJSON_GetObjectItem(root, "short_fragment");
    if (value) ips_settings.short_fragment = cJSON_IsTrue(value);
    value = cJSON_GetObjectItem(root, "long_icmp");
    if (value) ips_settings.long_icmp = cJSON_IsTrue(value);
    value = cJSON_GetObjectItem(root, "ip_option");
    if (value) ips_settings.ip_option = cJSON_IsTrue(value);
    value = cJSON_GetObjectItem(root, "icmp_fragment");
    if (value) ips_settings.icmp_fragment = cJSON_IsTrue(value);
    value = cJSON_GetObjectItem(root, "tcp_fin_fragment");
    if (value) ips_settings.tcp_fin_fragment = cJSON_IsTrue(value);
    value = cJSON_GetObjectItem(root, "land_attack");
    if (value) ips_settings.land_attack = cJSON_IsTrue(value);
    value = cJSON_GetObjectItem(root, "illegal_layer3");
    if (value) ips_settings.illegal_layer3 = cJSON_IsTrue(value);
    value = cJSON_GetObjectItem(root, "illegal_layer4");
    if (value) ips_settings.illegal_layer4 = cJSON_IsTrue(value);
    value = cJSON_GetObjectItem(root, "winnuke");
    if (value) ips_settings.winnuke = cJSON_IsTrue(value);
    value = cJSON_GetObjectItem(root, "ping_of_death");
    if (value) ips_settings.ping_of_death = cJSON_IsTrue(value);
    value = cJSON_GetObjectItem(root, "anti_source_route");
    if (value) ips_settings.anti_source_route = cJSON_IsTrue(value);
    
    cJSON_Delete(root);
    printf("Settings updated successfully. New settings:\n");
    char *current = get_ips_settings();
    printf("%s\n", current);
    free(current);
    return true;
} 
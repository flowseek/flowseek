<template>
  <div class="ips-config">
    <el-card class="ips-card">
      <template #header>
        <div class="card-header">
          <span>入侵防御配置</span>
          <el-button type="primary" @click="saveSettings">保存配置</el-button>
        </div>
      </template>

      <div class="settings-container">
        <el-row :gutter="20">
          <el-col :span="12" v-for="(item, key) in ipsSettings" :key="key">
            <div class="setting-item">
              <span class="setting-label">{{ item.label }}</span>
              <el-switch
                v-model="item.value"
                :active-value="true"
                :inactive-value="false"
                @change="(val) => handleChange(key, val)"
              />
            </div>
          </el-col>
        </el-row>
      </div>
    </el-card>
  </div>
</template>

<script>
import { reactive } from 'vue'
import { ElMessage } from 'element-plus'

export default {
  name: 'IPSConfig',
  setup() {
    const ipsSettings = reactive({
      fragment_attack: {
        label: '分片攻击',
        value: false
      },
      illegal_ip: {
        label: '非法IP',
        value: false
      },
      tcp_syn_frag: {
        label: 'TCP SYN分片',
        value: false
      },
      illegal_udp_check: {
        label: '非法UDP校验',
        value: false
      },
      illegal_tcp_flags: {
        label: '非法TCP标志位',
        value: false
      },
      layer2_unknown: {
        label: '二层不可知',
        value: false
      },
      layer3_unknown: {
        label: '三层不可知',
        value: false
      },
      short_fragment: {
        label: '过短分片',
        value: false
      },
      long_icmp: {
        label: '过长ICMP',
        value: false
      },
      ip_option: {
        label: 'IP OPTION',
        value: false
      },
      icmp_fragment: {
        label: 'ICMP分片',
        value: false
      },
      tcp_fin_fragment: {
        label: 'TCP FIN分片',
        value: false
      },
      land_attack: {
        label: 'Land_attack',
        value: false
      },
      illegal_layer3: {
        label: '非法3层',
        value: false
      },
      illegal_layer4: {
        label: '非法4层',
        value: false
      },
      winnuke: {
        label: 'winnuke',
        value: false
      },
      ping_of_death: {
        label: 'ping_of_death',
        value: false
      },
      anti_source_route: {
        label: '抗源路由',
        value: false
      }
    })

    // 获取配置
    const fetchSettings = async () => {
      try {
        const response = await fetch('/api/ips/settings')
        if (response.ok) {
          const data = await response.json()
          // 更新本地配置
          Object.keys(data).forEach(key => {
            if (ipsSettings[key]) {
              ipsSettings[key].value = data[key]
            }
          })
        }
      } catch (error) {
        ElMessage.error('获取配置失败')
      }
    }

    // 处理开关变化
    const handleChange = (key, value) => {
      console.log(`${key}: ${value}`)
    }

    // 保存所有配置
    const saveSettings = async () => {
      // 将配置转换为API需要的格式
      const settings = Object.keys(ipsSettings).reduce((acc, key) => {
        acc[key] = ipsSettings[key].value
        return acc
      }, {})

      try {
        const response = await fetch('/api/ips/settings', {
          method: 'PUT',
          headers: {
            'Content-Type': 'application/json'
          },
          body: JSON.stringify(settings)
        })

        if (response.ok) {
          ElMessage.success('保存成功')
          await fetchSettings()
        } else {
          throw new Error('保存失败')
        }
      } catch (error) {
        ElMessage.error(error.message || '保存失败')
      }
    }

    // 初始加载
    fetchSettings()

    return {
      ipsSettings,
      handleChange,
      saveSettings
    }
  }
}
</script>

<style scoped>
.ips-config {
  padding: 20px;
}
.ips-card {
  margin-bottom: 20px;
}
.card-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
}
.settings-container {
  padding: 20px 0;
}
.setting-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 10px;
  margin-bottom: 10px;
  background-color: #f8f9fa;
  border-radius: 4px;
}
.setting-label {
  font-size: 14px;
  color: #606266;
}
</style> 
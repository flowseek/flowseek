<template>
  <div class="home-page">
    <el-card class="product-intro">
      <div class="product-header">
        <h1>防火墙状态</h1>
      </div>
 
    <el-row :gutter="20">
      <!-- CPU状态 -->
        <el-card class="status-card">
          <template #header>
            <div class="card-header">
              <span>CPU使用率</span>
              <el-tag :type="getCpuStatusType">{{ cpuStatus.usage }}%</el-tag>
            </div>
          </template>
          <el-progress 
            :percentage="cpuStatus.usage" 
            :color="getCpuProgressColor"
          />
          <div class="detail-info">
            <div>温度: {{ cpuStatus.temperature }}°C</div>
            <div>频率: {{ cpuStatus.frequency }}MHz</div>
          </div>
        </el-card>
      
      <!-- 内存状态 -->
        <el-card class="status-card">
          <template #header>
            <div class="card-header">
              <span>内存使用率</span>
              <el-tag :type="getMemStatusType">{{ memStatus.usage }}%</el-tag>
            </div>
          </template>
          <el-progress 
            :percentage="memStatus.usage" 
            :color="getMemProgressColor"
          />
          <div class="detail-info">
            <div>总内存: {{ formatSize(memStatus.total) }}</div>
            <div>已使用: {{ formatSize(memStatus.used) }}</div>
          </div>
        </el-card>
      
    </el-row>

      <!-- 网络状态 -->
        <el-card class="status-card">
          <template #header>
            <div class="card-header">
              <span>网络状态</span>
            </div>
          </template>
          <el-table :data="networkStatus" style="width: 100%">
            <el-table-column prop="interface" label="接口" width="100" />
            <el-table-column prop="status" label="状态" width="100">
              <template #default="scope">
                <el-tag :type="scope.row.status === 'up' ? 'success' : 'danger'">
                  {{ scope.row.status === 'up' ? '在线' : '离线' }}
                </el-tag>
              </template>
            </el-table-column>
            <el-table-column prop="rx_speed" label="接收速率" />
            <el-table-column prop="tx_speed" label="发送速率" />
          </el-table>
        </el-card>

    </el-card>

  </div>
</template>

<script>
import { ref, onMounted, onUnmounted, computed } from 'vue'

export default {
  name: 'Home',
  setup() {
    const cpuStatus = ref({
      usage: 0,
      temperature: 0,
      frequency: 0
    })
    
    const memStatus = ref({
      usage: 0,
      total: 0,
      used: 0
    })
    
    const networkStatus = ref([])
    
    // 获取状态类型
    const getCpuStatusType = computed(() => {
      const usage = cpuStatus.value.usage
      if (usage < 60) return 'success'
      if (usage < 80) return 'warning'
      return 'danger'
    })
    
    const getMemStatusType = computed(() => {
      const usage = memStatus.value.usage
      if (usage < 70) return 'success'
      if (usage < 90) return 'warning'
      return 'danger'
    })
    
    // 获取进度条颜色
    const getCpuProgressColor = computed(() => {
      const usage = cpuStatus.value.usage
      if (usage < 60) return '#67C23A'
      if (usage < 80) return '#E6A23C'
      return '#F56C6C'
    })
    
    const getMemProgressColor = computed(() => {
      const usage = memStatus.value.usage
      if (usage < 70) return '#67C23A'
      if (usage < 90) return '#E6A23C'
      return '#F56C6C'
    })
    
    // 格式化大小
    const formatSize = (bytes) => {
      const units = ['B', 'KB', 'MB', 'GB', 'TB']
      let size = bytes
      let unitIndex = 0
      while (size >= 1024 && unitIndex < units.length - 1) {
        size /= 1024
        unitIndex++
      }
      return `${size.toFixed(2)} ${units[unitIndex]}`
    }
    
    // 更新状态
    const updateStatus = async () => {
      try {
        const response = await fetch('/api/system/status')
        if (response.ok) {
          const data = await response.json()
          cpuStatus.value = data.cpu
          memStatus.value = data.memory
          networkStatus.value = data.network
        }
      } catch (error) {
        console.error('获取系统状态失败:', error)
      }
    }
    
    let timer = null
    
    onMounted(() => {
      updateStatus()
      timer = setInterval(updateStatus, 5000) // 每5秒更新一次
    })
    
    onUnmounted(() => {
      if (timer) {
        clearInterval(timer)
      }
    })
    
    return {
      cpuStatus,
      memStatus,
      networkStatus,
      getCpuStatusType,
      getMemStatusType,
      getCpuProgressColor,
      getMemProgressColor,
      formatSize
    }
  }
}
</script>

<style scoped>
.home-page {
  padding: 20px;
}

.product-intro {
  margin-bottom: 20px;
}

.product-header {
  margin-bottom: 20px;
  text-align: center;
}

.product-header h1 {
  color: #303133;
  font-size: 24px;
  margin: 0;
}

.product-description {
  margin-bottom: 30px;
  line-height: 1.6;
  color: #606266;
}

.feature-section {
  margin-top: 30px;
}

.feature-section h2 {
  color: #303133;
  font-size: 20px;
  margin-bottom: 20px;
  padding-bottom: 10px;
  border-bottom: 2px solid #409EFF;
}

.feature-section h3 {
  color: #409EFF;
  font-size: 16px;
  margin: 20px 0 10px;
}

.feature-section ul {
  list-style: none;
  padding-left: 20px;
  margin: 0;
}

.feature-section li {
  position: relative;
  padding-left: 15px;
  margin-bottom: 8px;
  line-height: 1.6;
  color: #606266;
}

.feature-section li::before {
  content: "•";
  color: #409EFF;
  position: absolute;
  left: 0;
}

.status-card {
  margin-bottom: 20px;
}

.card-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.detail-info {
  margin-top: 15px;
  font-size: 14px;
  color: #606266;
}

.detail-info > div {
  margin-bottom: 5px;
}
</style> 

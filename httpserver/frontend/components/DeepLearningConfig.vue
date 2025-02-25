<template>
  <div class="deep-learning-config">
    <el-card class="config-card">
      <template #header>
        <div class="card-header">
          <span>深度学习配置</span>
        </div>
      </template>

      <div class="settings-container">
        <!-- 基础配置 -->
        <div class="section">
          <h3>基础配置</h3>
          <el-form :model="basicConfig" label-width="180px">
            <el-form-item label="启用深度学习">
              <el-switch v-model="basicConfig.enabled" />
            </el-form-item>
            <el-form-item label="FPGA加速">
              <el-switch v-model="basicConfig.fpgaEnabled" />
            </el-form-item>
            <el-form-item label="最大并发数">
              <el-input-number v-model="basicConfig.maxConcurrent" :min="1" :max="100" />
            </el-form-item>
          </el-form>
        </div>

        <!-- 检测配置 -->
        <div class="section">
          <h3>检测配置</h3>
          <el-form :model="detectionConfig" label-width="180px">
            <el-form-item label="恶意流量检测">
              <el-switch v-model="detectionConfig.maliciousTraffic" />
            </el-form-item>
            <el-form-item label="异常行为检测">
              <el-switch v-model="detectionConfig.abnormalBehavior" />
            </el-form-item>
            <el-form-item label="未知威胁检测">
              <el-switch v-model="detectionConfig.unknownThreats" />
            </el-form-item>
            <el-form-item label="检测阈值">
              <el-slider
                v-model="detectionConfig.threshold"
                :min="0"
                :max="100"
                :format-tooltip="formatThreshold"
              />
            </el-form-item>
          </el-form>
        </div>

        <!-- 模型配置 -->
        <div class="section">
          <h3>模型配置</h3>
          <el-form :model="modelConfig" label-width="180px">
            <el-form-item label="模型更新周期">
              <el-select v-model="modelConfig.updateCycle">
                <el-option label="每天" value="daily" />
                <el-option label="每周" value="weekly" />
                <el-option label="每月" value="monthly" />
              </el-select>
            </el-form-item>
            <el-form-item label="自动更新">
              <el-switch v-model="modelConfig.autoUpdate" />
            </el-form-item>
            <el-form-item label="当前模型版本">
              <el-input v-model="modelConfig.currentVersion" disabled />
            </el-form-item>
          </el-form>
        </div>

        <!-- 日志配置 -->
        <div class="section">
          <h3>日志配置</h3>
          <el-form :model="logConfig" label-width="180px">
            <el-form-item label="日志级别">
              <el-select v-model="logConfig.level">
                <el-option label="调试" value="debug" />
                <el-option label="信息" value="info" />
                <el-option label="警告" value="warning" />
                <el-option label="错误" value="error" />
              </el-select>
            </el-form-item>
            <el-form-item label="日志保存天数">
              <el-input-number v-model="logConfig.retentionDays" :min="1" :max="365" />
            </el-form-item>
          </el-form>
        </div>

        <!-- 保存按钮 -->
        <div class="save-button-container">
          <el-button type="primary" @click="saveSettings" size="large">
            保存配置
          </el-button>
        </div>
      </div>

      <el-divider>数据管理</el-divider>
      
      <div class="danger-zone">
        <h4 class="danger-title">
          <el-icon><Warning /></el-icon>
          危险区域
        </h4>
        <div class="danger-content">
          <div class="danger-action">
            <div class="action-desc">
              <h5>清除数据库</h5>
              <p>此操作将永久删除所有流量分析数据，请谨慎操作</p>
            </div>
            <el-button 
              type="danger" 
              @click="handleClearDatabase"
              :loading="clearLoading"
            >
              清除数据
            </el-button>
          </div>
        </div>
      </div>
    </el-card>
  </div>
</template>

<script>
import { reactive, ref } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'

export default {
  name: 'DeepLearningConfig',
  setup() {
    // 基础配置
    const basicConfig = reactive({
      enabled: true,
      fpgaEnabled: false,
      maxConcurrent: 10
    })

    // 检测配置
    const detectionConfig = reactive({
      maliciousTraffic: true,
      abnormalBehavior: true,
      unknownThreats: true,
      threshold: 80
    })

    // 模型配置
    const modelConfig = reactive({
      updateCycle: 'weekly',
      autoUpdate: true,
      currentVersion: 'v1.0.0'
    })

    // 日志配置
    const logConfig = reactive({
      level: 'info',
      retentionDays: 30
    })

    const clearLoading = ref(false)

    // 格式化阈值显示
    const formatThreshold = (val) => {
      return `${val}%`
    }

    // 获取配置
    const fetchSettings = async () => {
      try {
        const response = await fetch('/api/deep-learning/settings')
        const data = await response.json()
        
        basicConfig.enabled = data.basic.enabled
        basicConfig.fpgaEnabled = data.basic.fpgaEnabled
        basicConfig.maxConcurrent = data.basic.maxConcurrent
        detectionConfig.maliciousTraffic = data.detection.maliciousTraffic
        detectionConfig.abnormalBehavior = data.detection.abnormalBehavior
        detectionConfig.unknownThreats = data.detection.unknownThreats
        detectionConfig.threshold = data.detection.threshold
        modelConfig.updateCycle = data.model.updateCycle
        modelConfig.autoUpdate = data.model.autoUpdate
        modelConfig.currentVersion = data.model.currentVersion
        logConfig.level = data.log.level
        logConfig.retentionDays = data.log.retentionDays
      } catch (error) {
        ElMessage.error('获取配置失败')
      }
    }

    // 保存配置
    const saveSettings = async () => {
      try {
        const settings = {
          basic: basicConfig,
          detection: detectionConfig,
          model: modelConfig,
          log: logConfig
        }

        const response = await fetch('/api/deep-learning/settings', {
          method: 'PUT',
          headers: {
            'Content-Type': 'application/json'
          },
          body: JSON.stringify(settings)
        })

        if (response.ok) {
          ElMessage.success('保存成功')
        } else {
          throw new Error('保存失败')
        }
      } catch (error) {
        ElMessage.error(error.message || '保存失败')
      }
    }

    // 清除数据库
    const handleClearDatabase = async () => {
      try {
        // 显示确认对话框
        await ElMessageBox.confirm(
          '此操作将永久删除所有流量分析数据，是否继续?',
          '警告',
          {
            confirmButtonText: '确定',
            cancelButtonText: '取消',
            type: 'warning'
          }
        )
        
        clearLoading.value = true
        
        const response = await fetch('/api/deep-learning/clear-database', {
          method: 'POST'
        })
        
        if (!response.ok) {
          throw new Error('清除失败')
        }
        
        ElMessage.success('数据已清除')
      } catch (error) {
        if (error !== 'cancel') {
          ElMessage.error(error.message || '清除失败')
        }
      } finally {
        clearLoading.value = false
      }
    }

    // 初始加载
    fetchSettings()

    return {
      basicConfig,
      detectionConfig,
      modelConfig,
      logConfig,
      formatThreshold,
      saveSettings,
      handleClearDatabase,
      clearLoading
    }
  }
}
</script>

<style scoped>
.deep-learning-config {
  padding: 20px;
}

.config-card {
  margin-bottom: 20px;
}

.card-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.settings-container {
  padding: 20px 0;
  position: relative;
  min-height: 400px;
}

.save-button-container {
  position: absolute;
  bottom: 0;
  right: 0;
  padding: 20px 0;
}

.section {
  margin-bottom: 40px;
}

.section h3 {
  margin-bottom: 20px;
  padding-bottom: 10px;
  border-bottom: 1px solid #EBEEF5;
  color: #409EFF;
}

.el-form-item {
  margin-bottom: 20px;
}

.danger-zone {
  margin-top: 20px;
  border: 1px solid #fcd2d2;
  border-radius: 4px;
  padding: 16px;
}

.danger-title {
  margin: 0 0 16px;
  color: #f56c6c;
  font-size: 14px;
  font-weight: 600;
  display: flex;
  align-items: center;
  gap: 6px;
  
  .el-icon {
    font-size: 16px;
  }
}

.danger-content {
  padding: 8px 0;
}

.danger-action {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 12px 0;
  border-bottom: 1px solid #ebeef5;
  
  &:last-child {
    border-bottom: none;
  }
}

.action-desc {
  h5 {
    margin: 0 0 4px;
    font-size: 14px;
    color: #303133;
  }
  
  p {
    margin: 0;
    font-size: 12px;
    color: #909399;
  }
}

:deep(.el-divider__text) {
  font-size: 14px;
  color: #606266;
  background-color: #fff;
}
</style> 
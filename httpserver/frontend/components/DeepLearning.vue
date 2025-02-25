<template>
  <div class="deep-learning">

    <!-- NDPI 报文解析列表 -->
    <div class="ndpi-list" :class="{ 'menu-collapsed': isCollapsed }">
      <div class="refresh-bar">
        <span class="last-update">
          <i class="el-icon-time" />
          上次更新: {{ lastUpdateTime }}
        </span>
        <div class="refresh-actions">
          <el-switch
            v-model="autoRefresh"
            active-text="自动刷新"
            :loading="loading"
            size="small"
            class="custom-switch"
          />
          <el-button 
            type="primary" 
            @click="handleRefresh" 
            :loading="loading"
            :disabled="loading"
            size="small"
            class="refresh-button"
          >
            刷新
          </el-button>
        </div>
      </div>
      <div class="table-header">
        <div class="header-title">
          <i class="el-icon-data-line" />
          <h3>实时流量</h3>
        </div>
        <div class="table-stats">
          <div class="stat-item">
            <i class="el-icon-upload" />
            <span>总流量: {{ formatBytes(totalTraffic) }}</span>
          </div>
          <div class="stat-item">
            <i class="el-icon-connection" />
            <span>活跃连接: {{ total }}</span>
          </div>
        </div>
      </div>
      <div class="table-wrapper">
        <el-table 
          :data="currentPageData" 
          style="width: 100%"
          v-loading="loading"
          border
          stripe
          size="small"
          highlight-current-row
          fit
          :header-cell-style="{
            background: '#f5f7fa',
            color: '#606266',
            fontWeight: 500
          }"
        >
          <!-- Flow ID列 -->
          <el-table-column prop="flow_id" label="ID" min-width="50" align="center">
            <template #default="scope">
              <span class="flow-id">#{{ scope.row.flow_id }}</span>
            </template>
          </el-table-column>

          <!-- IP相关列 -->
          <el-table-column label="连接信息" min-width="260">
            <template #default="scope">
              <div class="connection-info">
                <div class="source">
                  <span class="ip">{{ scope.row.src_ip }}</span>
                  <span class="port">:{{ scope.row.src_port }}</span>
                </div>
                <div class="arrow">→</div>
                <div class="destination">
                  <span class="ip">{{ scope.row.dst_ip }}</span>
                  <span class="port">:{{ scope.row.dst_port }}</span>
                </div>
              </div>
            </template>
          </el-table-column>

          <!-- 其他列保持原样，但调整样式 -->
          <el-table-column prop="protocol" label="协议" min-width="70" align="center">
            <template #default="scope">
              <el-tag size="small" :type="getProtocolTagType(scope.row.protocol)">
                {{ scope.row.protocol }}
              </el-tag>
            </template>
          </el-table-column>

          <el-table-column prop="category" label="类别" min-width="70" align="center">
            <template #default="scope">
              <el-tag size="small" type="info" effect="plain">
                {{ scope.row.category }}
              </el-tag>
            </template>
          </el-table-column>

          <el-table-column prop="hostname" label="主机名" min-width="100" show-overflow-tooltip />
          <el-table-column label="流量统计" min-width="120">
            <template #default="scope">
              <el-tooltip placement="top">
                <template #content>
                  <div>上行: {{ scope.row.up_packets_formatted }}包 / {{ scope.row.up_bytes_formatted }}字节</div>
                  <div>下行: {{ scope.row.down_packets_formatted }}包 / {{ scope.row.down_bytes_formatted }}字节</div>
                </template>
                <div class="traffic-stats">
                  <div class="traffic-row">↑{{ formatBytes(scope.row.up_bytes) }}</div>
                  <div class="traffic-row">↓{{ formatBytes(scope.row.down_bytes) }}</div>
                </div>
              </el-tooltip>
            </template>
          </el-table-column>

          <el-table-column prop="risks" label="风险" min-width="80" show-overflow-tooltip>
            <template #default="scope">
              <el-tag 
                v-if="scope.row.risks !== '-'" 
                size="small" 
                type="danger" 
                effect="dark"
              >
                {{ scope.row.risks }}
              </el-tag>
              <span v-else>-</span>
            </template>
          </el-table-column>

          <!-- 添加类别列 -->
          <el-table-column prop="class" label="深度识别" min-width="80" align="center">
            <template #default="scope">
              <el-tag size="small" :type="getClassTagType(scope.row.class)">
                {{ scope.row.class }}
              </el-tag>
            </template>
          </el-table-column>

          <!-- 添加概率列 -->
          <el-table-column prop="probability" label="置信度" min-width="80" align="center">
            <template #default="scope">
              <span class="probability-text">
                {{ (scope.row.probability * 100).toFixed(1) }}%
              </span>
            </template>
          </el-table-column>

          <el-table-column label="时间" min-width="110">
            <template #default="scope">
              <el-tooltip placement="top">
                <template #content>
                  <div>首次: {{ formatTime(scope.row.first_seen) }}</div>
                  <div>最后: {{ formatTime(scope.row.last_seen) }}</div>
                </template>
                <div>{{ formatTime(scope.row.last_seen) }}</div>
              </el-tooltip>
            </template>
          </el-table-column>
        </el-table>
      </div>
      
      <!-- 分页器 -->
      <div class="pagination-container">
        <el-pagination
          v-model:current-page="currentPage"
          v-model:page-size="pageSize"
          :page-sizes="[10, 20, 30, 50]"
          :total="total"
          layout="sizes, prev, pager, next, jumper, total"
          background
          :small="true"
          @size-change="handleSizeChange"
          @current-change="handleCurrentChange"
        />
      </div>
    </div>
  </div>
</template>

<script>
import { reactive, ref, onMounted, computed, watch, onUnmounted } from 'vue'
import { ElMessage } from 'element-plus'

export default {
  name: 'DeepLearning',
  setup() {
    const ndpiList = ref([])
    const currentPageData = ref([])
    const loading = ref(false)
    const lastUpdateTime = ref('-')
    const currentPage = ref(1)
    const pageSize = ref(10)
    const total = ref(0)

    // 添加自动刷新状态
    const autoRefresh = ref(false)
    let refreshTimer = null

    // 添加导航折叠状态
    const isCollapsed = ref(false)

    // 处理每页显示数量变化
    const handleSizeChange = (val) => {
      pageSize.value = val
      currentPage.value = 1  // 重置到第一页
      fetchNdpiData()  // 重新获取数据
    }

    // 处理页码变化
    const handleCurrentChange = (val) => {
      currentPage.value = val
      fetchNdpiData()  // 重新获取数据
    }

    // 格式化阈值显示
    const formatThreshold = (val) => {
      return `${val}%`
    }

    // 格式化数字
    const formatNumber = (num) => {
      return num ? num.toLocaleString() : '0'
    }

    // 格式化时间显示
    const formatTime = (timeStr) => {
      if (!timeStr) return '';
      // 已经是格式化好的字符串，直接返回
      if (typeof timeStr === 'string' && timeStr.includes('.')) {
        return timeStr;
      }
      return '';
    }

    // 添加字节格式化函数
    const formatBytes = (bytes) => {
      if (!bytes) return '0 B'
      const k = 1024
      const sizes = ['B', 'KB', 'MB', 'GB', 'TB']
      const i = Math.floor(Math.log(bytes) / Math.log(k))
      return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i]
    }

    // 计算总流量
    const totalTraffic = computed(() => {
      return currentPageData.value.reduce((sum, item) => {
        const upBytes = typeof item.up_bytes === 'string' ? 
          parseInt(item.up_bytes.replace(/,/g, '')) : item.up_bytes || 0
        const downBytes = typeof item.down_bytes === 'string' ? 
          parseInt(item.down_bytes.replace(/,/g, '')) : item.down_bytes || 0
        return sum + upBytes + downBytes
      }, 0)
    })

    // 获取 NDPI 数据
    const fetchNdpiData = async () => {
      loading.value = true
      try {
        const response = await fetch(`/api/ndpi/stats?page=${currentPage.value}&pageSize=${pageSize.value}`)
        if (response.ok) {
          const data = await response.json()
          total.value = data.total
          currentPageData.value = data.protocols.map(p => {
            // 确保数值类型正确
            const up_bytes = parseInt(p.up_bytes) || 0
            const down_bytes = parseInt(p.down_bytes) || 0
            const up_packets = parseInt(p.up_packets) || 0
            const down_packets = parseInt(p.down_packets) || 0
            
            return {
              ...p,
              // 保存原始数值和格式化后的显示值
              up_packets: up_packets,
              up_bytes: up_bytes,
              down_packets: down_packets,
              down_bytes: down_bytes,
              up_packets_formatted: formatNumber(up_packets),
              up_bytes_formatted: formatNumber(up_bytes),
              down_packets_formatted: formatNumber(down_packets),
              down_bytes_formatted: formatNumber(down_bytes),
              // 确保 risks 字段有值
              risks: p.risks || '-',
              // 使用后端返回的class字符串值
              class: p.class || 'unknown',
              // 确保probability是数值且在0-1之间
              probability: Math.min(Math.max(parseFloat(p.probability) || 0.0, 0.0), 1.0)
            }
          })
          lastUpdateTime.value = formatTime(Date.now() / 1000)
        }
      } catch (error) {
        console.error('获取NDPI数据失败:', error)
        ElMessage.error('获取数据失败')
      } finally {
        loading.value = false
      }
    }

    // 处理刷新
    const handleRefresh = () => {
      if (!loading.value) {
        fetchNdpiData()
      }
    }

    // 监听自动刷新开关
    watch(autoRefresh, (newVal) => {
      if (newVal) {
        // 开启自动刷新，每5秒刷新一次
        refreshTimer = setInterval(handleRefresh, 5000)
      } else {
        // 关闭自动刷新
        if (refreshTimer) {
          clearInterval(refreshTimer)
          refreshTimer = null
        }
      }
    })

    // 添加协议标签类型判断函数
    const getProtocolTagType = (protocol) => {
      const types = {
        'HTTP': 'success',
        'HTTPS': 'success',
        'DNS': 'warning',
        'SSH': 'info',
        'FTP': 'warning',
        'SMTP': 'primary',
        'Unknown': 'info'
      }
      return types[protocol] || 'info'
    }

    // 获取类别标签类型
    const getClassTagType = (cls) => {
      const types = {
        'normal': 'success',
        'malicious': 'danger', 
        'suspicious': 'warning',
        'unknown': 'info'
      }
      return types[cls] || 'info'
    }

    // 获取概率状态
    const getProbabilityStatus = (probability) => {
      if (probability > 0.8) return 'exception'
      if (probability > 0.5) return 'warning'
      return 'success'
    }

    // 监听导航折叠状态
    const updateMenuState = () => {
      isCollapsed.value = document.body.classList.contains('menu-collapsed')
    }

    onMounted(() => {
      updateMenuState()
      // 创建 MutationObserver 监听 body 类名变化
      const observer = new MutationObserver((mutations) => {
        mutations.forEach((mutation) => {
          if (mutation.attributeName === 'class') {
            updateMenuState()
          }
        })
      })
      
      observer.observe(document.body, {
        attributes: true,
        attributeFilter: ['class']
      })
    })

    // 初始加载
    fetchNdpiData()

      return {
        ndpiList,
        currentPage,
        pageSize,
        currentPageData,
        total,
        handleSizeChange,
        handleCurrentChange,
        loading,
        lastUpdateTime,
        handleRefresh,
        formatTime,
        formatNumber,
        formatBytes,
        getProtocolTagType,
        getClassTagType,  // 添加这个方法
        getProbabilityStatus,  // 添加这个方法
        totalTraffic,
        autoRefresh,
        isCollapsed,
      }
  }
}
</script>

<style scoped>
.deep-learning {
  padding: 8px 20px;
  width: 100%;
  box-sizing: border-box;
  background-color: #f5f7fa;
  min-height: calc(100vh - 60px);
  display: flex;
  flex-direction: column;
  align-items: center;
  max-width: calc(100vw - 300px);
  margin-left: auto;
  margin-right: auto;
}

.ndpi-list {
  margin-top: 8px;
  width: 100%;
  transition: all 0.3s;
  padding: 0 16px;
  box-sizing: border-box;
  background: #fff;
  border-radius: 8px;
  box-shadow: 0 2px 12px rgba(0, 0, 0, 0.1);
  max-width: calc(100vw - 300px);
  margin-left: auto;
  margin-right: auto;
}

.ndpi-list h3 {
  margin-bottom: 12px;
  color: #333;
  font-weight: 500;
}

.refresh-bar {
  background: #fff;
  padding: 12px;
  border-radius: 4px;
  margin-bottom: 12px;
  display: flex;
  justify-content: space-between;
  align-items: center;
  width: 100%;
  box-sizing: border-box;
  border-bottom: 1px solid #ebeef5;
  transition: all 0.3s;
  
  &:hover {
    background-color: #f5f7fa;
  }
}

.refresh-actions {
  display: flex;
  align-items: center;
  gap: 16px;
}

.last-update {
  display: flex;
  align-items: center;
  gap: 6px;
  font-size: 13px;
  color: #606266;
  
  i {
    font-size: 16px;
    color: #409EFF;
  }
}

.pagination-container {
  margin-top: 20px;
  display: flex;
  justify-content: flex-end;
  width: 100%;
  box-sizing: border-box;
  padding: 0 4px;
}

.traffic-stats {
  font-size: 13px;
  line-height: 1.4;
  text-align: right;
  padding-right: 4px;
  color: #333;
}

.traffic-row {
  white-space: nowrap;
  overflow: hidden;
  text-overflow: ellipsis;
  padding: 1px 0;
}

/* 添加表格相关样式 */
:deep(.el-table) {
  font-size: 12px;
  width: 100% !important;
  table-layout: fixed;
  border: none;
}

:deep(.el-table td) {
  padding: 2px 8px;
  font-size: 13px;
  color: #333;
  height: 32px;
}

:deep(.el-table th) {
  font-size: 13px;
  font-weight: 600;
  color: #606266;
  padding: 8px;
  height: 36px;
}

:deep(.el-tooltip__trigger) {
  display: inline-block;
  width: 100%;
}

/* 确保tooltip内容的样式 */
:deep(.el-tooltip__content) {
  font-size: 12px;
  line-height: 1.4;
}

.table-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 16px;
  width: 100%;
  box-sizing: border-box;
  padding: 16px;
  background-color: #fff;
  border-radius: 4px;
  box-shadow: 0 1px 4px rgba(0, 0, 0, 0.05);
  transition: all 0.3s;
  
  &:hover {
    box-shadow: 0 2px 8px rgba(0, 0, 0, 0.08);
  }
}

.header-title {
  display: flex;
  align-items: center;
  gap: 8px;

  i {
    font-size: 20px;
    color: #409EFF;
  }

  h3 {
    margin: 0;
    font-size: 16px;
    font-weight: 600;
    color: #303133;
  }
}

.table-stats {
  display: flex;
  gap: 20px;
  color: #606266;
  font-size: 14px;
  align-items: center;
}

.stat-item {
  display: flex;
  align-items: center;
  gap: 6px;
  padding: 4px 12px;
  background-color: #f5f7fa;
  border-radius: 4px;
  transition: all 0.3s;
  
  &:hover {
    background-color: #ecf5ff;
    transform: translateY(-1px);
  }

  i {
    font-size: 16px;
    color: #409EFF;
  }

  span {
    font-weight: 500;
    color: #303133;
  }
}

.connection-info {
  display: flex;
  align-items: center;
  gap: 2px;
  font-family: monospace;
  font-size: 13px;
}

.source, .destination {
  display: flex;
  align-items: center;
}

.ip {
  color: #409EFF;
  font-weight: 600;
  font-size: 13px;
}

.port {
  color: #909399;
  margin-left: 2px;
  font-size: 13px;
}

.arrow {
  color: #909399;
  margin: 0 2px;
}

.flow-id {
  color: #909399;
  font-family: monospace;
}

:deep(.el-table__row) {
  transition: all 0.3s;
  height: 32px;
}

:deep(.el-table__row:hover) {
  transform: translateY(-1px);
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.08);
  z-index: 1;
}

/* 表格容器样式 */
.table-wrapper {
  width: 100%;
  overflow-x: auto;
  margin-bottom: 16px;
  position: relative;
  transition: all 0.3s;
  background-color: #fff;
  border-radius: 4px;
  padding: 1px;
  box-shadow: 0 1px 4px rgba(0, 0, 0, 0.05);
  
  &:hover {
    background-color: #fafafa;
    box-shadow: 0 2px 8px rgba(0, 0, 0, 0.08);
  }
}

/* 自定义滚动条样式 */
.table-wrapper::-webkit-scrollbar {
  height: 8px;
  width: 8px;
}

.table-wrapper::-webkit-scrollbar-thumb {
  background-color: #dcdfe6;
  border-radius: 4px;
}

.table-wrapper::-webkit-scrollbar-track {
  background-color: #f5f7fa;
}

/* 确保表格有最小宽度且自适应 */
:deep(.el-table) {
  width: 100% !important;
  min-width: min(860px, 100%);
  table-layout: fixed;
}

/* 添加列宽度控制 */
:deep(.el-table__header) {
  table-layout: fixed;
}

:deep(.el-table__body) {
  table-layout: fixed;
}

/* 优化表格内容溢出处理 */
:deep(.el-table .cell) {
  white-space: nowrap;
  overflow: hidden;
  text-overflow: ellipsis;
}

/* 调整标签大小 */
:deep(.el-tag--small) {
  font-size: 12px;
  padding: 0 4px;
  height: 20px;
  line-height: 18px;
}

/* 概率显示样式 */
.probability-text {
  margin-left: 8px;
  font-size: 12px;
  color: #606266;
}

:deep(.el-progress) {
  display: inline-block;
  width: 60px;
  vertical-align: middle;
}

:deep(.el-progress-bar) {
  padding-right: 0;
  margin-right: 0;
}

/* 优化表头样式 */
:deep(.el-table__header-row) {
  height: 36px;
}

/* 优化风险标签样式 */
:deep(.el-tag--danger) {
  font-weight: 600;
}

/* 导航折叠时的样式 */
.ndpi-list.menu-collapsed {
  max-width: calc(100vw - 104px);
  margin-left: 64px;
  margin-right: auto;
}

/* 响应式布局调整 */
@media screen and (max-width: 1400px) {
  .ndpi-list {
    max-width: calc(100vw - 260px);
  }
  .ndpi-list.menu-collapsed {
    max-width: calc(100vw - 84px);
  }
}

@media screen and (max-width: 1200px) {
  .ndpi-list {
    max-width: calc(100vw - 260px);
    padding: 0 12px;
  }
  .ndpi-list.menu-collapsed {
    max-width: calc(100vw - 84px);
  }
}

@media screen and (max-width: 992px) {
  .ndpi-list {
    padding: 0 8px;
    max-width: calc(100vw - 260px);
  }
  :deep(.el-table) {
    min-width: min(700px, 100%);
  }
  .ndpi-list.menu-collapsed {
    max-width: calc(100vw - 84px);
  }
}

/* 优化背景样式 */
:deep(.is-background) {
  .btn-prev,
  .btn-next,
  .el-pager li {
    margin: 0 2px;
    background-color: #f4f4f5;
    
    &:not(:disabled):hover {
      color: #409EFF;
      background-color: #ecf5ff;
      transition: all 0.2s;
    }
    
    &:disabled {
      background-color: #f4f4f5;
      color: #c0c4cc;
      cursor: not-allowed;
    }
  }

  .el-pager li:not(.disabled).active {
    background-color: #409EFF;
    color: #fff;
    font-weight: 500;
    
    &:hover {
      color: #fff;
      background-color: #409EFF;
    }
  }
}

/* 优化跳转页码输入框 */
:deep(.el-pagination__jump .el-input__wrapper) {
  box-shadow: none;
  padding: 0 4px;
  
  &:hover {
    box-shadow: none;
  }
  
  &.is-focus {
    box-shadow: none;
  }
}

:deep(.el-pagination__jump .el-input__inner) {
  height: 24px;
  line-height: 24px;
  width: 40px;
  padding: 0 2px;
  text-align: center;
}

:deep(.el-pagination .el-select .el-input) {
  width: 110px;
}

:deep(.el-pagination__sizes) {
  margin-right: 16px;
  height: 28px;
  line-height: 28px;
}

/* 优化每页显示个数选择器样式 */
:deep(.el-select-dropdown__item) {
  padding: 0 20px;
  font-size: 13px;
  height: 32px;
  line-height: 32px;
}

:deep(.el-input__wrapper) {
  padding: 0 8px;
  box-shadow: 0 0 0 1px #dcdfe6 inset;
  background-color: #ffffff;
  border-radius: 4px;
  transition: all 0.2s;
  
  &:hover {
    box-shadow: 0 0 0 1px #c0c4cc inset;
  }
  
  &.is-focus {
    box-shadow: 0 0 0 1px #409EFF inset;
  }
}

:deep(.el-input__inner) {
  font-size: 13px;
  height: 28px;
  line-height: 28px;
  color: #606266;
  
  &::placeholder {
    color: #c0c4cc;
  }
}

:deep(.el-select .el-select__tags) {
  padding: 0 4px;
  margin: 2px 0;
}

/* 优化选择器下拉面板样式 */
:deep(.el-select-dropdown) {
  border-radius: 4px;
  border: 1px solid #e4e7ed;
  box-shadow: 0 2px 12px 0 rgba(0,0,0,0.1);
}

:deep(.el-select-dropdown__list) {
  padding: 4px 0;
}

:deep(.el-select-dropdown__item.selected) {
  color: #409EFF;
  font-weight: normal;
}

:deep(.el-select-dropdown__item:hover) {
  background-color: #f5f7fa;
}

:deep(.el-pagination button) {
  min-width: 28px;
}

:deep(.el-pagination.is-background .el-pager li:not(.disabled).active) {
  background-color: #409EFF;
  color: #fff;
  font-weight: normal;
}

:deep(.el-pagination.is-background .el-pager li:not(.disabled):hover) {
  color: #409EFF;
}

:deep(.el-pagination__jump) {
  margin-left: 16px;
  margin-right: 16px;
}

:deep(.el-pagination__total) {
  font-weight: normal;
}

/* 优化表格边框和分割线 */
:deep(.el-table--border) {
  border: none;
}

:deep(.el-table__inner-wrapper::before) {
  display: none;
}

:deep(.el-table--border::after) {
  display: none;
}

/* 优化表格斑马纹 */
:deep(.el-table--striped .el-table__body tr.el-table__row--striped td) {
  background-color: #fafafa;
}

/* 优化表格内容垂直对齐 */
:deep(.el-table .cell) {
  display: flex;
  align-items: center;
  height: 32px;
  padding: 0 8px;
}

/* 优化表格hover效果 */
:deep(.el-table__body tr:hover > td) {
  background-color: #f0f9ff !important;
}

/* 优化刷新按钮 */
.refresh-button {
  min-width: 80px;
  font-weight: 500;
  transition: all 0.3s;
  
  &:hover {
    transform: scale(1.02);
  }
}

/* 优化自动刷新开关 */
:deep(.custom-switch) {
  .el-switch__core {
    background-color: #dcdfe6;
  }
  
  &.is-checked .el-switch__core {
    background-color: #409EFF;
  }
  
  .el-switch__label {
    color: #606266;
    font-size: 13px;
  }
}
</style> 

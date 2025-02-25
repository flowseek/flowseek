<template>
  <div class="acl-config">
    <!-- ACL规则列表 -->
    <el-card class="acl-card">
      <template #header>
        <div class="card-header">
          <span>访问控制列表配置</span>
          <el-button type="primary" @click="addRule">添加规则</el-button>
        </div>
      </template>

      <el-table :data="aclRules" stripe>
        <el-table-column prop="id" label="规则ID" width="80" />
        <el-table-column prop="action" label="动作" width="100">
          <template #default="scope">
            <el-tag :type="scope.row.action === 'permit' ? 'success' : 'danger'">
              {{ scope.row.action === 'permit' ? '允许' : '拒绝' }}
            </el-tag>
          </template>
        </el-table-column>
        <el-table-column prop="sourceIP" label="源IP" />
        <el-table-column prop="destIP" label="目标IP" />
        <el-table-column prop="protocol" label="协议" width="100">
          <template #default="scope">
            {{ protocolMap[scope.row.protocol] || scope.row.protocol }}
          </template>
        </el-table-column>
        <el-table-column prop="priority" label="优先级" width="100" />
        <el-table-column label="操作" width="150">
          <template #default="scope">
            <el-button size="small" @click="editRule(scope.row)">编辑</el-button>
            <el-button 
              size="small" 
              type="danger" 
              @click="deleteRule(scope.row.id)"
            >删除</el-button>
          </template>
        </el-table-column>
      </el-table>
    </el-card>

    <!-- 添加/编辑规则对话框 -->
    <el-dialog 
      :title="dialogTitle" 
      v-model="dialogVisible"
      width="500px"
    >
      <el-form :model="currentRule" :rules="rules" ref="ruleForm" label-width="100px">
        <el-form-item label="动作" prop="action">
          <el-select v-model="currentRule.action" placeholder="请选择动作">
            <el-option label="允许" value="permit" />
            <el-option label="拒绝" value="deny" />
          </el-select>
        </el-form-item>
        <el-form-item label="源IP" prop="sourceIP">
          <el-input v-model="currentRule.sourceIP" placeholder="例如: 192.168.1.0/24" />
        </el-form-item>
        <el-form-item label="目标IP" prop="destIP">
          <el-input v-model="currentRule.destIP" placeholder="例如: 10.0.0.0/8" />
        </el-form-item>
        <el-form-item label="协议" prop="protocol">
          <el-select v-model="currentRule.protocol" placeholder="请选择协议">
            <el-option label="TCP" value="tcp" />
            <el-option label="UDP" value="udp" />
            <el-option label="ICMP" value="icmp" />
            <el-option label="全部" value="any" />
          </el-select>
        </el-form-item>
        <el-form-item label="优先级" prop="priority">
          <el-input-number v-model="currentRule.priority" :min="1" :max="999" />
        </el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="dialogVisible = false">取消</el-button>
        <el-button type="primary" @click="saveRule">确定</el-button>
      </template>
    </el-dialog>
  </div>
</template>

<script>
import { ref, reactive } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'

export default {
  name: 'ACLConfig',
  setup() {
    const aclRules = ref([])
    const dialogVisible = ref(false)
    const dialogTitle = ref('')
    const ruleForm = ref(null)

    const protocolMap = {
      'tcp': 'TCP',
      'udp': 'UDP',
      'icmp': 'ICMP',
      'any': '全部'
    }

    const currentRule = reactive({
      id: null,
      action: 'permit',
      sourceIP: '',
      destIP: '',
      protocol: 'tcp',
      priority: 100
    })

    const rules = {
      action: [{ required: true, message: '请选择动作', trigger: 'change' }],
      sourceIP: [{ required: true, message: '请输入源IP', trigger: 'blur' }],
      destIP: [{ required: true, message: '请输入目标IP', trigger: 'blur' }],
      protocol: [{ required: true, message: '请选择协议', trigger: 'change' }],
      priority: [{ required: true, message: '请输入优先级', trigger: 'blur' }]
    }

    // 获取ACL规则列表
    const fetchRules = async () => {
      try {
        const response = await fetch('/api/acl/rules')
        const data = await response.json()
        aclRules.value = data.sort((a, b) => a.priority - b.priority)
      } catch (error) {
        ElMessage.error('获取规则列表失败')
      }
    }

    // 添加新规则
    const addRule = () => {
      dialogTitle.value = '添加规则'
      Object.assign(currentRule, {
        id: null,
        action: 'permit',
        sourceIP: '',
        destIP: '',
        protocol: 'tcp',
        priority: 100
      })
      dialogVisible.value = true
    }

    // 编辑规则
    const editRule = (rule) => {
      dialogTitle.value = '编辑规则'
      Object.assign(currentRule, rule)
      dialogVisible.value = true
    }

    // 删除规则
    const deleteRule = async (id) => {
      try {
        await ElMessageBox.confirm('确定要删除这条规则吗？', '提示', {
          type: 'warning'
        })
        
        const response = await fetch(`/api/acl/rules/${id}`, {
          method: 'DELETE'
        })
        
        if (response.ok) {
          ElMessage.success('删除成功')
          fetchRules()
        } else {
          throw new Error('删除失败')
        }
      } catch (error) {
        if (error !== 'cancel') {
          ElMessage.error('删除失败')
        }
      }
    }

    // 保存规则
    const saveRule = async () => {
      try {
        if (!ruleForm.value) return
        await ruleForm.value.validate()

        const method = currentRule.id ? 'PUT' : 'POST'
        const url = currentRule.id 
          ? `/api/acl/rules/${currentRule.id}`
          : '/api/acl/rules'

        const response = await fetch(url, {
          method,
          headers: {
            'Content-Type': 'application/json'
          },
          body: JSON.stringify(currentRule)
        })

        if (response.ok) {
          dialogVisible.value = false
          fetchRules()
          ElMessage.success(currentRule.id ? '更新成功' : '添加成功')
        } else {
          throw new Error(currentRule.id ? '更新失败' : '添加失败')
        }
      } catch (error) {
        if (error.message) {
          ElMessage.error(error.message)
        }
      }
    }

    // 初始加载
    fetchRules()

    return {
      aclRules,
      dialogVisible,
      dialogTitle,
      currentRule,
      rules,
      ruleForm,
      protocolMap,
      addRule,
      editRule,
      deleteRule,
      saveRule
    }
  }
}
</script>

<style scoped>
.acl-config {
  padding: 20px;
}
.acl-card {
  margin-bottom: 20px;
}
.card-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
}
</style> 

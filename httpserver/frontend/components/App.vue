<template>
  <div class="app-container">
    <Navigation />
    <div class="main-content">
      <component :is="currentView" />
    </div>
  </div>
</template>

<script>
import { ref, computed, onMounted } from 'vue'
import Navigation from './Navigation.vue'
import ACLConfig from './ACLConfig.vue'
import IPSConfig from './IPSConfig.vue'
import Home from './Home.vue'
import DeepLearning from './DeepLearning.vue'
import DeepLearningConfig from './DeepLearningConfig.vue'
import Product from './Product.vue'

export default {
  name: 'App',
  components: {
    Navigation,
    ACLConfig,
    IPSConfig,
    Home,
    DeepLearning,
    DeepLearningConfig,
    Product
  },
  setup() {
    const currentPath = ref('/')

    const updatePath = () => {
      // 从 URL 获取当前路径
      currentPath.value = window.location.pathname || '/'
    }

    // 监听 URL 变化
    window.addEventListener('popstate', updatePath)

    // 组件挂载时初始化路径
    onMounted(() => {
      updatePath()
    })

    const currentView = computed(() => {
      switch (currentPath.value) {
        case '/product':
          return 'Product'
        case '/acl':
          return 'ACLConfig'
        case '/ips':
          return 'IPSConfig'
        case '/deep-learning':
          return 'DeepLearning'
        case '/deep-learningConfig':
          return 'DeepLearningConfig'
        case '/':
          return 'Home'
        default:
          return 'Home'
      }
    })

    return {
      currentView
    }
  }
}
</script>

<style scoped>
.app-container {
  min-height: 100vh;
  display: flex;
}

.main-content {
  flex: 1;
  background-color: #f5f7fa;
  margin-left: 200px;
  padding: 20px;
  transition: margin-left 0.3s;
}

.nav-container.collapsed + .main-content {
  margin-left: 64px;
}
</style> 

<template>
  <div class="nav-container" :class="{ collapsed: isCollapsed }">
    <el-menu
      :default-active="activeIndex"
      class="nav-menu"
      :collapse="isCollapsed"
      background-color="#1e293b"
      text-color="#fff"
      active-text-color="#fff"
      @select="handleSelect"
    >
      <el-menu-item index="/">
        <el-icon><HomeFilled /></el-icon>
        <template #title>首页</template>
      </el-menu-item>
      <el-menu-item index="/product">
        <el-icon><Document /></el-icon>
        <template #title>产品说明</template>
      </el-menu-item>
      <el-sub-menu index="/deep-learning">
        <template #title>
          <el-icon><Monitor /></el-icon>
          <span>深度学习</span>
        </template>
        <el-menu-item index="/deep-learning">
          <el-icon><Warning /></el-icon>
          <template #title>流量分析</template>
        </el-menu-item>
        <el-menu-item index="/deep-learningConfig">
          <el-icon><Setting /></el-icon>
          <template #title>配置管理</template>
        </el-menu-item>
      </el-sub-menu>
      <el-menu-item index="/acl">
        <el-icon><Setting /></el-icon>
        <template #title>ACL配置</template>
      </el-menu-item>
      <el-menu-item index="/ips">
        <el-icon><Warning /></el-icon>
        <template #title>入侵防御</template>
      </el-menu-item>
      <el-menu-item index="/system">
        <el-icon><Tools /></el-icon>
        <template #title>系统设置</template>
      </el-menu-item>
    </el-menu>
    <div class="collapse-btn" @click="toggleCollapse">
      <el-icon>
        <component :is="isCollapsed ? 'Expand' : 'Fold'" />
      </el-icon>
    </div>
  </div>
</template>

<script>
import { ref } from 'vue'

export default {
  name: 'Navigation',
  setup() {
    const activeIndex = ref('/')
    const isCollapsed = ref(false)

    const handleSelect = (path) => {
      activeIndex.value = path
      // 更新 URL 但不刷新页面
      window.history.pushState({}, '', path)
      // 触发 popstate 事件
      window.dispatchEvent(new Event('popstate'))
    }

    const toggleCollapse = () => {
      isCollapsed.value = !isCollapsed.value
    }

    return {
      activeIndex,
      isCollapsed,
      handleSelect,
      toggleCollapse
    }
  }
}
</script>

<style scoped>
.nav-container {
  height: 100vh;
  transition: width 0.3s cubic-bezier(0.4, 0, 0.2, 1);
  width: 200px;
  background-color: #1e293b;
  border-right: 1px solid #2d3a4f;
  position: fixed;
  left: 0;
  top: 0;
  display: flex;
  flex-direction: column;
  overflow: hidden;
  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);
  z-index: 999;
}

.nav-container.collapsed {
  width: 64px;
  
  :deep(.el-menu-item), 
  :deep(.el-sub-menu__title) {
    padding: 0 !important;
    justify-content: center;
    
    .el-icon {
      margin: 0;
      font-size: 18px;
    }
    
    span {
      display: none;
    }
  }
  
  :deep(.el-menu--popup) {
    min-width: 180px !important;
    padding: 4px 0;
    background-color: #1e293b !important;
    border: 1px solid #2d3a4f;
    box-shadow: 0 2px 12px rgba(0, 0, 0, 0.2);
    left: 64px !important;
    transform: translateX(0) !important;
    
    .el-menu-item {
      height: 40px;
      line-height: 40px;
      padding: 0 16px !important;
      justify-content: flex-start;
      font-size: 13px;
      
      .el-icon {
        margin-right: 8px;
        font-size: 16px;
        color: #94a3b8;
        transition: color 0.3s;
      }
      
      span {
        display: inline-block;
        opacity: 1;
        transform: none;
      }
      
      &:hover {
        background-color: #0f172a !important;
        
        .el-icon {
          color: #fff;
        }
      }
      
      &.is-active {
        background-color: #0f172a !important;
        color: #409EFF !important;
        
        .el-icon {
          color: #409EFF !important;
        }
        
        &:hover {
          .el-icon {
            color: #409EFF !important;
          }
        }
      }
    }
  }
}

.nav-container.collapsed :deep(.el-menu-item span) {
  opacity: 0;
  transform: translateX(-20px);
}

.nav-menu {
  flex: 1;
  border-right: none;
  width: 200px !important;
  background-color: transparent !important;
  user-select: none;
}

:deep(.el-menu-item), 
:deep(.el-sub-menu__title) {
  height: 50px;
  line-height: 50px;
  padding: 0 20px !important;
  position: relative;
  transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
  border-left: 2px solid transparent;
  margin: 2px 0;
  font-size: 13px;
  
  .el-icon {
    margin-right: 8px;
    font-size: 18px;
    color: #94a3b8;
    transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
    transform-origin: center;
  }
  
  span {
    color: #94a3b8;
    transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
  }
  
  &:hover {
    background-color: #0f172a !important;
    border-left-color: #409EFF;
    padding-left: 24px !important;
    box-shadow: inset 0 0 12px rgba(0, 0, 0, 0.1);
    
    .el-icon {
      color: #fff;
      transform: translateX(2px);
    }
    
    span {
      color: #fff;
      transform: translateX(2px);
    }
  }
}

:deep(.el-menu-item.is-active),
:deep(.el-sub-menu.is-active > .el-sub-menu__title) {
  background-color: #0f172a !important;
  color: #409EFF !important;
  font-weight: bold;
  
  &::after {
    content: '';
    position: absolute;
    right: 0;
    top: 50%;
    transform: translateY(-50%);
    width: 3px;
    height: 24px;
    background-color: #409EFF;
    border-radius: 2px;
    box-shadow: 0 0 8px rgba(64, 158, 255, 0.5);
  }
  
  .el-icon {
    color: #409EFF !important;
    transform: translateX(2px);
  }
}

:deep(.el-sub-menu .el-menu-item) {
  min-width: unset;
  background-color: #1a2234 !important;
  padding-left: 48px !important;
  height: 46px;
  line-height: 46px;
  font-size: 13px;
  border-left: 2px solid transparent;
  margin: 2px 0;
  transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
  
  &:hover {
    background-color: #0f172a !important;
    border-left-color: #409EFF;
    padding-left: 52px !important;
    box-shadow: inset 0 0 12px rgba(0, 0, 0, 0.1);
  }
  
  &.is-active {
    background-color: #0f172a !important;
    color: #409EFF !important;
    border-left-color: #409EFF;
    padding-left: 52px !important;
    box-shadow: inset 0 0 12px rgba(0, 0, 0, 0.1);
  }
}

.collapse-btn {
  height: 56px;
  line-height: 56px;
  text-align: center;
  cursor: pointer;
  border-top: 1px solid #2d3a4f;
  color: #fff;
  transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
  user-select: none;
  
  .el-icon {
    transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
    transform-origin: center;
  }
  
  &:hover {
    background-color: #2d3a4f;
    box-shadow: inset 0 0 12px rgba(0, 0, 0, 0.1);
    
    .el-icon {
      transform: scale(1.1) rotate(180deg);
    }
  }
}

:deep(.el-sub-menu__title) {
  height: 50px;
  line-height: 50px;
  
  .el-icon {
    margin-right: 8px;
    color: #94a3b8;
    
    &:hover {
      color: #fff;
    }
  }
}

:deep(.el-sub-menu.is-active > .el-sub-menu__title) {
  color: #409EFF !important;
  
  .el-icon {
    color: #409EFF !important;
  }
}

/* 修复 popper 位置 */
:deep(.el-menu--vertical) {
  .el-menu {
    border: none;
  }

  .el-menu--popup {
    padding: 0 !important;
    margin: 0 !important;
  }
}

:deep(.el-popper.is-light) {
  border: none !important;
  padding: 0 !important;
  margin: 0 !important;
  left: 64px !important;
  transform: none !important;

  .el-popper__arrow {
    display: none !important;
  }
}
</style> 

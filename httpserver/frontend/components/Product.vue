<template>
  <div class="product-page">
    <el-card class="product-card">
      <template #header>
        <div class="card-header">
          <h1>智能安全网关</h1>
        </div>
      </template>

      <div class="product-content">
        <div class="product-intro">
          <h2>产品介绍</h2>
          <p>智能安全网关是阵源技术有限公司结合云计算、5G、物联网、IPv6、大数据及高性能计算的发展趋势，针对云计算数据中心、运营商CGN、大型企业及园区网出口等市场推出的新一代高性能多业务安全网关。</p>
        </div>

        <div class="product-description">
            <p>智能安全网关是阵源技术有限公司结合云计算、5G、物联网、IPv6、大数据及高性能计算的发展趋势，针对云计算数据中心、运营商CGN、大型企业及园区网出口等市场推出的新一代高性能多业务安全网关。</p>
        </div>

        <div class="feature-section">
            <h2>产品特点</h2>

            <h3>高性能的软硬件处理平台</h3>
            <ul>
                <li>采用控制、业务、数据相分离的全分布式架构，控制引擎、业务引擎及接口单元硬件分离，解耦合系统关键部件，提高系统可靠性</li>
                <li>独立的高性能控制引擎，实现系统统一配置管理和安全集群</li>
                <li>安全业务引擎采用最新多核高性能处理器，单板卡高速处理安全业务性能业界最高</li>
                <li>内置模块化软件系统，支持多进程的调度，进程间运行空间隔离</li>
            </ul>

            <h3>电信级设备高可靠性</h3>
            <ul>
                <li>采用H3C公司拥有自主知识产权的软、硬件平台</li>
                <li>支持状态1:1热备功能，支持Active/Active和Active/Passive两种工作模式</li>
                <li>整机采用分布式架构设计，所有模块均可实现硬件冗余配置</li>
            </ul>

            <h3>强大的安全防护功能</h3>
            <ul>
                <li>支持丰富的攻击防范功能，包括Land、Smurf、UDP Snork attack等多种攻击防范</li>
                <li>支持统一管理，主机+多业务引擎始终作为一个网元进行统一管理</li>
                <li>支持智能分流（IFF）部署多业务插卡</li>
                <li>支持安全ONE平台（SOP）</li>
                <li>支持安全区域管理</li>
            </ul>

            <h3>灵活可扩展的一体化深度安全</h3>
            <ul>
                <li>深入的WEB安全防护</li>
                <li>未知威胁检测</li>
                <li>终端识别，共享管理</li>
                <li>服务器异常外联检测</li>
                <li>高精度、高效率的入侵检测引擎</li>
            </ul>

            <h3>业界领先的IPv6</h3>
            <ul>
                <li>支持IPv6基础协议</li>
                <li>支持IPV6攻击防范</li>
                <li>支持IPv6各种过渡技术</li>
            </ul>

            <h3>专业的智能管理</h3>
            <ul>
                <li>自检运维，策略风险调优</li>
                <li>支持标准网管 SNMPv3</li>
                <li>支持基于接口及IP的报文捕获</li>
                <li>支持丢包统计功能</li>
                <li>支持网页诊断功能</li>
                <li>图形化界面，提供简单易用的Web管理</li>
            </ul>
        </div>
    </el-card>
      </div>
</template>

<script>
    export default {
        name: 'Product'
    }
</script>

<style scoped>
.product-page {
    padding: 20px;
}

.product-card {
    margin-bottom: 20px;
}

.card-header {
    text-align: center;
}

.card-header h1 {
    margin: 0;
    color: #303133;
    font-size: 24px;
}

.product-content {
    padding: 20px 0;
}

.product-intro {
    margin-bottom: 30px;
}

.product-intro h2 {
    color: #303133;
    font-size: 20px;
    margin-bottom: 15px;
    padding-bottom: 10px;
    border-bottom: 2px solid #409EFF;
}

.product-intro p {
    color: #606266;
    line-height: 1.6;
}

.feature-section h2 {
    color: #303133;
    font-size: 20px;
    margin-bottom: 20px;
    padding-bottom: 10px;
    border-bottom: 2px solid #409EFF;
}

.feature-section ul {
    list-style: none;
    padding-left: 20px;
    margin: 10px 0;
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

:deep(.el-collapse-item__header) {
    font-size: 16px;
    color: #409EFF;
}

:deep(.el-collapse-item__content) {
    padding-bottom: 15px;
}
</style> 

const { loadModule } = window['vue3-sfc-loader'];

const options = {
  moduleCache: {
    vue: Vue,
    'element-plus': ElementPlus
  },
  async getFile(url) {
    const res = await fetch(url);
    if (!res.ok) throw new Error(`Failed to load ${url}: ${res.statusText}`);
    return await res.text();
  },
  addStyle(textContent) {
    const style = Object.assign(document.createElement('style'), { textContent });
    const ref = document.head.getElementsByTagName('style')[0] || null;
    document.head.insertBefore(style, ref);
  },
};

// 加载根组件
loadModule('../components/App.vue', options)
  .then((component) => {
    const app = Vue.createApp(component);
    app.use(ElementPlus);
    // 注册图标组件
    app.component('HomeFilled', ElementPlusIconsVue.HomeFilled);
    app.component('Document', ElementPlusIconsVue.Document);
    app.component('Setting', ElementPlusIconsVue.Setting);
    app.component('Warning', ElementPlusIconsVue.Warning);
    app.component('Monitor', ElementPlusIconsVue.Monitor);
    app.component('Tools', ElementPlusIconsVue.Tools);
    app.component('Expand', ElementPlusIconsVue.Expand);
    app.component('Fold', ElementPlusIconsVue.Fold);
    app.mount('#app');
  })
  .catch((err) => {
    console.error('加载组件失败:', err);
  });

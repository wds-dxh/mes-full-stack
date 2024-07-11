import { createRouter, createWebHistory } from 'vue-router'
import Dashboard from '../components/Dashboard.vue'
import Login from '../components/Login.vue'

const routes = [
  { path: '/', component: Login },
  { path: '/Dashboard', component: Dashboard }
]

const router = createRouter({
  history: createWebHistory(),
  routes
})


export default router
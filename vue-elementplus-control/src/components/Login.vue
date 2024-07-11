
<template>
  <div class="login-container">
    <div class="login-box">
      <h1 class="login-title">重庆邮电大学智能工厂集成</h1>
      <img src="../assets/logo_cqupt.jfif" alt="Logo" class="login-logo">
      <el-form :model="loginForm" @submit.native.prevent="handleLogin" class="login-form">
        <el-form-item label="用户名" class="login-form-item">
          <el-input v-model="loginForm.username"></el-input>
        </el-form-item>
        <el-form-item label="密码" class="login-form-item">
          <el-input type="password" v-model="loginForm.password"></el-input>
        </el-form-item>
        <el-form-item>
          <el-button type="primary" @click="handleLogin" class="login-button">登录</el-button>
        </el-form-item>
      </el-form>
    </div>
  </div>
</template>

<script>


import axios from 'axios'
import { useRouter } from 'vue-router'
import { ref } from 'vue'


export default {
  setup() {
    const loginForm = ref({
      username: '',
      password: ''
    })
    const router = useRouter()

    const handleLogin = async () => {
      try {
        const response = await axios.post('http://localhost:5000/api/login', loginForm.value)
        if (response.data.success) {
          router.push(response.data.redirect_url)
        } else {
          alert('登录失败')
        }
      } catch (error) {
        console.error('登录错误', error)
      }
    }

    return {
      loginForm,
      handleLogin
    }
  }
}





</script>

<style scoped>
.login-container {
  display: flex;
  justify-content: center;
  align-items: center;
  height: 100vh;

}

.login-box {
  width: 400px;
  padding: 40px;
  background: white;
  box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
  border-radius: 8px;
  text-align: center;
}

.login-title {
  font-size: 24px;
  font-weight: bold;
  margin-bottom: 20px;
}

.login-logo {
  width: 400px;
  height: 200px;
  margin-bottom: 20px;
}

.login-form {
  text-align: left;
  width: 300px;
  height: 100px;
  margin: 0 auto;
}

.login-form-item {
  margin-bottom: 20px;
}
  /*width: 100%;*/
.login-button {

  width: 85px;
  height: 35px;
  margin: 0 auto;
}
</style>


<template>
  <el-container class="workshop-status">
    <el-header class="header">
      <h2 class="title">车间状态信息</h2>
    </el-header>
    <el-main class="main">
      <el-row :gutter="20" class="controls">
        <el-col :span="24">
          <el-form :inline="true" class="form-inline">
            <el-form-item label="查询记录数:" class="form-item">
              <el-input-number v-model="recordLimit" :min="1" :max="1000" size="large" />
            </el-form-item>
            <el-form-item class="form-item">
              <el-button type="primary" @click="fetchWorkshopData" size="large">查询</el-button>
            </el-form-item>
          </el-form>
        </el-col>
      </el-row>
      <el-row class="chart-controls">
        <el-col :span="24">
          <div class="radio-group">
            <el-radio-group v-model="selectedSeries">
              <el-radio label="温度">温度</el-radio>
              <el-radio label="湿度">湿度</el-radio>
              <el-radio label="照明度">照明度</el-radio>
            </el-radio-group>
          </div>
        </el-col>
      </el-row>
      <el-row>
        <el-col :span="24">
          <div ref="chart" class="chart"></div>
        </el-col>
      </el-row>
    </el-main>
  </el-container>
</template>

<script>
import * as echarts from 'echarts';
import axios from 'axios';
import { ElNotification } from 'element-plus';

export default {
  name: 'WorkshopStatus',
  data() {
    return {
      chart: null,
      workshopData: [],
      recordLimit: 100,  // 默认查询最近的 100 条记录
      selectedSeries: '温度', // 默认显示温度曲线
    };
  },
  mounted() {
    this.chart = echarts.init(this.$refs.chart);
    this.fetchWorkshopData();
  },
  watch: {
    selectedSeries() {
      this.renderChart();
    }
  },
  methods: {
    fetchWorkshopData() {
      axios.post('http://localhost:5000/workshop/status', { limit: this.recordLimit })
        .then(response => {
          this.workshopData = response.data;
          this.renderChart();
          this.showSuccessNotification();
        })
        .catch(error => {
          console.error("There was an error fetching the workshop data!", error);
        });
    },
    renderChart() {
      const temperatures = this.workshopData.map(item => item.temperature);
      const humidnesses = this.workshopData.map(item => item.humidness);
      const illuminations = this.workshopData.map(item => item.illumination);
      const timestamps = this.workshopData.map(item => item.timestamp);

      const series = [];

      if (this.selectedSeries === '温度') {
        series.push({
          name: '温度',
          type: 'line',
          data: temperatures,
          smooth: true,
          itemStyle: {
            color: '#FF6F61'
          }
        });
      }

      if (this.selectedSeries === '湿度') {
        series.push({
          name: '湿度',
          type: 'line',
          data: humidnesses,
          smooth: true,
          itemStyle: {
            color: '#6B8E23'
          }
        });
      }

      if (this.selectedSeries === '照明度') {
        series.push({
          name: '照明度',
          type: 'line',
          data: illuminations,
          smooth: true,
          itemStyle: {
            color: '#1E90FF'
          }
        });
      }

      const option = {
        title: {
          text: '车间状态信息',
          left: 'center'
        },
        tooltip: {
          trigger: 'axis'
        },
        legend: {
          data: [this.selectedSeries],
          top: '10%'
        },
        grid: {
          left: '3%',
          right: '4%',
          bottom: '3%',
          containLabel: true
        },
        xAxis: {
          type: 'category',
          data: timestamps
        },
        yAxis: {
          type: 'value'
        },
        series: series
      };

      this.chart.setOption(option);
    },
    showSuccessNotification() {
      ElNotification({
        title: '成功',
        message: '车间数据查询成功',
        type: 'success',
        duration: 3000
      });
    }
  }
};
</script>

<style scoped>
.workshop-status {
  padding: 20px;
  background-color: #f5f5f5;
  border-radius: 8px;
  box-shadow: 0 2px 12px rgba(0, 0, 0, 0.1);
}

.header {
  text-align: center;
  margin-bottom: 20px;
}

.title {
  color: #333;
  font-size: 24px;
}

.controls {
  margin-bottom: 20px;
}

.form-inline {
  display: flex;
  align-items: center;
}

.form-item {
  margin-right: 20px;
}

.chart-controls {
  margin-bottom: 20px;
  text-align: center;
  border-bottom: 1px solid #dcdcdc; /* 增加分界线 */
  padding-bottom: 10px;
}

.radio-group {
  display: flex;
  justify-content: center;
  margin-bottom: 10px;
}

.chart {
  width: 100%;
  height: 400px; /* 减小图表的高度 */
  background-color: white;
  border-radius: 8px;
  padding: 20px;
  box-shadow: 0 2px 12px rgba(0, 0, 0, 0.1);
}
</style>

<script setup>
import axios from "axios";
import { onMounted, ref } from 'vue';
import keyBDData from "../dataunpack/keyBDData.js"
import keys from "./keys.vue"
import chart from "../chart/chart.vue"

const press_count = ref(0);
var press_count_m = 0;
const press_count_today = ref(0);
var press_count_today_m = 0;

var data_ready = false;                 //打开页面时的获取全部数据操作是否完成, 完成才可以处理socket的数据
var all_data = [];
const keyboard_view = ref(null);        //显示键盘图形的控件
const keyboard_chart = ref(null);       //图表组件

function get_keyborad_data() {
    axios
        .get("/allData/keyBDData", { responseType: "arraybuffer" })
        .then(function (response) {
            if (response.data.byteLength > 0) {
                let data = new Uint8Array(response.data);
                all_data = keyBDData.binary_array_to_object(data);
            } else {
                //没有数据填入一个当前时间的空数据
                all_data.push(new keyBDData());
                all_data[0].init();
            }
            count();
            data_ready = true;
        })
        .catch(function (error) {
            console.log(error);
        });
}


/**
 * 遍历所有数计算键盘按下总数
 */
function count() {
    let today_date = new Date().setHours(0, 0, 0, 0);
    //先重置所有数据
    press_count_m = 0;
    press_count_today_m = 0;
    //最后一条数据不用于活跃时长的判定
    for (let i = 0; i < all_data.length - 1; i++) {
        press_count_m += all_data[i].press_count;
        let data_date = new Date(all_data[i].time).setHours(0, 0, 0, 0);
        if (today_date == data_date) {
            press_count_today_m += all_data[i].press_count;
        }
    }
    //加上最后一条数据的按下次数
    if (all_data.length > 0) {
        press_count.value = press_count_m + all_data[all_data.length - 1].press_count;
        if (is_today(all_data[all_data.length - 1].time))
            press_count_today.value = press_count_today_m + all_data[all_data.length - 1].press_count;
    }
    //默认统计前24小时的数据
    time_selected((new Date()).getTime() - 24 * 60 * 60 * 1000, undefined);
    // console.log(all_data);
}

/**
 * 判断一个时间是否在今天
 * 不在count()函数中使用, 应为count()里的for循环可能执行很多次, 用这个可能开销太大
 * @param {需要判断的时间} time 
 */
function is_today(time) {
    let today_date = new Date().setHours(0, 0, 0, 0);
    let input_date = new Date(time).setHours(0, 0, 0, 0);
    if (today_date == input_date) return true;
    return false;
}

function update_count(is_new_data = true, new_data = undefined) {
    //由于只有按下按键时触发后端更新数据, 新接收的数据可能不是今天的数据, 需要判断
    if (!is_new_data) {
        //新接收的数据和网页中最新存储的数据在同一分钟
        //更新每个按键按下次数
        keyboard_view.value.update(new_data, all_data[all_data.length - 1]);
        //更新图表
        keyboard_chart.value.update(new_data);
        //新接收的数据和最后一条数据在同一分钟, 用新接收的数据覆盖旧数据
        all_data[all_data.length - 1] = new_data;
    }
    else {
        //新接收的数据是新的一分钟的数据
        //保存数据
        all_data.push(new_data);
        //更新每个按键按下次数
        keyboard_view.value.update(new_data);
        //更新图表
        keyboard_chart.value.update(new_data, false);
        //更新除最后一分钟外的按下总次数
        press_count_m += all_data[all_data.length - 2].press_count;
        //由于是新一分钟的数据, 需要判断是否到了新的一天
        let is_today_flag = is_today(all_data[all_data.length - 2].time);
        if (!is_today_flag) {
            //存储的最后一条数据不是今天, 新一天到了
            //将今天相关的变量归零
            press_count_today_m = 0;
        }
        else {
            press_count_today_m += all_data[all_data.length - 2].press_count;
        }
    }
    let last_data = all_data[all_data.length - 1];
    press_count.value = press_count_m + last_data.press_count;
    if (is_today(last_data.time))
        press_count_today.value = press_count_today_m + last_data.press_count;
}

function time_selected(start_time, end_time) {
    keyboard_view.value.init(all_data, start_time, end_time);
    keyboard_chart.value.init(all_data, start_time, end_time);
}

onMounted(() => {
    get_keyborad_data();
    //建立websocket实时获取数据
    let socket = new WebSocket("ws://" + window.location.host + "/ws/kdata");
    socket.binaryType = 'arraybuffer';
    socket.addEventListener('message', (ev) => {
        if (!data_ready) return;
        let recv_k_data = new keyBDData();
        recv_k_data.load_data(new Uint8Array(ev.data));
        //当前没有数据
        if (all_data.length == 0) {
            all_data.push(recv_k_data);
            count();
        }
        else {
            let last_data = all_data[all_data.length - 1];
            if (last_data.time.getTime() == recv_k_data.time.getTime()) {
                update_count(false, recv_k_data);
            }
            else {
                //最新的数据时间与本次发送的数据的时间不同, 这是新的一分钟, 插入队列
                update_count(true, recv_k_data);
            }
        }
    })
})

defineExpose({
    time_selected
})
</script>
<template>
    <div class="text_container">
        <span class="text">
            总敲击次数
            <span class="num">{{ press_count }}</span>
            次
        </span>
        <span class="text">
            今日敲击次数
            <span class="num">{{ press_count_today }}</span>
            次
        </span>
    </div>
    <div class="k_container">
        <keys ref="keyboard_view" />
    </div>
    <div class="keyBD_chart">
        <chart ref="keyboard_chart" />
    </div>
</template>
<style scoped>
.k_container {
    display: flex;
    width: 100%;
    margin-bottom: 60px;
    justify-content: center;
    align-items: center;
}

.text_container {
    display: flex;
    width: 100%;
    justify-content: center;
    padding-top: 70px;
}

.text_container>div {
    margin: 5px 0;
}

span.text {
    font-weight: 600;
    font-size: 13px;
    padding: 0 10px;
}

span.num {
    font-weight: 600;
    font-size: 13px;
    padding: 0 5px;
    color: var(--vt-c-text-dark-1);
}

.keyBD_chart {
    height: 300px;
    width: 70%;
    margin: 0 auto;
}
</style>
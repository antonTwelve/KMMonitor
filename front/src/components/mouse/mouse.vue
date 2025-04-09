<script setup>
import axios from "axios";
import { onMounted, ref } from 'vue';
import mouseData from "../dataunpack/mouseData.js";
import { count_mouse_data, distance_format } from "./util.js";
import dataPanel from "./dataPanel.vue";


const count_today = ref({
    data: new count_mouse_data(),
    move_distance_str: "0"
});

//除去最后一条数据的统计数据
var count_today_m = new count_mouse_data();

const count_all = ref({
    data: new count_mouse_data(),
    move_distance_str: "0"
});

const data_panel = ref(null);

//除去最后一条数据的今日统计数据
var count_all_m = new count_mouse_data();
var data_ready = false;
var all_data = [];

function get_mouse_data() {
    axios
        .get("/allData/mouseData", { responseType: "arraybuffer" })
        .then(function (response) {
            if (response.data.byteLength > 0) {
                let data = new Uint8Array(response.data);
                all_data = mouseData.binary_array_to_object(data);
            } else {
                //没有数据填入一个当前时间的空数据
                all_data.push(new mouseData());
                all_data[0].init();
            }
            count();
            data_ready = true;
            // console.log(all_data);
        })
        .catch(function (error) {
            console.log(error);
        });
}

function format_distance(tar) {
    tar.move_distance_str = distance_format(tar.data.move_distance);
}

function count() {
    let today_date = new Date().setHours(0, 0, 0, 0);
    count_all_m.clear();
    count_today_m.clear();
    //统计除最后一条数据以外的数据
    for (let i = 0; i < all_data.length - 1; i++) {
        count_mouse_data.plus(count_all_m, count_all_m, all_data[i]);
        let data_date = new Date(all_data[i].time).setHours(0, 0, 0, 0);
        if (today_date == data_date) {
            count_mouse_data.plus(count_today_m, count_today_m, all_data[i]);
        }
    }
    //最后一条数据加上之间的数据再显示
    if (all_data.length > 0) {
        let i = all_data.length - 1;
        count_mouse_data.plus(count_all.value.data, count_all_m, all_data[i]);
        let data_date = new Date(all_data[i].time).setHours(0, 0, 0, 0);
        if (today_date == data_date) {
            count_mouse_data.plus(count_today.value.data, count_today_m, all_data[i]);
        }
    }
    format_distance(count_all.value);
    format_distance(count_today.value);
    //默认统计前24小时的数据
    time_selected((new Date()).getTime() - 24 * 60 * 60 * 1000, undefined);
}

function time_selected(start_time, end_time) {
    data_panel.value.init(all_data, start_time, end_time);
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
    //XXX:不合理, 新数据到来时不应该把new_data设置为undefined
    if (!is_new_data) {
        //新接收的数据和网页中最新存储的数据在同一分钟
        data_panel.value.update(new_data, all_data[all_data.length - 1]);
        //新接收的数据和最后一条数据在同一分钟, 用新接收的数据覆盖旧数据
        all_data[all_data.length - 1] = new_data;
    }
    else {
        //新接收的数据和是新的一分钟的数据
        //之前最后一分钟数据已经不是最后一分钟了, 需要更新最后一分钟数据
        count_mouse_data.plus(count_all_m, count_all_m, all_data[all_data.length - 2]);
        //由于是新一分钟的数据, 需要判断是否到了新的一天
        if (!is_today(all_data[all_data.length - 2].time)) {
            //存储的最后一条数据不是今天, 新一天到了
            //将今天相关的变量归零
            count_today_m = new count_mouse_data();
            count_today.value.data = new count_mouse_data();
            if (is_today(all_data[all_data.length - 1].time))
                count_mouse_data.plus(count_today_m, count_today_m, all_data[all_data.length - 1]);
        }
        else {
            count_mouse_data.plus(count_today_m, count_today_m, all_data[all_data.length - 2]);
        }
        data_panel.value.update(all_data[all_data.length - 1]);
    }
    count_mouse_data.plus(count_all.value.data, count_all_m, all_data[all_data.length - 1]);
    format_distance(count_all.value);
    if (is_today(all_data[all_data.length - 1].time)) {
        count_mouse_data.plus(count_today.value.data, count_today_m, all_data[all_data.length - 1]);
        format_distance(count_today.value);
    }
}

onMounted(() => {
    get_mouse_data();
    //建立websocket实时获取数据
    let socket = new WebSocket("ws://" + window.location.host + "/ws/mdata");
    socket.binaryType = 'arraybuffer';
    socket.addEventListener('message', (ev) => {
        if (!data_ready) return;
        let recv_k_data = new mouseData();
        recv_k_data.load_data(new Uint8Array(ev.data));
        if (all_data.length === 0) {
            all_data.push(recv_k_data);
            count();
        }
        else {
            if (all_data[all_data.length - 1].time.getTime() == recv_k_data.time.getTime()) {
                update_count(false, recv_k_data);
            }
            else {
                //最新的数据时间与本次发送的数据的时间不同, 这是新的一分钟, 插入队列
                all_data.push(recv_k_data);
                update_count();
            }
        }
    })
})

defineExpose({
    time_selected
})
</script>
<template>
    <div class="m_container">
        <div class="selected_data_wrapper">
            <dataPanel ref="data_panel" />
        </div>
        <div class="text_container">
            <div class="data_today">
                <h2>今日</h2>
                <div>
                    <span class="text">
                        鼠标左键<span class="num">{{ count_today.data.left_count }}</span>
                    </span>
                </div>
                <div>
                    <span class="text">
                        鼠标右键<span class="num">{{ count_today.data.right_count }}</span>
                    </span>
                </div>
                <div>
                    <span class="text">
                        鼠标滚轮<span class="num">{{ count_today.data.scroll_count }}</span>
                    </span>
                </div>
                <div>
                    <span class="text">
                        鼠标中键<span class="num">{{ count_today.data.scroll_press_count }}</span>
                    </span>
                </div>
                <div>
                    <span class="text">
                        移动距离<span class="num">{{ count_today.move_distance_str }} Pixel</span>
                    </span>
                </div>
            </div>
            <div class="data_all">
                <h2>总计</h2>
                <div>
                    <span class="text">
                        鼠标左键<span class="num">{{ count_all.data.left_count }}</span>
                    </span>
                </div>
                <div>
                    <span class="text">
                        鼠标右键<span class="num">{{ count_all.data.right_count }}</span>
                    </span>
                </div>
                <div>
                    <span class="text">
                        鼠标滚轮<span class="num">{{ count_all.data.scroll_count }}</span>
                    </span>
                </div>
                <div>
                    <span class="text">
                        鼠标中键<span class="num">{{ count_all.data.scroll_press_count }}</span>
                    </span>
                </div>
                <div>
                    <span class="text">
                        移动距离<span class="num">{{ count_all.move_distance_str }} Pixel</span>
                    </span>
                </div>
            </div>
        </div>
    </div>
</template>
<style scoped>
.m_container {
    width: 70%;
    display: flex;
    justify-content: center;
    margin: 50px auto;
}

.selected_data_wrapper {
    margin: 0 30px;
}

.text_container {
    width: 400px;
    display: flex;
}

.text_container>.data_today,
.text_container>.data_all {
    width: 400px;
}

span.text {
    font-weight: 600;
    font-size: 14px;
}

span.num {
    font-weight: 600;
    font-size: 18px;
    padding: 0 5px;
    color: var(--vt-c-text-dark-1);
}
</style>
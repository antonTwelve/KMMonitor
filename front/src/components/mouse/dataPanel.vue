<script setup>
import { ref } from "vue";
import dataText from "./dataText.vue";
import { count_mouse_data, distance_format } from "./util.js";

var start_time = undefined, end_time = undefined;
var data_count = ref(new count_mouse_data());

/**
 * 判断一个时间是否在选定的范围内
 * @param {*} time 需要判断的时间, 可以是时间戳, 也可以是Date对象
 */
function is_time_in_range(time) {
    if (start_time === undefined) return true;
    let time_stamp;
    if (time instanceof Date)
        time_stamp = time.getTime();
    else time_stamp = time;
    if (end_time === undefined) return time_stamp >= start_time;
    return (time_stamp >= start_time && time_stamp <= end_time)
}

/**
 * 遍历所有数据, 统计
 */
function count(all_data) {
    data_count.value = new count_mouse_data();
    if (all_data.length === 0) return;
    //遍历除最后一条数据外的所有数据
    all_data.forEach((data) => {
        if (is_time_in_range(data.time))
            count_mouse_data.plus(data_count.value, data_count.value, data);
    });
}

/**
 * 接收新数据时更新
 * @param {*} new_data 新数据
 * @param {*} old_data 需要更新的一分钟的旧数据, 不传入该参数表示直接加上新数据, 否则为加上新数据再减去旧数据
 */
function update(new_data, old_data = undefined) {
    if (!is_time_in_range(new_data.time)) return;
    if (old_data != undefined)
        count_mouse_data.sub(data_count.value, data_count.value, old_data);
    count_mouse_data.plus(data_count.value, data_count.value, new_data);
}

function init(all_data, start = undefined, end = undefined) {
    start_time = start;
    end_time = end;
    count(all_data);
}

defineExpose({
    update,
    init
})
</script>
<template>
    <div class="mouse_data_wrapper">
        <div>

        </div>
        <div class="d_wrapper">
            <div class="d_block">
                <dataText text="左键" :data="data_count.left_count" unit="次" />
            </div>
            <div class="d_block">
                <dataText text="右键" :data="data_count.right_count" unit="次" />
            </div>
        </div>
        <div class="d_wrapper">
            <div class="d_block">
                <dataText text="滚轮" :data="data_count.scroll_count" unit="次" />
            </div>
            <div class="d_block">
                <dataText text="中键" :data="data_count.scroll_press_count" unit="次" />
            </div>
        </div>
        <div style="width:200px">
            <dataText text="移动距离" :data="distance_format(data_count.move_distance)" unit="Pixel" />
        </div>
    </div>
</template>
<style scoped>
.mouse_data_wrapper {
    display: flex;
    padding-top: 40px;
}

.d_block {
    height: 50px;
    width: 150px;
    padding: 0 20px;
}
</style>
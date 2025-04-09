<script setup>
import { onMounted, ref } from "vue";
import { getCookie, setCookie } from "../utils.js";
import l_87_1 from "./layout/l_87_1.vue";
import l_80_1 from "./layout/l_80_1.vue";
import l_104_1 from "./layout/l_104_1.vue";
import dropdown from "./dropdown.vue";

let key_count = ref(new Array(300).fill(0));      //记录每个按键按下的次数, 键值最大为300
let key_color = ref(new Array(300).fill(0));    //每个按键的颜色, 0-1之间的小数
const selected_layout = ref(0);
const ele_dropdown = ref(null);
const MIN_MAX_COUNT = 200;
let max_count = MIN_MAX_COUNT;                        //最大的按键按下次数, 用于设定按键颜色, 不小于200

//以单个按键高度为基准构建键盘图形
document.documentElement.style.setProperty('--key-height', '50px');
//统计起止时间戳
let start_time = undefined;
let end_time = undefined;
//当前时间段内按下总次数
let press_sum = ref(0);

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
 * 更新所有按键的颜色, 
 */
function update_key_color() {
    for (let i = 0; i < key_count.value.length; i++) {
        key_color.value[i] = key_count.value[i] / max_count;
    }
}

/**
 * 更新某个按键按下的次数
 * 更新逻辑为: 总次数-之前按下次数+新按下次数
 * 如果是同一分钟数据, 则传入新次数和之前的次数
 * 如果是新一分钟的数据, 将之前次数设置为0即可
 * @param {*} key 按键键值 必须是数字
 * @param {*} new_count 新的按下次数 必须是数字
 * @param {*} old_count 之前的按下次数 必须是数字
 * @param {*} update_max_count 是否需要更新最大值, 默认为需要更新, 更新最大值可能触发更新所有按键颜色
 */
function key_map_update(key, new_count, old_count, update_max_count = true) {
    key_count.value[key] += (new_count - old_count);
    if (!update_max_count) return;
    if (key_count.value[key] > max_count) {
        //有新的最大值, 触发颜色更新
        max_count = key_count.value[key];
        update_key_color();
    } else {
        key_color.value[key] = key_count.value[key] / max_count;
    }
}

/**
 * 遍历所有数据, 统计每个按键按下次数
 * 调用此方法时会将所有按键按下次数重置为0
 */
function key_map_count(all_data) {
    all_data.forEach((data) => {
        if (is_time_in_range(data.time)) {
            press_sum.value += data.press_count;
            data.keyBD_count.forEach((value, key) => {
                key_map_update(key, value, 0, false);
            })
        }
    })
    //数据统计完成后刷新按键颜色
    max_count = Math.max(...key_count.value);
    if (max_count < MIN_MAX_COUNT) max_count = MIN_MAX_COUNT;
    update_key_color();
}

/**
 * 接收新数据时更新
 * @param {*} new_data 新数据
 * @param {*} old_data 需要更新的一分钟的旧数据, 不传入该参数表示直接加上新数据, 否则为加上新数据再减去旧数据
 */
function update(new_data, old_data = undefined) {
    //跳过不在范围内的数据
    if (!is_time_in_range(new_data.time)) return;
    //统计按下总次数
    press_sum.value += new_data.press_count;
    if (old_data != undefined) press_sum.value -= old_data.press_count;
    //统计每个按键按下次数
    new_data.keyBD_count.forEach((value, key) => {
        let old_count = 0;
        if (old_data != undefined) {
            old_count = old_data.keyBD_count.get(key);
            if (isNaN(old_count)) old_count = 0;
        }
        key_map_update(key, value, old_count);
    })
}

function init(all_data, start = undefined, end = undefined) {
    start_time = start;
    end_time = end;
    key_count.value.fill(0);
    press_sum.value = 0;
    key_map_count(all_data);
}

onMounted(() => {
    let temp_index = parseInt(getCookie("layout"));
    if (!isNaN(temp_index)) {
        selected_layout.value = temp_index;
        ele_dropdown.value.set_value(temp_index);
        //重写cookie, 刷新有效时间
        setCookie("layout", temp_index, 30);
    }
})

function keybd_layout_selected(index) {
    selected_layout.value = index;
    setCookie("layout", index, 30);
}

defineExpose({
    update,
    init
})
</script>

<template>
    <div>
        <div class="head">
            <div>总计 {{ press_sum }} 次</div>
            <div style="flex:1;">
            </div>
            <dropdown ref="ele_dropdown" @change="keybd_layout_selected" />
        </div>
        <l_87_1 v-if="selected_layout == 0" :key_count="key_count" :key_color="key_color" />
        <l_80_1 v-else-if="selected_layout == 1" :key_count="key_count" :key_color="key_color" />
        <l_104_1 v-else-if="selected_layout == 2" :key_count="key_count" :key_color="key_color" />
    </div>
</template>
<style scoped>
.head {
    display: flex;
    padding: 0 calc(0.15 * var(--key-height));
}
</style>
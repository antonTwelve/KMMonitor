<script setup>
import { onMounted, ref } from "vue";

const emit = defineEmits(['selected']);

const show_items = ref(false);
const show_btn = ref(null)
const items = ref(null);
const buttons = ref([
    { text: "前8小时", selected: false },
    { text: "前24小时", selected: true },
    { text: "今天", selected: false },
    { text: "全部", selected: false },
    { text: "自定义", selected: false }
])
const show_dialog = ref(false);
const time_select_dialog = ref(null);

/**
 * 起止时间戳
 */
var start_time = undefined;
var end_time = undefined;

function is_point_in_rect(x, y, rect) {
    if (x < rect.left || x > rect.right || y < rect.top || y > rect.bottom) return false;
    return true;
}

function page_click_handle(event) {
    if (!show_items.value) {
        //没有在显示, 该监听函数不应该存在
        document.removeEventListener('click', page_click_handle);
        return;
    }

    let rect1 = items.value.getBoundingClientRect();
    let rect2 = show_btn.value.getBoundingClientRect();
    let x = event.clientX;
    let y = event.clientY;
    if (!is_point_in_rect(x, y, rect1) && !is_point_in_rect(x, y, rect2)) {
        show_items.value = false;
        document.removeEventListener('click', page_click_handle);
    }

}

function show_btn_clicked() {
    show_items.value = !show_items.value;
    if (show_items.value) {
        document.addEventListener('click', page_click_handle);
    }
}

function selected(index) {
    end_time = undefined;
    buttons.value.forEach((b) => {
        b.selected = false;
    })
    buttons.value[index].selected = true;
    switch (index) {
        case 0:
            //前8小时
            start_time = (new Date()).getTime();
            start_time -= 8 * 60 * 60 * 1000;
            emit('selected', start_time, end_time);
            break;
        case 1:
            //前24小时
            start_time = (new Date()).getTime();
            start_time -= 24 * 60 * 60 * 1000;
            emit('selected', start_time, end_time);
            break;
        case 2:
            //今天
            start_time = (new Date()).setHours(0, 0, 0, 0);
            emit('selected', start_time, end_time);
            break;
        case 3:
            //全部
            start_time = undefined;
            emit('selected', start_time, end_time);
            break;
        case 4:
            //自定义时间
            show_dialog.value = true;
            set_input_time_value();
            break;
        default:
            break;
    }
    //等按钮变色了再隐藏
    setTimeout(() => {
        show_items.value = false;
    }, 50);
}

function dialog_click_handler(event) {
    let x = event.clientX;
    let y = event.clientY;
    if (time_select_dialog.value === null) return;
    let rect = time_select_dialog.value.getBoundingClientRect();
    if (!is_point_in_rect(x, y, rect))
        show_dialog.value = false;
}

onMounted(() => {
    document.addEventListener('keyup', (e) => {
        if (e.key == "Escape") {
            show_dialog.value = false;
        }
    })
})


//自定义时间段变量
const s_year = ref(0);
const s_mounth = ref(0);
const s_day = ref(0);
const s_hour = ref(0);
const s_minute = ref(0);
const e_year = ref(0);
const e_mounth = ref(0);
const e_day = ref(0);
const e_hour = ref(0);
const e_minute = ref(0);
var end_date = new Date();
var start_date = new Date(end_date.getTime() - 8 * 60 * 60 * 1000);

function set_input_time_value() {
    s_year.value = start_date.getFullYear();
    s_mounth.value = start_date.getMonth() + 1;
    s_day.value = start_date.getDate();
    s_hour.value = start_date.getHours();
    s_minute.value = start_date.getMinutes();
    e_year.value = end_date.getFullYear();
    e_mounth.value = end_date.getMonth() + 1;
    e_day.value = end_date.getDate();
    e_hour.value = end_date.getHours();
    e_minute.value = end_date.getMinutes();
}

/**
 * 检查输入的时间是否合规
 */
function check_time(e) {
    console.log(e);
    start_date.setFullYear(s_year.value);
    start_date.setMonth(s_mounth.value - 1);
    start_date.setDate(s_day.value);
    start_date.setHours(s_hour.value);
    start_date.setMinutes(s_minute.value);
    end_date.setFullYear(e_year.value);
    end_date.setMonth(e_mounth.value - 1);
    end_date.setDate(e_day.value);
    end_date.setHours(e_hour.value);
    end_date.setMinutes(e_minute.value);
    console.log(e_mounth.value);
    console.log(start_date);
    console.log(end_date);
    set_input_time_value();
}

/**
 * 选定时间段
 */
function custom_time_selected() {
    show_dialog.value = false;
    check_time();
    start_time = start_date.getTime();
    end_time = end_date.getTime();
    if (start_time >= end_time) {
        alert("起始时间大于等于结束时间");
        return;
    }
    console.log(start_time);
    console.log(end_time);
    emit('selected', start_time, end_time);
}

</script>

<template>
    <div class="time_seletor">
        <div class="select_btn flex_center" ref="show_btn" @click="show_btn_clicked">
            <svg class="icon" viewBox="0 0 1024 1024" version="1.1" xmlns="http://www.w3.org/2000/svg">
                <path
                    d="M426.666667 183.466667V128h170.666666v55.466667c149.333333 38.4 256 170.666667 256 328.533333 0 187.733333-153.6 341.333333-341.333333 341.333333s-341.333333-153.6-341.333333-341.333333c0-157.866667 106.666667-294.4 256-328.533333zM512 768c140.8 0 256-115.2 256-256s-115.2-256-256-256-256 115.2-256 256 115.2 256 256 256z m110.933333-426.666667L682.666667 401.066667 529.066667 554.666667 469.333333 494.933333 622.933333 341.333333z"
                    fill="currentColor"></path>
            </svg>
        </div>
        <Transition name="items_trans">
            <div class="items" ref="items" v-if="show_items">
                <div v-for="(button, index) in buttons" :key="index" class="s_item"
                    :style="{ backgroundColor: button.selected ? 'var(--btn_green)' : 'transparent' }"
                    @click="selected(index)">{{
            button.text
        }}</div>
            </div>
        </Transition>
    </div>
    <Transition name="dialog_trans">
        <div class="dialog" v-if="show_dialog" @click="dialog_click_handler">
            <div class="wrapper" ref="time_select_dialog">
                <div class="time_input flex_center">
                    从：
                    <input type="number" v-model="s_year" @change="check_time" />/
                    <input type="number" v-model="s_mounth" @change="check_time" />/
                    <input type="number" v-model="s_day" @change="check_time" />
                    <input type="number" v-model="s_hour" @change="check_time" />:
                    <input type="number" v-model="s_minute" @change="check_time" />
                </div>
                <div class="time_input flex_center">
                    到：
                    <input type="number" v-model="e_year" @change="check_time" />/
                    <input type="number" v-model="e_mounth" @change="check_time" />/
                    <input type="number" v-model="e_day" @change="check_time" />
                    <input type="number" v-model="e_hour" @change="check_time" />:
                    <input type="number" v-model="e_minute" @change="check_time" />
                </div>
                <span style="font-size: 12px; opacity: 0.5; margin: 8px 0px;">
                    将就着用吧, 想不出更好的法子了, UI越做越丑
                </span>
                <div>
                    <button class="confirm_btn btn_type1" @click="custom_time_selected">确认</button>
                </div>
            </div>
        </div>
    </Transition>
</template>

<style scoped>
.time_seletor {
    height: 50px;
    width: 50px;
    display: flex;
    align-items: center;
    justify-content: center;
    user-select: none;
    position: absolute;
    top: 0px;
    left: 0px;
}

.flex_center {
    display: flex;
    align-items: center;
    justify-content: center;
}

.select_btn {
    width: 30px;
    height: 30px;
    border-radius: 50%;
    background-color: var(--btn_green);
    padding: 2px;
}

.items {
    padding: 5px 0px;
    width: 100px;
    position: absolute;
    left: 50px;
    top: 10px;
    background-color: var(--color-background);
    border-radius: 4px;
    border: 1px solid #27272a;
    transition: all .2s ease-in-out;
}

.items_trans-enter-active {
    transition: all 0.3s ease-in-out;
}

.items_trans-leave-active {
    transition: all 0.3s ease-in-out;
}

.items_trans-enter-from,
.items_trans-leave-to {
    transform: translateX(-20px);
    opacity: 0;
}

.s_item {
    padding: 0px 10px;
    margin: 5px 0px;
}

.dialog {
    position: fixed;
    top: 0;
    left: 0;
    height: 100vh;
    width: 100vw;
    background-color: rgba(0, 0, 0, 0.5);
    display: flex;
    justify-content: center;
    overflow: hidden;
}

.dialog>.wrapper {
    height: 190px;
    width: 480px;
    background-color: var(--color-background);
    margin-top: 100px;
    border-radius: 8px;
    padding: 16px;
    border: 1px solid #27272a;
}

.time_input {
    font-weight: 800;
    user-select: none;
    margin: 8px 0px;
}

input {
    width: 70px;
    margin: 0 4px;
    border: none;
    background-color: var(--color-background);
    border: 1px solid #27272a;
    outline: none;
    color: var(--color-text);
    font-size: 16px;
    padding: 8px 16px;
    font-weight: 600;
    border-radius: 6px;
    text-align: center;
}

input[type=number]::-webkit-inner-spin-button,
input[type=number]::-webkit-outer-spin-button {
    display: none;
}

.dialog_trans-enter-active {
    transition: all 0.3s ease-in-out;
}

.dialog_trans-leave-active {
    transition: all 0.3s ease-in-out;
}

.dialog_trans-enter-from,
.dialog_trans-leave-to {
    opacity: 0;
}


.dialog_trans-enter-active .wrapper {
    transition: all 0.3s ease-in-out;
}

.dialog_trans-leave-active .wrapper {
    transition: all 0.3s ease-in-out;
}

.dialog_trans-enter-from .wrapper,
.dialog_trans-leave-to .wrapper {
    transform: translateY(30px);
}

.confirm_btn {
    float: right;
}
</style>
<script setup>
import { ref } from "vue";

const emit = defineEmits(["change"]);
const show_list_flag = ref(false);
const layout_list = ref([
    { text: "87配列" },
    { text: "80配列" },
    { text: "104配列" }
])
const btn_text = ref(layout_list.value[0].text);
const list_ele = ref(null);
const list_btn_ele = ref(null);

function selected(index) {
    btn_text.value = layout_list.value[index].text;
    emit("change", index);
    hide_list();
}

function is_point_in_rect(x, y, rect) {
    if (x < rect.left || x > rect.right || y < rect.top || y > rect.bottom) return false;
    return true;
}

function window_click_handler(e) {
    let x = e.clientX;
    let y = e.clientY;
    let rect1 = list_ele.value.getBoundingClientRect();
    let rect2 = list_btn_ele.value.getBoundingClientRect();
    if (!is_point_in_rect(x, y, rect1) && !is_point_in_rect(x, y, rect2)) {
        hide_list();
    }
}

function btn_clicked() {
    show_list_flag.value = !show_list_flag.value;
    window.addEventListener("click", window_click_handler);
}

function hide_list() {
    show_list_flag.value = false;
    window.removeEventListener("click", window_click_handler);
}

function set_value(index) {
    btn_text.value = layout_list.value[index].text;
}

defineExpose({
    set_value
})

</script>

<template>
    <div class="wrapper">
        <button ref="list_btn_ele" class="btn_type1 flex_center" @click="btn_clicked">
            {{ btn_text }}
            <svg width="15" height="15" viewBox="0 0 15 15" fill="none" xmlns="http://www.w3.org/2000/svg"
                style="margin-left: 4px;">
                <path
                    d="M3.13523 6.15803C3.3241 5.95657 3.64052 5.94637 3.84197 6.13523L7.5 9.56464L11.158 6.13523C11.3595 5.94637 11.6759 5.95657 11.8648 6.15803C12.0536 6.35949 12.0434 6.67591 11.842 6.86477L7.84197 10.6148C7.64964 10.7951 7.35036 10.7951 7.15803 10.6148L3.15803 6.86477C2.95657 6.67591 2.94637 6.35949 3.13523 6.15803Z"
                    fill="currentColor" fill-rule="evenodd" clip-rule="evenodd"></path>
            </svg>
        </button>
        <Transition name="list_transition">
            <div ref="list_ele" v-if="show_list_flag" class="s_list">
                <div v-for="(ly, index) in layout_list" :key="index" class="list_item" @click="selected(index)">
                    {{ ly.text }}
                </div>
            </div>
        </Transition>
    </div>
</template>

<style scoped>
.wrapper {
    position: relative;
}

.flex_center {
    display: flex;
    align-items: center;
    justify-content: center;
}

.s_list {
    position: absolute;
    z-index: 999;
    top: 40px;
    padding: 5px 0px;
    background-color: var(--color-background);
    border-radius: 4px;
    border: 1px solid #27272a;
}

.list_item {
    position: relative;
    height: 30px;
    min-width: 100px;
    padding: 0px 10px;
    margin: 5px 0px;
    user-select: none;
    transition: background-color 0.2s ease-in-out;
}

.list_item:hover {
    background-color: var(--btn_green);
}

.list_transition-enter-active {
    transition: all 0.1s ease-in-out;
}

.list_transition-leave-active {
    transition: all 0.1s ease-in-out;
}

.list_transition-enter-from,
.list_transition-leave-to {
    opacity: 0;
    transform: scale(0.9);
}
</style>
<script setup>
import { onMounted, ref } from 'vue';

const props = defineProps(["keyname", "keycount", "keycolor"])

const key_ele = ref(null);

const hover_dia_ele = ref(null);

onMounted(() => {
    key_ele.value.addEventListener("mousemove", (e) => {
        if (props.keycount === undefined) return;
        hover_dia_ele.value.style.display = "block";
        hover_dia_ele.value.style.top = `${e.clientY + 5}px`;
        hover_dia_ele.value.style.left = `${e.clientX + 10}px`;
    });

    key_ele.value.addEventListener("mouseleave", () => {
        hover_dia_ele.value.style.display = "none";
    })
})

</script>

<template>
    <div class="key" ref="key_ele"
        :style="{ backgroundColor: `rgba( ${55 + Math.round(props.keycolor * 11)}, ${56 + Math.round(props.keycolor * 129)}, ${57 + Math.round(props.keycolor * 74)}, 1)` }">
        <!-- start 55, 56, 57 -->
        <!-- 66, 185, 131 -->
        <span>{{ props.keyname }}</span>
        <span class="key_count">{{ props.keycount }}</span>
        <div class="hover_num" ref="hover_dia_ele">
            {{ props.keycount }}次
        </div>
    </div>
</template>

<style scoped>
.key {
    width: var(--key-height);
    height: var(--key-height);
    margin: 0 calc(0.15 * var(--key-height));
    background-color: #373839;
    border-radius: 3px;
    overflow: hidden;
}

.key>span {
    display: block;
    text-align: center;
    font-weight: 500;
    font-size: 0.32 * var(--key-height);
}

.key>.key_count {
    font-size: 0.24 * var(--key-height);
}

.hover_num {
    position: fixed;
    z-index: 1000;
    background-color: #181818;
    border: 1px solid #27272a;
    border-radius: 4px;
    padding: 4px 8px;
    font-size: 12px;
    font-weight: 600;
    display: none;
    left: 0px;
    top: var(--key-height);
}
</style>
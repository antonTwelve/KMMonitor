<script setup>
const props = defineProps(["text", "data", "unit"])
</script>
<template>
    <div class="wrapper">
        <span class="text">{{ props.text }} {{ props.data }} {{ props.unit }}</span>
    </div>
</template>
<style scoped>
.wrapper {
    width: 100%;
}

.text {
    display: block;
    color: #42b983;
    font-weight: 600;
    font-size: 15px;
}
</style>
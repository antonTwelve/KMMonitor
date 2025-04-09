<script setup>
import {ref} from "vue";
import keyboard from "./components/keyboard/keyboard.vue";
import mouse from "./components/mouse/mouse.vue";
import timeSelector from "./components/time_selector.vue";

const keyboard_v = ref(null);
const mouse_v = ref(null);

function time_selected(start_time, end_time) {
  keyboard_v.value.time_selected(start_time, end_time);
  mouse_v.value.time_selected(start_time, end_time);
}
</script>

<template>
  <div>
    <keyboard ref="keyboard_v" />
    <mouse ref="mouse_v"/>
    <timeSelector @selected="time_selected" />
    <div class="test"></div>
  </div>
</template>


<style scoped></style>
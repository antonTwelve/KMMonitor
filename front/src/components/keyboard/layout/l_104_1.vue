<script setup>
import { onMounted } from "vue";
import key from "./keyStyle/key1.vue";

const props = defineProps(["key_count", "key_color"]);

function set_key_width() {
    let windowWidth = window.innerWidth;
    let key_width = parseInt((windowWidth - 300) / 30);
    if (key_width > 50) key_width = 50;
    else if (key_width < 48) key_width = 48;
    document.documentElement.style.setProperty('--key-height', `${key_width}px`);
}

onMounted(() => {
    set_key_width();
    window.addEventListener("resize", () => {
        set_key_width();
    })
});

</script>

<template>
    <div id="keyboard_wrapper" class="flex_block">
        <div>
            <div class="flex_block key_board_line" style="margin-bottom: calc(0.6 * var(--key-height));">
                <key keyname="Esc" :keycount="props.key_count[27]" :keycolor="props.key_color[27]" />
                <div class="blank" style="width: var(--key-height);"></div>
                <key keyname="F1" :keycount="props.key_count[112]" :keycolor="props.key_color[112]" />
                <key keyname="F2" :keycount="props.key_count[113]" :keycolor="props.key_color[113]" />
                <key keyname="F3" :keycount="props.key_count[114]" :keycolor="props.key_color[114]" />
                <key keyname="F4" :keycount="props.key_count[115]" :keycolor="props.key_color[115]" />
                <div class="blank" style="width: calc(0.35 * var(--key-height));"></div>
                <key keyname="F5" :keycount="props.key_count[116]" :keycolor="props.key_color[116]" />
                <key keyname="F6" :keycount="props.key_count[117]" :keycolor="props.key_color[117]" />
                <key keyname="F7" :keycount="props.key_count[118]" :keycolor="props.key_color[118]" />
                <key keyname="F8" :keycount="props.key_count[119]" :keycolor="props.key_color[119]" />
                <div class="blank" style="width: calc(0.35 * var(--key-height));"></div>
                <key keyname="F9" :keycount="props.key_count[120]" :keycolor="props.key_color[120]" />
                <key keyname="F10" :keycount="props.key_count[121]" :keycolor="props.key_color[121]" />
                <key keyname="F11" :keycount="props.key_count[122]" :keycolor="props.key_color[122]" />
                <key keyname="F12" :keycount="props.key_count[123]" :keycolor="props.key_color[123]" />
                <div class="blank blank_type_1"></div>
                <key keyname="PrtSc" :keycount="props.key_count[44]" :keycolor="props.key_color[44]" />
                <key keyname="ScrLk" :keycount="props.key_count[145]" :keycolor="props.key_color[145]" />
                <key keyname="Pause" :keycount="props.key_count[19]" :keycolor="props.key_color[19]" />
            </div>
            <div class="flex_block key_board_line">
                <key keyname="` ~" :keycount="props.key_count[192]" :keycolor="props.key_color[192]" />
                <key keyname="1 !" :keycount="props.key_count[49]" :keycolor="props.key_color[49]" />
                <key keyname="2 @" :keycount="props.key_count[50]" :keycolor="props.key_color[50]" />
                <key keyname="3 #" :keycount="props.key_count[51]" :keycolor="props.key_color[51]" />
                <key keyname="4 $" :keycount="props.key_count[52]" :keycolor="props.key_color[52]" />
                <key keyname="5 %" :keycount="props.key_count[53]" :keycolor="props.key_color[53]" />
                <key keyname="6 ^" :keycount="props.key_count[54]" :keycolor="props.key_color[54]" />
                <key keyname="7 &" :keycount="props.key_count[55]" :keycolor="props.key_color[55]" />
                <key keyname="8 *" :keycount="props.key_count[56]" :keycolor="props.key_color[56]" />
                <key keyname="9 (" :keycount="props.key_count[57]" :keycolor="props.key_color[57]" />
                <key keyname="0 )" :keycount="props.key_count[48]" :keycolor="props.key_color[48]" />
                <key keyname="- _" :keycount="props.key_count[189]" :keycolor="props.key_color[189]" />
                <key keyname="= +" :keycount="props.key_count[187]" :keycolor="props.key_color[187]" />
                <key keyname="⬅ Back" :keycount="props.key_count[8]" :keycolor="props.key_color[8]"
                    style="width: calc(2.3 * var(--key-height));" />
                <div class="blank blank_type_1"></div>
                <key keyname="Ins" :keycount="props.key_count[45]" :keycolor="props.key_color[45]" />
                <key keyname="Home" :keycount="props.key_count[36]" :keycolor="props.key_color[36]" />
                <key keyname="PgUp" :keycount="props.key_count[33]" :keycolor="props.key_color[33]" />
            </div>
            <div class="flex_block key_board_line">
                <key keyname="Tab" :keycount="props.key_count[9]" :keycolor="props.key_color[9]"
                    style="width: calc(1.65 * var(--key-height));" />
                <key keyname="Q" :keycount="props.key_count[81]" :keycolor="props.key_color[81]" />
                <key keyname="W" :keycount="props.key_count[87]" :keycolor="props.key_color[87]" />
                <key keyname="E" :keycount="props.key_count[69]" :keycolor="props.key_color[69]" />
                <key keyname="R" :keycount="props.key_count[82]" :keycolor="props.key_color[82]" />
                <key keyname="T" :keycount="props.key_count[84]" :keycolor="props.key_color[84]" />
                <key keyname="Y" :keycount="props.key_count[89]" :keycolor="props.key_color[89]" />
                <key keyname="U" :keycount="props.key_count[85]" :keycolor="props.key_color[85]" />
                <key keyname="I" :keycount="props.key_count[73]" :keycolor="props.key_color[73]" />
                <key keyname="O" :keycount="props.key_count[79]" :keycolor="props.key_color[79]" />
                <key keyname="P" :keycount="props.key_count[80]" :keycolor="props.key_color[80]" />
                <key keyname="[ {" :keycount="props.key_count[219]" :keycolor="props.key_color[219]" />
                <key keyname="] }" :keycount="props.key_count[221]" :keycolor="props.key_color[221]" />
                <key keyname="\ |" :keycount="props.key_count[220]" :keycolor="props.key_color[220]"
                    style="width: calc(1.65 * var(--key-height));" />
                <div class="blank blank_type_1"></div>
                <key keyname="Del" :keycount="props.key_count[46]" :keycolor="props.key_color[46]" />
                <key keyname="End" :keycount="props.key_count[35]" :keycolor="props.key_color[35]" />
                <key keyname="PgDn" :keycount="props.key_count[34]" :keycolor="props.key_color[34]" />
            </div>
            <div class="flex_block key_board_line">
                <key keyname="CapsLock" :keycount="props.key_count[20]" :keycolor="props.key_color[20]"
                    style="width: calc(2.04 * var(--key-height));" />
                <key keyname="A" :keycount="props.key_count[65]" :keycolor="props.key_color[65]" />
                <key keyname="S" :keycount="props.key_count[83]" :keycolor="props.key_color[83]" />
                <key keyname="D" :keycount="props.key_count[68]" :keycolor="props.key_color[68]" />
                <key keyname="F" :keycount="props.key_count[70]" :keycolor="props.key_color[70]" />
                <key keyname="G" :keycount="props.key_count[71]" :keycolor="props.key_color[71]" />
                <key keyname="H" :keycount="props.key_count[72]" :keycolor="props.key_color[72]" />
                <key keyname="J" :keycount="props.key_count[74]" :keycolor="props.key_color[74]" />
                <key keyname="K" :keycount="props.key_count[75]" :keycolor="props.key_color[75]" />
                <key keyname="L" :keycount="props.key_count[76]" :keycolor="props.key_color[76]" />
                <key keyname="; :" :keycount="props.key_count[186]" :keycolor="props.key_color[186]" />
                <key keyname="'" :keycount="props.key_count[222]" :keycolor="props.key_color[222]" />
                <key keyname="Enter" :keycount="props.key_count[13]" :keycolor="props.key_color[13]"
                    style="width: calc(2.56 * var(--key-height));" />
            </div>
            <div class="flex_block key_board_line">
                <key keyname="Shift" :keycount="props.key_count[160]" :keycolor="props.key_color[160]"
                    style="width: calc(2.95 * var(--key-height));" />
                <key keyname="Z" :keycount="props.key_count[90]" :keycolor="props.key_color[90]" />
                <key keyname="X" :keycount="props.key_count[88]" :keycolor="props.key_color[88]" />
                <key keyname="C" :keycount="props.key_count[67]" :keycolor="props.key_color[67]" />
                <key keyname="V" :keycount="props.key_count[86]" :keycolor="props.key_color[86]" />
                <key keyname="B" :keycount="props.key_count[66]" :keycolor="props.key_color[66]" />
                <key keyname="N" :keycount="props.key_count[78]" :keycolor="props.key_color[78]" />
                <key keyname="M" :keycount="props.key_count[77]" :keycolor="props.key_color[77]" />
                <key keyname=", <" :keycount="props.key_count[188]" :keycolor="props.key_color[188]" />
                <key keyname=". >" :keycount="props.key_count[190]" :keycolor="props.key_color[190]" />
                <key keyname="/ ?" :keycount="props.key_count[191]" :keycolor="props.key_color[191]" />
                <key keyname="Shift" :keycount="props.key_count[161]" :keycolor="props.key_color[161]"
                    style="width: calc(2.95 * var(--key-height));" />
                <div class="blank blank_type_1"></div>
                <div class="blank" style="width: calc(var(--key-height));"></div>
                <key keyname="↑" :keycount="props.key_count[38]" :keycolor="props.key_color[38]" />
            </div>
            <div class="flex_block key_board_line">
                <key keyname="Ctrl" style="width: calc(1.4 * var(--key-height))" :keycount="props.key_count[162]"
                    :keycolor="props.key_color[162]" />
                <key keyname="Win" style="width: calc(1.4 * var(--key-height))" :keycount="props.key_count[91]"
                    :keycolor="props.key_color[91]" />
                <key keyname="Alt" style="width: calc(1.4 * var(--key-height))" :keycount="props.key_count[164]"
                    :keycolor="props.key_color[164]" />
                <key keyname=" " style="width: calc(7.3 * var(--key-height))" :keycount="props.key_count[32]"
                    :keycolor="props.key_color[32]" />
                <key keyname="Alt" style="width: calc(1.4 * var(--key-height))" :keycount="props.key_count[165]"
                    :keycolor="props.key_color[165]" />
                <!-- Fn键没有vk码, 无法监听 -->
                <key keyname="Fn" style="width: calc(1.4 * var(--key-height))" />
                <key keyname="Menu" style="width: calc(1.4 * var(--key-height))" :keycount="props.key_count[93]"
                    :keycolor="props.key_color[93]" />
                <key keyname="Ctrl" style="width: calc(1.4 * var(--key-height))" :keycount="props.key_count[163]"
                    :keycolor="props.key_color[163]" />
                <div class="blank blank_type_1"></div>
                <key keyname="←" :keycount="props.key_count[37]" :keycolor="props.key_color[37]" />
                <key keyname="↓" :keycount="props.key_count[40]" :keycolor="props.key_color[40]" />
                <key keyname="→" :keycount="props.key_count[39]" :keycolor="props.key_color[39]" />
            </div>
        </div>
        <div class="flex_block"
            style="margin-top: calc(1.6 * var(--key-height)); margin-left: calc(0.45 * var(--key-height))">
            <div>
                <div class="flex_block key_board_line">
                    <key keyname="Num" :keycount="props.key_count[144]" :keycolor="props.key_color[144]" />
                    <key keyname="/" :keycount="props.key_count[111]" :keycolor="props.key_color[111]" />
                    <key keyname="*" :keycount="props.key_count[106]" :keycolor="props.key_color[106]" />
                </div>
                <div class="flex_block key_board_line">
                    <key keyname="7" :keycount="props.key_count[103]" :keycolor="props.key_color[103]" />
                    <key keyname="8" :keycount="props.key_count[104]" :keycolor="props.key_color[104]" />
                    <key keyname="9" :keycount="props.key_count[105]" :keycolor="props.key_color[105]" />
                </div>
                <div class="flex_block key_board_line">
                    <key keyname="4" :keycount="props.key_count[100]" :keycolor="props.key_color[100]" />
                    <key keyname="5" :keycount="props.key_count[101]" :keycolor="props.key_color[101]" />
                    <key keyname="6" :keycount="props.key_count[102]" :keycolor="props.key_color[102]" />
                </div>
                <div class="flex_block key_board_line">
                    <key keyname="1" :keycount="props.key_count[97]" :keycolor="props.key_color[97]" />
                    <key keyname="2" :keycount="props.key_count[98]" :keycolor="props.key_color[98]" />
                    <key keyname="3" :keycount="props.key_count[99]" :keycolor="props.key_color[99]" />
                </div>
                <div class="flex_block key_board_line">
                    <key keyname="0" :keycount="props.key_count[96]" :keycolor="props.key_color[96]"
                        style="width: calc(2.3 * var(--key-height));" />
                    <key keyname="." :keycount="props.key_count[110]" :keycolor="props.key_color[110]" />
                </div>
            </div>
            <div class="key_board_line">
                <key keyname="-" :keycount="props.key_count[109]" :keycolor="props.key_color[109]"
                    style="margin-bottom: calc(0.15 * var(--key-height));" />
                <key keyname="+" :keycount="props.key_count[107]" :keycolor="props.key_color[107]"
                    style="margin-bottom: calc(0.15 * var(--key-height)); height: calc(2.3 * var(--key-height));" />
                <!-- 无法区分小键盘Enter与主键盘Enter, 故不显示数据 -->
                <!-- <key keyname="Enter" :keycount="props.key_count[13]" :keycolor="props.key_color[13]"
                    style="margin-bottom: calc(0.15 * var(--key-height)); height: calc(2.3 * var(--key-height));" /> -->
                <key keyname="Enter"
                    style="margin-bottom: calc(0.15 * var(--key-height)); height: calc(2.3 * var(--key-height));" />
            </div>
        </div>
    </div>
</template>

<style scoped>
.blank {
    opacity: 0;
    margin: 0 calc(0.15 * var(--key-height));
}

.blank_type_1 {
    width: calc(0.15 * var(--key-height));
}

.flex_block {
    display: flex;
}

.key_board_line {
    margin: calc(0.2 * var(--key-height)) 0;
}

div {
    user-select: none;
}
</style>./keyStyle/key1.vue
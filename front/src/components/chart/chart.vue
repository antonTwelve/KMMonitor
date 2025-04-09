<script setup>
import { onMounted, ref } from "vue";
import { find_data_from_list, time_round } from "./util.js"
import { draw_background, draw_curve, draw_cursor, draw_text } from "./paint.js"

//TODO:再有BUG就重构吧, 乱七八糟, 没救了

const chart_canvas = ref(null);
const cache_canvas = ref(null);
//起止时间(时间戳), 是否是固定不变的时间区间
var start_time = undefined, end_time = undefined, fixed_time = false;
//y轴, 按键计数 
var y_list = null, y_coor = null;
//x轴, 时间戳
var x_list = null, x_coor = null;
//渲染的数据的起止时间, 必须是整分钟的时间
var render_data_start_time = 0, render_data_end_time = 0;
//当前渲染的数据中的最大值
var current_max_y = 0;
//y轴1单位长度对应的像素长度
var pix_per_count = 0;
//当前鼠标所处位置的时间
var mouse_time = null;
var mouse_x, mouse_y;
//鼠标按下, 用于拖动事件
var mouse_down = false;
var mouse_down_offsetX = 0, mouse_down_start_time = 0, mouse_down_time_range = 0;;
//绘制耗时(ms)
var draw_time = 0;
//初始化完成标记, 避免鼠标事件在初始化完成之前触发
var ready = false;
//缩放倍率
var scale_ratio = 1;

onMounted(() => {
    chart_canvas.value.addEventListener("mouseleave", () => {
        if (!ready) return;
        //鼠标移出, 当前索引设置为空, 重新绘图
        mouse_time = null;
        mouse_down = false;
        draw("show_info");
    });

    chart_canvas.value.addEventListener("mousemove", (e) => {
        if (e.offsetX < 0 || !ready) return;
        let offsetx = e.offsetX * scale_ratio, offsety = e.offsetY * scale_ratio;
        let target_width = e.target.width;
        let new_mouse_time = time_round(parseInt(render_data_start_time + offsetx * (render_data_end_time - render_data_start_time) / target_width));
        mouse_x = offsetx;
        mouse_y = offsety;
        mouse_time = new_mouse_time;
        if (mouse_down) {
            let dx = mouse_down_offsetX - offsetx;
            let new_start_time = mouse_down_start_time + dx * mouse_down_time_range / target_width;
            let new_end_time = new_start_time + mouse_down_time_range;
            if (new_start_time > start_time && new_end_time < end_time) {
                //新的时间在范围内, 需要刷新, 否则当作正常的鼠标移动处理
                render_data_start_time = new_start_time;
                render_data_end_time = new_end_time;
                draw("move");
                return;
            }
        }
        draw("show_info");
    });

    chart_canvas.value.addEventListener('mousedown', function (event) {
        if (!ready) return;
        mouse_down = true;
        mouse_down_offsetX = event.offsetX * scale_ratio;
        mouse_down_start_time = render_data_start_time;
        mouse_down_time_range = render_data_end_time - render_data_start_time;
    });

    chart_canvas.value.addEventListener('mouseup', function (event) {
        mouse_down = false;
    });

    chart_canvas.value.addEventListener("wheel", function (event) {
        if (mouse_down || !ready) return;  //禁止按下时缩放
        event.preventDefault();
        let zoom_rate = 1.1;
        let l = render_data_end_time - render_data_start_time;
        if (event.deltaY < 0) {
            //放大
            zoom_rate = 1.1;
        }
        else if (event.deltaY > 0) {
            //缩小
            zoom_rate = 0.9;
        }
        render_data_start_time = mouse_time - (mouse_time - render_data_start_time) / zoom_rate;
        render_data_end_time = render_data_start_time + l / zoom_rate;
        if (render_data_end_time > end_time) render_data_end_time = end_time;
        if (render_data_start_time < start_time) render_data_start_time = start_time;
        draw("resize");
    })

    window.addEventListener("resize", () => {
        if (!ready) return;
        draw("resize");
    });
    scale_ratio = window.devicePixelRatio;
})

/**
 * 判断一个时间是否在选定的范围内
 * @param {Date} time 需要判断的时间, Date对象
 */
function is_time_in_range(time) {
    if (start_time === undefined) return true;
    let time_stamp = time.getTime();
    if (end_time === undefined) return time_stamp >= start_time;
    return (time_stamp >= start_time && time_stamp <= end_time)
}

/**
 * 根据x与y值重新计算所有点的坐标
 */
function calc_coor(width, height) {
    x_coor = [];
    y_coor = [];
    let start_index, end_index;

    //找到渲染数据的起止索引
    //TODO:二分找
    for (start_index = 0; start_index < x_list.length; start_index++)
        if (x_list[start_index] >= render_data_start_time)
            break;
    for (end_index = start_index; end_index < x_list.length; end_index++)
        if (x_list[end_index] >= render_data_end_time)
            break;
    let x_len = render_data_end_time - render_data_start_time;
    current_max_y = Math.max(...y_list) * 1.1;  //适当放大, 在图表顶部留下空间
    pix_per_count = height / current_max_y;
    if (render_data_start_time != x_list[start_index]) {
        x_coor.push(0);
        y_coor.push(height);
        x_coor.push(width * (x_list[start_index] - 60000 - render_data_start_time) / x_len);
        y_coor.push(height);
    }
    for (let i = start_index; i <= end_index; i++) {
        if (i > start_index && (x_list[i] - x_list[i - 1] > 60000)) {
            //两点相差一分钟以上, 在两点之间插入两个y轴为0的点
            x_coor.push(width * (x_list[i - 1] + 60000 - render_data_start_time) / x_len);
            x_coor.push(width * (x_list[i] - 60000 - render_data_start_time) / x_len);
            y_coor.push(height);
            y_coor.push(height);
        }
        let new_x_coor = parseInt(width * (x_list[i] - render_data_start_time) / x_len);
        if (i == start_index || (i > start_index && new_x_coor != x_coor[x_coor.length - 1])) {
            x_coor.push(width * (x_list[i] - render_data_start_time) / x_len);
            y_coor.push(height * (1 - y_list[i] / current_max_y));
        }
    }
}

/**
 * 刷新, 绘制图形
 * 传入的参数表示当前是哪种情况下的重新绘制, 影响坐标更新
 * resize: 页面缩放
 * scale: 滚轮缩放数据范围
 * replace: 新数据替换最后一条数据
 * update: 新插入一条数据
 * init: 初始化
 * show_info: 鼠标移入, 显示数据
 */
function draw(draw_state) {
    const draw_start_time = new Date();
    scale_ratio = window.devicePixelRatio;
    let canvas_ele = chart_canvas.value;
    let cache_canvas_ele = cache_canvas.value;
    //获取画布大小
    let canvas_rect = canvas_ele.getBoundingClientRect();
    let canvas_height = canvas_rect.height * scale_ratio;
    let canvas_width = canvas_rect.width * scale_ratio;
    //设置画布大小和元素大小相同
    if (canvas_ele.height != parseInt(canvas_height) || canvas_ele.width != parseInt(canvas_width)) {
        canvas_ele.height = canvas_height;
        canvas_ele.width = canvas_width;
        cache_canvas_ele.height = canvas_height;
        cache_canvas_ele.width = canvas_width;
    }
    //计算绘图区域大小
    let height = canvas_height - 30;
    let width = canvas_width;
    let ctx = canvas_ele.getContext("2d");
    let cache_ctx = cache_canvas_ele.getContext("2d");
    ctx.clearRect(0, 0, canvas_width, canvas_height);

    //============================================================
    //* 计算各点坐标
    //* 不同状态下重新绘制时采用不同的坐标更新策略
    //------------------------------------------------------------
    switch (draw_state) {
        case "resize":
        case "scale":
        case "init":
        case "move":
        case "replace":
        case "update":
            calc_coor(width, height);
            break;
        case "show_info":
        //显示数据, 不需要重新计算坐标
        default:
            break;
    }
    //============================================================
    if (draw_state != "show_info") {
        cache_ctx.clearRect(0, 0, canvas_width, canvas_height);
        //* 绘制背景方格
        draw_background(cache_ctx, pix_per_count, width, height);

        //* 绘制折线
        draw_curve(cache_ctx, x_coor, y_coor, width, height);
    }
    ctx.drawImage(cache_canvas_ele, 0, 0);

    //* 绘制当前鼠标所指数据信息
    if (mouse_time != null) {
        let mouse_x_coor = width * (mouse_time - render_data_start_time) / (render_data_end_time - render_data_start_time);
        let count = find_data_from_list(x_list, y_list, mouse_time);
        draw_cursor(ctx, width, height, mouse_x_coor, mouse_x, mouse_y, mouse_time, count);
    }

    const draw_end_time = new Date();
    draw_time = draw_end_time - draw_start_time;

    //* 绘制一些调试信息
    // draw_text(ctx, 0, 12, "#aaaaab", "绘制时间: " + draw_time + "ms");
}

/**
 * 更新数据
 * @param data 新数据
 * @param replace true为替换旧数据, false为插入新数据, 默认为true
 */
function update(data, replace = true) {
    if (fixed_time) return;
    //替换
    if (replace) {
        //和之前的数据相同, 不需要更新
        if (data.press_count === y_list[y_list.length - 1]) return;
        y_list[y_list.length - 1] = data.press_count;
        draw("replace");
        return;
    }
    //新数据
    y_list.push(data.press_count);
    x_list.push(data.time.getTime());
    if (render_data_end_time === data.time.getTime() - 60000)
        render_data_end_time = data.time.getTime();
    end_time = data.time.getTime();
    //重新绘制
    draw("update");
}

/**
 * 传入所有数据, 进行初始化
 * all_data需要是按时间排序的数据
 */
function init(all_data, start = undefined, end = undefined) {
    ready = false;
    //根据起止时间提取、复制数据
    start_time = start === undefined ? all_data[0].time.getTime() : start;
    end_time = end === undefined ? all_data[all_data.length - 1].time.getTime() : end;
    if (start != undefined && end != undefined) fixed_time = true;

    y_list = [];
    x_list = [];
    all_data.forEach((data) => {
        if (is_time_in_range(data.time)) {
            y_list.push(data.press_count);
            x_list.push(data.time.getTime());
        }
    });
    if (start != undefined && x_list[0] != start) {
        x_list.unshift(start);
        y_list.unshift(0);
    }
    render_data_start_time = start_time;
    render_data_end_time = end_time;
    draw("init");
    ready = true;
}

defineExpose({
    update,
    init
})

</script>
<template>
    <canvas ref="chart_canvas" class="chart_canvas"></canvas>
    <canvas ref="cache_canvas" class="cache_canvas"></canvas>
</template>
<style scoped>
.chart_canvas {
    height: 100%;
    width: 100%;
}

.cache_canvas {
    display: none;
}
</style>
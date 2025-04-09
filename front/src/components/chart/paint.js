import { time_format } from "./util.js";

/**
 * 绘制一个半透明矩形
 * @param {*} ctx 
 * @param {*} x 
 * @param {*} y 
 * @param {*} width 
 * @param {*} height 
 * @param {*} radius 
 */
function draw_round_rect(ctx, x, y, width, height, radius) {
    ctx.globalAlpha = 0.3;
    ctx.beginPath();
    ctx.moveTo(x + radius, y);
    ctx.arcTo(x + width, y, x + width, y + height, radius);
    ctx.arcTo(x + width, y + height, x, y + height, radius);
    ctx.arcTo(x, y + height, x, y, radius);
    ctx.arcTo(x, y, x + width, y, radius);
    ctx.closePath();
    ctx.fillStyle = "white";
    ctx.fill();
    ctx.globalAlpha = 1;
}

/**
 * 绘制文本
 * @param {*} ctx 
 * @param {*} x 
 * @param {*} y 
 * @param {*} color 
 * @param {*} text 
 * @param {*} font_size 
 */
function draw_text(ctx, x, y, color, text, font_size = 12) {
    ctx.font = `${font_size}px Arial`;
    ctx.fillStyle = color;
    ctx.fillText(text, x, y);
}

/**
 * 绘制背景方格
 * @param {*} ctx 
 * @param {*} pix_per_count 
 * @param {*} width 
 * @param {*} height 
 */
function draw_background(ctx, pix_per_count, width, height) {
    ctx.globalAlpha = 0.3;
    ctx.beginPath();
    let grid_size = pix_per_count * 10;

    //y轴
    for (let i = height; i > 0; i -= grid_size) {
        ctx.moveTo(0, i);
        ctx.lineTo(width, i);
    }
    //x轴
    for (let i = grid_size; i < width; i += grid_size) {
        ctx.moveTo(i, 0);
        ctx.lineTo(i, height);
    }

    ctx.lineWidth = 0.5;
    ctx.strokeStyle = "#909191";
    ctx.stroke();
    ctx.globalAlpha = 1;
}

/**
 * 绘制曲线(其实是折线啦...)
 * @param {Canvas Context} ctx 从Canvas获取的Context
 * @param {Array} coor_x 所有x轴坐标
 * @param {Array} coor_y 所有y轴坐标
 * @param {Number} width 绘画区域宽度
 * @param {Number} height 绘画区域高度
 */
function draw_curve(ctx, coor_x, coor_y, width, height) {
    if (!coor_y.length > 0) return;
    ctx.beginPath();
    ctx.moveTo(0, coor_y[0]);
    for (let i = 0; i < coor_x.length; i++) {
        ctx.lineTo(coor_x[i], coor_y[i]);
    }
    ctx.lineWidth = 1;
    ctx.strokeStyle = "#42b983";
    ctx.stroke();
    var gradient = ctx.createLinearGradient(0, 0, 0, height);
    gradient.addColorStop(0, "rgba(66,185,131, 1)");
    gradient.addColorStop(1, "rgba(24, 24, 24, 0.5)");
    ctx.lineTo(width, height);
    ctx.lineTo(0, height);
    ctx.closePath();
    ctx.fillStyle = gradient;
    ctx.fill();
}

/**
 * 显示当前鼠标所在位置的数据
 * @param {*} ctx 
 * @param {*} width 
 * @param {*} height 
 * @param {*} line_x_coor 
 * @param {*} mouse_x 
 * @param {*} mouse_y 
 * @param {*} time 
 * @param {*} count 
 */
function draw_cursor(ctx, width, height, line_x_coor, mouse_x, mouse_y, time, count) {
    ctx.beginPath();
    ctx.moveTo(line_x_coor, 0);
    ctx.lineTo(line_x_coor, height);
    ctx.lineWidth = 1;
    ctx.strokeStyle = "#42b983";
    ctx.stroke();
    let r_c_x = mouse_x, r_c_y = mouse_y;
    let r_c_width = 130, r_c_height = 40;
    if (r_c_x + r_c_width > width) r_c_x = mouse_x - r_c_width - 5;
    if (r_c_y + r_c_height > height) r_c_y = mouse_y - r_c_height;
    draw_round_rect(ctx, r_c_x, r_c_y, r_c_width, r_c_height, 4);
    draw_text(ctx, r_c_x + 10, r_c_y + 16, "#aaaaab", time_format(new Date(time)), 14);
    draw_text(ctx, r_c_x + 10, r_c_y + 32, "#aaaaab", count + "次", 14);
}

export {
    draw_background,
    draw_curve,
    draw_cursor,
    draw_text
}
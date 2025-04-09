class count_mouse_data {
    constructor() {
        this.left_count = 0;
        this.right_count = 0;
        this.scroll_count = 0;
        this.scroll_press_count = 0;
        this.move_distance = 0;
    }

    /**
     * y = a + b;
     * y, a, b可以是count_mouse_data对象, 也可以是mouseData对象
     * 二者兼容
     * @param {*} y 
     * @param {*} a 
     * @param {*} b 
     */
    static plus(y, a, b) {
        y.left_count = a.left_count + b.left_count;
        y.right_count = a.right_count + b.right_count;
        y.scroll_count = a.scroll_count + b.scroll_count;
        y.scroll_press_count = a.scroll_press_count + b.scroll_press_count;
        y.move_distance = a.move_distance + b.move_distance;
    }

    static sub(y, a, b) {
        y.left_count = a.left_count - b.left_count;
        y.right_count = a.right_count - b.right_count;
        y.scroll_count = a.scroll_count - b.scroll_count;
        y.scroll_press_count = a.scroll_press_count - b.scroll_press_count;
        y.move_distance = a.move_distance - b.move_distance;
    }

    clear() {
        this.left_count = 0;
        this.right_count = 0;
        this.scroll_count = 0;
        this.scroll_press_count = 0;
        this.move_distance = 0;
    }
}

function distance_format(distance){
    let temp_distance = distance;
    const units = ["", "K", "M"];
    let unit = 0;
    while (temp_distance >= 1000) {
        temp_distance = (temp_distance / 1000).toFixed(2);
        unit += 1;
        if (unit >= units.length - 1) break;
    }
    return temp_distance + units[unit];
}

export {
    count_mouse_data,
    distance_format
}
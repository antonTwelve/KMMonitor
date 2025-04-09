/**
 * 时间格式化
 * @param {Date} date 
 */
function time_format(date) {
    var year = date.getFullYear();
    var month = date.getMonth() + 1;
    var day = date.getDate();
    var hours = date.getHours();
    var minutes = date.getMinutes();

    var timeString = `${year}-${month < 10 ? '0' + month : month}-${day < 10 ? '0' + day : day} ${hours < 10 ? '0' + hours : hours}:${minutes < 10 ? '0' + minutes : minutes}`;

    return timeString;
}

/**
 * 根据时间返回对应时间的数据, 未找到返回0
 * 要求数据按时间排序
 * @param {*} time_arr  时间戳列表
 * @param {*} count_arr 数据列表
 * @param {*} time 
 */
function find_data_from_list(time_arr, count_arr, time) {
    //TODO:优化成二分
    for (let i = 0; i < time_arr.length; i++) {
        if (time_arr[i] === time) {
            return count_arr[i];
        }
        else if (time_arr[i] > time) return 0;
    }
    return 0;
}

/**
 * 将时间在分钟上四舍五入
 * 秒大于等于30加一分钟
 * 之后秒、毫秒均设置为0
 * @param {*} time 时间戳
 * @returns 新的时间戳
 */
function time_round(time) {
    let new_time = new Date(time);
    if (new_time.getSeconds() >= 30) new_time.setMinutes(new_time.getMinutes() + 1);
    new_time.setSeconds(0);
    new_time.setMilliseconds(0);
    return new_time.getTime();
}

export {
    time_format,
    find_data_from_list,
    time_round
};
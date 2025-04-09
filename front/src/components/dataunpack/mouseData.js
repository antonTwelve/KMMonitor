class mouseData {
	constructor() {
		this.time = undefined;
		this.left_count = 0;
		this.right_count = 0;
		this.move_distance = 0;
		this.scroll_count = 0;
		this.scroll_press_count = 0;
	}

	load_data(binary_data, index = 0) {
		let data_view = new DataView(binary_data.buffer);

		let timestamp = Number(data_view.getBigUint64(index, true));
		this.time = new Date(timestamp * 1000);
		index += 8;

		this.left_count = Number(data_view.getUint16(index, true));
		index += 2;

		this.right_count = Number(data_view.getUint16(index, true));
		index += 2;

		this.move_distance = Number(data_view.getFloat64(index, true));
		index += 8;

		this.scroll_count = Number(data_view.getUint16(index, true));
		index += 2;

		this.scroll_press_count = Number(data_view.getUint16(index, true));
		index += 2;
		return index;
	}

	init() {
		this.time = new Date();
	}

	//将一组原始二进制数据转换为mouseData对象数组
	static binary_array_to_object(binary_data) {
		const mouse_datas = [];
		let m_index = 0;
		while (m_index < binary_data.length) {
			let m = new mouseData();
			m_index = m.load_data(binary_data, m_index);
			mouse_datas.push(m);
		}
		return mouse_datas;
	}
}

export default mouseData;

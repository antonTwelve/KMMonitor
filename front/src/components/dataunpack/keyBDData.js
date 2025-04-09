class keyBDData {
	constructor() {
		this.time = undefined;
		this.press_count = 0;
		this.keyBD_count = new Map();
	}

	load_data(binary_data, index = 0) {
		let data_view = new DataView(binary_data.buffer);

		let timestamp = Number(data_view.getBigUint64(index, true));
		this.time = new Date(timestamp * 1000);
		index += 8;

		this.press_count = Number(data_view.getUint16(index, true));
		index += 2;

		let key = 0,
			count = 0;
		while (index < binary_data.length) {
			key = Number(data_view.getUint16(index, true));
			index += 2;
			if (key >= 0xffff) return index;

			count = Number(data_view.getUint16(index, true));
			index += 2;
			this.keyBD_count.set(key, count);
		}
		throw "keyboard data not end with 0xffff, search error code:28973";
	}

	init() {
		this.time = new Date();
	}

	//将一组原始二进制数据转换为keyBDDate对象数组
	static binary_array_to_object(binary_data) {
		const keyBD_datas = [];
		let k_index = 0;
		while (k_index < binary_data.length) {
			let k = new keyBDData();
			k_index = k.load_data(binary_data, k_index);
			keyBD_datas.push(k);
		}
		return keyBD_datas;
	}
}

export default keyBDData;

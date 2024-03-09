# KMMonitor
一个键鼠使用次数记录工具

基于vue和libhv

界面如下：
![image](https://github.com/antonTwelve/KMMonitor/assets/88929910/f4ffa6cc-dddc-4249-ac8a-a8377d9adefd)


**如何启动**
解压到你要放这玩意的地方，双击exe，完事。
启动后不会有界面，到右下角找托盘图标点击打开。
![image](https://github.com/antonTwelve/KMMonitor/assets/88929910/6ced1f10-4c38-4d54-937b-843be1e1fb39)

**如何设置开机自启动**
启动之后右下角找托盘图标，右键，点开机自启动。
![image](https://github.com/antonTwelve/KMMonitor/assets/88929910/ccc1c88e-304a-49d9-9673-84d99a0def74)

**开源吗**
暂时不开
开毛线
代码写的跟屎一样
水平不如GPT
这代码我自己都看不懂


注意：
前端会一次加载所有数据，理论上应该后端处理数据再传输，但我还没写。。。所以当前版本连续用个一两年的话前端估计会因为数据过多卡死。
一两个月的数据应该不会卡，如果卡了到exe目录里删掉mouseData和keyBDData这俩文件，这俩就是保存的数据，删了就清空了，啥都没了哈。

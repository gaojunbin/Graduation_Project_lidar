Vgg16 在运行速度上：
（考虑点云实时视频流处理）
Vgg16 params: 15M
Vgg16 MACCs:  8380M
Vgg16 MAes:   840M

224*224三通道图片在CPU上运行效率约80ms
加上数据预处理的过程，大约每秒可达10帧
远远超过激光雷达点云采集速率




参考链接：
http://machinethink.net/blog/how-fast-is-my-model/
https://blog.csdn.net/leayc/article/details/81001801
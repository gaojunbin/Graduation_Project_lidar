'''
* @file    Draw_Cloud_Figure.py
* @author  Junbin Gao
* @data    2019.12
* @version v1.1.0
* @notes   None
'''

from PIL import Image
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import os

np.set_printoptions(threshold=np.inf)  # 避免打印较大数组的时候用省略号代替

def Translate_Array_Image():
    '''
    @ 功能：学习数组和图片互转，没什么功能
    '''
    image_dir = './fig.png'    #图片路径

    x = Image.open(image_dir)  # 打开图片
    data = np.asarray(x)       # 转换为矩阵
    # print(data.dtype)        # 查看变量的类型
    # print(data.shape)        # 查看数组的形状

    output = open('data.txt', 'w', encoding='gbk')   # 创建文件，用于保存上述转化后的矩阵
    output.write(str(data))                          # 写入数据

    image = Image.fromarray(data)   # 将之前的矩阵转换为图片
    image.show()                    # 调用本地软件显示图片


def study_pandas_and_numpy():
    '''
    @ 功能：熟悉padans和numpy，没什么功能
    '''
    data = pd.read_csv("ceshi.csv")  # 读取csv文件数据，类型为DateFram
    # print(data['x'])               # 展示了如何表示特定索引列，类似于字典
    # data2 = np.random.randn(6,4)   
    # print(data.values)             # 如何将DataFrame类型转化成列表

    out1 = data.Y.astype('uint8')    # 如何进行数据类型强制转换
    out2 = data.Z.astype('uint8')    # 如何读取特定索引的列或者行
    out3 = data.R.astype('uint8')    # RGB数据类型必须为uint8，否则不可以将数组直接成图，会报错
    out = pd.concat([out1, out2, out3], axis=1)  # 如何将DateFrame数据合并，具体参数可查阅该函数
    output = out.values.reshape(-1, 50, 3)       # 如何将一维数组重组成三维数组，用以表示二维三通道的图像
    real = out[out.Y==3].empty                   # 如何限定索引条件，以及如何判断DateFrame的变量是否为空，为空返回true，非空返回false

    real2 = out[out.Y==3][out[out.Y==3].Z==0]    # 如何同时限定两个索引

    # print(output.dtype)
    # print(output)
    # output2 = np.asarray(output)



def Draw_With_Image():
    '''
    @ 功能：通过pandas操作csv文件，形成数组，而后通过Image库直接将数组转换成图片
    @ 优点：暂时没想到优点，或许对学习这些东西有帮助，没有实用性和可操作性
    @ 缺点：需要通过在文件中找到x，y的边缘位置，从而确定图像的像素点数量；
           需要通过搜索画布中每个像素在表格中的通道强度，需要花费非常非常长的时间；
    '''
    filepath = "data.csv"            # 需要绘制的文件路径
    data = pd.read_csv(filepath)            # 打开文件
    data.replace(np.nan, 0, inplace=True)   # 将数据为nan的地方替换为0
    data_y = ((data.y)).astype('int')       # 获取文件中的特定列数据，并转化数据类型
    data_z = ((data.z)).astype('int')       # 在这里获取y,z坐标
    reflectivity = (data.reflectivity).astype('uint8')                      # 获取强度信息
    new_cloud = pd.concat([data_y, data_z, reflectivity], axis=1)           # 提取有效信息并合并

    new_cloud.drop_duplicates(subset=['y','z'],keep='first',inplace=True)   # 去除y、z重复数据，只取第一次出现的数据，这里比较简单粗暴，后续可以优化，对x距离信息进行滤波
 
    y_arange = np.max(data_y)-np.min(data_y)+1          # 图像的长
    z_arange = np.max(data_z)-np.min(data_z)+1          # 图像的宽
    print(y_arange,z_arange)                            # 输入图像大小信息
    image = np.ones((y_arange,z_arange),dtype='uint8')  # 初始化图像数组，这里是单通道图像，若是三通道图像使用初始化成(x,x,3)
    print("创建数组完成")

    for i in np.arange(np.min(data_y),np.max(data_y)+1):        # 对y数据进行扫描
        for j in np.arange(np.min(data_z),np.max(data_z)+1):    # 对z数据进行扫描
            if (j-np.min(data_z)) % 500==0:
                print("执行已完成%d/%d"%(i-np.min(data_y),y_arange))      # 反馈程序执行情况
            if not new_cloud[new_cloud.y==i][new_cloud[new_cloud.y==i].z==j].empty:     # 若该位置的点被采集到，则使用反馈信息
                image[i-np.min(data_y)][j-np.min(data_z)] = new_cloud[new_cloud.y==i][new_cloud[new_cloud.y==i].z==j].reflectivity.values[0]
            else:      # 若没采集到，则赋为0，优化程序后，可以初始化数组为0，没采集到则不操作。或者后续可以修改点云数据显示的颜色，暂定使用单通道黑白像
                image[i-np.min(data_y)][j-np.min(data_z)] = 0
    print("转换已完成")
    fig = Image.fromarray(image)  # 将之前的矩阵转换为图片
    fig.show()                    # 调用本地软件显示图片


def Draw_With_Pyplot(filename):
    '''
    @ 功能：通过pandas读取csv文件，而后直接通过Pyplot库瞄点画出
    @ para:
        filename 读取的csv文件名及对应输出的图片文件名
    @ 优点：简单、快速
    @ 缺点：暂时没想到什么缺点
    '''
    filepath = filename                     # 需要绘制的csv文件路径
    figpath  = filename+".png"              # 需要保存的png文件路径
    data = pd.read_csv(filepath)            # 使用pandas库读取csv文件内容
    data.replace(np.nan, 0, inplace=True)   # 将数据为nan的地方替换为0
    data_y = ((data.y)*1000).astype('int64').values 
    data_z = ((data.z)*1000).astype('int64').values  
    reflectivity = (data.reflectivity).astype('uint8').values

    plt.figure(dpi=200,figsize=(5,3),frameon=False)          # 设置窗口大小，像素
    plt.scatter(data_y,data_z,s=1,c=reflectivity,alpha=0.5)  # 绘制散点函数，具体参数意义需要的话请自行查阅

    plt.axes().get_xaxis().set_visible(False)   # 隐藏坐标轴
    plt.axes().get_yaxis().set_visible(False)   # 隐藏坐标轴
    
    plt.title(filename)         #设置名称
    plt.savefig(figpath)        #图片保存
    # plt.show()                #图片显示
    print(filename+"处理完毕")

def draw_batch(dataset_path):
    name = os.listdir(dataset_path)
    for eachname in name:
        Draw_With_Pyplot(dataset_path+'/'+eachname)


if __name__ == "__main__":
    '''
    @ 主程序开始部分
    '''
    draw_batch("./data")
    

## Organize Codes in Graduation-Project（updating...）
```
@Author  Junbin Gao 
@data    2019.12
@version v1.0.0
```

+ The use of Livox

[click me to DJI_Livox homeweb](https://www.livoxtech.com/)

Before the start, please visit the web above to download the necessary documents and softwares(Livox Viewer & Livox SDK) to the computer.

In this project, we only should know how to use the Host computer to view the cloud data figures and use the SDK to copy the cloud data saving as csv files on our computer.

First about how to set the GigE to connect the Livox. I will not describe too much on this part. Please read the download pdf and follow its step. Attention: as long as you can connect the lidar, you can go down and you needn't set static IP if you only use it for experiments.

Then about Host computer, here called Livox Viewer, it is easy to use because of Visualization. As far as I am concerned, I use it at Linux, such as ubuntu16.04 or others. After you download the Livox Viewer, at the Viewer root directory, open the terminal. Then run the ```.sh``` document. You can see the software interface. Of cause, it can also run at Windows. About how to use the software, please reference the document you download or search it on baidu. I believe it is so easy. In our project, we usually use it to intuitively see the figures the lidar gets, what's more, we can set the control modles by it easily. When we want to get data set from SDK, we should first use the Viewer to see and choose the needed area.

Finally about the Livox SDK, this is written with C language, and made by cmake. If you first use cmake, please install cmake and learn some basic skills about using cmake. Here is a web you can see to learn. [click me to learn cmake](https://blog.csdn.net/whahu1989/article/details/82078563)
The cloud-data-copy codes is in the c document ```mylidar/main.c```, and its makefile is also in the same directory. I upload the file I write(my_lidar) and the whole project(Livox-SDK). You can download the mylidar directory to your computer's SDK projects, but don't forget add needed information to the root makefile.txt. About the the specific content of main.c, I will decribe in the code file.

+ some pyhon files to deal with the data sets

Draw Cloud Figure：through this file, you can use csv files got from lidar to generate 2D images. This can be batch processing.

About Image mode and Image format: If your images is got from screenshots, they are may be RGBA mode, through this file, you can change them to RGB mode.

+ all codes about deep learning

To be continued......



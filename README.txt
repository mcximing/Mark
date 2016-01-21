
Instructions
=============

Purpose:
------------
This program is for manually labeling/mark a rectangular target region on an 
image or a video frame. The target regions are saved to stand-alone images. 
This requirement is often known in computer vision area, as in the case of 
collecting training samples. 

Functions:
------------
There are three applications:

1. Open a single image, drag a box with mouse and save the target region.
2. Open a video file and navigate for particular frames, label and save.
3. Open a folder and read the images in this folder, label and save.

A special labeling way: click mouse and generate a sample box with fixed size.

You can use the mouse left-click to draw the box, right-click to clear and 
double-click to save. Navigating with scrolling is supported in video frames.
Ctrl+S is the same with Save.

For more details please see instructions in Chinese below.


Update log:
============

2016.01
Rebuild by Visual Studio Community 2013 + OpenCV 2.4.8.
Upload to Github with BSD License.

2014.05
Add: read configurations from ini file.

2013.08
1. Suppport selecting multiple boxes in a scene and save together.
2. Change the format of output TXT file according to requirements.

2013.06
Fix: The output error caused by Chinese words in the file path.

2013.03 
1. Implement the fixed-size labeling box mode.
2. Fror high resolution images, now we can save at original size.
3. Add a setting for outputting a whole image (for frame capturing in video).

2013.01
Add the function of outputting the coordinates to TXT file.

2012.12 
Basic function implementation.


This program is distributed under the BSD 3-clause License.
Author: mcximing@sina.cn

================================================================================


标注程序使用说明
================

用途介绍：
针对一幅图片或者视频中的某一帧，由操作者手工标注出目标区域，将目标区域保存成图片，可用于图像训练样本采集等。


功能说明：
应需求，有三种模式：

1、点击按钮打开单张图像，用鼠标在图片上按下左键，拖拽出矩形框，将目标区域包含其中，保存框内的目标图像。
2、打开视频文件，按前进后退，在其中浏览选取特定的帧，以同样方式标注和保存。
3、顺序读取待标注文件夹中所有图像，点击“顺次读取”按钮即开始读取指定文件夹下的图像文件。

又：2013年3月更新时，扩充一种标注方式：
鼠标在图上点击一下，可按照预先设定大小，以点击处为中心生成标注框。此功能使用前需在设置选项中开启。

一般操作方式：鼠标滚轮浏览，左键框选，右键清除，双击保存。

个别操作实现了键盘映射以方便使用：

Ctrl+S，保存标注；
Ctrl+右箭头，视频前进；
Ctrl+左箭头，视频后退。
（相当于点击对应的按钮）


此外，鼠标滚轮支持的操作有：

打开视频后，操作对视频帧的前进后退，相当于按那两个按钮；
遍历文件夹时，向下滚动鼠标滚轮相当于顺次向下读取图片，不能倒退读取。

如果觉得滚轮响应碍事可以去掉界面上的勾选。


补充说明：

支持一次标注多个矩形框，可以在同一张图片上反复勾选保存。（2013.8 更新支持多个）
如果直接勾选多个框后一齐保存，则输出TXT记录时会将其记录到同一行中。
其中，单张图片时输出文件与图片在一起保存，浏览文件夹和视频时都是TXT文件保存两份，一在标注图那里，一在源文件那里

保存方法有三种：点击“保存标注”按钮，点击鼠标右键，或按键盘CTRL+S。（效果相同）
如果打开单张图片进行标注，保存标注时会询问保存位置；
打开视频文件或遍历图片文件夹进行标注时，会默认保存至程序exe文件所在目录下的ImgDst文件夹（自动创建）；
第三种模式下默认遍历读取的图片文件夹目录是程序exe文件所在目录下的ImgSrc文件夹。

如需修改两个目录，可在界面下方的路径处点击按钮修改。

自动保存的标注图片命名方式为：原视频文件名+时间戳（年月日时分秒）+视频当前帧序。
标注图片的保存格式为.jpg。

如想从视频的某一帧开始向后浏览，请在打开视频文件后，修改“当前帧”，再按“前进”就会从这个帧次开始向后读。

打开高清视频时，因要将图像缩放显示，每一帧的显示会有点延迟，所以不要前进太快，如需快速浏览请加大每次跳的帧数间隔。

程序图片框的分辨率稍大于720X576，如果视频文件分辨率大于这个值，标注的图片会以缩放后的实际显示图片为准进行截取。
（2013年3月修正此方面，默认在选项中勾选“保存标注时按原图比例保存”，此时可以从原图提取标注内容，不受缩放显示的影响。）


其他扩充功能（在设置选项中可选的其他输出方式）：

1、输出TXT记录文件，记录每张图片上标注的矩形坐标，用于和其他程序可能的交互（单张图片不支持输出TXT）；
2、直接输出原尺寸的图：此功能可用于视频按帧截图，亦可用于在文件夹中筛选图片；
3、输出整张图片，不过在标注框内的部分被黑色覆盖，即简单的局部屏蔽处理。



更新记录：

2016.01
用 Visual Studio Community 2013 + OpenCV 2.4.8 重新编译，以BSD许可证放到Github上。

2014.05
增加：配置从INI文件读取，以方便从外部修改起始设置

2013.08
1、工作流程改进，支持一次选择多个标注框并一起保存
2、按特定需求修改输出TXT记录文件格式，一次保存多个标注框将写入同一行记录

2013.06
BUG修复：中文名长度计算错误导致的输出截断

2013.03 
1、实现一次点击选中一个预定大小方框的标注模式
2、保存高清图片的标注时可以按原图大小保存
3、输出整张图片，可选做局部屏蔽处理

2013.01
增加设置选项，输出图片或记录两种输出方式可选

2012.12 
基本标注功能实现


如有缺陷BUG等请随时反馈。
mcximing@sina.cn

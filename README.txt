
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


��ע����ʹ��˵��
================

��;���ܣ�
���һ��ͼƬ������Ƶ�е�ĳһ֡���ɲ������ֹ���ע��Ŀ�����򣬽�Ŀ�����򱣴��ͼƬ��������ͼ��ѵ�������ɼ��ȡ�


����˵����
Ӧ����������ģʽ��

1�������ť�򿪵���ͼ���������ͼƬ�ϰ����������ק�����ο򣬽�Ŀ������������У�������ڵ�Ŀ��ͼ��
2������Ƶ�ļ�����ǰ�����ˣ����������ѡȡ�ض���֡����ͬ����ʽ��ע�ͱ��档
3��˳���ȡ����ע�ļ���������ͼ�񣬵����˳�ζ�ȡ����ť����ʼ��ȡָ���ļ����µ�ͼ���ļ���

�֣�2013��3�¸���ʱ������һ�ֱ�ע��ʽ��
�����ͼ�ϵ��һ�£��ɰ���Ԥ���趨��С���Ե����Ϊ�������ɱ�ע�򡣴˹���ʹ��ǰ��������ѡ���п�����

һ�������ʽ������������������ѡ���Ҽ������˫�����档

�������ʵ���˼���ӳ���Է���ʹ�ã�

Ctrl+S�������ע��
Ctrl+�Ҽ�ͷ����Ƶǰ����
Ctrl+���ͷ����Ƶ���ˡ�
���൱�ڵ����Ӧ�İ�ť��


���⣬������֧�ֵĲ����У�

����Ƶ�󣬲�������Ƶ֡��ǰ�����ˣ��൱�ڰ���������ť��
�����ļ���ʱ�����¹����������൱��˳�����¶�ȡͼƬ�����ܵ��˶�ȡ��

������ù�����Ӧ���¿���ȥ�������ϵĹ�ѡ��


����˵����

֧��һ�α�ע������ο򣬿�����ͬһ��ͼƬ�Ϸ�����ѡ���档��2013.8 ����֧�ֶ����
���ֱ�ӹ�ѡ������һ�뱣�棬�����TXT��¼ʱ�Ὣ���¼��ͬһ���С�
���У�����ͼƬʱ����ļ���ͼƬ��һ�𱣴棬����ļ��к���Ƶʱ����TXT�ļ��������ݣ�һ�ڱ�עͼ���һ��Դ�ļ�����

���淽�������֣�����������ע����ť���������Ҽ����򰴼���CTRL+S����Ч����ͬ��
����򿪵���ͼƬ���б�ע�������עʱ��ѯ�ʱ���λ�ã�
����Ƶ�ļ������ͼƬ�ļ��н��б�עʱ����Ĭ�ϱ���������exe�ļ�����Ŀ¼�µ�ImgDst�ļ��У��Զ���������
������ģʽ��Ĭ�ϱ�����ȡ��ͼƬ�ļ���Ŀ¼�ǳ���exe�ļ�����Ŀ¼�µ�ImgSrc�ļ��С�

�����޸�����Ŀ¼�����ڽ����·���·���������ť�޸ġ�

�Զ�����ı�עͼƬ������ʽΪ��ԭ��Ƶ�ļ���+ʱ�����������ʱ���룩+��Ƶ��ǰ֡��
��עͼƬ�ı����ʽΪ.jpg��

�������Ƶ��ĳһ֡��ʼ�����������ڴ���Ƶ�ļ����޸ġ���ǰ֡�����ٰ���ǰ�����ͻ�����֡�ο�ʼ������

�򿪸�����Ƶʱ����Ҫ��ͼ��������ʾ��ÿһ֡����ʾ���е��ӳ٣����Բ�Ҫǰ��̫�죬������������Ӵ�ÿ������֡�������

����ͼƬ��ķֱ����Դ���720X576�������Ƶ�ļ��ֱ��ʴ������ֵ����ע��ͼƬ�������ź��ʵ����ʾͼƬΪ׼���н�ȡ��
��2013��3�������˷��棬Ĭ����ѡ���й�ѡ�������עʱ��ԭͼ�������桱����ʱ���Դ�ԭͼ��ȡ��ע���ݣ�����������ʾ��Ӱ�졣��


�������书�ܣ�������ѡ���п�ѡ�����������ʽ����

1�����TXT��¼�ļ�����¼ÿ��ͼƬ�ϱ�ע�ľ������꣬���ں�����������ܵĽ���������ͼƬ��֧�����TXT����
2��ֱ�����ԭ�ߴ��ͼ���˹��ܿ�������Ƶ��֡��ͼ������������ļ�����ɸѡͼƬ��
3���������ͼƬ�������ڱ�ע���ڵĲ��ֱ���ɫ���ǣ����򵥵ľֲ����δ���



���¼�¼��

2016.01
�� Visual Studio Community 2013 + OpenCV 2.4.8 ���±��룬��BSD���֤�ŵ�Github�ϡ�

2014.05
���ӣ����ô�INI�ļ���ȡ���Է�����ⲿ�޸���ʼ����

2013.08
1���������̸Ľ���֧��һ��ѡ������ע��һ�𱣴�
2�����ض������޸����TXT��¼�ļ���ʽ��һ�α�������ע��д��ͬһ�м�¼

2013.06
BUG�޸������������ȼ�������µ�����ض�

2013.03 
1��ʵ��һ�ε��ѡ��һ��Ԥ����С����ı�עģʽ
2���������ͼƬ�ı�עʱ���԰�ԭͼ��С����
3���������ͼƬ����ѡ���ֲ����δ���

2013.01
��������ѡ����ͼƬ���¼���������ʽ��ѡ

2012.12 
������ע����ʵ��


����ȱ��BUG������ʱ������
mcximing@sina.cn

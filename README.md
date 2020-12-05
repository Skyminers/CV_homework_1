# 计算机视觉作业1

> 姓名：刘一辰   
> 学号：3180102886

## 一、视频流程

视频总共由 $3$ 部分构成，分别是:
1. 自己制作的以文字飞入、文字渐显两种动画为主的文字片头
2. 由 $5$ 张 *明日方舟* 图片组成的幻灯片部分。为其中第一张幻灯片的出现实现了渐显的动画，为两张幻灯片切换时实现了从下到上的滑动切换的切换动画。
3. 播放来自 *bilibili明日方舟PV* 的视频画面，由于原视频是 $30$ 帧而本视频是 $60$ 帧视频，于是对于原视频的每一帧都连续插入两帧在本视频中。

## 二、使用的库

代码调用了一些库函数来完成一些功能需求：
* 调用 freestyle 库完成对输出**中文**的功能需求
* 使用 VideoWriter 类来生成最终的视频
* 使用 Mat 矩阵类来完成对图片 BGR 信息的储存
* 使用 VideoCapture 类 来完成对输入视频的视频画面的捕捉

## 三、代码流程

首先需要通过 VideoWriter 类来对最终输出的视频进行参数设置：
设置输出文件名`fileName`,编码方式`fourcc`,视频帧率`FPS`,视频分辨率`frameSize`,视频彩色与否`isColor`

```c++
VideoWriter video;
Size frameSize = Size(WIDTH, HEIGHT);
string fileName = "video.mp4";
video = VideoWriter(fileName, VideoWriter::fourcc('D', 'I', 'V', 'X'), FPS, frameSize, true);
```

随后是对于使用的字体的设置，我这里使用了系统自带的**苹方**字体：

```c++
Ptr<freetype::FreeType2> font;
font = freetype::createFreeType2();
font->loadFontData("/System/Library/Fonts/PingFang.ttc",0);
```

---



下面一部分制作片头，我的片头由三行文字的动画效果组成，分别是：飞入、飞入、逐渐显示。

下面为飞入效果的第一行的代码：

```c++
Mat temp(frameSize,CV_8UC3);
Mat out = Mat::zeros(frameSize, CV_8UC3);
for(int i=1;i<=FPS;++i){     
    out = temp.clone();
    font->putText(out, "Doctor, 你醒啦",
                  Point(WIDTH/3,HEIGHT/3*((double)i/FPS)),
                  50,Scalar(255,255,255),-1,4,true);
    video.write(out);
}
temp = out.clone();
for(int i=1;i<=FPS;++i){
    video.write(temp);
}
cout << "First line." << endl;
```

主要的是在于对于文字位置的转化，动画时长为 1s ，根据 `i` 的值等比例计算出该帧时文字所在的位置，然后在相应位置上进行绘制。我在这里区分了基础层和输出层。
* 基础层：用于防止基本元素，是每一次输出时使用的基本层
* 输出层：每一次输出前都会从元素层进行深拷贝，动画效果是绘制在这一层的

代码中的动画效果绘制在`out`(输出层)，可以使得动画效果不污染原始层。

第二行字幕的飞入代码与上段代码类似。

第三行字幕的渐隐效果代码如下：

```c++
for (int j = 0; j < 2*FPS; ++j) {
    int scalar = (int)((j > FPS ? 1 : (double)j/FPS)*255);
    out = temp.clone();
    font->putText(out, "刘一辰 3180102886",
                  Point(WIDTH/3,3*HEIGHT/4),
                  32,Scalar(scalar,scalar,scalar),-1,4,true);
    video.write(out);
}
cout << "Third line." << endl;
```

动画效果时长为 1s ，通过 $\frac i{FPS}$ 来对应该段文字的透明度。由于 VideoWriter 只能传入 BGR 图片，所以将 alpha 直接作用到 BGR 的值中再传入。   
当透明度大于 1 时，设置为 1，因为 BGR 最大值不能超过 255 。停留 1s 以进行展示。

---

接下来是幻灯片切换的部分：

从幻灯片部分开始，每一次输出前都会加上:

```c++
font->putText(out, STDID, Point(WIDTH/5, HEIGHT-30), 28, Scalar(255, 255, 255), -1,4,true);
```

来进行水印制作

幻灯片部分，会搜索给定路径下的`1.jpg`，`2.jpg` ...   
当搜索不到 `i.jpg` 时，会终止搜索。

幻灯片部分代码如下：
```c++
Mat image;
Mat lastImage;
for (int i = 1; ; ++i) {
    stringstream str;
    str << path << '/' << i << ".jpg";
    Mat oriImg = imread(str.str());
    if(!oriImg.data){
        cout << "Found " << i-1 << " jpg files" << endl;
        break;
    }
    resize(oriImg, image, frameSize);
    if (i == 1) {
        //cvtColor(image, image, COLOR_BGR2BGRA);
        for (int j = 0; j < 2*FPS; ++j) { // 2s per image
            modifyAlpha(image, out, (double) j / FPS);
            font->putText(out, STDID, Point(WIDTH/5, HEIGHT-30), 28, Scalar(255, 255, 255), -1,4,true);
            video.write(out);
        }
        lastImage = image.clone();
    } else {
        // 1s per switch
        Mat out = Mat::zeros(frameSize, CV_8UC3);
        for (int j = 1; j <= 2*FPS; ++j) {
            int len = (int)(HEIGHT * ( (double)j/(2*FPS) ));
            if(j != 2*FPS) lastImage(Rect(0,len, WIDTH,HEIGHT-len)).copyTo(out(Rect(0,0,WIDTH,HEIGHT-len)));
            //lastImage(Rect(0,len, WIDTH,HEIGHT-len)).copyTo(out(Rect(0,0,WIDTH,HEIGHT-len)));
            image(Rect(0,0, WIDTH, len)).copyTo(out(Rect(0,HEIGHT-len, WIDTH, len)));
            font->putText(out, STDID, Point(WIDTH/5, HEIGHT-30), 28, Scalar(255, 255, 255), -1,4,true);
            video.write(out);
        }
        lastImage = image.clone();
        // 2s per image
        int frameNumber = 2*FPS;
        while (frameNumber--) {
            font->putText(image, STDID, Point(WIDTH/5, HEIGHT-30), 28, Scalar(255, 255, 255), -1,4,true);
            video.write(image);
        }
    }
    cout << "Solved " << i << "s image." << endl;
}
```

关于动画处理，分为两部分：
1. 第一张幻灯片的逐渐出现动画
2. 第二张幻灯片及之后幻灯片的
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/freetype.hpp>

using namespace cv;
using namespace std;

const string STDID = "刘一辰 3180102886";
const int WIDTH = 1280;
const int HEIGHT = 720;
const int FPS = 60;

void modifyAlpha(Mat &src, Mat &des, double alpha) {
    des = Mat(src.rows,src.cols,CV_8UC3);
    if(alpha > 1) alpha = 1;
    for (int i = 0; i < src.rows; ++i) {
        for (int j = 0; j < src.cols; ++j) {
            auto &p = src.at<Vec3b>(i, j);
            auto &q = des.at<Vec3b>(i, j);
            q[0] = p[0]*alpha;
            q[1] = p[1]*alpha;
            q[2] = p[2]*alpha;
        }
    }
}

int main(int argc, char *argv[]) {
    if(argc != 2){
        cerr << "Input Error" << endl;
        return -1;
    }
    // Init
    string path = argv[1];
    VideoWriter video;
    Size frameSize = Size(WIDTH, HEIGHT);
    string fileName = "video.mp4";
    video = VideoWriter(fileName, VideoWriter::fourcc('D', 'I', 'V', 'X'), FPS, frameSize, true);
    Ptr<freetype::FreeType2> font;
    font = freetype::createFreeType2();
    font->loadFontData("/System/Library/Fonts/PingFang.ttc",0);

    // begin
    cout << "Begin to run" << endl;

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

    for(int i=1;i<=FPS;++i){
        out = temp.clone();
        font->putText(out, "来看看你的干员们吧！",
                      Point(WIDTH/3,HEIGHT/2*((double)i/FPS)),
                      50,Scalar(255,255,255),-1,4,true);
        video.write(out);
    }
    temp = out.clone();
    for(int i=1;i<=FPS;++i) {
        video.write(temp);
    }
    cout << "Second line." << endl;

    for (int j = 0; j < 2*FPS; ++j) {
        int scalar = (int)((j > FPS ? 1 : (double)j/FPS)*255);
        out = temp.clone();
        font->putText(out, "刘一辰 3180102886",
                      Point(WIDTH/3,3*HEIGHT/4),
                      32,Scalar(scalar,scalar,scalar),-1,4,true);
        video.write(out);
    }
    cout << "Third line." << endl;

    temp = out.clone();
    for (int i = 0; i < FPS; ++i) {
        modifyAlpha(temp, out, (double) (FPS-i) / FPS);
        video.write(out);
    }


    // Get image
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

    // Get video
    string videoPath = path + "/1.mp4";
    VideoCapture capture = VideoCapture(videoPath);
    if(!capture.isOpened()){
        cerr << "Can not open " << path << "/1.mp4 .";
        capture = VideoCapture(path + "/1.avi");
        if(!capture.isOpened()) {
            cerr << "Can not open " << path << "/1.avi." << endl;
            cerr << "Process will exit." << endl;
            return 0;
        }
    }
    int count = 0;
    while(true){
        capture >> temp;
        if(temp.empty()) break;
        resize(temp, out, frameSize);
        if(++count == FPS*30){
            count = 0;
            cout << "Solved 1 min." << endl;
        }
        font->putText(out, STDID, Point(WIDTH/5, HEIGHT-30), 28, Scalar(255, 255, 255), -1,4,true);
        video.write(out);
        video.write(out);
    }

    video.release();
    return 0;
}

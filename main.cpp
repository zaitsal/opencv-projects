//#include <QCoreApplication>
//#include <QDir>
//#include <QTextStream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/video/tracking.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>

using namespace cv;
using namespace std;



void RegisterShotVideo(Mat image);
string init_gun(int reg_num);
void readBaseWeapon();

bool pk = false;
int lm = 0;
int k[12]={};
int mng[8] = {128,64,32,16,8,4,2,1};
int g[7][7]={
            {195, 225, 112, 113, 97},
            {243,249, 121, 251, 125},
            {51, 57, 25, 29, 59},
            {63, 31},
            {204, 206, 102, 230, 231, 238, 115},
            {248 ,240, 224},
            {230, 236, 242, 246, 251}};
int g2[6][7];
int count = 0;

int main()   //
{

    VideoCapture video(CAP_DSHOW+1);

    Mat frame;


    video.set(CAP_PROP_FPS, 120);
    video.set(CAP_PROP_FOURCC, VideoWriter::fourcc('M','J','P','G'));
    video.set(CAP_PROP_CONVERT_RGB, 0);
    //video.set(CAP_PROP_SETTINGS, 0);
    video.set(CAP_PROP_FRAME_WIDTH, 640);
    video.set(CAP_PROP_FRAME_HEIGHT, 360);
    video  >> frame;
    video.set(CAP_PROP_EXPOSURE, -8.0);




    if(!video.isOpened())
    {
        cout << "Camera error..." << endl;
        return -1;
    }


    string windowName = "Video";
    namedWindow(windowName, WINDOW_NORMAL);

    while(1)
    {

        video >> frame;

       //putText(frame, to_string(static_cast<int>(frame.cols))+" | "+to_string(static_cast<int>(frame.rows)),Point(20, 40), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,0,255), 2);
       //putText(frame, "FPS: "+to_string(static_cast<int>(video.get(CAP_PROP_FPS))),Point(20, 80), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,0,255), 2);
       RegisterShotVideo(frame);
       //imshow(windowName, frame);

        if(pollKey() == 'q')
            break;
    }

    video.release();
    destroyAllWindows();

    return 0;
}



void RegisterShotVideo(Mat image)
{
   // QDir dir;
    Mat dst, thresh;
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;


    cvtColor(image, dst, COLOR_BGR2GRAY);
    threshold(dst, thresh, 120, 255, THRESH_BINARY);
    findContours(thresh, contours, hierarchy, RETR_LIST , CHAIN_APPROX_NONE);

    vector<Moments> mu(contours.size());
    vector<Point2f> mc(contours.size());
    double max_x=0.0, max_y=0.0;
    for( int i = 0; i<contours.size(); i++ )
    {
        mu[i] = moments( contours[i], false );

    }
    for( int i = 0; i<contours.size(); i++ )
        mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );

    for( int i = 0; i<contours.size(); i++ )
    {
        if(static_cast<int>(mc[i].x) > max_x )
            max_x=mc[i].x;
        if(static_cast<int>(mc[i].y) > max_y )
            max_y=mc[i].y;
        circle( image, mc[i], 4, Scalar(255, 255, 0), -1, 8, 0 );
        line(image, mc[i], Point(mc[i].x+130,mc[i].y), Scalar(255, 178, 50), 2);
        putText(image, to_string(static_cast<int>(mc[i].x))+"/"+to_string(static_cast<int>(mc[i].y)),Point(mc[i].x+20, mc[i].y-10), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 178, 50), 2);
    }


    if(!contours.empty() && !pk)
    {
        pk = true;
        cout << max_x << ";" << max_y << ";" ;
       // cout << "Size :" << contours.size() ;
    }
    if(pk)
    {
        k[lm] = !contours.empty();
        lm++;

        if(lm > 13)
        {
            pk = false;
            lm = 0;
            int reg_num = 0;
           for(int i=4; i <= 11; i++)
           {
               // cout << k[i];
                reg_num = reg_num + mng[i-4] * k[i];
           }


           cout << init_gun(reg_num);

          // cout << "   " << reg_num;
           cout << endl;
        }

    }
}

string init_gun(int reg_num)
{
    int num_gun = 0;
    string name_gun;
    for(int i = 0; i < 7; i++)
    {
        for(int k = 0; k < 7; k++)
        {
            if(g[i][k] == reg_num)
            {
                num_gun = g[i][k];
                break;
            }
        }
        if(num_gun != 0)
        {
            name_gun = "K-" + to_string(i+1);
            break;
        }
     }

    return name_gun;
}


void readBaseWeapon()
{
    string line, num;
    int i = 0;
    ifstream in("C:/myProg/Config/baseweapon.txt");
    if (in.is_open())
    {
         while (getline(in, line))
         {
             for(int k = 0; k < line.length(); k++)
             {
                 if(isdigit(line[k]))
                 {
                    num += line[k];
                    cout << num << " ";
                 }
                 if(isspace(line[k]))
                 {
                    g2[i][k] = stoi(num);
                 }
             }
             cout << line << endl;
             i++;
         }
        in.close();
      for(int i = 0; i < 6; i++)
     {
        for(int k = 0; k < 7; k++)
           cout << g2[i][k] << " ";
        cout << endl;
      }
    }
    else
        cout << "Err open file.... \n";
}
/*
void Register::CountChange(Mat image, vector<vector<Point>> contours)
{

    // get the moments
    vector<Moments> mu(contours.size());
    for( int i = 0; i<contours.size(); i++ )
         mu[i] = moments( contours[i], false );

    // get the centroid of figures.
    vector<Point2f> mc(contours.size());
    for( int i = 0;  i<contours.size(); i++)
        mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );

    for( int i = 0;  i<contours.size(); i++)
    {
        circle( image, mc[i], 4, Scalar(255, 255, 0), -1, 8, 0 );
        line(image, mc[i], Point(mc[i].x+130,mc[i].y), BLUE, 2);
        putText(image, to_string(static_cast<int>(mc[i].x))+"/"+to_string(static_cast<int>(mc[i].y)),Point(mc[i].x+20, mc[i].y-10), FONT_FACE, 0.7, BLUE, 2);
    }


    if(contours.size() > 0)
        ofvar.k[ofvar.lm-1] = 1;
    else
        ofvar.k[ofvar.lm-1] = 0;

    ofvar.lm++;
    if(ofvar.lm > 12)
    {
        ofvar.pk = false;
        ofvar.lm = 1;
        for(int i=0; i <= 11; i++)
         {
            cout << ofvar.k[i] << " ";
         }
        cout << endl;
    }

}
*/

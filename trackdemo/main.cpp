#include "lktracking.h"
#include "detection.h"
#include <iostream>

using namespace std;

int main()
{
    itr_math::MathObjStandInit();
    char path[50]="bin/Debug/09_carchase/pgm/%05d.pgm";
    char file[50]="bin/Debug/09_carchase/pgm/00001.pgm";
    Matrix current,last;
    IOpnm::ReadPGMFile(file, current);
    IOpnm::ReadPGMFile(file, last);

    ///读取初始位置
    RectangleS rect(0, 0, 0, 0);
    FILE *InitInfo=fopen("bin/Debug/09_carchase/init.txt","r");
    fscanf(InitInfo,"%d,%d,%d,%d",&rect.X,&rect.Y,&rect.Width,&rect.Height);
    rect.Width-=rect.X;
    rect.Height-=rect.Y;

    ///卡尔曼滤波用
    KalmanFilter kf;
    kf.Init(4);
    F32 data[24]= {2 ,0,-1,0,0,2,0,-1,0,0,1,0,0,0,0,1,1,0,0,0,0,1,0,0};
    kf.F_x.CopyFrom(data);
    Matrix H(2,4),R(2,2);
    R.SetDiag(3.012306);
    H.CopyFrom(data+16);
    Vector z(2),X(4),v(2);
    kf.x[0]=rect.X;
    kf.x[1]=rect.Y;
    kf.x[2]=rect.X;
    kf.x[3]=rect.Y;

    F32 _x,_y,_u=0,_v=0;
    Detection detection(current,rect,10);
    lktracking tracking;
    tracking.Init(current,rect);
    for(int k=2; k<200; ++k)
    {
        sprintf(file, path, k);
        printf("%s\n",file);
        last=current;
        IOpnm::ReadPGMFile(file, current);
        tracking.Go(current,rect,_u,_v);
        rect.X+=_u;
        rect.Y+=_v;
        detection.Go(current,rect,_x,_y);
        rect.X=_x;
        rect.Y=_y;
//        z[0]=_x;
//        z[1]=_y;
//        kf.UpdateModel();
//        X=kf.UpdateMeasure(H,R,z);
//        z[0]=rect.X+_u;
//        z[1]=rect.Y+_v;
//        kf.UpdateModel();
//        X=kf.UpdateMeasure(H,R,z);
//        rect.X=X[0];
//        rect.Y=X[1];
        Draw::Rectangle(current,rect,255);
        sprintf(file,"bin/Debug/output/%05d.pgm",k);
        IOpnm::WritePGMFile(file,current);
    }
    return 0;
}

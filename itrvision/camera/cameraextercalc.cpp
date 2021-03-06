#include "cameraextercalc.h"
#include "alglib.h"
#include "stdio.h"
#define resid_MAX_2 64
namespace itr_vision
{
CameraExterCalc::CameraExterCalc()
{
    //ctor
    H.Init(3,3);
    Hinv.Init(3,3);
    R.Init(3,6);
    t.Init(3,2);
    N.Init(3,2);
    V.Init(3,3);
}

CameraExterCalc::~CameraExterCalc()
{
    //dtor
}

CameraExterCalc::CameraExterCalc(const CameraExterCalc &other)
{
    //copy ctor
    this->H=other.H;
    this->V=other.V;
    this->R=other.R;
    this->t=other.t;
    this->N=other.N;
}

BOOL CameraExterCalc::CalcH(const std::vector<VectorFeaturePoint>& PointList0,const std::vector<VectorFeaturePoint>& PointList1,S32 MatchedNum)
{
    //筛选最优 H 时所用
    Matrix best_H(3,3);

    Vector pos_2s(3),pos_1(3);
    F32 red_counter,tmp_red,best_counter=0, best_red=0;
    F32 risde;

    //bucketing
    S32 bucket_counter[16]= {0};
    S32 *bucket;
    bucket=new S32 [16*MatchedNum]();
    //bucket={0};

    F32 *tempvalue_u, *tempvalue_v;
    S32 *tempID;
    tempvalue_u=new F32[MatchedNum]();
    tempvalue_v=new F32[MatchedNum]();
    tempID=new S32[MatchedNum]();
    MatchedNum=0;
    S32 List1Num=PointList0.size();
    for(S32 i=0; i<List1Num; i++)
    {
        if(PointList0[i].ID!=-1)
        {
            tempID[MatchedNum]=i;
            tempvalue_u[MatchedNum]=PointList0[i].X;
            tempvalue_v[MatchedNum]=PointList0[i].Y;
            MatchedNum++;
        }
    }
    F32 U,V;
    itr_math::CalculateObj->Max(tempvalue_u,MatchedNum,U);
    itr_math::CalculateObj->Max(tempvalue_v,MatchedNum,V);
    /// bucket for 16 bucket
    for(S32 i=0; i<MatchedNum; i++)
    {
        if(tempvalue_u[i]<=U/4&&tempvalue_v[i]<=V/4)
        {
            bucket_counter[0]++;
            *(bucket+(0*MatchedNum+bucket_counter[0]))=i;
            continue;
        }
        else if(tempvalue_u[i]<=U/4&&tempvalue_v[i]<=V/2)
        {
            bucket_counter[1]++;
            *(bucket+(1*MatchedNum+bucket_counter[1]))=i;
            continue;
        }
        else if(tempvalue_u[i]<=U/4&&tempvalue_v[i]<=3*V/4)
        {
            bucket_counter[2]++;
            *(bucket+(2*MatchedNum+bucket_counter[2]))=i;
            continue;
        }
        else if(tempvalue_u[i]<=U/4)
        {
            bucket_counter[3]++;
            *(bucket+(3*MatchedNum+bucket_counter[3]))=i;
            continue;
        }
        else if(tempvalue_u[i]<=U/2&&tempvalue_v[i]<=V/4)
        {
            bucket_counter[4]++;
            *(bucket+(4*MatchedNum+bucket_counter[4]))=i;
            continue;
        }
        else if(tempvalue_u[i]<=U/2&&tempvalue_v[i]<=V/2)
        {
            bucket_counter[5]++;
            *(bucket+(5*MatchedNum+bucket_counter[5]))=i;
            continue;
        }
        else if(tempvalue_u[i]<=U/2&&tempvalue_v[i]<=3*V/4)
        {
            bucket_counter[6]++;
            *(bucket+(6*MatchedNum+bucket_counter[6]))=i;
            continue;
        }
        else if(tempvalue_u[i]<=U/2)
        {
            bucket_counter[7]++;
            *(bucket+(7*MatchedNum+bucket_counter[7]))=i;
            continue;
        }
        else if(tempvalue_u[i]<=3*U/4&&tempvalue_v[i]<=V/4)
        {
            bucket_counter[8]++;
            *(bucket+(8*MatchedNum+bucket_counter[8]))=i;
            continue;
        }
        else if(tempvalue_u[i]<=3*U/4&&tempvalue_v[i]<=V/2)
        {
            bucket_counter[9]++;
            *(bucket+(9*MatchedNum+bucket_counter[9]))=i;
            continue;
        }
        else if(tempvalue_u[i]<=3*U/4&&tempvalue_v[i]<=3*V/4)
        {
            bucket_counter[10]++;
            *(bucket+(10*MatchedNum+bucket_counter[10]))=i;
            continue;
        }
        else if(tempvalue_u[i]<=3*U/4)
        {
            bucket_counter[11]++;
            *(bucket+(11*MatchedNum+bucket_counter[11]))=i;
            continue;
        }
        else if(tempvalue_v[i]<=V/4)
        {
            bucket_counter[12]++;
            *(bucket+(12*MatchedNum+bucket_counter[12]))=i;
            continue;
        }
        else if(tempvalue_v[i]<=V/2)
        {
            bucket_counter[13]++;
            *(bucket+(13*MatchedNum+bucket_counter[13]))=i;
            continue;
        }
        else if(tempvalue_v[i]<=3*V/4)
        {
            bucket_counter[14]++;
            *(bucket+(14*MatchedNum+bucket_counter[14]))=i;
            continue;
        }
        else
        {
            bucket_counter[15]++;
            *(bucket+(15*MatchedNum+bucket_counter[15]))=i;
            continue;
        }
    }
    S32 amptybucket=0;
    for(S32 i=0; i<16; i++)
        if(bucket_counter[i]==0)
            amptybucket++;
    if(amptybucket>12)
        return false;

    F32 ratio_bucket[16]= {0};
    ratio_bucket[0]=((F32)bucket_counter[0])/MatchedNum;
    for(S32 i=1; i<16; i++)
    {
        ratio_bucket[i]=ratio_bucket[i-1] + (F32)bucket_counter[i]/MatchedNum;
    }
    /// RANSAC,calculate H
    S16 b[4]= {0};
    S32 c[4]= {0};
    F32 p=0,wght,u1,v1,u2,v2;
    S32 q=0,tmp_k=0;
    S32 i=0;
    do      ///for(S32 i=0; i<20; i++)     /// RANSAC begin:
    {
        //pick 4 buckets
        for(S32 j=0; j<4; j++)
        {
            itr_math::NumericalObj->Rand(p);
            for(S32 k=0; k<16; k++)
            {
                if(k==0)
                {
                    if(p<ratio_bucket[0])
                    {
                        q=1;
                        break;
                    }
                }
                else
                {
                    if(ratio_bucket[k-1]<p&&ratio_bucket[k]>=p)
                    {
                        if(bucket_counter[k]>0)
                            q=k;
                        else
                        {
                            tmp_k =k;
                            while(bucket_counter[tmp_k]==0)
                                tmp_k--;
                            q=tmp_k;
                        }
                        break;
                    }
                }
            }
            if(j>0)
            {
                for(S32 z=0; z<j; z++)
                    if(q==b[z])
                    {
                        q=-1;
                        j--;
                    }
            }
            if(q!=-1)
                b[j]=q;
        }
        //pick 4 feature point
        for(S32 j=0; j<4; j++)
        {
            itr_math::NumericalObj->Rand(p);
            itr_math::NumericalObj->Ceil(p*bucket_counter[b[j]],q);
            c[j]=tempID[*(bucket+(b[j]*MatchedNum+q))];
        }

        double M[72]= {0}; //最小二乘矩阵
        for(S32 j=0; j<4; j++)
        {
            u1 = PointList0[c[j]].X;
            v1 = PointList0[c[j]].Y;
            u2 = PointList1[PointList0[c[j]].ID].X;
            v2 = PointList1[PointList0[c[j]].ID].Y;
            wght = PointList1[PointList0[c[j]].ID].Quality;
            M[2*j*9+3]=-u1*wght;
            M[2*j*9+4]=-v1*wght;
            M[2*j*9+5]=-wght;
            M[2*j*9+6]=u1*v2*wght;
            M[2*j*9+7]=v1*v2*wght;
            M[2*j*9+8]=v2*wght;
            M[(2*j+1)*9+0]=-u1*wght;
            M[(2*j+1)*9+1]=-v1*wght;
            M[(2*j+1)*9+2]=-wght;
            M[(2*j+1)*9+6]=u1*u2*wght;
            M[(2*j+1)*9+7]=v1*u2*wght;
            M[(2*j+1)*9+8]=u2*wght;
        }
        //svd
        double V[81]= {0};
        alglib::real_2d_array Marray,Varray,Uarray;
        alglib::real_1d_array Warray;
        Marray.setcontent(8,9,M);
        Varray.setcontent(9,9,V);
        alglib::rmatrixsvd(Marray,8,9,0,2,2,Warray,Uarray,Varray);

        for(S32 j=0; j<3; j++)
            for(S32 k=0; k<3; k++)
                H(j,k)=Varray(8,j*3+k);

        //残差量
        red_counter=0,tmp_red=0;
        pos_1[2]=1;
        for(S32 j=0; j<MatchedNum; j++)
        {
            if(tempID[j]==c[0]||tempID[j]==c[1]||tempID[j]==c[2]||tempID[j]==c[3])
                risde=0;
            else
            {
                pos_1[0]=tempvalue_u[j];
                pos_1[1]=tempvalue_v[j];

                pos_2s=H*pos_1;

                pos_2s[0]/=pos_2s[2];
                pos_2s[1]/=pos_2s[2];

                pos_2s[0]-=PointList1[PointList0[tempID[j]].ID].X;
                pos_2s[1]-=PointList1[PointList0[tempID[j]].ID].Y;
                itr_math::CalculateObj->MultiSum(pos_2s.GetData(),pos_2s.GetData(),2,risde);
            }
            if(risde<resid_MAX_2)   /// 与王论文相比 risde 未开方
                red_counter++;
            tmp_red+=risde;
        }

        if(i==0)
        {
            best_H=H;
            best_counter=red_counter;
            best_red=tmp_red;
        }
        else
        {
            if((red_counter>best_counter)||((red_counter==best_red)&&(tmp_red<best_red)))
            {
                best_red = tmp_red;
                best_counter = red_counter;
                best_H = H;
            }
        }
        i++;
    }
    while(best_counter<0.8*MatchedNum&&i<10);         ///end of RANSAC

/// M-estimate

    H=best_H;
    Hinv=H.Inv();

    delete[] bucket;
    delete[] tempvalue_u;
    delete[] tempvalue_v;
    delete[] tempID;

    return true;
}

BOOL CameraExterCalc::CalcMotion(CameraInterCalc &CameraInterPara,F32 D)
{
    Matrix tmpA(3,3);
    Matrix tmpApinv(3,3);
    CameraInterPara.MatC2P.CopyTo(0,0,3,3,tmpA.GetData());
    tmpA.Inv(tmpApinv);
    double Udata[9]= {0},Vdata[9]= {0},Ddata[3]= {0},Hdata[9]; //tmpAdata[9]={0};tmpApinvdata[9]={0};
    ///     Harray=H;
    for(S32 i=0; i<3; i++)
        for(S32 j=0; j<3; j++)
            Hdata[i*3+j]=(double)H(i,j);

    alglib::real_2d_array Harray,Varray,Uarray;//tmpAarray,tmpAinvarray;
    alglib::real_1d_array Darray;
    Harray.setcontent(3,3,Hdata);
    Varray.setcontent(3,3,Vdata);
    Uarray.setcontent(3,3,Udata);
    Darray.setcontent(3,Ddata);

    /// Harray=H,Varray=tmpApinv;
    for(S32 i=0; i<3; i++)
        for(S32 j=0; j<3; j++)
            Varray(i,j)=(double)tmpApinv(i,j);

    ///    Uarray=tempApinv*H
    alglib::rmatrixgemm(3, 3, 3, 1, Varray, 0, 0, 0, Harray, 0, 0, 0, 0.0, Uarray, 0, 0);

    /// Varray=tmpA;
    for(S32 i=0; i<3; i++)
        for(S32 j=0; j<3; j++)
            Varray(i,j)=(double)tmpA(i,j);

    ///    Harray=Uarray*tmpA;
    alglib::rmatrixgemm(3, 3, 3, 1, Uarray, 0, 0, 0, Varray, 0, 0, 0, 0.0, Harray, 0, 0);
    // Hc=tmpApinv*H*tmpA;

    alglib::rmatrixsvd(Harray,3,3,2,2,2,Darray,Uarray,Varray);

    Matrix U(3,3);
    for(S32 j=0; j<3; j++)
        for(S32 k=0; k<3; k++)
            U(j,k)=(F32)Uarray(j,k);
    for(S32 j=0; j<3; j++)
        for(S32 k=0; k<3; k++)
            V(j,k)=(F32)Varray(j,k);
    Vector u1v(3),u3v(3),v1v(3),v3v(3);
    u1v[0]=(F32)Uarray(0,0),u1v[1]=(F32)Uarray(1,0),u1v[2]=(F32)Uarray(2,0);
    u3v[0]=(F32)Uarray(0,2),u3v[1]=(F32)Uarray(1,2),u3v[2]=(F32)Uarray(2,2);
    v1v[0]=(F32)Varray(0,0),v1v[1]=(F32)Varray(0,1),v1v[2]=(F32)Varray(0,2);
    v3v[0]=(F32)Varray(2,0),v3v[1]=(F32)Varray(2,1),v3v[2]=(F32)Varray(2,2);
    F32 q1,q2,q3;

    q1=(F32)Darray[0],q2=(F32)Darray[1],q3=(F32)Darray[2];

    F32 err1=q1-q2;
    F32 err2=q2-q3;
    F32 err_ratio=0.005*q2;
    S16 state;
    if (err1<=err_ratio && err2<=err_ratio)
        state=1;
    else if(err1>err_ratio&& err2>err_ratio)
        state=3;
    else
        state=2;

    F32 s,stmp;
    U.Det(s);
    V.Det(stmp);
    s*=stmp;

    Matrix R1m(3,3);
    Matrix temp33(3,3);
    Matrix u3m(3,1),v3m(3,1),v3tm(1,3);
    U.CopyColTo(2, u3m.GetData());
    V.CopyRowTo(2, v3tm.GetData());
    V.CopyRowTo(2, v3m.GetData());
    Vector t1v(3);
    switch (state)
    {
    case 1:
    {
        //R1m.CopyFrom(0,0,3,3,H.GetData());
        for(S32 i=0; i<3; i++)
            for(S32 j=0; j<3; j++)
                R1m(i,j)=(F32)Harray(i,j);
        R1m.AllMul(1/q2);
        t1v.Set(0);
        R.CopyFrom(0,0,3,3,R1m.GetData());
        t.Set(0);
    }
    break;
    case 2:
    {
        //temp33=u3m*v3tm;
        for(S32 i=0; i<3; i++)
            for(S32 j=0; j<3; j++)
                temp33(i,j)=u3v[i]*v3v[j];
        temp33.AllMul(q3/q1-s);
        //R1m.CopyFrom(0,0,3,3,H.GetData());
        for(S32 i=0; i<3; i++)
            for(S32 j=0; j<3; j++)
                R1m(i,j)=(F32)Harray(i,j);
        R1m.AllMul(1/q2);
        R1m=R1m-temp33; ///R1 = H/q1 - (q3/q1-s)*u3*v3'

        t1v=u3v;
        t1v.Mul(D*(q3/q1-s));///

        double ndata[3]= {Varray(2,0),Varray(2,1),Varray(2,2)},nddata[3]= {0};
        alglib::real_2d_array narray,nvarray,nuarray;
        alglib::real_1d_array ndarray;
        narray.setcontent(3,1,ndata);
        ndarray.setcontent(3,nddata);
        alglib::rmatrixsvd(narray,3,1,0,0,2,ndarray,nuarray,nvarray);
        F32 W_n=(F32)ndarray[0]; /// W_n=normest(n),n=v3
        t1v.Mul(-W_n);
        R.CopyFrom(0,0,3,3,R1m.GetData());
        t.CopyFrom(0,0,1,3,t1v.GetData());
        N(0,0)=V(2,0),N(1,0)=V(2,1),N(2,0)=V(2,2);/// N=V(2,:);
    }
    break;
    case 3:
    {
        F32 r=sqrt((q1*q1-q2*q2)/(q2*q2-q3*q3));
        F32 a=(q1+s*q3*r*r)/(q2+q2*r*r);
        F32 b=-sqrt(1-a*a);
        temp33(0,0)=a;
        temp33(0,1)=0;
        temp33(0,2)=b;
        temp33(1,0)=0;
        temp33(1,1)=1;
        temp33(1,2)=0;
        temp33(2,0)=-s*b;
        temp33(2,1)=0;
        temp33(2,2)=s*a;

        R1m=U*temp33*V;

        Vector n1v(3);
        n1v=v1v;
        n1v.Mul(r);
        n1v=n1v+v3v;

        Vector temp3v(3);
        t1v=u1v;
        t1v.Mul(-b);
        temp3v=u3v;
        temp3v.Mul(q3/q2-s*a);
        t1v=t1v+temp3v;
        t1v.Mul(D);  //t1=(u1*(-b)+u3*(q3/q2-s*a))*D;

        N.CopyFrom(0,0,1,3,n1v.GetData());
        /// normest(n1v)
        double ndata[3]= {(double)n1v[0],(double)n1v[1],(double)n1v[2]},nddata[3]= {0};
        alglib::real_2d_array narray,nvarray,nuarray;
        alglib::real_1d_array ndarray;
        narray.setcontent(3,1,ndata);
        ndarray.setcontent(3,nddata);
        alglib::rmatrixsvd(narray,3,1,0,0,2,ndarray,nuarray,nvarray);

        F32 W_n=ndarray[0];

        t1v.Mul(-W_n);
        R.CopyFrom(0,0,3,3,R1m.GetData());
        t.CopyFrom(0,0,1,3,t1v.GetData());

        r=-r;
        b=-b;
        temp33(0,2)=b;
        temp33(2,0)=-s*b;
        Matrix R2m(3,3);
        R2m=U*temp33*V;


        Vector t2v(3);
        t2v=u1v;
        t2v.Mul(-b);

        temp3v=u3v;
        temp3v.Mul(q3/q2-s*a);
        t2v=t2v+temp3v;
        t2v.Mul(D);

        for(S32 j=0; j<3; j++)
            //narray(j,0)=(double)n2m(j,0);
        {
            narray(j,0)=v1v[j]*r+v3v[j];
            N(j,1)=narray(j,0);
        }
        alglib::rmatrixsvd(narray,3,1,0,0,2,ndarray,nuarray,nvarray);
        W_n=ndarray[0];
        t2v.Mul(W_n);
        t2v.Mul(-1);
        R.CopyFrom(0,3,3,3,R2m.GetData());
        t.CopyFrom(0,1,1,3,t2v.GetData());

    }
    break;
    }
    V=V.Tran();
    return true;
}

BOOL CameraExterCalc::CalcForwardPoint(const Point2D& Point0,Point2D& Point1)
{
    Matrix pos1(3,1);
    Matrix pos2(3,1);

    pos1[0]=Point0.X;
    pos1[1]=Point0.Y;
    pos1[2]=1;
    pos2=H*pos1;
    Point1.X=pos2[0]/pos2[2];
    Point1.Y=pos2[1]/pos2[2];
    return true;
}


BOOL CameraExterCalc::CalcBackwardPoint(const Point2D& Point1,Point2D& Point0)
{
    Matrix pos1(3,1);
    Matrix pos2(3,1);

    pos2[0]=Point1.X;
    pos2[1]=Point1.Y;
    pos2[2]=1;
    pos1=Hinv*pos2;
    Point0.X=pos1[0]/pos1[2];
    Point0.Y=pos1[1]/pos1[2];
    return true;
}


}

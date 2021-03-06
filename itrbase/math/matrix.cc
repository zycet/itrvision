/*
 *
 *   Copyright (C) 2013 BUAA iTR Research Center. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * matrix.cc
 *  Created on: 2013-9-14
 *      Author: zhouyi
 */

#include <stddef.h>
#include "../platform/platform.h"
#include "math.h"

namespace itr_math
{
/*
 * 初始化一个指定行列数的空矩阵(自动分配内存)
 */
Matrix::Matrix(S32 Row, S32 Col)
{
    row=0;
    col=0;
    data=NULL;
    localData=false;
    Init(Row,Col);
}
/*
 * 初始化一个指定行列数的矩阵(以传入的指针为数据区,不分配本地内存)
 */
Matrix::Matrix(S32 Row, S32 Col, F32* Data)
{
    row=0;
    col=0;
    data=NULL;
    localData=false;
    Init(Row,Col,Data);
}
/*
 * 初始化一个指定阶数的方阵(自动分配内存)
 */
Matrix::Matrix(S32 RowCol)
{
    row=0;
    col=0;
    data=NULL;
    localData=false;
    Init(RowCol);
}
/*
 * 初始化完全一样的矩阵(Clone)
 */
Matrix::Matrix(const Matrix& Mat)
{
    assert(NumericalObj!=NULL && CalculateObj!=NULL);
    data = new F32[Mat.GetRow() * Mat.GetCol()]();
    assert(data!=NULL);
    row = Mat.GetRow();
    col = Mat.GetCol();
    localData = true;
    this->CopyFrom(Mat.data);
}
/*
 * 用于初始化列表的空构造函数
 * 在构造后需手动调用Init函数
 */
Matrix::Matrix()
{
    row=0;
    col=0;
    data=NULL;
    localData=false;
}
/*
 * 回收自动分配的内存
 */
Matrix::~Matrix()
{
    if (localData == true)
    {
        delete data;
    }
}

//**********后初始化函数**********
/*
* 初始化一个指定行列数的空矩阵(自动分配内存)
*/
void Matrix::Init(S32 Row, S32 Col)
{
    assert(data==NULL);
    assert(NumericalObj!=NULL && CalculateObj!=NULL);
    assert(Row>0 && Col>0);
    data = new F32[Row * Col]();
    assert(data!=NULL);
    row = Row;
    col = Col;
    localData = true;
}
/*
 * 初始化一个指定行列数的矩阵(以传入的指针为数据区)
 */
void Matrix::Init(S32 Row, S32 Col, F32* Data)
{
    assert(data==NULL);
    assert(NumericalObj!=NULL && CalculateObj!=NULL);
    assert(Row>0 && Col>0);
    data = Data;
    assert(data != NULL);
    row = Row;
    col = Col;
    localData = false;
}
/*
 * 初始化一个指定阶数的方阵(自动分配内存)
 */
void Matrix::Init(S32 RowCol)
{
    assert(data==NULL);
    assert(NumericalObj!=NULL && CalculateObj!=NULL);
    assert(RowCol>0);
    data = new F32[RowCol * RowCol]();
    assert(data!=NULL);
    row = RowCol;
    col = RowCol;
    localData = true;
}

//**********初等变换**********
/*
 * 将RowPosSource行加RowPosTarget行
 */
void Matrix::AddRow(S32 RowPosSource, S32 RowPosTarget)
{
    assert(RowPosSource >= 0 && RowPosSource < row);
    assert(RowPosTarget >= 0 && RowPosTarget < row);
    CalculateObj->Add(data + RowPosSource* col, data + RowPosTarget * col, col,data + RowPosTarget * col);
}
/*
 * 将Data加至RowPosTarget行
 */
void Matrix::AddRow(F32* Data, S32 RowPosTarget)
{
    assert(Data!=NULL);
    assert(RowPosTarget >= 0 && RowPosTarget < row);
    CalculateObj->Add(Data, data + RowPosTarget * col, col, data + RowPosTarget * col);
}
/*
 * 将RowPosSource行减至RowPosTarget行
 */
void Matrix::SubRow(S32 RowPosSource, S32 RowPosTarget)
{
    assert(RowPosSource >= 0 && RowPosSource < row);
    assert(RowPosTarget >= 0 && RowPosTarget < row);
    CalculateObj->Sub( data + RowPosTarget * col,data + RowPosSource* col, col,data + RowPosTarget * col);
}
/*
 * 将Data减至RowPosTarget行
 */
void Matrix::SubRow(F32* Data, S32 RowPosTarget)
{
    assert(Data!=NULL);
    assert(RowPosTarget >= 0 && RowPosTarget < row);
    CalculateObj->Sub(data + RowPosTarget * col, Data, col,data + RowPosTarget  * col);
}
/*
 * 将RowPosTarget行乘以K
 */
void Matrix::MulRow(F32 K, S32 RowPosTarget)
{
    assert(RowPosTarget >= 0 && RowPosTarget < row);
    CalculateObj->Scale(data + RowPosTarget  * col, K, col, data + RowPosTarget * col);
}
//Swap Row
/*
 * 交换RowNoA行和RowNoB行
 */
void Matrix::SwapRow(S32 RowPosA, S32 RowPosB)
{
    assert(RowPosA >= 0 && RowPosA < row);
    assert(RowPosB >= 0 && RowPosB < row);
    MemorySwap(data + RowPosA * col, data + RowPosB * col, col*sizeof(F32));
}
/*
 * 将ColNoAdd列加至ColNoResult列
 */
void Matrix::AddCol(S32 ColPosSource, S32 ColPosTarget)
{
    assert(ColPosSource >= 0 && ColPosSource < col);
    assert(ColPosTarget >= 0 && ColPosTarget < col);
    for (S32 i = 0; i < row; i++)
    {
        data[ColPosTarget]+=data[ColPosSource];
        ColPosSource+=col;
        ColPosTarget+=col;
    }
}
/*
 * 将Data加至ColNoResult列
 */
void Matrix::AddCol(F32* Data, S32 ColPosTarget)
{
    assert(Data!=NULL);
    assert(ColPosTarget >= 0 && ColPosTarget<col);

    for (S32 i = 0; i < row; i++)
    {
        data[ColPosTarget]+=Data[i];
        ColPosTarget+=col;
    }
}
/*
 * 将ColNoSub列减至ColNoResult列
 */
void Matrix::SubCol(S32 ColPosSource, S32 ColPosTarget)
{
    assert(ColPosSource >= 0 && ColPosSource < col);
    assert(ColPosTarget >= 0 && ColPosTarget < col);
    for (S32 i = 0; i < row; i++)
    {
        data[ColPosTarget]-=data[ColPosSource];
        ColPosSource+=col;
        ColPosTarget+=col;
    }
}
/*
 * 将Data减至ColNoResult列
 */
void Matrix::SubCol(F32* Data, S32 ColPosTarget)
{
    assert(Data!=NULL);
    assert(ColPosTarget >= 0 && ColPosTarget<col);

    for (S32 i = 0; i < row; i++)
    {
        data[ColPosTarget]-=Data[i];
        ColPosTarget+=col;
    }
}
/*
 * 将ColNoResult列乘以K
 */
void Matrix::MulCol(F32 K, S32 ColPosTarget)
{
    assert(ColPosTarget >= 0 && ColPosTarget < col);
    for (S32 i = 0; i < row; i++)
    {
        data[ColPosTarget ] *= K;
        ColPosTarget+=col;
    }
}
/*
 * 交换ColNoA列和ColNoB列
 */
void Matrix::SwapCol(S32 ColPosA, S32 ColPosB)
{
    assert(ColPosA >= 0 && ColPosA < col);
    assert(ColPosB >= 0 && ColPosB < col);
    F32 temp;
    for (S32 i = 0; i < row; i++)
    {
        temp=data[ColPosB];
        data[ColPosB]=data[ColPosA];
        data[ColPosA]=temp;
        ColPosA+=col;
        ColPosB+=col;
    }
}
//**********常量相关计算**********
/*
 * 全部元素加上K
 */
void Matrix::AllAdd(F32 K)
{
    CalculateObj->Offset(data , K, col*row, data );
}
/*
 * 全部元素乘以K
 */
void Matrix::AllMul(F32 K)
{
    CalculateObj->Scale(data , K, col*row, data);
}
//**********矩阵相关计算**********
/*
 * 加上矩阵MatrixAdd
 */
void Matrix::Add(const Matrix& Mat)
{
    CalculateObj->Add(this->data, Mat.GetData(), this->row * this->col, this->data);
}
/*
 * 减去矩阵MatrixSub
 */
void Matrix::Sub(const Matrix& Mat)
{
    CalculateObj->Sub(this->data, Mat.GetData(), this->row * this->col, this->data);
}
/*
 * 右乘矩阵Mat并将结果存至MatResult
 */
void Matrix::Mul(const Matrix& Mat, Matrix& MatResult) const
{
    assert(this->col == Mat.row);
    assert(this->row == MatResult.row);
    assert(Mat.col == MatResult.col);
    F32* tempVect = new F32[Mat.row];
    F32* tempVectAns = new F32[MatResult.row];
    for (S32 i = 0; i < Mat.col; i++)
    {
        Mat.CopyColTo(i,tempVect);
        for (S32 k = 0; k < MatResult.row; k++)
            CalculateObj->MultiSum(data + k * col, tempVect, Mat.row, tempVectAns[k]);
        MatResult.CopyColFrom(i , tempVectAns);
    }
    delete tempVect;
    delete tempVectAns;
}

Matrix Matrix::operator*(const Matrix& Mat) const
{
    assert(Mat.row == col);
    Matrix MatResult(row, Mat.col);
    Mul(Mat, MatResult);
    return MatResult;
}

Matrix Matrix::operator+(const Matrix& Mat) const
{
    assert(row == Mat.row);
    assert(col == Mat.col);
    Matrix Result(*this);
    Result.Add(Mat);
    return Result;
}
Matrix Matrix::operator-(const Matrix& Mat) const
{
    assert(row == Mat.row);
    assert(col == Mat.col);
    Matrix Result(*this);
    Result.Sub(Mat);
    return Result;
}
Matrix Matrix::operator=(const Matrix& Mat)
{
    assert(row == Mat.row);
    assert(col == Mat.col);
    this->CopyFrom(Mat.data);
    return *this;
}
Vector Matrix::operator*(const Vector& vec) const
{
    assert(this->col==vec.GetDim());
    Vector VecResult(this->row);
    for (S32 i = 0; i < this->row; i++)
    {
        CalculateObj->MultiSum(this->data + i * this->col, vec.GetData(), vec.GetDim(),
                               VecResult.GetData()[i]);
    }
    return VecResult;
}
/******************************
*******Advanced Function*******
*******************************/
/*
 * 求矩阵逆并将结果放至MatResult
 */
BOOL Matrix::Inv(Matrix& MatResult) const
{
    S32 i, j, k;
    Matrix MatTemp(this->row, this->col);
    F32 lMax, temp;
    S32 tempN;//,tempM, ;
    //tempM = MatTemp.row;
    tempN = MatTemp.col;
    MemoryCopy(MatTemp.data, this->data, this->row * this->col * sizeof(F32));
    MatResult.Clear();
    MatResult.SetDiag(1);
    for (i = 0; i < this->row; i++)
    {
        //寻找主元
        lMax = MatTemp.data[i * tempN + i];
        k = i;
        for (j = i + 1; j < this->row; j++)
        {
            if (GET_ABS(MatTemp.data[j*tempN+i]) > GET_ABS(lMax))
            {
                lMax = MatTemp.data[j * tempN + i];
                k = j;
            }
        }
        //如果主元所在行不是第i行，进行行变换
        if (k != i)
        {
            for (j = 0; j < this->col; j++)
            {
                temp = MatTemp.data[i * tempN + j];
                MatTemp.data[i * tempN + j] = MatTemp.data[k * tempN + j];
                MatTemp.data[k * tempN + j] = temp;
                temp = MatResult.data[i * MatResult.col + j];
                MatResult.data[i * MatResult.col + j] = MatResult.data[k * MatResult.col + j];
                MatResult.data[k * MatResult.col + j] = temp;
            }
        }
        //判断主元是否是0，如果是，则矩阵不是满秩矩阵，不存在逆
        if (MatTemp.data[i * tempN + i] == 0)
        {
            return false;
        }
        //使对角线元素为1
        temp = MatTemp.data[i * tempN + i];
        for (j = 0; j < this->col; j++)
        {
            MatTemp.data[i * tempN + j] = MatTemp.data[i * tempN + j] / temp;
            MatResult.data[i * tempN + j] = MatResult.data[i * tempN + j] / temp;
        }
        for (j = 0; j < this->row; j++)
        {
            if (j != i)
            {
                temp = MatTemp.data[j * tempN + i];
                for (k = 0; k < this->col; k++)
                {
                    MatTemp.data[j * tempN + k] = MatTemp.data[j * tempN + k]
                                                  - MatTemp.data[i * tempN + k] * temp;
                    MatResult.data[j * tempN + k] = MatResult.data[j * tempN + k]
                                                    - MatResult.data[i * tempN + k] * temp;
                }
            }
        }
    }
    return true;
}
/*
 * 求矩阵逆
 */
Matrix Matrix::Inv() const
{
    Matrix MatResult(this->row,this->col);
    if(Inv(MatResult))
        return MatResult;
    else
        return NULL;
}
/*
 * 求矩阵转置并将结果放至MatResult
 */
void Matrix::Tran(Matrix& MatResult) const
{
    assert(MatResult.row == this->col && MatResult.col == this->row);
    S32 i, j;
    for (i = 0; i < this->row; i++)
    {
        for (j = 0; j < this->col; j++)
            MatResult.data[j * MatResult.col + i] = this->data[i * this->col + j];
    }
}
/*
 * 求矩阵转置
 */
Matrix Matrix::Tran() const
{
    Matrix TranAns(this->col,this->row);
    Tran(TranAns);
    return TranAns;
}


void Matrix::Det(F32 &reslut) /*求矩阵行列式*/
{
    assert(row>0);
    assert(col>0);
    assert(row==col);
    S32 i,j,k,p,r;
    F32 temp=1,temp1=1,s=0,s1=0;

     if(col==2)
     {
         for(i=0;i<row;i++)
            for(j=0;j<col;j++)
                if((i+j)%2)
                    temp1*=this->data[i*col+j];
                else
                    temp*=this->data[i*col+j];
         reslut=temp-temp1;
     }
     else
     {
         for(k=0;k<col;k++)
         {
             for(i=0,j=k;i<row&&j<col;i++,j++)
                temp*=this->data[i*col+j];
             if(row-i)
             {
                 for(p=row-i,r=row-1;p>0;p--,r--)
                    temp*=this->data[r*col+p-1];
             }
             s+=temp;
             temp=1;
         }

         for(k=col-1;k>=0;k--)
         {
             for(i=0,j=k;i<row&&j>=0;i++,j--)
                temp1*=this->data[i*col+j];
             if(row-i)
             {
                for(p=row-1,r=i;r<row;p--,r++)
                    temp1*=this->data[r*col+p];
             }
             s1+=temp1;
             temp1=1;
         }
         reslut=s-s1;
     }

}
//void  Matrix::pinv(Matrix &MatResult) const
//{
//    assert(row>0);
//    assert(col>0);
//    assert(row>=col);
//    Matrix V(col,col);
//    Matrix A(row,col),U(row,row);
//    Vector s(col);
//    A.CopyFrom(0,0,col,row,data);
//    A.Svdcmp(s,V);
//    for(S32 j=0; j<row; j++)
//        for(S32 k=0; k<col; k++)
//            U(j,k)=A(j,k);
//
//    S32 i;
//    for( i=0; i<col; i++)
//    {
//        if(fabs(s[i])<tol)
//            break;
//    }
//    if(i==0)
//        MatResult.Set(0);
//    else
//    {
//        for(S32 j=0;j<i;j++)
//            s[j]=1/s[j];
//        Matrix S(i,i),Vt(col,i),Ut(i,row);
//        S.SetDiag(s.GetData());
//
//        V.CopyTo(0,0,i,col,Vt.GetData());
//        Matrix At(U.GetCol(),U.GetRow());
//        U.Tran(At);
//        At.CopyTo(0,0,row,i,Ut.GetData());
//
//        MatResult=Vt*S*Ut;
//    }
//}

}
// namespace itr_math

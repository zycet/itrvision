#ifndef COMMFEATUREPOINT_H
#define COMMFEATUREPOINT_H

#include "itrbase.h"

using namespace itr_math;

namespace itr_vision
{
/**
* brief 通用特征点表示类
*/
class CommFeaturePoint:public Point2D
{
public:
    /**
    * \brief 建立空的特征点,需要执行Init后才可使用.
    */
    CommFeaturePoint();
    /**
    * \brief 释放占用的资源
    */
    virtual ~CommFeaturePoint();
    /**
    * \brief 对象深复制(要求特征维度一致)
    */
    CommFeaturePoint(const CommFeaturePoint &other);
    /**
    * \brief 以全面的参数初始化特征点
    */
    void Init(Point2D Pos,S32 ID,F32 Dir,F32 Quality,F32 Value);
    /**
    * \brief 面向一般特征点算法的特征点建立(光流,角点等),可继续添加类似方法.
    */
    void Init(Point2D Pos,F32 Quality,F32 Value);
    /**
    * \brief 用来进行区分或者反向索引的标示
    */
    S32 ID;
    /**
    * \brief 用以指向附加数据
    */
    void *Tag;
    /**
    * \brief 方向(采用角度制,从+X轴算起,指向+Y轴为正方向)
    */
    F32 Dir;
    /**
    * \brief 质量,用于表示此特征点的有效性,各算法可采用不同设计,一般上值越大质量越高.
    */
    F32 Quality;
    /**
    * \brief 如算法包含一个标量形式的用于区分的特征值可以使用此量
    */
    F32 Value;
    /**
    * \brief 比较两个特征点的有效性。即比较Quality
    */
    bool operator>(const CommFeaturePoint &other) const
    {
        return this->Quality>other.Quality;
    }
    bool operator<(const CommFeaturePoint &other) const
    {
        return this->Quality<other.Quality;
    }
protected:
private:
};
}
#endif // COMMFEATUREPOINT_H

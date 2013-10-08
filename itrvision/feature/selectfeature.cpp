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
 * selectfeature.cpp
 *  Created on: 2013-10-3
 *      Author: ghdawn
 */

#include "selectseature.h"

namespace itr_vision
{

    SelectFeature::SelectFeature(const ImageGray& Img, int WindowWidth) :
            img(Img)
    {
        // TODO Auto-generated constructor stub
        windowWidth = WindowWidth;
    }

    SelectFeature::~SelectFeature()
    {
        // TODO Auto-generated destructor stub
    }

    float SelectFeature::_minEigenvalue(float gxx, float gxy, float gyy)
    {
        itr_math::NumericalObj->Sqrt((gxx - gyy) * (gxx - gyy) + 4 * gxy * gxy, gxy);
        return (float) ((gxx + gyy - gxy) * 0.5f);
    }
    void SelectFeature::SelectGoodFeature(const RectangleS& rect, vector<FeaturePoint>& fl)
    {
        ImageGray gxx(rect.Width, rect.Height), gxy(rect.Width, rect.Height), gyy(rect.Width,
                rect.Height);
        vector<FeaturePoint> featurelist(rect.Width * rect.Height);
        int bordy = rect.Y + rect.Height;
        int bordx = rect.X + rect.Width;
        int x,y,xx,yy;
        for( y=rect.Y;y<bordy;++y)
        for( x=rect.X;x<bordx;++x)
        {
            for(yy=y-windowWidth;yy<=y+windowWidth;++yy)
                for(xx=x-windowWidth;xx<=x+windowWidth;++xx)
                {

                }
        }
    }

}
// namespace itr_vision

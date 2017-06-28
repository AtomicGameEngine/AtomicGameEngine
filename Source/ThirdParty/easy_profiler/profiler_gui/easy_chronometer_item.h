/************************************************************************
* file name         : easy_chronometer_item.h
* ----------------- :
* creation time     : 2016/09/15
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains declaration of EasyChronometerItem - an item
*                   : used to display selected interval on graphics scene.
* ----------------- :
* change log        : * 2016/09/15 Victor Zarubkin: moved sources from blocks_graphics_view.h
*                   :
*                   : *
* ----------------- :
* license           : Lightweight profiler library for c++
*                   : Copyright(C) 2016-2017  Sergey Yagovtsev, Victor Zarubkin
*                   :
*                   : Licensed under either of
*                   :     * MIT license (LICENSE.MIT or http://opensource.org/licenses/MIT)
*                   :     * Apache License, Version 2.0, (LICENSE.APACHE or http://www.apache.org/licenses/LICENSE-2.0)
*                   : at your option.
*                   :
*                   : The MIT License
*                   :
*                   : Permission is hereby granted, free of charge, to any person obtaining a copy
*                   : of this software and associated documentation files (the "Software"), to deal
*                   : in the Software without restriction, including without limitation the rights 
*                   : to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
*                   : of the Software, and to permit persons to whom the Software is furnished 
*                   : to do so, subject to the following conditions:
*                   : 
*                   : The above copyright notice and this permission notice shall be included in all 
*                   : copies or substantial portions of the Software.
*                   : 
*                   : THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
*                   : INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
*                   : PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
*                   : LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
*                   : TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE 
*                   : USE OR OTHER DEALINGS IN THE SOFTWARE.
*                   : 
*                   : The Apache License, Version 2.0 (the "License")
*                   :
*                   : You may not use this file except in compliance with the License.
*                   : You may obtain a copy of the License at
*                   :
*                   : http://www.apache.org/licenses/LICENSE-2.0
*                   :
*                   : Unless required by applicable law or agreed to in writing, software
*                   : distributed under the License is distributed on an "AS IS" BASIS,
*                   : WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*                   : See the License for the specific language governing permissions and
*                   : limitations under the License.
************************************************************************/

#ifndef EASY_CHRONOMETER_ITEM_H
#define EASY_CHRONOMETER_ITEM_H

#include <QGraphicsItem>
#include <QRectF>
#include <QPolygonF>
#include <QColor>

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class QWidget;
class QPainter;
class QStyleOptionGraphicsItem;
class EasyGraphicsView;

class EasyChronometerItem : public QGraphicsItem
{
    typedef QGraphicsItem Parent;
    typedef EasyChronometerItem This;

    QPolygonF  m_indicator; ///< Indicator displayed when this chrono item is out of screen (displaying only for main item)
    QRectF  m_boundingRect; ///< boundingRect (see QGraphicsItem)
    QColor         m_color; ///< Color of the item
    qreal  m_left, m_right; ///< Left and right bounds of the selection zone
    bool           m_bMain; ///< Is this chronometer main (true, by default)
    bool        m_bReverse; ///< 
    bool m_bHoverIndicator; ///< Mouse hover above indicator
    bool  m_bHoverLeftBorder;
    bool m_bHoverRightBorder;

public:

    explicit EasyChronometerItem(bool _main = true);
    virtual ~EasyChronometerItem();

    // Public virtual methods

    QRectF boundingRect() const override;
    void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget = nullptr) override;

public:

    // Public non-virtual methods

    void hide();

    void setColor(const QColor& _color);

    void setBoundingRect(qreal x, qreal y, qreal w, qreal h);
    void setBoundingRect(const QRectF& _rect);

    void setLeftRight(qreal _left, qreal _right);

    void setReverse(bool _reverse);

    void setHoverIndicator(bool _hover);

    bool indicatorContains(const QPointF& _pos) const;

    void setHoverLeft(bool _hover);
    void setHoverRight(bool _hover);

    bool hoverLeft(qreal _x) const;
    bool hoverRight(qreal _x) const;

    QPointF toItem(const QPointF& _pos) const;
    qreal toItem(qreal _x) const;

    inline bool hoverIndicator() const
    {
        return m_bHoverIndicator;
    }

    inline bool hoverLeft() const
    {
        return m_bHoverLeftBorder;
    }

    inline bool hoverRight() const
    {
        return m_bHoverRightBorder;
    }

    inline bool reverse() const
    {
        return m_bReverse;
    }

    inline qreal left() const
    {
        return m_left;
    }

    inline qreal right() const
    {
        return m_right;
    }

    inline qreal width() const
    {
        return m_right - m_left;
    }

private:

    ///< Returns pointer to the EasyGraphicsView widget.
    const EasyGraphicsView* view() const;
    EasyGraphicsView* view();

}; // END of class EasyChronometerItem.

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#endif // EASY_CHRONOMETER_ITEM_H

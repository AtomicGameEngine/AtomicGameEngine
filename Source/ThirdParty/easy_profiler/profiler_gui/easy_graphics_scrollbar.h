/************************************************************************
* file name         : easy_graphics_scrollbar.h
* ----------------- : 
* creation time     : 2016/07/04
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- : 
* description       : This file contains declaration of 
* ----------------- : 
* change log        : * 2016/07/04 Victor Zarubkin: Initial commit.
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

#ifndef EASY__GRAPHICS_SCROLLBAR__H
#define EASY__GRAPHICS_SCROLLBAR__H

#include <stdlib.h>
#include <thread>
#include <atomic>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QAction>
#include <QPolygonF>
#include <QImage>
#include "easy_qtimer.h"
#include "common_types.h"

//////////////////////////////////////////////////////////////////////////

// TODO: use profiler_core/spin_lock.h

#define EASY_GUI_USE_CRITICAL_SECTION // Use CRITICAL_SECTION instead of std::atomic_flag
#if defined(_WIN32) && defined(EASY_GUI_USE_CRITICAL_SECTION)
namespace profiler_gui {
    // std::atomic_flag on Windows works slower than critical section, so we will use it instead of std::atomic_flag...
    // By the way, Windows critical sections are slower than std::atomic_flag on Unix.
    class spin_lock { void* m_lock; public:
        void lock();
        void unlock();
        spin_lock();
        ~spin_lock();
    };
#else
namespace profiler_gui {
    // std::atomic_flag on Unix works fine and very fast (almost instant!)
    class spin_lock {
        ::std::atomic_flag m_lock; public:

        void lock() {
            while (m_lock.test_and_set(::std::memory_order_acquire));
        }

        void unlock() {
            m_lock.clear(::std::memory_order_release);
        }

        spin_lock() {
            m_lock.clear();
        }
    };
#endif

} // END of namespace profiler_gui.

//////////////////////////////////////////////////////////////////////////

class EasyGraphicsSliderItem : public QGraphicsRectItem
{
    typedef QGraphicsRectItem      Parent;
    typedef EasyGraphicsSliderItem   This;

private:

    QPolygonF m_indicator;
    qreal m_halfwidth;
    bool m_bMain;

public:

    explicit EasyGraphicsSliderItem(bool _main);
    virtual ~EasyGraphicsSliderItem();

    void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget = nullptr) override;

    qreal width() const;
    qreal halfwidth() const;

    void setWidth(qreal _width);
    void setHalfwidth(qreal _halfwidth);

    void setColor(QRgb _color);
    void setColor(const QColor& _color);

}; // END of class EasyGraphicsSliderItem.

//////////////////////////////////////////////////////////////////////////

class EasyHistogramItem : public QGraphicsItem
{
    typedef QGraphicsItem Parent;
    typedef EasyHistogramItem This;

public:

    enum HistRegime : uint8_t { Hist_Pointer, Hist_Id };

private:

    QRectF                               m_boundingRect;
    qreal                                 m_topDuration;
    qreal                              m_bottomDuration;
    qreal                                 m_maxDuration;
    qreal                                 m_minDuration;
    qreal                                      m_mouseY;
    qreal                                 m_imageOrigin;
    qreal                                  m_imageScale;
    qreal                           m_imageOriginUpdate;
    qreal                            m_imageScaleUpdate;
    qreal                           m_workerImageOrigin;
    qreal                            m_workerImageScale;
    qreal                           m_workerTopDuration;
    qreal                        m_workerBottomDuration;
    ::profiler::timestamp_t         m_blockTotalDuraion;
    QString                            m_topDurationStr;
    QString                         m_bottomDurationStr;
    QString                                m_threadName;
    QString                                 m_blockName;
    ::profiler::BlocksTree::children_t m_selectedBlocks;
    QImage                                  m_mainImage;
    EasyQTimer                                  m_timer;
    EasyQTimer                          m_boundaryTimer;
    ::std::thread                        m_workerThread;
    ::profiler::timestamp_t            m_threadDuration;
    ::profiler::timestamp_t        m_threadProfiledTime;
    ::profiler::timestamp_t            m_threadWaitTime;
    const ::profiler_gui::EasyItems*          m_pSource;
    QImage*                               m_workerImage;
    const ::profiler::BlocksTreeRoot* m_pProfilerThread;
    ::profiler::thread_id_t                  m_threadId;
    ::profiler::block_index_t                 m_blockId;
    int                                      m_timeouts;
    ::profiler_gui::TimeUnits               m_timeUnits;
    HistRegime                                 m_regime;
    bool                           m_bPermitImageUpdate; ///< Is false when m_workerThread is parsing input dataset (when setSource(_block_id) is called)
    ::profiler_gui::spin_lock                    m_spin;
    ::std::atomic_bool                         m_bReady;

public:

    explicit EasyHistogramItem();
    virtual ~EasyHistogramItem();

    // Public virtual methods

    QRectF boundingRect() const override;
    void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget = nullptr) override;

public:

    // Public non-virtual methods

    ::profiler::thread_id_t threadId() const;

    void setBoundingRect(const QRectF& _rect);
    void setBoundingRect(qreal x, qreal y, qreal w, qreal h);

    void setSource(::profiler::thread_id_t _thread_id, const ::profiler_gui::EasyItems* _items);
    void setSource(::profiler::thread_id_t _thread_id, ::profiler::block_id_t _block_id);
    void rebuildSource(HistRegime _regime);
    void rebuildSource();
    void validateName();
    void updateImage();
    void cancelImageUpdate();

    void pickTopBoundary(qreal _y);
    void increaseTopBoundary();
    void decreaseTopBoundary();

    void pickBottomBoundary(qreal _y);
    void increaseBottomBoundary();
    void decreaseBottomBoundary();

    void setMouseY(qreal _mouseY);
    void pickFrameTime(qreal _y) const;

    void onValueChanged();
    void onModeChanged();

private:

    void paintBusyIndicator(QPainter* _painter, qreal _current_scale);
    void paintMouseIndicator(QPainter* _painter, qreal _top, qreal _bottom, qreal _width, qreal _height, qreal _top_width, qreal _mouse_y, qreal _delta_time, int _font_h);
    void paintByPtr(QPainter* _painter);
    void paintById(QPainter* _painter);
    void onTimeout();
    void updateImage(QRectF _boundingRect, HistRegime _regime, qreal _current_scale,
                     qreal _minimum, qreal _maximum, qreal _range,
                     qreal _value, qreal _width, qreal _top_duration, qreal _bottom_duration, bool _bindMode,
                     float _frame_time, ::profiler::timestamp_t _begin_time, qreal _origin, bool _autoAdjustHist);

}; // END of class EasyHistogramItem.

//////////////////////////////////////////////////////////////////////////

class EasyGraphicsScrollbar : public QGraphicsView
{
    Q_OBJECT

private:

    typedef QGraphicsView         Parent;
    typedef EasyGraphicsScrollbar   This;

    qreal                           m_minimumValue;
    qreal                           m_maximumValue;
    qreal                                  m_value;
    qreal                            m_windowScale;
    QPoint                         m_mousePressPos;
    Qt::MouseButtons                m_mouseButtons;
    EasyGraphicsSliderItem*               m_slider;
    EasyGraphicsSliderItem* m_chronometerIndicator;
    EasyHistogramItem*             m_histogramItem;
    int                        m_defaultFontHeight;
    bool                              m_bScrolling;
    bool                               m_bBindMode;
    bool                                 m_bLocked;

public:

    explicit EasyGraphicsScrollbar(QWidget* _parent = nullptr);
    virtual ~EasyGraphicsScrollbar();

    // Public virtual methods

    void mousePressEvent(QMouseEvent* _event) override;
    void mouseReleaseEvent(QMouseEvent* _event) override;
    void mouseMoveEvent(QMouseEvent* _event) override;
    void wheelEvent(QWheelEvent* _event) override;
    void resizeEvent(QResizeEvent* _event) override;

    void dragEnterEvent(QDragEnterEvent*) override {}

public:

    // Public non-virtual methods

    void clear();

    bool bindMode() const;
    qreal getWindowScale() const;
    ::profiler::thread_id_t hystThread() const;

    qreal minimum() const;
    qreal maximum() const;
    qreal range() const;
    qreal value() const;
    qreal sliderWidth() const;
    qreal sliderHalfWidth() const;
    int defaultFontHeight() const;

    void setValue(qreal _value);
    void setRange(qreal _minValue, qreal _maxValue);
    void setSliderWidth(qreal _width);
    void setChronoPos(qreal _left, qreal _right);
    void showChrono();
    void hideChrono();

    void setHistogramSource(::profiler::thread_id_t _thread_id, const::profiler_gui::EasyItems* _items);
    void setHistogramSource(::profiler::thread_id_t _thread_id, ::profiler::block_id_t _block_id);

    inline void setHistogramSource(::profiler::thread_id_t _thread_id, const ::profiler_gui::EasyItems& _items)
    {
        setHistogramSource(_thread_id, &_items);
    }

    inline void lock()
    {
        m_bLocked = true;
    }

    inline void unlock()
    {
        m_bLocked = false;
    }

signals:

    void rangeChanged();
    void valueChanged(qreal _value);
    void wheeled(qreal _mouseX, int _wheelDelta);

private slots:

    void onThreadViewChanged();
    void onWindowWidthChange(qreal _width);

}; // END of class EasyGraphicsScrollbar.

//////////////////////////////////////////////////////////////////////////

#endif // EASY__GRAPHICS_SCROLLBAR__H

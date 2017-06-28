/************************************************************************
* file name         : easy_graphics_scrollbar.cpp
* ----------------- :
* creation time     : 2016/07/04
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : .
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

#include <algorithm>
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QContextMenuEvent>
#include <QMenu>
#include "easy_graphics_scrollbar.h"
#include "globals.h"


// TODO: use profiler_core/spin_lock.h

#if defined(_WIN32) && defined(EASY_GUI_USE_CRITICAL_SECTION)
# include <Windows.h>
# ifdef min
#  undef min
# endif
# ifdef max
#  undef max
# endif

namespace profiler_gui {
    void spin_lock::lock() {
        EnterCriticalSection((CRITICAL_SECTION*)m_lock);
    }

    void spin_lock::unlock() {
        LeaveCriticalSection((CRITICAL_SECTION*)m_lock);
    }

    spin_lock::spin_lock() : m_lock(new CRITICAL_SECTION) {
        InitializeCriticalSection((CRITICAL_SECTION*)m_lock);
    }

    spin_lock::~spin_lock() {
        DeleteCriticalSection((CRITICAL_SECTION*)m_lock);
        delete ((CRITICAL_SECTION*)m_lock);
    }
}
#endif

//////////////////////////////////////////////////////////////////////////

const int DEFAULT_TOP = -40;
const int DEFAULT_HEIGHT = 80;
const int INDICATOR_SIZE = 6;
const int INDICATOR_SIZE_x2 = INDICATOR_SIZE << 1;
const int HIST_COLUMN_MIN_HEIGHT = 2;
const int WORKER_THREAD_CHECK_INTERVAL = 40;
const int BOUNDARY_TIMER_INTERVAL = 100;

//////////////////////////////////////////////////////////////////////////

inline qreal clamp(qreal _minValue, qreal _value, qreal _maxValue)
{
    return (_value < _minValue ? _minValue : (_value > _maxValue ? _maxValue : _value));
}

inline qreal sqr(qreal _value)
{
    return _value * _value;
}

inline qreal calculate_color1(qreal h, qreal, qreal k)
{
    return ::std::min(h * k, 0.9999999);
}

inline qreal calculate_color2(qreal, qreal duration, qreal k)
{
    return ::std::min(sqr(sqr(duration)) * k, 0.9999999);
}

//////////////////////////////////////////////////////////////////////////

EasyGraphicsSliderItem::EasyGraphicsSliderItem(bool _main) : Parent(), m_halfwidth(0), m_bMain(_main)
{
    m_indicator.reserve(3);

    if (_main)
    {
        m_indicator.push_back(QPointF(0, DEFAULT_TOP + INDICATOR_SIZE));
        m_indicator.push_back(QPointF(-INDICATOR_SIZE, DEFAULT_TOP));
        m_indicator.push_back(QPointF(INDICATOR_SIZE, DEFAULT_TOP));
    }
    else
    {
        m_indicator.push_back(QPointF(0, DEFAULT_TOP + DEFAULT_HEIGHT - INDICATOR_SIZE));
        m_indicator.push_back(QPointF(-INDICATOR_SIZE, DEFAULT_TOP + DEFAULT_HEIGHT));
        m_indicator.push_back(QPointF(INDICATOR_SIZE, DEFAULT_TOP + DEFAULT_HEIGHT));
    }

    setWidth(1);
    setBrush(Qt::SolidPattern);
}

EasyGraphicsSliderItem::~EasyGraphicsSliderItem()
{

}

void EasyGraphicsSliderItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget)
{
    if (static_cast<const EasyGraphicsScrollbar*>(scene()->parent())->bindMode())
    {
        return;
    }

    const auto currentScale = static_cast<const EasyGraphicsScrollbar*>(scene()->parent())->getWindowScale();
    const auto br = rect();

    qreal w = width() * currentScale;
    QRectF r(br.left() * currentScale, br.top() + INDICATOR_SIZE, w, br.height() - INDICATOR_SIZE_x2);
    const auto r_right = r.right();
    const auto r_bottom = r.bottom();
    auto b = brush();

    _painter->save();
    _painter->setTransform(QTransform::fromScale(1.0 / currentScale, 1), true);
    _painter->setBrush(b);
    
    if (w > 1)
    {
        _painter->setPen(Qt::NoPen);
        _painter->drawRect(r);

        // Draw left and right borders
        auto cmode = _painter->compositionMode();
        if (m_bMain) _painter->setCompositionMode(QPainter::CompositionMode_Exclusion);
        _painter->setPen(QColor::fromRgba(0xe0000000 | b.color().rgb()));
        _painter->drawLine(QPointF(r.left(), r.top()), QPointF(r.left(), r_bottom));
        _painter->drawLine(QPointF(r_right, r.top()), QPointF(r_right, r_bottom));
        if (!m_bMain) _painter->setCompositionMode(cmode);
    }
    else
    {
        _painter->setPen(QColor::fromRgba(0xe0000000 | b.color().rgb()));
        _painter->drawLine(QPointF(r.left(), r.top()), QPointF(r.left(), r_bottom));
        if (m_bMain) _painter->setCompositionMode(QPainter::CompositionMode_Exclusion);
    }

    // Draw triangle indicators for small slider
    _painter->setTransform(QTransform::fromTranslate(r.left() + w * 0.5, 0), true);
    _painter->setPen(b.color().rgb());
    _painter->drawPolygon(m_indicator);

    _painter->restore();
}

qreal EasyGraphicsSliderItem::width() const
{
    return m_halfwidth * 2.0;
}

qreal EasyGraphicsSliderItem::halfwidth() const
{
    return m_halfwidth;
}

void EasyGraphicsSliderItem::setWidth(qreal _width)
{
    m_halfwidth = _width * 0.5;
    setRect(-m_halfwidth, DEFAULT_TOP, _width, DEFAULT_HEIGHT);
}

void EasyGraphicsSliderItem::setHalfwidth(qreal _halfwidth)
{
    m_halfwidth = _halfwidth;
    setRect(-m_halfwidth, DEFAULT_TOP, m_halfwidth * 2.0, DEFAULT_HEIGHT);
}

void EasyGraphicsSliderItem::setColor(QRgb _color)
{
    setColor(QColor::fromRgba(_color));
}

void EasyGraphicsSliderItem::setColor(const QColor& _color)
{
    auto b = brush();
    b.setColor(_color);
    setBrush(b);
}

//////////////////////////////////////////////////////////////////////////

EasyHistogramItem::EasyHistogramItem() : Parent(nullptr)
    , m_threadDuration(0)
    , m_threadProfiledTime(0)
    , m_threadWaitTime(0)
    , m_pSource(nullptr)
    , m_workerImage(nullptr)
    , m_topDuration(0)
    , m_maxDuration(0)
    , m_minDuration(0)
    , m_imageOrigin(0)
    , m_imageScale(1)
    , m_workerImageOrigin(0)
    , m_workerImageScale(1)
    , m_workerTopDuration(0)
    , m_workerBottomDuration(0)
    , m_blockTotalDuraion(0)
    , m_timer(::std::bind(&This::onTimeout, this))
    , m_boundaryTimer([this](){ updateImage(); }, true)
    , m_pProfilerThread(nullptr)
    , m_threadId(0)
    , m_blockId(::profiler_gui::numeric_max<decltype(m_blockId)>())
    , m_timeouts(0)
    , m_timeUnits(::profiler_gui::TimeUnits_auto)
    , m_regime(Hist_Pointer)
    , m_bPermitImageUpdate(false)
{
    m_bReady = ATOMIC_VAR_INIT(false);
}

EasyHistogramItem::~EasyHistogramItem()
{
    m_bReady.store(true, ::std::memory_order_release);
    if (m_workerThread.joinable())
        m_workerThread.join();
    delete m_workerImage;
}

QRectF EasyHistogramItem::boundingRect() const
{
    return m_boundingRect;
}

void EasyHistogramItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget)
{
    if (!m_bPermitImageUpdate || (m_regime == Hist_Pointer && m_pSource == nullptr) || (m_regime == Hist_Id && (m_threadId == 0 || ::profiler_gui::is_max(m_blockId))))
        return;

    if (m_regime == Hist_Pointer)
        paintByPtr(_painter);
    else
        paintById(_painter);
}

void EasyHistogramItem::paintBusyIndicator(QPainter* _painter, qreal _current_scale)
{
    const auto width = m_boundingRect.width() * _current_scale;
    const auto h = _painter->fontMetrics().height();

    _painter->setPen(Qt::black);
    _painter->drawText(QRectF(0, m_boundingRect.top(), width, m_boundingRect.height() - h),
                       Qt::AlignCenter, "Generating image");
    _painter->drawText(QRectF(0, m_boundingRect.top() + h, width, m_boundingRect.height() - h),
                       Qt::AlignCenter, QString(m_timeouts, QChar('.')));
}

void EasyHistogramItem::paintMouseIndicator(QPainter* _painter, qreal _top, qreal _bottom, qreal _width, qreal _height, qreal _top_width, qreal _mouse_y, qreal _delta_time, int _font_h)
{
    if (_font_h != 0 && _top < _mouse_y && _mouse_y < _bottom)
    {
        const int half_font_h = _font_h >> 1;

        _painter->setPen(Qt::blue);

        const auto mouseStr = ::profiler_gui::timeStringReal(m_timeUnits, m_bottomDuration + _delta_time * (_bottom - _mouse_y) / _height, 3);
        qreal mouseIndicatorRight = _width;
        if (_mouse_y < _top + half_font_h)
            mouseIndicatorRight = _top_width;

        qreal mouseIndicatorLeft = 0;
        const QRectF rect(0, _mouse_y - _font_h, _width, _font_h << 1);
        if (_mouse_y > _bottom - half_font_h)
        {
            _painter->drawText(rect, Qt::AlignLeft | Qt::AlignTop, mouseStr);
        }
        else if (_mouse_y < _top + half_font_h)
        {
            _painter->drawText(rect, Qt::AlignLeft | Qt::AlignBottom, mouseStr);
        }
        else
        {
            _painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, mouseStr);
            mouseIndicatorLeft = _painter->fontMetrics().width(mouseStr) + 3;
        }

        _painter->drawLine(QLineF(mouseIndicatorLeft, _mouse_y, mouseIndicatorRight, _mouse_y));
    }
}

void EasyHistogramItem::paintByPtr(QPainter* _painter)
{
    const auto widget = static_cast<const EasyGraphicsScrollbar*>(scene()->parent());
    const bool bindMode = widget->bindMode();
    const auto currentScale = widget->getWindowScale();
    const auto bottom = m_boundingRect.bottom();
    const auto width = m_boundingRect.width() * currentScale;
    const auto dtime = m_topDuration - m_bottomDuration;
    const auto maxColumnHeight = m_boundingRect.height();
    const auto coeff = (m_boundingRect.height() - HIST_COLUMN_MIN_HEIGHT) / (dtime > 1e-3 ? dtime : 1.);

    QRectF rect;
    QBrush brush(Qt::SolidPattern);
    //QRgb previousColor = 0;

    _painter->save();
    _painter->setTransform(QTransform::fromScale(1.0 / currentScale, 1), true);

    if (!m_pSource->empty())
    {
        _painter->setPen(Qt::NoPen);

        if (!bindMode)
            _painter->drawImage(0, m_boundingRect.top(), m_mainImage);
        else
        {
            const auto range = widget->sliderWidth();
            const auto minimum = widget->value();
            const auto slider_k = widget->range() / range;

            /*if (false)//slider_k < 8)
            {
                _painter->setTransform(QTransform::fromScale(slider_k, 1), true);
                _painter->drawImage((widget->minimum() - minimum) * currentScale, m_boundingRect.top(), m_mainImage);
                _painter->setTransform(QTransform::fromScale(1. / slider_k, 1), true);
            }
            else*/
            {
                const auto deltaScale = slider_k / m_imageScale;
                _painter->setTransform(QTransform::fromScale(deltaScale, 1), true);
                _painter->drawImage((widget->minimum() + m_imageOrigin - minimum) * currentScale * m_imageScale, m_boundingRect.top(), m_mainImage);
                _painter->setTransform(QTransform::fromScale(1. / deltaScale, 1), true);
            }

            /*if (false)
            {
                const bool gotFrame = EASY_GLOBALS.frame_time > 1e-6f;
                qreal frameCoeff = 1;
                if (gotFrame)
                {
                    if (EASY_GLOBALS.frame_time <= m_bottomDuration)
                        frameCoeff = m_boundingRect.height();
                    else
                        frameCoeff = 0.9 / EASY_GLOBALS.frame_time;
                }

                auto const calculate_color = gotFrame ? calculate_color2 : calculate_color1;
                auto const k = gotFrame ? sqr(sqr(frameCoeff)) : 1.0 / m_boundingRect.height();

                const auto& items = *m_pSource;
                const auto maximum = minimum + range;
                const auto realScale = currentScale * slider_k;
                const auto offset = minimum * realScale;

                auto first = ::std::lower_bound(items.begin(), items.end(), minimum, [](const ::profiler_gui::EasyBlockItem& _item, qreal _value)
                {
                    return _item.left() < _value;
                });

                if (first != items.end())
                {
                    if (first != items.begin())
                        --first;
                }
                else
                {
                    first = items.begin() + items.size() - 1;
                }

                qreal previous_x = -1e30, previous_h = -1e30;
                for (auto it = first, end = items.end(); it != end; ++it)
                {
                    // Draw rectangle

                    if (it->left() > maximum)
                        break;

                    if (it->right() < minimum)
                        continue;

                    const qreal item_x = it->left() * realScale - offset;
                    const qreal item_w = ::std::max(it->width() * realScale, 1.0);
                    const qreal item_r = item_x + item_w;
                    const qreal h = it->width() <= m_bottomDuration ? HIST_COLUMN_MIN_HEIGHT :
                        (it->width() > m_topDuration ? maxColumnHeight : (HIST_COLUMN_MIN_HEIGHT + (it->width() - m_bottomDuration) * coeff));

                    if (h < previous_h && item_r < previous_x)
                        continue;

                    const auto col = calculate_color(h, it->width(), k);
                    const auto color = 0x00ffffff & QColor::fromHsvF((1.0 - col) * 0.375, 0.85, 0.85).rgb();

                    if (previousColor != color)
                    {
                        // Set background color brush for rectangle
                        previousColor = color;
                        brush.setColor(QColor::fromRgba(0xc0000000 | color));
                        _painter->setBrush(brush);
                    }

                    rect.setRect(item_x, bottom - h, item_w, h);
                    _painter->drawRect(rect);

                    previous_x = item_r;
                    previous_h = h;
                }
            }*/
        }
    }

    //if (!m_bReady.load(::std::memory_order_acquire))
    //    paintBusyIndicator(_painter, currentScale);

    qreal top_width = width, bottom_width = width;
    int font_h = 0;
    if (!m_topDurationStr.isEmpty())
    {
        rect.setRect(0, m_boundingRect.top() - INDICATOR_SIZE, width - 3, m_boundingRect.height() + INDICATOR_SIZE_x2);

        if (m_timeUnits != EASY_GLOBALS.time_units)
        {
            m_timeUnits = EASY_GLOBALS.time_units;
            m_topDurationStr = ::profiler_gui::timeStringReal(m_timeUnits, m_topDuration, 3);
            m_bottomDurationStr = ::profiler_gui::timeStringReal(m_timeUnits, m_bottomDuration, 3);
        }

        auto fm = _painter->fontMetrics();
        font_h = fm.height();
        //bottom_width -= fm.width(m_bottomDurationStr) + 7;
        top_width -= fm.width(m_topDurationStr) + 7;

        _painter->setPen(m_topDuration < m_maxDuration ? Qt::darkRed : Qt::black);
        _painter->drawText(rect, Qt::AlignRight | Qt::AlignTop, m_topDurationStr);

        rect.setRect(0, bottom, width - 3, font_h);
        _painter->setPen(m_bottomDuration > m_minDuration ? Qt::darkRed : Qt::black);
        _painter->drawText(rect, Qt::AlignRight | Qt::AlignTop, m_bottomDurationStr);
    }

    _painter->setPen(Qt::darkGray);
    _painter->drawLine(QLineF(0, bottom, bottom_width, bottom));
    _painter->drawLine(QLineF(0, m_boundingRect.top(), top_width, m_boundingRect.top()));

    paintMouseIndicator(_painter, m_boundingRect.top(), bottom, width, maxColumnHeight - HIST_COLUMN_MIN_HEIGHT, top_width, m_mouseY, dtime, font_h);

    if (m_bottomDuration < EASY_GLOBALS.frame_time && EASY_GLOBALS.frame_time < m_topDuration)
    {
        // Draw marker displaying expected frame_time step
        const auto h = bottom - (EASY_GLOBALS.frame_time - m_bottomDuration) * coeff;
        _painter->setPen(Qt::DashLine);

        auto w = width;
        const auto boundary = INDICATOR_SIZE - font_h;
        if (h < (m_boundingRect.top() - boundary))
            w = top_width;
        else if (h > (bottom + boundary))
            w = bottom_width;

        _painter->drawLine(QLineF(0, h, w, h));
    }

    _painter->setPen(Qt::black);
    rect.setRect(0, bottom + 2, width, widget->defaultFontHeight());
    const auto eventsSize = m_pProfilerThread->events.size();
    _painter->drawText(rect, Qt::AlignHCenter | Qt::TextDontClip, QString("%1  |  duration: %2  |  profiled: %3 (%4%)  |  wait: %5 (%6%)  |  %7 frames  |  %8 blocks  |  %9 markers")
                       .arg(m_threadName)
                       .arg(::profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units, m_threadDuration))
                       .arg(::profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units, m_threadProfiledTime))
                       .arg(m_threadDuration ? QString::number(100. * (double)m_threadProfiledTime / (double)m_threadDuration, 'f', 2) : QString("0"))
                       .arg(::profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units, m_threadWaitTime))
                       .arg(m_threadDuration ? QString::number(100. * (double)m_threadWaitTime / (double)m_threadDuration, 'f', 2) : QString("0"))
                       .arg(m_pProfilerThread->frames_number)
                       .arg(m_pProfilerThread->blocks_number - eventsSize)
                       .arg(eventsSize));

    _painter->drawText(rect, Qt::AlignLeft, bindMode ? " MODE: zoom" : " MODE: overview");

    _painter->restore();
}

void EasyHistogramItem::paintById(QPainter* _painter)
{
    const auto widget = static_cast<const EasyGraphicsScrollbar*>(scene()->parent());
    const bool bindMode = widget->bindMode();
    const auto currentScale = widget->getWindowScale();
    const auto bottom = m_boundingRect.bottom();
    const auto width = m_boundingRect.width() * currentScale;
    const auto dtime = m_topDuration - m_bottomDuration;
    const auto maxColumnHeight = m_boundingRect.height();
    const auto coeff = (m_boundingRect.height() - HIST_COLUMN_MIN_HEIGHT) / (dtime > 1e-3 ? dtime : 1.);

    QRectF rect;
    QBrush brush(Qt::SolidPattern);
    //QRgb previousColor = 0;

    _painter->save();
    _painter->setTransform(QTransform::fromScale(1.0 / currentScale, 1), true);

    const auto& items = m_selectedBlocks;
    if (!items.empty())
    {
        _painter->setPen(Qt::NoPen);

        if (!bindMode)
            _painter->drawImage(0, m_boundingRect.top(), m_mainImage);
        else
        {
            const auto range = widget->sliderWidth();
            auto minimum = widget->value();
            const auto slider_k = widget->range() / range;

            /*if (false)//slider_k < 8)
            {
                _painter->setTransform(QTransform::fromScale(slider_k, 1), true);
                _painter->drawImage((widget->minimum() - minimum) * currentScale, m_boundingRect.top(), m_mainImage);
                _painter->setTransform(QTransform::fromScale(1. / slider_k, 1), true);
            }
            else*/
            {
                const auto deltaScale = slider_k / m_imageScale;
                _painter->setTransform(QTransform::fromScale(deltaScale, 1), true);
                _painter->drawImage((widget->minimum() + m_imageOrigin - minimum) * currentScale * m_imageScale, m_boundingRect.top(), m_mainImage);
                _painter->setTransform(QTransform::fromScale(1. / deltaScale, 1), true);
            }

            /*if (false)
            {
                minimum *= 1e3;
                const auto maximum = minimum + range * 1e3;
                const auto realScale = currentScale * slider_k;
                const auto offset = minimum * realScale;

                auto first = ::std::lower_bound(items.begin(), items.end(), minimum + EASY_GLOBALS.begin_time, [](::profiler::block_index_t _item, qreal _value)
                {
                    return easyBlock(_item).tree.node->begin() < _value;
                });

                if (first != items.end())
                {
                    if (first != items.begin())
                        --first;
                }
                else
                {
                    first = items.begin() + (items.size() - 1);
                }

                auto last = ::std::upper_bound(first, items.end(), maximum + EASY_GLOBALS.begin_time, [](qreal _value, ::profiler::block_index_t _item)
                {
                    return _value < easyBlock(_item).tree.node->begin();
                });

                const auto n = static_cast<uint32_t>(::std::distance(first, last));

                if (n > 0)
                {
                    const bool gotFrame = EASY_GLOBALS.frame_time > 1e-6f;
                    qreal frameCoeff = 1;
                    if (gotFrame)
                    {
                        if (EASY_GLOBALS.frame_time <= m_bottomDuration)
                            frameCoeff = m_boundingRect.height();
                        else
                            frameCoeff = 0.9 / EASY_GLOBALS.frame_time;
                    }

                    auto const calculate_color = gotFrame ? calculate_color2 : calculate_color1;
                    auto const k = gotFrame ? sqr(sqr(frameCoeff)) : 1.0 / m_boundingRect.height();

                    const auto draw = [this, &previousColor, &brush, &_painter](qreal x, qreal y, qreal w, qreal h, QRgb color)
                    {
                        m_spin.lock();

                        if (previousColor != color)
                        {
                            // Set background color brush for rectangle
                            previousColor = color;
                            brush.setColor(QColor::fromRgba(0xc0000000 | color));
                            _painter->setBrush(brush);
                        }

                        _painter->drawRect(QRectF(x, y, w, h));

                        m_spin.unlock();
                    };

                    ::std::vector<::std::thread> threads;
                    const auto n_threads = ::std::min(n, ::std::thread::hardware_concurrency());
                    threads.reserve(n_threads);
                    const auto n_items = n / n_threads;
                    for (uint32_t i = 0; i < n_threads; ++i)
                    {
                        auto begin = first + i * n_items;
                        threads.emplace_back([this, &draw, &maximum, &minimum, &realScale, &offset, &coeff, &calculate_color, &k, &bottom, &maxColumnHeight](decltype(begin) it, decltype(begin) end)
                        {
                            qreal previous_x = -1e30, previous_h = -1e30;

                            //for (auto it = first, end = items.end(); it != end; ++it)
                            for (; it != end; ++it)
                            {
                                // Draw rectangle
                                const auto item = easyBlock(*it).tree.node;

                                const auto beginTime = item->begin() - EASY_GLOBALS.begin_time;
                                if (beginTime > maximum)
                                    break;

                                const auto endTime = item->end() - EASY_GLOBALS.begin_time;
                                if (endTime < minimum)
                                    continue;

                                const qreal duration = item->duration() * 1e-3;
                                const qreal item_x = (beginTime * realScale - offset) * 1e-3;
                                const qreal item_w = ::std::max(duration * realScale, 1.0);
                                const qreal item_r = item_x + item_w;
                                const qreal h = duration <= m_bottomDuration ? HIST_COLUMN_MIN_HEIGHT :
                                    (duration > m_topDuration ? maxColumnHeight : (HIST_COLUMN_MIN_HEIGHT + (duration - m_bottomDuration) * coeff));

                                if (h < previous_h && item_r < previous_x)
                                    continue;

                                const auto col = calculate_color(h, duration, k);
                                const auto color = 0x00ffffff & QColor::fromHsvF((1.0 - col) * 0.375, 0.85, 0.85).rgb();

                                draw(item_x, bottom - h, item_w, h, color);
                                //if (previousColor != color)
                                //{
                                //    // Set background color brush for rectangle
                                //    previousColor = color;
                                //    brush.setColor(QColor::fromRgba(0xc0000000 | color));
                                //    _painter->setBrush(brush);
                                //}

                                //rect.setRect(item_x, bottom - h, item_w, h);
                                //_painter->drawRect(rect);

                                previous_x = item_r;
                                previous_h = h;
                            }
                        }, begin, i == (n_threads - 1) ? items.end() : begin + n_items);
                    }

                    for (auto& t : threads)
                        t.join();
                }
            }*/
        }
    }

    //if (!m_bReady.load(::std::memory_order_acquire))
    //    paintBusyIndicator(_painter, currentScale);

    qreal top_width = width, bottom_width = width;
    int font_h = 0;
    if (!m_topDurationStr.isEmpty())
    {
        rect.setRect(0, m_boundingRect.top() - INDICATOR_SIZE, width - 3, m_boundingRect.height() + INDICATOR_SIZE_x2);

        if (m_timeUnits != EASY_GLOBALS.time_units)
        {
            m_timeUnits = EASY_GLOBALS.time_units;
            m_topDurationStr = ::profiler_gui::timeStringReal(m_timeUnits, m_topDuration, 3);
            m_bottomDurationStr = ::profiler_gui::timeStringReal(m_timeUnits, m_bottomDuration, 3);
        }

        auto fm = _painter->fontMetrics();
        font_h = fm.height();
        //bottom_width -= fm.width(m_bottomDurationStr) + 7;
        top_width -= fm.width(m_topDurationStr) + 7;

        _painter->setPen(Qt::black);
        _painter->setPen(m_topDuration < m_maxDuration ? Qt::darkRed : Qt::black);
        _painter->drawText(rect, Qt::AlignRight | Qt::AlignTop, m_topDurationStr);

        rect.setRect(0, bottom, width - 3, font_h);
        _painter->setPen(m_bottomDuration > m_minDuration ? Qt::darkRed : Qt::black);
        _painter->drawText(rect, Qt::AlignRight | Qt::AlignTop, m_bottomDurationStr);
    }

    _painter->setPen(Qt::darkGray);
    _painter->drawLine(QLineF(0, bottom, bottom_width, bottom));
    _painter->drawLine(QLineF(0, m_boundingRect.top(), top_width, m_boundingRect.top()));

    paintMouseIndicator(_painter, m_boundingRect.top(), bottom, width, maxColumnHeight - HIST_COLUMN_MIN_HEIGHT, top_width, m_mouseY, dtime, font_h);

    if (m_bottomDuration < EASY_GLOBALS.frame_time && EASY_GLOBALS.frame_time < m_topDuration)
    {
        // Draw marker displaying required frame_time step
        const auto h = bottom - (EASY_GLOBALS.frame_time - m_bottomDuration) * coeff;
        _painter->setPen(Qt::DashLine);

        auto w = width;
        const auto boundary = INDICATOR_SIZE - font_h;
        if (h < (m_boundingRect.top() - boundary))
            w = top_width;
        else if (h >(bottom + boundary))
            w = bottom_width;

        _painter->drawLine(QLineF(0, h, w, h));
    }

    _painter->setPen(Qt::black);
    rect.setRect(0, bottom + 2, width, widget->defaultFontHeight());

    if (!m_selectedBlocks.empty())
    {
        _painter->drawText(rect, Qt::AlignHCenter | Qt::TextDontClip, QString("%1  |  %2  |  %3 calls  |  %4% of thread profiled time")
                           .arg(m_threadName).arg(m_blockName).arg(m_selectedBlocks.size())
                           .arg(m_threadProfiledTime ? QString::number(100. * (double)m_blockTotalDuraion / (double)m_threadProfiledTime, 'f', 2) : QString("100")));
    }
    else
    {
        _painter->drawText(rect, Qt::AlignHCenter | Qt::TextDontClip, QString("%1  |  %2  |  0 calls").arg(m_threadName).arg(m_blockName));
    }

    _painter->drawText(rect, Qt::AlignLeft, bindMode ? " MODE: zoom" : " MODE: overview");

    _painter->restore();
}

::profiler::thread_id_t EasyHistogramItem::threadId() const
{
    return m_threadId;
}

void EasyHistogramItem::setBoundingRect(const QRectF& _rect)
{
    m_boundingRect = _rect;
}

void EasyHistogramItem::setBoundingRect(qreal x, qreal y, qreal w, qreal h)
{
    m_boundingRect.setRect(x, y, w, h);
}

void EasyHistogramItem::rebuildSource(HistRegime _regime)
{
    if (m_regime == _regime)
        rebuildSource();
}

void EasyHistogramItem::rebuildSource()
{
    if (m_regime == Hist_Id)
    {
        m_regime = Hist_Pointer;
        setSource(m_threadId, m_blockId);
    }
    else
    {
        m_regime = Hist_Id;
        setSource(m_threadId, m_pSource);
    }
}

void EasyHistogramItem::setSource(::profiler::thread_id_t _thread_id, const ::profiler_gui::EasyItems* _items)
{
    if (m_regime == Hist_Pointer && m_threadId == _thread_id && m_pSource == _items)
        return;

    m_timer.stop();
    m_boundaryTimer.stop();

    m_bReady.store(true, ::std::memory_order_release);
    if (m_workerThread.joinable())
        m_workerThread.join();

    m_blockName.clear();
    m_blockTotalDuraion = 0;

    delete m_workerImage;
    m_workerImage = nullptr;
    m_imageOriginUpdate = m_imageOrigin = 0;
    m_imageScaleUpdate = m_imageScale = 1;

    m_selectedBlocks.clear();
    { ::profiler::BlocksTree::children_t().swap(m_selectedBlocks); }

    m_bPermitImageUpdate = false;
    m_regime = Hist_Pointer;
    m_pSource = _items;
    m_threadId = _thread_id;
    ::profiler_gui::set_max(m_blockId);

    if (m_pSource != nullptr)
    {
        if (m_pSource->empty())
        {
            m_pSource = nullptr;
        }
        else
        {
            const auto& root = EASY_GLOBALS.profiler_blocks[_thread_id];
            m_threadName = ::profiler_gui::decoratedThreadName(EASY_GLOBALS.use_decorated_thread_name, root, EASY_GLOBALS.hex_thread_id);

            if (root.children.empty())
                m_threadDuration = 0;
            else
                m_threadDuration = easyBlock(root.children.back()).tree.node->end() - easyBlock(root.children.front()).tree.node->begin();

            m_threadProfiledTime = root.profiled_time;
            m_threadWaitTime = root.wait_time;
            m_pProfilerThread = &root;
            m_timeUnits = EASY_GLOBALS.time_units;

            m_bReady.store(false, ::std::memory_order_release);
            m_workerThread = ::std::thread([this](const ::profiler_gui::EasyItems* _source)
            {
                m_maxDuration = 0;
                m_minDuration = 1e30;

                bool empty = true;
                for (const auto& item : *_source)
                {
                    if (m_bReady.load(::std::memory_order_acquire))
                        return;

                    if (easyDescriptor(easyBlock(item.block).tree.node->id()).type() == ::profiler::BLOCK_TYPE_EVENT)
                        continue;

                    const auto w = item.width();

                    if (w > m_maxDuration)
                        m_maxDuration = w;

                    if (w < m_minDuration)
                        m_minDuration = w;

                    empty = false;
                }

                if ((m_maxDuration - m_minDuration) < 1e-3)
                {
                    if (m_minDuration > 0.1)
                    {
                        m_minDuration -= 0.1;
                    }
                    else
                    {
                        m_maxDuration = 0.1;
                        m_minDuration = 0;
                    }
                }

                m_topDuration = m_maxDuration;
                m_bottomDuration = m_minDuration;

                if (!empty)
                {
                    m_topDurationStr = ::profiler_gui::timeStringReal(m_timeUnits, m_topDuration, 3);
                    m_bottomDurationStr = ::profiler_gui::timeStringReal(m_timeUnits, m_bottomDuration, 3);
                }
                else
                {
                    m_topDurationStr.clear();
                    m_bottomDurationStr.clear();
                }

                m_bReady.store(true, ::std::memory_order_release);

            }, m_pSource);

            m_timeouts = 3;
            m_timer.start(WORKER_THREAD_CHECK_INTERVAL);
            show();
        }
    }

    if (m_pSource == nullptr)
    {
        m_pProfilerThread = nullptr;
        m_topDurationStr.clear();
        m_bottomDurationStr.clear();
        m_threadName.clear();
        hide();
    }
}

void EasyHistogramItem::setSource(::profiler::thread_id_t _thread_id, ::profiler::block_id_t _block_id)
{
    if (m_regime == Hist_Id && m_threadId == _thread_id && m_blockId == _block_id)
        return;

    m_bPermitImageUpdate = false; // Set to false because m_workerThread have to parse input data first. This will be set to true when m_workerThread finish - see onTimeout()
    m_regime = Hist_Id;

    m_timer.stop();
    m_boundaryTimer.stop();

    m_bReady.store(true, ::std::memory_order_release);
    if (m_workerThread.joinable())
        m_workerThread.join();

    m_pSource = nullptr;
    m_topDurationStr.clear();
    m_bottomDurationStr.clear();
    m_blockName.clear();
    m_blockTotalDuraion = 0;

    delete m_workerImage;
    m_workerImage = nullptr;
    m_imageOriginUpdate = m_imageOrigin = 0;
    m_imageScaleUpdate = m_imageScale = 1;

    m_selectedBlocks.clear();
    { ::profiler::BlocksTree::children_t().swap(m_selectedBlocks); }

    m_threadId = _thread_id;
    m_blockId = _block_id;

    if (m_threadId != 0 && !::profiler_gui::is_max(m_blockId))
    {
        m_blockName = ::profiler_gui::toUnicode(easyDescriptor(m_blockId).name());

        const auto& root = EASY_GLOBALS.profiler_blocks[_thread_id];
        m_threadName = ::profiler_gui::decoratedThreadName(EASY_GLOBALS.use_decorated_thread_name, root, EASY_GLOBALS.hex_thread_id);
        m_pProfilerThread = &root;
        m_timeUnits = EASY_GLOBALS.time_units;

        if (root.children.empty())
        {
            m_threadDuration = 0;
            m_threadProfiledTime = 0;
            m_threadWaitTime = 0;

            m_topDuration = m_maxDuration = 0;
            m_bottomDuration = m_minDuration = 1e30;

            m_bPermitImageUpdate = true;

            m_bReady.store(true, ::std::memory_order_release);
        }
        else
        {
            m_threadDuration = easyBlock(root.children.back()).tree.node->end() - easyBlock(root.children.front()).tree.node->begin();
            m_threadProfiledTime = root.profiled_time;
            m_threadWaitTime = root.wait_time;

            m_bReady.store(false, ::std::memory_order_release);
            m_workerThread = ::std::thread([this](decltype(root) profiler_thread, ::profiler::block_index_t selected_block, bool _showOnlyTopLevelBlocks)
            {
                typedef ::std::vector<::std::pair<::profiler::block_index_t, ::profiler::block_index_t> > Stack;

                m_maxDuration = 0;
                m_minDuration = 1e30;
                //const auto& profiler_thread = EASY_GLOBALS.profiler_blocks[m_threadId];
                Stack stack;
                stack.reserve(profiler_thread.depth);

                const bool has_selected_block = !::profiler_gui::is_max(selected_block);

                for (auto frame : profiler_thread.children)
                {
                    const auto& frame_block = easyBlock(frame).tree;
                    if (frame_block.node->id() == m_blockId || (!has_selected_block && m_blockId == easyDescriptor(frame_block.node->id()).id()))
                    {
                        m_selectedBlocks.push_back(frame);

                        const auto w = frame_block.node->duration();
                        if (w > m_maxDuration)
                            m_maxDuration = w;

                        if (w < m_minDuration)
                            m_minDuration = w;

                        m_blockTotalDuraion += w;
                    }

                    if (_showOnlyTopLevelBlocks)
                        continue;

                    stack.push_back(::std::make_pair(frame, 0U));
                    while (!stack.empty())
                    {
                        if (m_bReady.load(::std::memory_order_acquire))
                            return;

                        auto& top = stack.back();
                        const auto& top_children = easyBlock(top.first).tree.children;
                        const auto stack_size = stack.size();
                        for (auto end = top_children.size(); top.second < end; ++top.second)
                        {
                            if (m_bReady.load(::std::memory_order_acquire))
                                return;

                            const auto child_index = top_children[top.second];
                            const auto& child = easyBlock(child_index).tree;
                            if (child.node->id() == m_blockId || (!has_selected_block && m_blockId == easyDescriptor(child.node->id()).id()))
                            {
                                m_selectedBlocks.push_back(child_index);

                                const auto w = child.node->duration();
                                if (w > m_maxDuration)
                                    m_maxDuration = w;

                                if (w < m_minDuration)
                                    m_minDuration = w;

                                m_blockTotalDuraion += w;
                            }

                            if (!child.children.empty())
                            {
                                ++top.second;
                                stack.push_back(::std::make_pair(child_index, 0U));
                                break;
                            }
                        }

                        if (stack_size == stack.size())
                        {
                            stack.pop_back();
                        }
                    }
                }

                if (m_selectedBlocks.empty())
                {
                    m_topDurationStr.clear();
                    m_bottomDurationStr.clear();
                }
                else
                {
                    if (has_selected_block)
                    {
                        const auto& item = easyBlock(selected_block).tree;
                        if (*item.node->name() != 0)
                            m_blockName = ::profiler_gui::toUnicode(item.node->name());
                    }

                    m_maxDuration *= 1e-3;
                    m_minDuration *= 1e-3;

                    if ((m_maxDuration - m_minDuration) < 1e-3)
                    {
                        if (m_minDuration > 0.1)
                        {
                            m_minDuration -= 0.1;
                        }
                        else
                        {
                            m_maxDuration = 0.1;
                            m_minDuration = 0;
                        }
                    }

                    m_topDurationStr = ::profiler_gui::timeStringReal(m_timeUnits, m_maxDuration, 3);
                    m_bottomDurationStr = ::profiler_gui::timeStringReal(m_timeUnits, m_minDuration, 3);
                }



                m_topDuration = m_maxDuration;
                m_bottomDuration = m_minDuration;

                m_bReady.store(true, ::std::memory_order_release);

            }, std::ref(root), EASY_GLOBALS.selected_block, EASY_GLOBALS.display_only_frames_on_histogram);

            m_timeouts = 3;
            m_timer.start(WORKER_THREAD_CHECK_INTERVAL);
        }

        show();
    }
    else
    {
        m_pProfilerThread = nullptr;
        m_threadName.clear();
        hide();
    }
}

//////////////////////////////////////////////////////////////////////////

void EasyHistogramItem::validateName()
{
    if (m_threadName.isEmpty())
        return;
    m_threadName = ::profiler_gui::decoratedThreadName(EASY_GLOBALS.use_decorated_thread_name, EASY_GLOBALS.profiler_blocks[m_threadId], EASY_GLOBALS.hex_thread_id);
}

//////////////////////////////////////////////////////////////////////////

void EasyHistogramItem::onTimeout()
{
    if (!isVisible())
    {
        m_timer.stop();
        return;
    }

    if (++m_timeouts > 8)
        m_timeouts = 3;

    if (m_bReady.load(::std::memory_order_acquire))
    {
        m_timer.stop();
        if (!m_bPermitImageUpdate)
        {
            // Worker thread have finished parsing input data (when setSource(_block_id) was called)
            m_bPermitImageUpdate = true; // From now we can update an image
            updateImage();
        }
        else
        {
            // Image updated

            if (m_workerThread.joinable())
                m_workerThread.join();

            m_workerImage->swap(m_mainImage);
            delete m_workerImage;
            m_workerImage = nullptr;

            m_imageOriginUpdate = m_imageOrigin = m_workerImageOrigin;
            m_imageScaleUpdate = m_imageScale = m_workerImageScale;

            if (EASY_GLOBALS.auto_adjust_histogram_height && !m_topDurationStr.isEmpty())
            {
                m_topDuration = m_workerTopDuration;
                m_bottomDuration = m_workerBottomDuration;

                m_topDurationStr = ::profiler_gui::timeStringReal(m_timeUnits, m_topDuration, 3);
                m_bottomDurationStr = ::profiler_gui::timeStringReal(m_timeUnits, m_bottomDuration, 3);
            }
        }
    }

    scene()->update();
}

//////////////////////////////////////////////////////////////////////////

void EasyHistogramItem::pickTopBoundary(qreal _y)
{
    if (m_bPermitImageUpdate && m_boundingRect.top() < _y && _y < m_boundingRect.bottom() && !m_topDurationStr.isEmpty())
    {
        m_topDuration = m_bottomDuration + (m_topDuration - m_bottomDuration) * (m_boundingRect.bottom() - _y) / (m_boundingRect.height() - HIST_COLUMN_MIN_HEIGHT);
        m_topDurationStr = ::profiler_gui::timeStringReal(m_timeUnits, m_topDuration, 3);
        m_boundaryTimer.stop();
        updateImage();
        scene()->update(); // to update top-boundary text right now
    }
}

void EasyHistogramItem::increaseTopBoundary()
{
    if (m_bPermitImageUpdate && m_topDuration < m_maxDuration && !m_topDurationStr.isEmpty())
    {
        auto step = 0.05 * (m_maxDuration - m_bottomDuration);
        if (m_topDuration < (m_bottomDuration + 1.25 * step))
            step = 0.1 * (m_topDuration - m_bottomDuration);

        m_topDuration = std::min(m_maxDuration, m_topDuration + step);
        m_topDurationStr = ::profiler_gui::timeStringReal(m_timeUnits, m_topDuration, 3);
        updateImage();
        scene()->update(); // to update top-boundary text right now

        m_boundaryTimer.stop();
        m_boundaryTimer.start(BOUNDARY_TIMER_INTERVAL);
    }
}

void EasyHistogramItem::decreaseTopBoundary()
{
    if (m_bPermitImageUpdate && m_topDuration > m_bottomDuration && !m_topDurationStr.isEmpty())
    {
        auto step = 0.05 * (m_maxDuration - m_bottomDuration);
        if (m_topDuration < (m_bottomDuration + 1.25 * step))
            step = std::max(0.1 * (m_topDuration - m_bottomDuration), 0.3);

        if (m_topDuration > (m_bottomDuration + 1.25 * step))
        {
            m_topDuration = std::max(m_bottomDuration + step, m_topDuration - step);
            m_topDurationStr = ::profiler_gui::timeStringReal(m_timeUnits, m_topDuration, 3);
            scene()->update(); // to update top-boundary text right now

            m_boundaryTimer.stop();
            m_boundaryTimer.start(BOUNDARY_TIMER_INTERVAL);
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void EasyHistogramItem::pickBottomBoundary(qreal _y)
{
    if (m_bPermitImageUpdate && m_boundingRect.top() < _y && _y < m_boundingRect.bottom() && !m_bottomDurationStr.isEmpty())
    {
        m_bottomDuration = m_bottomDuration + (m_topDuration - m_bottomDuration) * (m_boundingRect.bottom() - _y) / (m_boundingRect.height() - HIST_COLUMN_MIN_HEIGHT);
        m_bottomDurationStr = ::profiler_gui::timeStringReal(m_timeUnits, m_bottomDuration, 3);
        m_boundaryTimer.stop();
        updateImage();
        scene()->update(); // to update top-boundary text right now
    }
}

void EasyHistogramItem::increaseBottomBoundary()
{
    if (m_bPermitImageUpdate && m_bottomDuration < m_topDuration && !m_bottomDurationStr.isEmpty())
    {
        auto step = 0.05 * (m_topDuration - m_minDuration);
        if (m_bottomDuration > (m_topDuration - 1.25 * step))
            step = 0.1 * (m_topDuration - m_bottomDuration);

        if (m_bottomDuration < (m_topDuration - 1.25 * step))
        {
            m_bottomDuration = std::min(m_topDuration - step, m_bottomDuration + step);
            m_bottomDurationStr = ::profiler_gui::timeStringReal(m_timeUnits, m_bottomDuration, 3);
            scene()->update(); // to update bottom-boundary text right now

            m_boundaryTimer.stop();
            m_boundaryTimer.start(BOUNDARY_TIMER_INTERVAL);
        }
    }
}

void EasyHistogramItem::decreaseBottomBoundary()
{
    if (m_bPermitImageUpdate && m_bottomDuration > m_minDuration && !m_bottomDurationStr.isEmpty())
    {
        auto step = 0.05 * (m_topDuration - m_minDuration);
        if (m_bottomDuration > (m_topDuration - 1.25 * step))
            step = std::max(0.1 * (m_topDuration - m_bottomDuration), 0.3);

        m_bottomDuration = std::max(m_minDuration, m_bottomDuration - step);
        m_bottomDurationStr = ::profiler_gui::timeStringReal(m_timeUnits, m_bottomDuration, 3);
        scene()->update(); // to update top-boundary text right now

        m_boundaryTimer.stop();
        m_boundaryTimer.start(BOUNDARY_TIMER_INTERVAL);
    }
}

//////////////////////////////////////////////////////////////////////////

void EasyHistogramItem::setMouseY(qreal _mouseY)
{
    m_mouseY = _mouseY;
}

void EasyHistogramItem::pickFrameTime(qreal _y) const
{
    if (m_bPermitImageUpdate && m_boundingRect.top() < _y && _y < m_boundingRect.bottom() && !m_topDurationStr.isEmpty())
    {
        EASY_GLOBALS.frame_time = m_bottomDuration + (m_topDuration - m_bottomDuration) * (m_boundingRect.bottom() - _y) / (m_boundingRect.height() - HIST_COLUMN_MIN_HEIGHT);
        emit EASY_GLOBALS.events.expectedFrameTimeChanged();
    }
}

//////////////////////////////////////////////////////////////////////////

void EasyHistogramItem::onValueChanged()
{
    const auto widget = static_cast<const EasyGraphicsScrollbar*>(scene()->parent());

    if (!widget->bindMode())
        return;

    m_boundaryTimer.stop();

    const auto sliderWidth_inv = 1.0 / widget->sliderWidth();
    const auto k = widget->range() * sliderWidth_inv;

    const auto deltaScale = m_imageScaleUpdate < k ? (k / m_imageScaleUpdate) : (m_imageScaleUpdate / k);
    if (deltaScale > 4) {
        updateImage();
        return;
    }

    const auto deltaOffset = (widget->value() - m_imageOriginUpdate) * sliderWidth_inv;
    if (deltaOffset < 1.5 || deltaOffset > 4.5) {
        updateImage();
        return;
    }

    m_boundaryTimer.start(BOUNDARY_TIMER_INTERVAL);
}

//////////////////////////////////////////////////////////////////////////

void EasyHistogramItem::onModeChanged()
{
    if (!m_bPermitImageUpdate)
        return;

    const auto widget = static_cast<const EasyGraphicsScrollbar*>(scene()->parent());

    if (!widget->bindMode() && EASY_GLOBALS.auto_adjust_histogram_height)
    {
        m_topDuration = m_maxDuration;
        m_bottomDuration = m_minDuration;
    }

    m_boundaryTimer.stop();
    updateImage();
}

//////////////////////////////////////////////////////////////////////////

void EasyHistogramItem::cancelImageUpdate()
{
    if (!m_bPermitImageUpdate)
        return;

    m_bReady.store(true, ::std::memory_order_release);
    if (m_workerThread.joinable())
        m_workerThread.join();
    m_bReady.store(false, ::std::memory_order_release);

    delete m_workerImage;
    m_workerImage = nullptr;

    m_imageOriginUpdate = m_imageOrigin;
    m_imageScaleUpdate = m_imageScale;

    m_timer.stop();
}

void EasyHistogramItem::updateImage()
{
    if (!m_bPermitImageUpdate)
        return;

    const auto widget = static_cast<const EasyGraphicsScrollbar*>(scene()->parent());

    m_bReady.store(true, ::std::memory_order_release);
    if (m_workerThread.joinable())
        m_workerThread.join();
    m_bReady.store(false, ::std::memory_order_release);

    delete m_workerImage;
    m_workerImage = nullptr;

    m_imageScaleUpdate = widget->range() / widget->sliderWidth();
    m_imageOriginUpdate = widget->bindMode() ? (widget->value() - widget->sliderWidth() * 3) : widget->minimum();

    m_workerThread = ::std::thread([this](QRectF _boundingRect, HistRegime _regime, qreal _current_scale,
        qreal _minimum, qreal _maximum, qreal _range, qreal _value, qreal _width, qreal _top_duration, qreal _bottom_duration,
        bool _bindMode, float _frame_time, ::profiler::timestamp_t _begin_time, qreal _origin, bool _autoAdjustHist)
    {
        updateImage(_boundingRect, _regime, _current_scale, _minimum, _maximum, _range, _value, _width, _top_duration, _bottom_duration, _bindMode, _frame_time, _begin_time, _origin, _autoAdjustHist);
        m_bReady.store(true, ::std::memory_order_release);
    }, m_boundingRect, m_regime, widget->getWindowScale(), widget->minimum(), widget->maximum(), widget->range(), widget->value(), widget->sliderWidth(),
        m_topDuration, m_bottomDuration, widget->bindMode(), EASY_GLOBALS.frame_time, EASY_GLOBALS.begin_time, m_imageOriginUpdate, EASY_GLOBALS.auto_adjust_histogram_height);

    m_timeouts = 3;
    m_timer.start(WORKER_THREAD_CHECK_INTERVAL);
}

void EasyHistogramItem::updateImage(QRectF _boundingRect, HistRegime _regime, qreal _current_scale,
                                    qreal _minimum, qreal _maximum, qreal _range,
                                    qreal _value, qreal _width, qreal _top_duration, qreal _bottom_duration,
                                    bool _bindMode, float _frame_time, ::profiler::timestamp_t _begin_time,
                                    qreal _origin, bool _autoAdjustHist)
{
    const auto bottom = _boundingRect.height();//_boundingRect.bottom();
    const auto screenWidth = _boundingRect.width() * _current_scale;
    const auto maxColumnHeight = _boundingRect.height();
    const auto viewScale = _range / _width;

    if (_bindMode)
    {
        m_workerImageScale = viewScale;
        m_workerImageOrigin = _value - _width * 3;
        m_workerImage = new QImage(screenWidth * 7 + 0.5, _boundingRect.height(), QImage::Format_ARGB32);
    }
    else
    {
        m_workerImageScale = 1;
        m_workerImageOrigin = _minimum;
        m_workerImage = new QImage(screenWidth + 0.5, _boundingRect.height(), QImage::Format_ARGB32);
    }

    m_workerImage->fill(0);
    QPainter p(m_workerImage);
    p.setPen(Qt::NoPen);

    QRectF rect;
    QBrush brush(Qt::SolidPattern);
    QRgb previousColor = 0;

    qreal previous_x = -1e30, previous_h = -1e30, offset = 0.;
    auto realScale = _current_scale;

    const bool gotFrame = _frame_time > 1e-6f;
    qreal frameCoeff = 1;
    if (gotFrame)
    {
        if (_frame_time <= _bottom_duration)
            frameCoeff = _boundingRect.height();
        else
            frameCoeff = 0.9 / _frame_time;
    }

    auto const calculate_color = gotFrame ? calculate_color2 : calculate_color1;
    auto const k = gotFrame ? sqr(sqr(frameCoeff)) : 1.0 / _boundingRect.height();

    if (_regime == Hist_Pointer)
    {
        const auto& items = *m_pSource;
        if (items.empty())
            return;

        auto first = items.begin();

        if (_bindMode)
        {
            _minimum = m_workerImageOrigin;
            _maximum = m_workerImageOrigin + _width * 7;
            realScale *= viewScale;
            offset = _minimum * realScale;

            first = ::std::lower_bound(items.begin(), items.end(), _minimum, [](const ::profiler_gui::EasyBlockItem& _item, qreal _value)
            {
                return _item.left() < _value;
            });

            if (first != items.end())
            {
                if (first != items.begin())
                    --first;
            }
            else
            {
                first = items.begin() + items.size() - 1;
            }

            if (_autoAdjustHist)
            {
                const auto maxVal = _value + _width;
                decltype(_top_duration) maxDuration = 0;
                decltype(_bottom_duration) minDuration = 1e30;
                size_t iterations = 0;
                for (auto it = first, end = items.end(); it != end; ++it)
                {
                    // Draw rectangle
                    if (it->left() > maxVal)
                        break;

                    if (it->right() < _value)
                        continue;

                    if (maxDuration < it->width())
                        maxDuration = it->width();

                    if (minDuration > it->width())
                        minDuration = it->width();

                    ++iterations;
                }

                if (iterations)
                {
                    _top_duration = maxDuration;
                    _bottom_duration = minDuration;

                    if ((_top_duration - _bottom_duration) < 1e-3)
                    {
                        if (_bottom_duration > 0.1)
                        {
                            _bottom_duration -= 0.1;
                        }
                        else
                        {
                            _top_duration = 0.1;
                            _bottom_duration = 0;
                        }
                    }
                }
            }
        }

        const auto dtime = _top_duration - _bottom_duration;
        const auto coeff = (_boundingRect.height() - HIST_COLUMN_MIN_HEIGHT) / (dtime > 1e-3 ? dtime : 1.);

        for (auto it = first, end = items.end(); it != end; ++it)
        {
            // Draw rectangle
            if (it->left() > _maximum)
                break;

            if (it->right() < _minimum)
                continue;

            const qreal item_x = it->left() * realScale - offset;
            const qreal item_w = ::std::max(it->width() * realScale, 1.0);
            const qreal item_r = item_x + item_w;
            const qreal h = it->width() <= _bottom_duration ? HIST_COLUMN_MIN_HEIGHT : 
                (it->width() > _top_duration ? maxColumnHeight : (HIST_COLUMN_MIN_HEIGHT + (it->width() - _bottom_duration) * coeff));

            if (h < previous_h && item_r < previous_x)
                continue;

            const auto col = calculate_color(h, it->width(), k);
            const auto color = 0x00ffffff & QColor::fromHsvF((1.0 - col) * 0.375, 0.85, 0.85).rgb();

            if (previousColor != color)
            {
                // Set background color brush for rectangle
                previousColor = color;
                brush.setColor(QColor::fromRgba(0xc0000000 | color));
                p.setBrush(brush);
            }

            rect.setRect(item_x, bottom - h, item_w, h);
            p.drawRect(rect);

            previous_x = item_r;
            previous_h = h;
        }
    }
    else
    {
        auto first = m_selectedBlocks.begin();

        if (_bindMode)
        {
            _minimum = m_workerImageOrigin;
            _maximum = m_workerImageOrigin + _width * 7;
            realScale *= viewScale;
            offset = _minimum * 1e3 * realScale;

            first = ::std::lower_bound(m_selectedBlocks.begin(), m_selectedBlocks.end(), _minimum * 1e3 + _begin_time, [](::profiler::block_index_t _item, qreal _value)
            {
                return easyBlock(_item).tree.node->begin() < _value;
            });

            if (first != m_selectedBlocks.end())
            {
                if (first != m_selectedBlocks.begin())
                    --first;
            }
            else
            {
                first = m_selectedBlocks.begin() + m_selectedBlocks.size() - 1;
            }

            _minimum *= 1e3;
            _maximum *= 1e3;

            if (_autoAdjustHist)
            {
                const auto minVal = _value * 1e3, maxVal = (_value + _width) * 1e3;
                decltype(_top_duration) maxDuration = 0;
                decltype(_bottom_duration) minDuration = 1e30;
                size_t iterations = 0;
                for (auto it = first, end = m_selectedBlocks.end(); it != end; ++it)
                {
                    const auto item = easyBlock(*it).tree.node;

                    const auto beginTime = item->begin() - _begin_time;
                    if (beginTime > maxVal)
                        break;

                    const auto endTime = item->end() - _begin_time;
                    if (endTime < minVal)
                        continue;

                    const qreal duration = item->duration() * 1e-3;

                    if (maxDuration < duration)
                        maxDuration = duration;

                    if (minDuration > duration)
                        minDuration = duration;

                    ++iterations;
                }

                if (iterations)
                {
                    _top_duration = maxDuration;
                    _bottom_duration = minDuration;

                    if ((_top_duration - _bottom_duration) < 1e-3)
                    {
                        if (_bottom_duration > 0.1)
                        {
                            _bottom_duration -= 0.1;
                        }
                        else
                        {
                            _top_duration = 0.1;
                            _bottom_duration = 0;
                        }
                    }
                }
            }
        }
        else
        {
            _minimum *= 1e3;
            _maximum *= 1e3;
        }

        const auto dtime = _top_duration - _bottom_duration;
        const auto coeff = (_boundingRect.height() - HIST_COLUMN_MIN_HEIGHT) / (dtime > 1e-3 ? dtime : 1.);

        for (auto it = first, end = m_selectedBlocks.end(); it != end; ++it)
        {
            // Draw rectangle
            const auto item = easyBlock(*it).tree.node;

            const auto beginTime = item->begin() - _begin_time;
            if (beginTime > _maximum)
                break;

            const auto endTime = item->end() - _begin_time;
            if (endTime < _minimum)
                continue;

            const qreal duration = item->duration() * 1e-3;
            const qreal item_x = (beginTime * realScale - offset) * 1e-3;
            const qreal item_w = ::std::max(duration * realScale, 1.0);
            const qreal item_r = item_x + item_w;
            const auto h = duration <= _bottom_duration ? HIST_COLUMN_MIN_HEIGHT :
                (duration > _top_duration ? maxColumnHeight : (HIST_COLUMN_MIN_HEIGHT + (duration - _bottom_duration) * coeff));

            if (h < previous_h && item_r < previous_x)
                continue;

            const auto col = calculate_color(h, duration, k);
            const auto color = 0x00ffffff & QColor::fromHsvF((1.0 - col) * 0.375, 0.85, 0.85).rgb();

            if (previousColor != color)
            {
                // Set background color brush for rectangle
                previousColor = color;
                brush.setColor(QColor::fromRgba(0xc0000000 | color));
                p.setBrush(brush);
            }

            rect.setRect(item_x, bottom - h, item_w, h);
            p.drawRect(rect);

            previous_x = item_r;
            previous_h = h;
        }
    }

    m_workerTopDuration = _top_duration;
    m_workerBottomDuration = _bottom_duration;
}

//////////////////////////////////////////////////////////////////////////

EasyGraphicsScrollbar::EasyGraphicsScrollbar(QWidget* _parent)
    : Parent(_parent)
    , m_minimumValue(0)
    , m_maximumValue(500)
    , m_value(10)
    , m_windowScale(1)
    , m_mouseButtons(Qt::NoButton)
    , m_slider(nullptr)
    , m_chronometerIndicator(nullptr)
    , m_histogramItem(nullptr)
    , m_defaultFontHeight(0)
    , m_bScrolling(false)
    , m_bBindMode(false)
    , m_bLocked(false)
{
    setCacheMode(QGraphicsView::CacheNone);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    //setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setOptimizationFlag(QGraphicsView::DontSavePainterState, true);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setContentsMargins(0, 0, 0, 0);

    auto selfScene = new QGraphicsScene(this);
    m_defaultFontHeight = QFontMetrics(selfScene->font()).height();
    selfScene->setSceneRect(0, DEFAULT_TOP, 500, DEFAULT_HEIGHT + m_defaultFontHeight + 2);
    setFixedHeight(DEFAULT_HEIGHT + m_defaultFontHeight + 2);

    setScene(selfScene);

    m_histogramItem = new EasyHistogramItem();
    m_histogramItem->setPos(0, 0);
    m_histogramItem->setBoundingRect(0, DEFAULT_TOP + INDICATOR_SIZE, scene()->width(), DEFAULT_HEIGHT - INDICATOR_SIZE_x2);
    selfScene->addItem(m_histogramItem);
    m_histogramItem->hide();

    m_chronometerIndicator = new EasyGraphicsSliderItem(false);
    m_chronometerIndicator->setPos(0, 0);
    m_chronometerIndicator->setColor(0x40000000 | ::profiler_gui::CHRONOMETER_COLOR.rgba());
    selfScene->addItem(m_chronometerIndicator);
    m_chronometerIndicator->hide();

    m_slider = new EasyGraphicsSliderItem(true);
    m_slider->setPos(0, 0);
    m_slider->setColor(0x40c0c0c0);
    selfScene->addItem(m_slider);
    m_slider->hide();

    connect(&EASY_GLOBALS.events, &::profiler_gui::EasyGlobalSignals::expectedFrameTimeChanged, [this]()
    {
        if (m_histogramItem->isVisible())
        {
            m_histogramItem->updateImage();
            scene()->update();
        }
    });

    connect(&EASY_GLOBALS.events, &::profiler_gui::EasyGlobalSignals::autoAdjustHistogramChanged, [this]()
    {
        if (m_histogramItem->isVisible())
            m_histogramItem->onModeChanged();
    });

    connect(&EASY_GLOBALS.events, &::profiler_gui::EasyGlobalSignals::displayOnlyFramesOnHistogramChanged, [this]()
    {
        if (m_histogramItem->isVisible())
            m_histogramItem->rebuildSource(EasyHistogramItem::Hist_Id);
    });

    connect(&EASY_GLOBALS.events, &::profiler_gui::EasyGlobalSignals::threadNameDecorationChanged, this, &This::onThreadViewChanged);
    connect(&EASY_GLOBALS.events, &::profiler_gui::EasyGlobalSignals::hexThreadIdChanged, this, &This::onThreadViewChanged);

    centerOn(0, 0);
}

EasyGraphicsScrollbar::~EasyGraphicsScrollbar()
{

}

//////////////////////////////////////////////////////////////////////////

void EasyGraphicsScrollbar::onThreadViewChanged()
{
    if (m_histogramItem->isVisible())
    {
        m_histogramItem->validateName();
        scene()->update();
    }
}

//////////////////////////////////////////////////////////////////////////

void EasyGraphicsScrollbar::clear()
{
    setHistogramSource(0, nullptr);
    hideChrono();
    setRange(0, 100);
    setSliderWidth(2);
    setValue(0);
}

//////////////////////////////////////////////////////////////////////////

bool EasyGraphicsScrollbar::bindMode() const
{
    return m_bBindMode;
}

qreal EasyGraphicsScrollbar::getWindowScale() const
{
    return m_windowScale;
}

::profiler::thread_id_t EasyGraphicsScrollbar::hystThread() const
{
    return m_histogramItem->threadId();
}

qreal EasyGraphicsScrollbar::minimum() const
{
    return m_minimumValue;
}

qreal EasyGraphicsScrollbar::maximum() const
{
    return m_maximumValue;
}

qreal EasyGraphicsScrollbar::range() const
{
    return m_maximumValue - m_minimumValue;
}

qreal EasyGraphicsScrollbar::value() const
{
    return m_value;
}

qreal EasyGraphicsScrollbar::sliderWidth() const
{
    return m_slider->width();
}

qreal EasyGraphicsScrollbar::sliderHalfWidth() const
{
    return m_slider->halfwidth();
}

int EasyGraphicsScrollbar::defaultFontHeight() const
{
    return m_defaultFontHeight;
}

//////////////////////////////////////////////////////////////////////////

void EasyGraphicsScrollbar::setValue(qreal _value)
{
    m_value = clamp(m_minimumValue, _value, ::std::max(m_minimumValue, m_maximumValue - m_slider->width()));
    m_slider->setX(m_value + m_slider->halfwidth());
    emit valueChanged(m_value);

    if (m_histogramItem->isVisible())
        m_histogramItem->onValueChanged();
}

void EasyGraphicsScrollbar::setRange(qreal _minValue, qreal _maxValue)
{
    const auto oldRange = range();
    const auto oldValue = oldRange < 1e-3 ? 0.0 : m_value / oldRange;

    m_minimumValue = _minValue;
    m_maximumValue = _maxValue;
    scene()->setSceneRect(_minValue, DEFAULT_TOP, _maxValue - _minValue, DEFAULT_HEIGHT + m_defaultFontHeight + 4);

    m_histogramItem->cancelImageUpdate();
    m_histogramItem->setBoundingRect(_minValue, DEFAULT_TOP + INDICATOR_SIZE, _maxValue, DEFAULT_HEIGHT - INDICATOR_SIZE_x2);

    emit rangeChanged();

    setValue(_minValue + oldValue * range());

    onWindowWidthChange(width());

    if (m_histogramItem->isVisible())
        m_histogramItem->updateImage();
}

void EasyGraphicsScrollbar::setSliderWidth(qreal _width)
{
    m_slider->setWidth(_width);
    setValue(m_value);
}

//////////////////////////////////////////////////////////////////////////

void EasyGraphicsScrollbar::setChronoPos(qreal _left, qreal _right)
{
    m_chronometerIndicator->setWidth(_right - _left);
    m_chronometerIndicator->setX(_left + m_chronometerIndicator->halfwidth());
}

void EasyGraphicsScrollbar::showChrono()
{
    m_chronometerIndicator->show();
}

void EasyGraphicsScrollbar::hideChrono()
{
    m_chronometerIndicator->hide();
}

//////////////////////////////////////////////////////////////////////////

void EasyGraphicsScrollbar::setHistogramSource(::profiler::thread_id_t _thread_id, const ::profiler_gui::EasyItems* _items)
{
    if (m_bLocked)
        return;

    m_histogramItem->setSource(_thread_id, _items);
    m_slider->setVisible(m_histogramItem->isVisible());
    scene()->update();
}

void EasyGraphicsScrollbar::setHistogramSource(::profiler::thread_id_t _thread_id, ::profiler::block_id_t _block_id)
{
    if (m_bLocked)
        return;

    m_histogramItem->setSource(_thread_id, _block_id);
    m_slider->setVisible(m_histogramItem->isVisible());
    scene()->update();
}

//////////////////////////////////////////////////////////////////////////

void EasyGraphicsScrollbar::mousePressEvent(QMouseEvent* _event)
{
    _event->accept();

    m_mouseButtons = _event->buttons();

    if (m_mouseButtons & Qt::LeftButton)
    {
        if (_event->modifiers() & Qt::ControlModifier)
        {
            m_histogramItem->pickBottomBoundary(mapToScene(_event->pos()).y());
        }
        else if (_event->modifiers() & Qt::ShiftModifier)
        {
            m_histogramItem->pickTopBoundary(mapToScene(_event->pos()).y());
        }
        else
        {
            m_bScrolling = true;
            m_mousePressPos = _event->pos();
            if (!m_bBindMode)
                setValue(mapToScene(m_mousePressPos).x() - m_minimumValue - m_slider->halfwidth());
        }
    }

    if (m_mouseButtons & Qt::RightButton)
    {
        if (_event->modifiers())
        {
            m_histogramItem->pickFrameTime(mapToScene(_event->pos()).y());
        }
        else
        {
            m_bBindMode = !m_bBindMode;
            if (m_histogramItem->isVisible())
                m_histogramItem->onModeChanged();
        }
    }

    //QGraphicsView::mousePressEvent(_event);
}

void EasyGraphicsScrollbar::mouseReleaseEvent(QMouseEvent* _event)
{
    m_mouseButtons = _event->buttons();
    m_bScrolling = false;
    _event->accept();
    //QGraphicsView::mouseReleaseEvent(_event);
}

void EasyGraphicsScrollbar::mouseMoveEvent(QMouseEvent* _event)
{
    const auto pos = _event->pos();

    if (m_mouseButtons & Qt::LeftButton)
    {
        const auto delta = pos - m_mousePressPos;
        m_mousePressPos = pos;

        if (m_bScrolling)
        {
            auto realScale = m_windowScale;
            if (m_bBindMode)
                realScale *= -range() / sliderWidth();
            setValue(m_value + delta.x() / realScale);
        }
    }

    if (m_histogramItem->isVisible())
    {
        m_histogramItem->setMouseY(mapToScene(pos).y());
        scene()->update();
    }
}

void EasyGraphicsScrollbar::wheelEvent(QWheelEvent* _event)
{
    _event->accept();

    if (_event->modifiers() & Qt::ShiftModifier)
    {
        // Shift + mouse wheel will change histogram top boundary

        if (m_histogramItem->isVisible())
        {
            if (_event->delta() > 0)
                m_histogramItem->increaseTopBoundary();
            else
                m_histogramItem->decreaseTopBoundary();
        }

        return;
    }

    if (_event->modifiers() & Qt::ControlModifier)
    {
        // Ctrl + mouse wheel will change histogram bottom boundary

        if (m_histogramItem->isVisible())
        {
            if (_event->delta() > 0)
                m_histogramItem->increaseBottomBoundary();
            else
                m_histogramItem->decreaseBottomBoundary();
        }

        return;
    }

    if (!m_bBindMode)
    {
        const auto w = m_slider->halfwidth() * (_event->delta() < 0 ? ::profiler_gui::SCALING_COEFFICIENT : ::profiler_gui::SCALING_COEFFICIENT_INV);
        setValue(mapToScene(_event->pos()).x() - m_minimumValue - w);
        emit wheeled(w * m_windowScale, _event->delta());
    }
    else
    {
        const auto x = (mapToScene(_event->pos()).x() - m_minimumValue) * m_windowScale;
        emit wheeled(x, _event->delta());
    }
}

void EasyGraphicsScrollbar::resizeEvent(QResizeEvent* _event)
{
    onWindowWidthChange(_event->size().width());
    if (m_histogramItem->isVisible())
        m_histogramItem->updateImage();
}

//////////////////////////////////////////////////////////////////////////

void EasyGraphicsScrollbar::onWindowWidthChange(qreal _width)
{
    const auto oldScale = m_windowScale;
    const auto scrollingRange = range();

    if (scrollingRange < 1e-3)
    {
        m_windowScale = 1;
    }
    else
    {
        m_windowScale = _width / scrollingRange;
    }

    scale(m_windowScale / oldScale, 1);
}

//////////////////////////////////////////////////////////////////////////

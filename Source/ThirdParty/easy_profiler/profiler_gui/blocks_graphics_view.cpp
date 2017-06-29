/************************************************************************
* file name         : blocks_graphics_view.cpp
* ----------------- :
* creation time     : 2016/06/26
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains implementation of GraphicsScene and GraphicsView and
*                   : it's auxiliary classes for displyaing easy_profiler blocks tree.
* ----------------- :
* change log        : * 2016/06/26 Victor Zarubkin: Moved sources from graphics_view.h
*                   :       and renamed classes from My* to Prof*.
*                   :
*                   : * 2016/06/27 Victor Zarubkin: Added text shifting relatively to it's parent item.
*                   :       Disabled border lines painting because of vertical lines painting bug.
*                   :       Changed height of blocks. Variable thread-block height.
*                   :
*                   : * 2016/06/29 Victor Zarubkin: Highly optimized painting performance and memory consumption.
*                   :
*                   : * 2016/06/30 Victor Zarubkin: Replaced doubles with floats (in ProfBlockItem) for less memory consumption.
*                   :
*                   : * 2016/09/15 Victor Zarubkin: Moved sources of EasyGraphicsItem and EasyChronometerItem to separate files.
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

#include <math.h>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QScrollBar>
#include <QGridLayout>
#include <QDebug>
#include <QSignalBlocker>
#include <QGraphicsDropShadowEffect>
#include "blocks_graphics_view.h"
#include "easy_graphics_item.h"
#include "easy_chronometer_item.h"
#include "easy_graphics_scrollbar.h"
#include "globals.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

const qreal MIN_SCALE = pow(::profiler_gui::SCALING_COEFFICIENT_INV, 70); // Up to 1000 sec scale
const qreal MAX_SCALE = pow(::profiler_gui::SCALING_COEFFICIENT, 45); // ~23000 --- Up to 10 ns scale
const qreal BASE_SCALE = pow(::profiler_gui::SCALING_COEFFICIENT_INV, 25); // ~0.003

const uint16_t TIMELINE_ROW_SIZE = 20;

const QRgb BACKGROUND_1 = ::profiler::colors::Grey300;
const QRgb BACKGROUND_2 = ::profiler::colors::White;
const QRgb TIMELINE_BACKGROUND = 0x20000000 | (::profiler::colors::Grey800 & 0x00ffffff);// 0x20303030;

const int IDLE_TIMER_INTERVAL = 200; // 5Hz
const uint64_t IDLE_TIME = 400;

const int FLICKER_INTERVAL = 10; // 100Hz
const qreal FLICKER_FACTOR = 16.0 / FLICKER_INTERVAL;

const auto BG_FONT = ::profiler_gui::EFont("Helvetica", 10, QFont::Bold);
const auto CHRONOMETER_FONT = ::profiler_gui::EFont("Helvetica", 16, QFont::Bold);

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

//////////////////////////////////////////////////////////////////////////

inline int sign(int _value) { return _value < 0 ? -1 : 1; }
inline int absmin(int _a, int _b) { return abs(_a) < abs(_b) ? _a : _b; }
inline qreal clamp(qreal _minValue, qreal _value, qreal _maxValue) { return _value < _minValue ? _minValue : (_value > _maxValue ? _maxValue : _value); }

//////////////////////////////////////////////////////////////////////////

template <int N, class T>
inline T logn(T _value)
{
    static const double div = 1.0 / log2((double)N);
    return log2(_value) * div;
}

//////////////////////////////////////////////////////////////////////////

EasyBoldLabel::EasyBoldLabel(const QString& _text, QWidget* _parent) : QLabel(_text, _parent)
{
    auto f = font();
    f.setBold(true);
    setFont(f);
}

EasyBoldLabel::~EasyBoldLabel()
{

}

//////////////////////////////////////////////////////////////////////////

void EasyBackgroundItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    auto const sceneView = static_cast<EasyGraphicsView*>(scene()->parent());
    const auto visibleSceneRect = sceneView->visibleSceneRect();
    const auto currentScale = sceneView->scale();
    const auto offset = sceneView->offset();
    const auto left = offset * currentScale;
    const auto h = visibleSceneRect.height();
    const auto visibleBottom = h - 1;

    QRectF rect;

    _painter->save();
    _painter->setTransform(QTransform::fromTranslate(-x(), -y()));

    const auto& items = sceneView->getItems();
    if (!items.empty())
    {
        static const uint16_t OVERLAP = ::profiler_gui::THREADS_ROW_SPACING >> 1;
        static const QBrush brushes[2] = {QColor::fromRgb(BACKGROUND_1), QColor::fromRgb(BACKGROUND_2)};
        int i = -1;

        // Draw background
        _painter->setPen(Qt::NoPen);
        for (auto item : items)
        {
            ++i;

            auto br = item->boundingRect();
            auto top = item->y() + br.top() - visibleSceneRect.top();
            auto bottom = top + br.height();

            if (top > h || bottom < 0)
                continue;

            if (item->threadId() == EASY_GLOBALS.selected_thread)
                _painter->setBrush(QBrush(QColor::fromRgb(::profiler_gui::SELECTED_THREAD_BACKGROUND)));
            else
                _painter->setBrush(brushes[i & 1]);

            rect.setRect(0, top - OVERLAP, visibleSceneRect.width(), br.height() + ::profiler_gui::THREADS_ROW_SPACING);
            const auto dh = rect.bottom() - visibleBottom;
            if (dh > 0)
                rect.setHeight(rect.height() - dh);

            if (rect.top() < 0)
                rect.setTop(0);

            _painter->drawRect(rect);
        }
    }

    // Draw timeline scale marks ----------------
    _painter->setBrush(QColor::fromRgba(TIMELINE_BACKGROUND));

    const auto sceneStep = sceneView->timelineStep();
    const auto factor = ::profiler_gui::timeFactor(sceneStep);
    const auto step = sceneStep * currentScale;
    auto first = static_cast<quint64>(offset / sceneStep);
    const int odd = first & 1;
    const auto nsteps = (1 + odd) * 2 + static_cast<int>(visibleSceneRect.width() / step);
    first -= odd;

    QPen pen(Qt::darkGray);
    pen.setWidth(2);
    _painter->setPen(pen);
    _painter->drawLine(QPointF(0, h), QPointF(visibleSceneRect.width(), h));
    _painter->setPen(Qt::darkGray);

    QLineF marks[20];
    qreal first_x = first * sceneStep;
    const auto textWidth = QFontMetricsF(_painter->font(), sceneView).width(QString::number(static_cast<quint64>(0.5 + first_x * factor))) * ::profiler_gui::FONT_METRICS_FACTOR + 10;
    const int n = 1 + static_cast<int>(textWidth / step);
    int next = first % n;
    if (next)
        next = n - next;

    first_x *= currentScale;
    for (int i = 0; i < nsteps; ++i, --next)
    {
        auto current = first_x - left + step * i;

        if ((i & 1) == 0)
        {
            rect.setRect(current, 0, step, h);
            _painter->drawRect(rect);

            for (int j = 0; j < 20; ++j)
            {
                auto xmark = current + j * step * 0.1;
                marks[j].setLine(xmark, h, xmark, h + ((j % 5) ? 4 : 8));
            }

            _painter->drawLines(marks, 20);
        }

        if (next <= 0)
        {
            next = n;
            _painter->setPen(Qt::black);
            _painter->drawText(QPointF(current + 1, h + 17), QString::number(static_cast<quint64>(0.5 + (current + left) * factor / currentScale)));
            _painter->setPen(Qt::darkGray);
        }

        // TEST
        // this is for testing (order of lines will be painted):
        //_painter->setPen(Qt::black);
        //_painter->drawText(QPointF(current + step * 0.4, h - 20), QString::number(i));
        //_painter->setPen(Qt::gray);
        // TEST
    }
    // END Draw timeline scale marks ~~~~~~~~~~~~

    _painter->restore();
}

//////////////////////////////////////////////////////////////////////////

void EasyTimelineIndicatorItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    const auto sceneView = static_cast<const EasyGraphicsView*>(scene()->parent());
    const auto visibleSceneRect = sceneView->visibleSceneRect();
    const auto step = sceneView->timelineStep() * sceneView->scale();
    const QString text = ::profiler_gui::autoTimeStringInt(units2microseconds(sceneView->timelineStep())); // Displayed text

    // Draw scale indicator
    _painter->save();
    _painter->setTransform(QTransform::fromTranslate(-x(), -y()));
    //_painter->setCompositionMode(QPainter::CompositionMode_Difference);
    _painter->setBrush(Qt::NoBrush);

    QPen pen(Qt::black);
    pen.setWidth(3);
    _painter->setPen(pen);

    _painter->drawLine(QLineF(visibleSceneRect.width() - 9 - step, visibleSceneRect.height() - 10, visibleSceneRect.width() - 11, visibleSceneRect.height() - 10));

    _painter->setPen(Qt::black);
    _painter->drawLine(QLineF(visibleSceneRect.width() - 10 - step, visibleSceneRect.height() - 6, visibleSceneRect.width() - 10 - step, visibleSceneRect.height() - 14));
    _painter->drawLine(QLineF(visibleSceneRect.width() - 10, visibleSceneRect.height() - 6, visibleSceneRect.width() - 10, visibleSceneRect.height() - 14));

    _painter->setPen(Qt::black);
    _painter->setFont(BG_FONT);
    _painter->drawText(QRectF(visibleSceneRect.width() - 10 - step, visibleSceneRect.height() - 63, step, 50), Qt::AlignRight | Qt::AlignBottom | Qt::TextDontClip, text);

    _painter->restore();
}

//////////////////////////////////////////////////////////////////////////

EasyGraphicsView::EasyGraphicsView(QWidget* _parent)
    : Parent(_parent)
    , m_beginTime(::std::numeric_limits<decltype(m_beginTime)>::max())
    , m_sceneWidth(0)
    , m_scale(1)
    , m_offset(0)
    , m_timelineStep(0)
    , m_idleTime(0)
    , m_mouseButtons(Qt::NoButton)
    , m_pScrollbar(nullptr)
    , m_chronometerItem(nullptr)
    , m_chronometerItemAux(nullptr)
    , m_popupWidget(nullptr)
    , m_flickerSpeedX(0)
    , m_flickerSpeedY(0)
    , m_flickerCounterX(0)
    , m_flickerCounterY(0)
    , m_bDoubleClick(false)
    , m_bUpdatingRect(false)
    , m_bEmpty(true)
{
    initMode();
    setScene(new QGraphicsScene(this));
    updateVisibleSceneRect();
}

EasyGraphicsView::~EasyGraphicsView()
{
}

//////////////////////////////////////////////////////////////////////////

void EasyGraphicsView::removePopup(bool _removeFromScene)
{
    if (m_popupWidget != nullptr)
    {
        auto widget = m_popupWidget->widget();
        widget->setParent(nullptr);
        m_popupWidget->setWidget(nullptr);
        delete widget;

        if (_removeFromScene)
            scene()->removeItem(m_popupWidget);

        m_popupWidget = nullptr;
    }
}

//////////////////////////////////////////////////////////////////////////

qreal EasyGraphicsView::sceneWidth() const
{
    return m_sceneWidth;
}

qreal EasyGraphicsView::chronoTime() const
{
    return m_chronometerItem->width();
}

qreal EasyGraphicsView::chronoTimeAux() const
{
    return m_chronometerItemAux->width();
}

//////////////////////////////////////////////////////////////////////////

EasyChronometerItem* EasyGraphicsView::createChronometer(bool _main)
{
    auto chronoItem = new EasyChronometerItem(_main);
    chronoItem->setColor(_main ? ::profiler_gui::CHRONOMETER_COLOR : ::profiler_gui::CHRONOMETER_COLOR2);
    chronoItem->setBoundingRect(sceneRect());
    chronoItem->hide();
    scene()->addItem(chronoItem);

    return chronoItem;
}

//////////////////////////////////////////////////////////////////////////

void EasyGraphicsView::clear()
{
    const QSignalBlocker blocker(this), sceneBlocker(scene()); // block all scene signals (otherwise clear() would be extremely slow!)

    // Stop flicking
    m_flickerTimer.stop();
    m_flickerSpeedX = 0;
    m_flickerSpeedY = 0;
    m_flickerCounterX = 0;
    m_flickerCounterY = 0;

    // Clear all items
    removePopup();
    scene()->clear();
    m_items.clear();
    m_selectedBlocks.clear();

    m_beginTime = ::std::numeric_limits<decltype(m_beginTime)>::max(); // reset begin time
    m_scale = 1; // scale back to initial 100% scale
    m_timelineStep = 1;
    m_offset = 0; // scroll back to the beginning of the scene

    m_idleTimer.stop();
    m_idleTime = 0;

    // Reset necessary flags
    m_bEmpty = true;

    m_sceneWidth = 10;
    setSceneRect(0, 0, 10, 10);

    // notify ProfTreeWidget that selection was reset
    emit intervalChanged(m_selectedBlocks, m_beginTime, 0, 0, false);
}

void EasyGraphicsView::setTree(const ::profiler::thread_blocks_tree_t& _blocksTree)
{
    // clear scene
    clear();

    if (_blocksTree.empty())
    {
        return;
    }

    auto bgItem = new EasyBackgroundItem();
    scene()->addItem(bgItem);

    // set new blocks tree
    // calculate scene size and fill it with items

    // Calculating start and end time
    ::profiler::timestamp_t finish = 0, busyTime = 0;
    ::profiler::thread_id_t longestTree = 0, mainTree = 0;
    for (const auto& threadTree : _blocksTree)
    {
        const auto& t = threadTree.second;

        auto timestart = m_beginTime;
        auto timefinish = finish;
        
        if (!t.children.empty())
            timestart = blocksTree(t.children.front()).node->begin();
        if (!t.sync.empty())
            timestart = ::std::min(timestart, blocksTree(t.sync.front()).node->begin());

        if (!t.children.empty())
            timefinish = blocksTree(t.children.back()).node->end();
        if (!t.sync.empty())
            timefinish = ::std::max(timefinish, blocksTree(t.sync.back()).node->end());

        if (m_beginTime > timestart)
            m_beginTime = timestart;

        if (finish < timefinish)
            finish = timefinish;

        if (t.profiled_time > busyTime) {
            busyTime = t.profiled_time;
            longestTree = threadTree.first;
        }

        if (mainTree == 0 && !strcmp(t.name(), "Main"))
            mainTree = threadTree.first;
    }

    const decltype(m_beginTime) additional_offset = (finish - m_beginTime) / 20; // Additional 5% before first block and after last block
    finish += additional_offset;
    m_beginTime -= ::std::min(m_beginTime, additional_offset);
    EASY_GLOBALS.begin_time = m_beginTime;

    // Sort threads by name
    ::std::vector<::std::reference_wrapper<const ::profiler::BlocksTreeRoot> > sorted_roots;
    sorted_roots.reserve(_blocksTree.size());
    for (const auto& threadTree : _blocksTree)
        sorted_roots.push_back(threadTree.second);
    ::std::sort(sorted_roots.begin(), sorted_roots.end(), [](const ::profiler::BlocksTreeRoot& _a, const ::profiler::BlocksTreeRoot& _b) {
        return _a.thread_name < _b.thread_name;
    });

    // Filling scene with items
    m_items.reserve(_blocksTree.size());
    qreal y = TIMELINE_ROW_SIZE;
    const EasyGraphicsItem *longestItem = nullptr, *mainThreadItem = nullptr;
    for (const ::profiler::BlocksTreeRoot& t : sorted_roots)
    {
        if (m_items.size() == 0xff)
        {
            qWarning() << "Warning: Maximum threads number (255 threads) exceeded! See EasyGraphicsView::setTree() : " << __LINE__ << " in file " << __FILE__;
            break;
        }

        // fill scene with new items
        qreal h = 0, x = 0;
        
        if (!t.children.empty())
            x = time2position(blocksTree(t.children.front()).node->begin());
        else if (!t.sync.empty())
            x = time2position(blocksTree(t.sync.front()).node->begin());

        auto item = new EasyGraphicsItem(static_cast<uint8_t>(m_items.size()), t);
        if (t.depth)
            item->setLevels(t.depth);
        item->setPos(0, y);

        qreal children_duration = 0;

        if (!t.children.empty())
        {
            uint32_t dummy = 0;
            children_duration = setTree(item, t.children, h, dummy, y, 0);
        }
        else
        {
            if (!t.sync.empty())
                children_duration = time2position(blocksTree(t.sync.back()).node->end()) - x;
            h = ::profiler_gui::GRAPHICS_ROW_SIZE;
        }

        item->setBoundingRect(0, 0, children_duration + x, h);
        m_items.push_back(item);
        scene()->addItem(item);

        y += h + ::profiler_gui::THREADS_ROW_SPACING;

        if (longestTree == t.thread_id)
            longestItem = item;

        if (mainTree == t.thread_id)
            mainThreadItem = item;
    }

    // Calculating scene rect
    m_sceneWidth = time2position(finish);
    setSceneRect(0, 0, m_sceneWidth, y + TIMELINE_ROW_SIZE);

    // Center view on the beginning of the scene
    updateVisibleSceneRect();
    setScrollbar(m_pScrollbar);

    // Create new chronometer item (previous item was destroyed by scene on scene()->clear()).
    // It will be shown on mouse right button click.
    m_chronometerItemAux = createChronometer(false);
    m_chronometerItem = createChronometer(true);

    bgItem->setBoundingRect(0, 0, m_sceneWidth, y);
    auto indicator = new EasyTimelineIndicatorItem();
    indicator->setBoundingRect(0, 0, m_sceneWidth, y);
    scene()->addItem(indicator);

    // Setting flags
    m_bEmpty = false;

    scaleTo(BASE_SCALE);


    emit treeChanged();

    if (mainThreadItem != nullptr)
    {
        longestItem = mainThreadItem;
    }

    if (longestItem != nullptr)
    {
        EASY_GLOBALS.selected_thread = longestItem->threadId();
        emit EASY_GLOBALS.events.selectedThreadChanged(longestItem->threadId());

        scrollTo(longestItem);
        m_pScrollbar->setHistogramSource(longestItem->threadId(), longestItem->items(0));
        if (!longestItem->items(0).empty())
            m_pScrollbar->setValue(longestItem->items(0).front().left() - m_pScrollbar->sliderWidth() * 0.25);
    }

    m_idleTimer.start(IDLE_TIMER_INTERVAL);
}

const EasyGraphicsView::Items &EasyGraphicsView::getItems() const
{
    return m_items;
}

qreal EasyGraphicsView::setTree(EasyGraphicsItem* _item, const ::profiler::BlocksTree::children_t& _children, qreal& _height, uint32_t& _maxDepthChild, qreal _y, short _level)
{
    if (_children.empty())
    {
        return 0;
    }

    const auto level = static_cast<uint8_t>(_level);
    const auto n = static_cast<unsigned int>(_children.size());
    _item->reserve(level, n);

    _maxDepthChild = 0;
    uint8_t maxDepth = 0;
    const short next_level = _level + 1;
    bool warned = false;
    qreal total_duration = 0, prev_end = 0, maxh = 0;
    qreal start_time = -1;
    uint32_t j = 0;
    for (auto child_index : _children)
    {
        auto& gui_block = easyBlock(child_index);
        const auto& child = gui_block.tree;
        if (child.depth > maxDepth)
        {
            maxDepth = child.depth;
            _maxDepthChild = j;
        }

        auto xbegin = time2position(child.node->begin());
        if (start_time < 0)
        {
            start_time = xbegin;
        }

        auto duration = time2position(child.node->end()) - xbegin;

        //const auto dt = xbegin - prev_end;
        //if (dt < 0)
        //{
        //    duration += dt;
        //    xbegin -= dt;
        //}

        //static const qreal MIN_DURATION = 0.25;
        //if (duration < MIN_DURATION)
        //    duration = MIN_DURATION;

        const auto i = _item->addItem(level);
        auto& b = _item->getItem(level, i);

        gui_block.graphics_item = _item->index();
        gui_block.graphics_item_level = level;
        gui_block.graphics_item_index = i;

        if (next_level < 256 && next_level < _item->levels() && !child.children.empty())
        {
            b.children_begin = static_cast<unsigned int>(_item->items(static_cast<uint8_t>(next_level)).size());
        }
        else
        {
            ::profiler_gui::set_max(b.children_begin);
        }

        qreal h = 0;
        qreal children_duration = 0;
        uint32_t maxDepthChild = 0;

        if (next_level < 256)
        {
            children_duration = setTree(_item, child.children, h, maxDepthChild, _y + ::profiler_gui::GRAPHICS_ROW_SIZE_FULL, next_level);
        }
        else if (!child.children.empty() && !warned)
        {
            warned = true;
            qWarning() << "Warning: Maximum blocks depth (255) exceeded! See EasyGraphicsView::setTree() : " << __LINE__ << " in file " << __FILE__;
        }

        if (duration < children_duration)
        {
            duration = children_duration;
        }

        if (h > maxh)
        {
            maxh = h;
        }

        b.block = child_index;// &child;

#ifndef EASY_GRAPHICS_ITEM_RECURSIVE_PAINT
        b.neighbours = n;
        b.state = j > 0 || level == 0 ? 0 : -1;
#else
        b.max_depth_child = maxDepthChild;
#endif

        b.setPos(xbegin, duration);
        //b.totalHeight = ::profiler_gui::GRAPHICS_ROW_SIZE + h;

        prev_end = xbegin + duration;
        total_duration = prev_end - start_time;

        ++j;
    }

    _height += ::profiler_gui::GRAPHICS_ROW_SIZE_FULL + maxh;

    return total_duration;
}

//////////////////////////////////////////////////////////////////////////

void EasyGraphicsView::setScrollbar(EasyGraphicsScrollbar* _scrollbar)
{
    auto const prevScrollbar = m_pScrollbar;
    const bool makeConnect = prevScrollbar == nullptr || prevScrollbar != _scrollbar;

    if (prevScrollbar != nullptr && prevScrollbar != _scrollbar)
    {
        disconnect(prevScrollbar, &EasyGraphicsScrollbar::valueChanged, this, &This::onGraphicsScrollbarValueChange);
        disconnect(prevScrollbar, &EasyGraphicsScrollbar::wheeled, this, &This::onGraphicsScrollbarWheel);
    }

    m_pScrollbar = _scrollbar;
    m_pScrollbar->clear();
    m_pScrollbar->setRange(0, m_sceneWidth);

    auto vbar = verticalScrollBar();
    const int vbar_width = (vbar != nullptr && vbar->isVisible() ? vbar->width() + 2 : 0);
    m_pScrollbar->setSliderWidth(m_visibleSceneRect.width() + vbar_width);

    if (makeConnect)
    {
        connect(m_pScrollbar, &EasyGraphicsScrollbar::valueChanged, this, &This::onGraphicsScrollbarValueChange);
        connect(m_pScrollbar, &EasyGraphicsScrollbar::wheeled, this, &This::onGraphicsScrollbarWheel);
    }

    EASY_GLOBALS.selected_thread = 0;
    emit EASY_GLOBALS.events.selectedThreadChanged(0);
}

//////////////////////////////////////////////////////////////////////////

int EasyGraphicsView::updateVisibleSceneRect()
{
    m_visibleSceneRect = mapToScene(rect()).boundingRect();

    auto vbar = verticalScrollBar();
    int vbar_width = 0;
    if (vbar && vbar->isVisible())
        vbar_width = vbar->width() + 2;

    m_visibleSceneRect.setWidth(m_visibleSceneRect.width() - vbar_width);
    m_visibleSceneRect.setHeight(m_visibleSceneRect.height() - TIMELINE_ROW_SIZE);

    return vbar_width;
}

void EasyGraphicsView::updateTimelineStep(qreal _windowWidth)
{
    const auto time = units2microseconds(_windowWidth);
    if (time < 100)
        m_timelineStep = 1e-2;
    else if (time < 10e3)
        m_timelineStep = 1;
    else if (time < 10e6)
        m_timelineStep = 1e3;
    else
        m_timelineStep = 1e6;

    const auto optimal_steps = static_cast<int>(40 * m_visibleSceneRect.width() / 1500);
    auto steps = time / m_timelineStep;
    while (steps > optimal_steps) {
        m_timelineStep *= 10;
        steps *= 0.1;
    }

    m_timelineStep = microseconds2units(m_timelineStep);
}

void EasyGraphicsView::repaintScene()
{
    scene()->update(m_visibleSceneRect);
    emit sceneUpdated();
}

//////////////////////////////////////////////////////////////////////////

void EasyGraphicsView::scaleTo(qreal _scale)
{
    if (m_bEmpty)
    {
        return;
    }

    // have to limit scale because of Qt's QPainter feature: it doesn't draw text
    // with very big coordinates (but it draw rectangles with the same coordinates good).
    m_scale = clamp(MIN_SCALE, _scale, MAX_SCALE); 
    const int vbar_width = updateVisibleSceneRect();

    // Update slider width for scrollbar
    const auto windowWidth = (m_visibleSceneRect.width() + vbar_width) / m_scale;
    m_pScrollbar->setSliderWidth(windowWidth);

    updateTimelineStep(windowWidth);
    repaintScene();
}

void EasyGraphicsView::wheelEvent(QWheelEvent* _event)
{
    m_idleTime = 0;

    if (!m_bEmpty)
        onWheel(mapToScene(_event->pos()).x(), _event->delta());
    _event->accept();
}

void EasyGraphicsView::onGraphicsScrollbarWheel(qreal _mouseX, int _wheelDelta)
{
    m_idleTime = 0;

    for (auto item : m_items)
    {
        if (item->threadId() == EASY_GLOBALS.selected_thread)
        {
            scrollTo(item);
            break;
        }
    }

    onWheel(_mouseX, _wheelDelta);
}

void EasyGraphicsView::scrollTo(const EasyGraphicsItem* _item)
{
    m_bUpdatingRect = true;
    auto vbar = verticalScrollBar();
    vbar->setValue(_item->y() + (_item->boundingRect().height() - vbar->pageStep()) * 0.5);
    m_bUpdatingRect = false;
}

void EasyGraphicsView::onWheel(qreal _mouseX, int _wheelDelta)
{
    const decltype(m_scale) scaleCoeff = _wheelDelta > 0 ? ::profiler_gui::SCALING_COEFFICIENT : ::profiler_gui::SCALING_COEFFICIENT_INV;

    // Remember current mouse position
    _mouseX = clamp(0., _mouseX, m_sceneWidth);
    const auto mousePosition = m_offset + _mouseX / m_scale;

    // have to limit scale because of Qt's QPainter feature: it doesn't draw text
    // with very big coordinates (but it draw rectangles with the same coordinates good).
    m_scale = clamp(MIN_SCALE, m_scale * scaleCoeff, MAX_SCALE);

    //updateVisibleSceneRect(); // Update scene rect

    // Update slider width for scrollbar
    auto vbar = verticalScrollBar();
    const int vbar_width = (vbar != nullptr && vbar->isVisible() ? vbar->width() + 2 : 0);
    const auto windowWidth = (m_visibleSceneRect.width() + vbar_width) / m_scale;
    m_pScrollbar->setSliderWidth(windowWidth);

    // Calculate new offset to simulate QGraphicsView::AnchorUnderMouse scaling behavior
    m_offset = clamp(0., mousePosition - _mouseX / m_scale, m_sceneWidth - windowWidth);

    // Update slider position
    m_bUpdatingRect = true; // To be sure that updateVisibleSceneRect will not be called by scrollbar change
    m_pScrollbar->setValue(m_offset);
    m_bUpdatingRect = false;

    updateVisibleSceneRect(); // Update scene rect
    updateTimelineStep(windowWidth);
    repaintScene(); // repaint scene
}

//////////////////////////////////////////////////////////////////////////

void EasyGraphicsView::mousePressEvent(QMouseEvent* _event)
{
    m_idleTime = 0;

    if (m_bEmpty)
    {
        _event->accept();
        return;
    }

    m_mouseButtons = _event->buttons();
    m_mousePressPos = _event->pos();

    if (m_mouseButtons & Qt::LeftButton)
    {
        if (m_chronometerItemAux->isVisible() && (m_chronometerItemAux->hoverLeft() || m_chronometerItemAux->hoverRight()))
        {
            m_chronometerItemAux->setReverse(m_chronometerItemAux->hoverLeft());
            m_bDoubleClick = true;
        }
        else if (m_chronometerItem->isVisible() && (m_chronometerItem->hoverLeft() || m_chronometerItem->hoverRight()))
        {
            m_chronometerItem->setReverse(m_chronometerItem->hoverLeft());
            m_mouseButtons = Qt::RightButton;
            return;
        }
    }

    if (m_mouseButtons & Qt::RightButton)
    {
        if (m_chronometerItem->isVisible() && (m_chronometerItem->hoverLeft() || m_chronometerItem->hoverRight()))
        {
            m_chronometerItem->setReverse(m_chronometerItem->hoverLeft());
        }
        else
        {
            const auto mouseX = m_offset + mapToScene(m_mousePressPos).x() / m_scale;
            m_chronometerItem->setLeftRight(mouseX, mouseX);
            m_chronometerItem->hide();
            m_pScrollbar->hideChrono();
        }
    }

    _event->accept();
}

void EasyGraphicsView::mouseDoubleClickEvent(QMouseEvent* _event)
{
    m_idleTime = 0;

    if (m_bEmpty)
    {
        _event->accept();
        return;
    }

    m_mouseButtons = _event->buttons();
    m_mousePressPos = _event->pos();
    m_bDoubleClick = true;

    if (m_mouseButtons & Qt::LeftButton)
    {
        const auto mouseX = m_offset + mapToScene(m_mousePressPos).x() / m_scale;
        m_chronometerItemAux->setLeftRight(mouseX, mouseX);
        m_chronometerItemAux->hide();
        emit sceneUpdated();
    }

    _event->accept();
}

//////////////////////////////////////////////////////////////////////////

void EasyGraphicsView::mouseReleaseEvent(QMouseEvent* _event)
{
    m_idleTime = 0;

    if (m_bEmpty)
    {
        _event->accept();
        return;
    }

    bool chronoHidden = false;
    bool changedSelection = false, changedSelectedItem = false;
    if (m_mouseButtons & Qt::RightButton)
    {
        if (m_chronometerItem->isVisible() && m_chronometerItem->width() < 1e-6)
        {
            m_chronometerItem->hide();
            m_pScrollbar->hideChrono();
        }

        if (!m_selectedBlocks.empty())
        {
            changedSelection = true;
            m_selectedBlocks.clear();
        }

        if (m_chronometerItem->isVisible())
        {
            //printf("INTERVAL: {%lf, %lf} ms\n", m_chronometerItem->left(), m_chronometerItem->right());

            for (auto item : m_items)
            {
                if (!EASY_GLOBALS.only_current_thread_hierarchy || item->threadId() == EASY_GLOBALS.selected_thread)
                    item->getBlocks(m_chronometerItem->left(), m_chronometerItem->right(), m_selectedBlocks);
            }

            if (!m_selectedBlocks.empty())
            {
                changedSelection = true;
            }
        }
    }

    const ::profiler_gui::EasyBlock* selectedBlock = nullptr;
    ::profiler::thread_id_t selectedBlockThread = 0;
    const auto previouslySelectedBlock = EASY_GLOBALS.selected_block;
    if (m_mouseButtons & Qt::LeftButton)
    {
        bool clicked = false;

        if (m_chronometerItemAux->isVisible() && m_chronometerItemAux->width() < 1e-6)
        {
            chronoHidden = true;
            m_chronometerItemAux->hide();
        }
        else if (m_chronometerItem->isVisible() && m_chronometerItem->hoverIndicator())
        {
            // Jump to selected zone
            clicked = true;
            m_flickerSpeedX = m_flickerSpeedY = 0;
            m_pScrollbar->setValue(m_chronometerItem->left() + m_chronometerItem->width() * 0.5 - m_pScrollbar->sliderHalfWidth());
        }

        if (!clicked && m_mouseMovePath.manhattanLength() < 5)
        {
            // Handle Click

            //clicked = true;
            auto mouseClickPos = mapToScene(m_mousePressPos);
            if (mouseClickPos.x() >= 0)
            {
                mouseClickPos.setX(m_offset + mouseClickPos.x() / m_scale);

                // Try to select one of item blocks
                for (auto item : m_items)
                {
                    ::profiler::block_index_t i = ~0U;
                    auto block = item->intersect(mouseClickPos, i);
                    if (block)
                    {
                        changedSelectedItem = true;
                        selectedBlock = block;
                        selectedBlockThread = item->threadId();
                        EASY_GLOBALS.selected_block = i;
                        EASY_GLOBALS.selected_block_id = easyBlock(i).tree.node->id();
                        break;
                    }
                }

                if (!changedSelectedItem && !::profiler_gui::is_max(EASY_GLOBALS.selected_block))
                {
                    changedSelectedItem = true;
                    ::profiler_gui::set_max(EASY_GLOBALS.selected_block);
                    ::profiler_gui::set_max(EASY_GLOBALS.selected_block_id);
                }
            }
        }
    }

    m_bDoubleClick = false;
    m_mouseButtons = _event->buttons();
    m_mouseMovePath = QPoint();
    _event->accept();

    if (changedSelection)
    {
        emit intervalChanged(m_selectedBlocks, m_beginTime, position2time(m_chronometerItem->left()), position2time(m_chronometerItem->right()), m_chronometerItem->reverse());
    }

    if (changedSelectedItem)
    {
        m_bUpdatingRect = true;
        if (selectedBlock != nullptr && previouslySelectedBlock == EASY_GLOBALS.selected_block && !selectedBlock->tree.children.empty())
        {
            EASY_GLOBALS.gui_blocks[previouslySelectedBlock].expanded = !EASY_GLOBALS.gui_blocks[previouslySelectedBlock].expanded;
            emit EASY_GLOBALS.events.itemsExpandStateChanged();
        }
        emit EASY_GLOBALS.events.selectedBlockChanged(EASY_GLOBALS.selected_block);

        if (EASY_GLOBALS.selecting_block_changes_thread && selectedBlock != nullptr && EASY_GLOBALS.selected_thread != selectedBlockThread)
        {
            EASY_GLOBALS.selected_thread = selectedBlockThread;

            m_pScrollbar->lock();
            emit EASY_GLOBALS.events.selectedThreadChanged(EASY_GLOBALS.selected_thread);
            m_pScrollbar->unlock();
        }
        m_bUpdatingRect = false;

        if (selectedBlock != nullptr && selectedBlockThread == EASY_GLOBALS.selected_thread)
            m_pScrollbar->setHistogramSource(EASY_GLOBALS.selected_thread, EASY_GLOBALS.selected_block_id);
        else
        {
            for (auto item : m_items)
            {
                if (item->threadId() == EASY_GLOBALS.selected_thread)
                {
                    m_pScrollbar->setHistogramSource(EASY_GLOBALS.selected_thread, item->items(0));
                    break;
                }
            }
        }

        repaintScene();
    }
    else if (chronoHidden)
    {
        emit sceneUpdated();
    }
}

//////////////////////////////////////////////////////////////////////////

bool EasyGraphicsView::moveChrono(EasyChronometerItem* _chronometerItem, qreal _mouseX)
{
    if (_chronometerItem->reverse())
    {
        if (_mouseX > _chronometerItem->right())
        {
            _chronometerItem->setReverse(false);
            _chronometerItem->setLeftRight(_chronometerItem->right(), _mouseX);

            if (_chronometerItem->hoverLeft())
            {
                _chronometerItem->setHoverLeft(false);
                _chronometerItem->setHoverRight(true);
            }
        }
        else
        {
            _chronometerItem->setLeftRight(_mouseX, _chronometerItem->right());
        }
    }
    else
    {
        if (_mouseX < _chronometerItem->left())
        {
            _chronometerItem->setReverse(true);
            _chronometerItem->setLeftRight(_mouseX, _chronometerItem->left());

            if (_chronometerItem->hoverRight())
            {
                _chronometerItem->setHoverLeft(true);
                _chronometerItem->setHoverRight(false);
            }
        }
        else
        {
            _chronometerItem->setLeftRight(_chronometerItem->left(), _mouseX);
        }
    }

    if (!_chronometerItem->isVisible() && _chronometerItem->width() > 1e-6)
    {
        _chronometerItem->show();
        return true;
    }

    return false;
}

void EasyGraphicsView::mouseMoveEvent(QMouseEvent* _event)
{
    m_idleTime = 0;

    if (m_bEmpty || (m_mouseButtons == 0 && !m_chronometerItem->isVisible() && !m_chronometerItemAux->isVisible()))
    {
        _event->accept();
        return;
    }

    bool needUpdate = false;
    const auto pos = _event->pos();
    const auto delta = pos - m_mousePressPos;
    m_mousePressPos = pos;

    if (m_mouseButtons != 0)
    {
        m_mouseMovePath.setX(m_mouseMovePath.x() + qAbs(delta.x()));
        m_mouseMovePath.setY(m_mouseMovePath.y() + qAbs(delta.y()));
    }

    auto mouseScenePos = mapToScene(m_mousePressPos);
    mouseScenePos.setX(m_offset + mouseScenePos.x() / m_scale);
    const auto x = clamp(0., mouseScenePos.x(), m_sceneWidth);

    if (m_mouseButtons & Qt::RightButton)
    {
        bool showItem = moveChrono(m_chronometerItem, x);
        m_pScrollbar->setChronoPos(m_chronometerItem->left(), m_chronometerItem->right());

        if (showItem)
        {
            m_pScrollbar->showChrono();
        }

        needUpdate = true;
    }

    if (m_mouseButtons & Qt::LeftButton)
    {
        if (m_bDoubleClick)
        {
            moveChrono(m_chronometerItemAux, x);
        }
        else
        {
            auto vbar = verticalScrollBar();

            m_bUpdatingRect = true; // Block scrollbars from updating scene rect to make it possible to do it only once
            vbar->setValue(vbar->value() - delta.y());
            m_pScrollbar->setValue(m_pScrollbar->value() - delta.x() / m_scale);
            m_bUpdatingRect = false;
            // Seems like an ugly stub, but QSignalBlocker is also a bad decision
            // because if scrollbar does not emit valueChanged signal then viewport does not move

            updateVisibleSceneRect(); // Update scene visible rect only once

            // Update flicker speed
            m_flickerSpeedX += delta.x() >> 1;
            m_flickerSpeedY += delta.y();
            if (!m_flickerTimer.isActive())
            {
                // If flicker timer is not started, then start it
                m_flickerTimer.start(FLICKER_INTERVAL);
            }
        }

        needUpdate = true;
    }

    if (m_mouseButtons == 0)
    {
        if (m_chronometerItem->isVisible())
        {
            auto prevValue = m_chronometerItem->hoverIndicator();
            m_chronometerItem->setHoverIndicator(m_chronometerItem->indicatorContains(mouseScenePos));
            needUpdate = needUpdate || (prevValue != m_chronometerItem->hoverIndicator());

            prevValue = m_chronometerItem->hoverLeft();
            m_chronometerItem->setHoverLeft(m_chronometerItem->hoverLeft(mouseScenePos.x()));
            needUpdate = needUpdate || (prevValue != m_chronometerItem->hoverLeft());

            if (!m_chronometerItem->hoverLeft())
            {
                prevValue = m_chronometerItem->hoverRight();
                m_chronometerItem->setHoverRight(m_chronometerItem->hoverRight(mouseScenePos.x()));
                needUpdate = needUpdate || (prevValue != m_chronometerItem->hoverRight());
            }
        }

        if (m_chronometerItemAux->isVisible())
        {
            auto prevValue = m_chronometerItemAux->hoverLeft();
            m_chronometerItemAux->setHoverLeft(m_chronometerItemAux->hoverLeft(mouseScenePos.x()));
            needUpdate = needUpdate || (prevValue != m_chronometerItemAux->hoverLeft());

            if (!m_chronometerItemAux->hoverLeft())
            {
                prevValue = m_chronometerItemAux->hoverRight();
                m_chronometerItemAux->setHoverRight(m_chronometerItemAux->hoverRight(mouseScenePos.x()));
                needUpdate = needUpdate || (prevValue != m_chronometerItemAux->hoverRight());
            }
        }
    }

    if (needUpdate)
    {
        repaintScene(); // repaint scene
    }

    _event->accept();
}

//////////////////////////////////////////////////////////////////////////

void EasyGraphicsView::keyPressEvent(QKeyEvent* _event)
{
    static const int KeyStep = 100;

    const int key = _event->key();
    m_idleTime = 0;

    switch (key)
    {
        case Qt::Key_Right:
        case Qt::Key_6:
        {
            m_pScrollbar->setValue(m_pScrollbar->value() + KeyStep / m_scale);
            break;
        }

        case Qt::Key_Left:
        case Qt::Key_4:
        {
            m_pScrollbar->setValue(m_pScrollbar->value() - KeyStep / m_scale);
            break;
        }

        case Qt::Key_Up:
        case Qt::Key_8:
        {
            auto vbar = verticalScrollBar();
            vbar->setValue(vbar->value() - KeyStep);
            break;
        }

        case Qt::Key_Down:
        case Qt::Key_2:
        {
            auto vbar = verticalScrollBar();
            vbar->setValue(vbar->value() + KeyStep);
            break;
        }

        case Qt::Key_Plus:
        case Qt::Key_Equal:
        {
            onWheel(mapToScene(mapFromGlobal(QCursor::pos())).x(), KeyStep);
            break;
        }

        case Qt::Key_Minus:
        {
            onWheel(mapToScene(mapFromGlobal(QCursor::pos())).x(), -KeyStep);
            break;
        }
    }

    //m_keys.insert(key);
    _event->accept();
}

void EasyGraphicsView::keyReleaseEvent(QKeyEvent* _event)
{
    //const int key = _event->key();
    m_idleTime = 0;

    //m_keys.erase(key);
    _event->accept();
}

//////////////////////////////////////////////////////////////////////////

void EasyGraphicsView::resizeEvent(QResizeEvent* _event)
{
    Parent::resizeEvent(_event);

    const QRectF previousRect = m_visibleSceneRect;
    const int vbar_width = updateVisibleSceneRect(); // Update scene visible rect only once    

    // Update slider width for scrollbar
    const auto windowWidth = (m_visibleSceneRect.width() + vbar_width) / m_scale;
    m_pScrollbar->setSliderWidth(windowWidth);

    // Calculate new offset to save old screen center
    const auto deltaWidth = m_visibleSceneRect.width() - previousRect.width();
    m_offset = clamp(0., m_offset - deltaWidth * 0.5 / m_scale, m_sceneWidth - windowWidth);

    // Update slider position
    m_bUpdatingRect = true; // To be sure that updateVisibleSceneRect will not be called by scrollbar change
    m_pScrollbar->setValue(m_offset);
    m_bUpdatingRect = false;

    repaintScene(); // repaint scene
}

//////////////////////////////////////////////////////////////////////////

void EasyGraphicsView::initMode()
{
    // TODO: find mode with least number of bugs :)
    // There are always some display bugs...

    setCacheMode(QGraphicsView::CacheNone);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setOptimizationFlag(QGraphicsView::DontSavePainterState, true);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &This::onScrollbarValueChange);
    connect(&m_flickerTimer, &QTimer::timeout, this, &This::onFlickerTimeout);
    connect(&m_idleTimer, &QTimer::timeout, this, &This::onIdleTimeout);

    auto globalSignals = &EASY_GLOBALS.events;
    connect(globalSignals, &::profiler_gui::EasyGlobalSignals::hierarchyFlagChanged, this, &This::onHierarchyFlagChange);
    connect(globalSignals, &::profiler_gui::EasyGlobalSignals::selectedThreadChanged, this, &This::onSelectedThreadChange);
    connect(globalSignals, &::profiler_gui::EasyGlobalSignals::selectedBlockChanged, this, &This::onSelectedBlockChange);
    connect(globalSignals, &::profiler_gui::EasyGlobalSignals::itemsExpandStateChanged, this, &This::onRefreshRequired);
    connect(globalSignals, &::profiler_gui::EasyGlobalSignals::refreshRequired, this, &This::onRefreshRequired);

    connect(globalSignals, &::profiler_gui::EasyGlobalSignals::selectedBlockIdChanged, [this](::profiler::block_id_t)
    {
        if (::profiler_gui::is_max(EASY_GLOBALS.selected_block_id))
        {
            if (EASY_GLOBALS.selected_thread != 0)
            {
                for (auto item : m_items)
                {
                    if (item->threadId() == EASY_GLOBALS.selected_thread)
                    {
                        m_pScrollbar->setHistogramSource(EASY_GLOBALS.selected_thread, item->items(0));
                        break;
                    }
                }
            }
            else
            {
                m_pScrollbar->setHistogramSource(0, nullptr);
            }
        }
        else
            m_pScrollbar->setHistogramSource(EASY_GLOBALS.selected_thread, EASY_GLOBALS.selected_block_id);
        onRefreshRequired();
    });

    connect(globalSignals, &::profiler_gui::EasyGlobalSignals::threadNameDecorationChanged, this, &This::onThreadViewChanged);
    connect(globalSignals, &::profiler_gui::EasyGlobalSignals::hexThreadIdChanged, this, &This::onThreadViewChanged);

    connect(globalSignals, &::profiler_gui::EasyGlobalSignals::blocksTreeModeChanged, [this]()
    {
        if (!m_selectedBlocks.empty())
            emit intervalChanged(m_selectedBlocks, m_beginTime, position2time(m_chronometerItem->left()), position2time(m_chronometerItem->right()), m_chronometerItem->reverse());
    });
}

//////////////////////////////////////////////////////////////////////////

void EasyGraphicsView::onThreadViewChanged()
{
    if (m_bEmpty)
        return;

    for (auto item : m_items)
        item->validateName();

    emit treeChanged();

    updateVisibleSceneRect();
    onHierarchyFlagChange(EASY_GLOBALS.only_current_thread_hierarchy);

    repaintScene();
}

//////////////////////////////////////////////////////////////////////////

void EasyGraphicsView::onScrollbarValueChange(int)
{
    if (!m_bUpdatingRect && !m_bEmpty)
        updateVisibleSceneRect();
}

void EasyGraphicsView::onGraphicsScrollbarValueChange(qreal _value)
{
    if (!m_bEmpty)
    {
        m_offset = _value;
        if (!m_bUpdatingRect)
        {
            updateVisibleSceneRect();
            repaintScene();
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void EasyGraphicsView::onFlickerTimeout()
{
    ++m_flickerCounterX;
    ++m_flickerCounterY;

    if (m_mouseButtons & Qt::LeftButton)
    {
        // Fast slow-down and stop if mouse button is pressed, no flicking.
        m_flickerSpeedX >>= 1;
        m_flickerSpeedY >>= 1;
        if (m_flickerSpeedX == -1) m_flickerSpeedX = 0;
        if (m_flickerSpeedY == -1) m_flickerSpeedY = 0;
    }
    else
    {
        // Flick when mouse button is not pressed

        auto vbar = verticalScrollBar();

        m_bUpdatingRect = true; // Block scrollbars from updating scene rect to make it possible to do it only once
        m_pScrollbar->setValue(m_pScrollbar->value() - m_flickerSpeedX / m_scale);
        vbar->setValue(vbar->value() - m_flickerSpeedY);
        m_bUpdatingRect = false;
        // Seems like an ugly stub, but QSignalBlocker is also a bad decision
        // because if scrollbar does not emit valueChanged signal then viewport does not move

        updateVisibleSceneRect(); // Update scene visible rect only once
        repaintScene(); // repaint scene

        const int dx = static_cast<int>(sign(m_flickerSpeedX) * m_flickerCounterX / FLICKER_FACTOR);
        const int dy = static_cast<int>(sign(m_flickerSpeedY) * m_flickerCounterY / FLICKER_FACTOR);

        if (abs(dx) > 0)
        {
            m_flickerSpeedX -= absmin(dx, m_flickerSpeedX);
            m_flickerCounterX = 0;
        }

        if (abs(dy) > 0)
        {
            m_flickerSpeedY -= absmin(dy, m_flickerSpeedY);
            m_flickerCounterY = 0;
        }
    }

    if (m_flickerSpeedX == 0 && m_flickerSpeedY == 0)
    {
        // Flicker stopped, no timer needed.
        m_flickerTimer.stop();
        m_flickerSpeedX = 0;
        m_flickerSpeedY = 0;
        m_flickerCounterX = 0;
        m_flickerCounterY = 0;
    }
}

//////////////////////////////////////////////////////////////////////////

void EasyGraphicsView::onIdleTimeout()
{
    m_idleTime += IDLE_TIMER_INTERVAL;

    if (m_idleTime < IDLE_TIME)
    {
        removePopup(true);
        return;
    }

    if (m_popupWidget != nullptr)
        return;

    auto scenePos = mapToScene(mapFromGlobal(QCursor::pos()));

    if (scenePos.x() < m_visibleSceneRect.left() || scenePos.x() > m_visibleSceneRect.right())
        return;

    if (scenePos.y() < m_visibleSceneRect.top() || scenePos.y() > m_visibleSceneRect.bottom())
        return;

    decltype(scenePos) pos(m_offset + scenePos.x() / m_scale, scenePos.y());

    // Try to select one of context switches or items
    for (auto item : m_items)
    {
        ::profiler::block_index_t i = ~0U;
        auto block = item->intersect(pos, i);
        if (block)
        {
            const auto& itemBlock = block->tree;
            const auto& itemDesc = easyDescriptor(itemBlock.node->id());
            auto name = *itemBlock.node->name() != 0 ? itemBlock.node->name() : itemDesc.name();

            auto widget = new QWidget(nullptr, Qt::FramelessWindowHint);
            if (widget == nullptr)
                return;

            widget->setAttribute(Qt::WA_ShowWithoutActivating, true);
            widget->setFocusPolicy(Qt::NoFocus);

            auto lay = new QGridLayout(widget);
            if (lay == nullptr)
                return;

            int row = 0;
            if (itemDesc.type() == ::profiler::BLOCK_TYPE_BLOCK)
            {
                //lay->addWidget(new QLabel("Name:", widget), row, 0, Qt::AlignRight);
                lay->addWidget(new EasyBoldLabel(::profiler_gui::toUnicode(name), widget), row, 0, 1, 5, Qt::AlignHCenter);
                ++row;

                const auto duration = itemBlock.node->duration();
                lay->addWidget(new QLabel("Duration:", widget), row, 0, Qt::AlignRight);
                lay->addWidget(new QLabel(::profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units, duration, 3), widget), row, 1, 1, 3, Qt::AlignLeft);
                ++row;

                ::profiler::timestamp_t children_duration = 0;
                for (auto child : itemBlock.children)
                    children_duration += easyBlock(child).tree.node->duration();

                const auto self_duration = duration - children_duration;
                const auto self_percent = duration == 0 ? 100. : ::profiler_gui::percentReal(self_duration, duration);
                lay->addWidget(new QLabel("Self:", widget), row, 0, Qt::AlignRight);
                lay->addWidget(new QLabel(QString("%1 (%2%)").arg(::profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units, self_duration, 3)).arg(QString::number(self_percent, 'g', 3)), widget), row, 1, 1, 3, Qt::AlignLeft);
                ++row;
            }
            else
            {
                lay->addWidget(new EasyBoldLabel("User defined event", widget), row, 0, 1, 2, Qt::AlignHCenter);
                ++row;

                lay->addWidget(new QLabel("Name:", widget), row, 0, Qt::AlignRight);
                lay->addWidget(new QLabel(::profiler_gui::toUnicode(name), widget), row, 1, Qt::AlignLeft);
                ++row;
            }

            if (itemBlock.per_thread_stats)
            {
                if (itemDesc.type() == ::profiler::BLOCK_TYPE_BLOCK)
                {
                    const auto duration = itemBlock.node->duration();

                    lay->addWidget(new QLabel("Average:", widget), row, 0, Qt::AlignRight);
                    lay->addWidget(new QLabel(::profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units, itemBlock.per_thread_stats->average_duration(), 3), widget), row, 1, 1, 3, Qt::AlignLeft);
                    ++row;

                    // Calculate idle/active time
                    {
                        auto threadRoot = item->root();

                        ::profiler::block_index_t ind = 0;
                        auto it = ::std::lower_bound(threadRoot->sync.begin(), threadRoot->sync.end(), itemBlock.node->begin(), [](::profiler::block_index_t _cs_index, ::profiler::timestamp_t _val)
                        {
                            return EASY_GLOBALS.gui_blocks[_cs_index].tree.node->begin() < _val;
                        });

                        if (it != threadRoot->sync.end())
                        {
                            ind = it - threadRoot->sync.begin();
                            if (ind > 0)
                                --ind;
                        }
                        else
                        {
                            ind = static_cast<::profiler::block_index_t>(threadRoot->sync.size());
                        }

                        ::profiler::timestamp_t idleTime = 0;
                        for (::profiler::block_index_t ncs = static_cast<::profiler::block_index_t>(threadRoot->sync.size()); ind < ncs; ++ind)
                        {
                            auto cs_index = threadRoot->sync[ind];
                            const auto cs = EASY_GLOBALS.gui_blocks[cs_index].tree.node;

                            if (cs->begin() > itemBlock.node->end())
                                break;

                            if (itemBlock.node->begin() <= cs->begin() && cs->end() <= itemBlock.node->end())
                                idleTime += cs->duration();
                        }

                        const auto active_time = duration - idleTime;
                        const auto active_percent = duration == 0 ? 100. : ::profiler_gui::percentReal(active_time, duration);
                        lay->addWidget(new QLabel("Active time:", widget), row, 0, Qt::AlignRight);
                        lay->addWidget(new QLabel(QString("%1 (%2%)").arg(::profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units, active_time, 3)).arg(QString::number(active_percent, 'g', 3)), widget), row, 1, 1, 3, Qt::AlignLeft);
                        ++row;
                    }

                    lay->addWidget(new EasyBoldLabel("-------- Statistics --------", widget), row, 0, 1, 5, Qt::AlignHCenter);
                    lay->addWidget(new QLabel("per ", widget), row + 1, 0, Qt::AlignRight);
                    lay->addWidget(new QLabel("This %:", widget), row + 2, 0, Qt::AlignRight);
                    lay->addWidget(new QLabel("Sum %:", widget), row + 3, 0, Qt::AlignRight);
                    lay->addWidget(new QLabel("Sum self %:", widget), row + 4, 0, Qt::AlignRight);
                    lay->addWidget(new QLabel("N Calls:", widget), row + 5, 0, Qt::AlignRight);

                    lay->addWidget(new QLabel("Thread", widget), row + 1, 1, Qt::AlignHCenter);

                    auto percent = ::profiler_gui::percentReal(duration, item->root()->profiled_time);
                    lay->addWidget(new QLabel(0.005 < percent && percent < 0.5001 ? QString::number(percent, 'f', 2) : QString::number(static_cast<int>(0.5 + percent)), widget), row + 2, 1, Qt::AlignHCenter);

                    lay->addWidget(new QLabel(QString::number(::profiler_gui::percent(itemBlock.per_thread_stats->total_duration, item->root()->profiled_time)), widget), row + 3, 1, Qt::AlignHCenter);

                    lay->addWidget(new QLabel(QString::number(::profiler_gui::percent(itemBlock.per_thread_stats->total_duration - itemBlock.per_thread_stats->total_children_duration, item->root()->profiled_time)), widget), row + 4, 1, Qt::AlignHCenter);

                    lay->addWidget(new QLabel(QString::number(itemBlock.per_thread_stats->calls_number), widget), row + 5, 1, Qt::AlignHCenter);

                    int col = 1;

                    if (itemBlock.per_frame_stats->parent_block != i && !::profiler_gui::is_max(itemBlock.per_frame_stats->parent_block))
                    {
                        ++col;
                        auto frame_duration = blocksTree(itemBlock.per_frame_stats->parent_block).node->duration();

                        lay->addWidget(new QLabel("Frame", widget), row + 1, col, Qt::AlignHCenter);

                        percent = ::profiler_gui::percentReal(duration, frame_duration);
                        lay->addWidget(new QLabel(0.005 < percent && percent < 0.5001 ? QString::number(percent, 'f', 2) : QString::number(static_cast<int>(0.5 + percent)), widget), row + 2, col, Qt::AlignHCenter);

                        percent = ::profiler_gui::percentReal(itemBlock.per_frame_stats->total_duration, frame_duration);
                        lay->addWidget(new QLabel(0.005 < percent && percent < 0.5001 ? QString::number(percent, 'f', 2) : QString::number(static_cast<int>(0.5 + percent)), widget), row + 3, col, Qt::AlignHCenter);

                        percent = ::profiler_gui::percentReal(itemBlock.per_frame_stats->total_duration - itemBlock.per_frame_stats->total_children_duration, frame_duration);
                        lay->addWidget(new QLabel(0.005 < percent && percent < 0.5001 ? QString::number(percent, 'f', 2) : QString::number(static_cast<int>(0.5 + percent)), widget), row + 4, col, Qt::AlignHCenter);

                        lay->addWidget(new QLabel(QString::number(itemBlock.per_frame_stats->calls_number), widget), row + 5, col, Qt::AlignHCenter);
                    }

                    if (!::profiler_gui::is_max(itemBlock.per_parent_stats->parent_block))// != item->threadId())
                    {
                        ++col;
                        auto parent_duration = blocksTree(itemBlock.per_parent_stats->parent_block).node->duration();

                        lay->addWidget(new QLabel("Parent", widget), row + 1, col, Qt::AlignHCenter);

                        percent = ::profiler_gui::percentReal(duration, parent_duration);
                        lay->addWidget(new QLabel(0.005 < percent && percent < 0.5001 ? QString::number(percent, 'f', 2) : QString::number(static_cast<int>(0.5 + percent)), widget), row + 2, col, Qt::AlignHCenter);

                        percent = ::profiler_gui::percentReal(itemBlock.per_parent_stats->total_duration, parent_duration);
                        lay->addWidget(new QLabel(0.005 < percent && percent < 0.5001 ? QString::number(percent, 'f', 2) : QString::number(static_cast<int>(0.5 + percent)), widget), row + 3, col, Qt::AlignHCenter);

                        percent = ::profiler_gui::percentReal(itemBlock.per_parent_stats->total_duration - itemBlock.per_parent_stats->total_children_duration, parent_duration);
                        lay->addWidget(new QLabel(0.005 < percent && percent < 0.5001 ? QString::number(percent, 'f', 2) : QString::number(static_cast<int>(0.5 + percent)), widget), row + 4, col, Qt::AlignHCenter);

                        lay->addWidget(new QLabel(QString::number(itemBlock.per_parent_stats->calls_number), widget), row + 5, col, Qt::AlignHCenter);

                        ++col;
                    }
                }
                else
                {
                    lay->addWidget(new QLabel("N calls/Thread:", widget), row, 0, Qt::AlignRight);
                    lay->addWidget(new QLabel(QString::number(itemBlock.per_thread_stats->calls_number), widget), row, 1, Qt::AlignLeft);
                }
            }

            m_popupWidget = new QGraphicsProxyWidget();
            m_popupWidget->setWidget(widget);

            break;
        }

        auto cse = item->intersectEvent(pos);
        if (cse)
        {
            const auto& itemBlock = cse->tree;

            auto widget = new QWidget(nullptr, Qt::FramelessWindowHint);
            if (widget == nullptr)
                return;

            widget->setAttribute(Qt::WA_ShowWithoutActivating, true);
            widget->setFocusPolicy(Qt::NoFocus);

            auto lay = new QGridLayout(widget);
            if (lay == nullptr)
                return;

            int row = 0;
            lay->addWidget(new EasyBoldLabel("Context switch event", widget), row, 0, 1, 3, Qt::AlignHCenter);
            ++row;

            lay->addWidget(new QLabel("Thread:", widget), row, 0, Qt::AlignRight);

            const char* process_name = "";
            ::profiler::thread_id_t tid = 0;
            if (EASY_GLOBALS.version < ::profiler_gui::V130)
            {
                tid = cse->tree.node->id();
                process_name = cse->tree.node->name();
            }
            else
            {
                tid = cse->tree.cs->tid();
                process_name = cse->tree.cs->name();
            }

            auto it = EASY_GLOBALS.profiler_blocks.find(tid);

            if (it != EASY_GLOBALS.profiler_blocks.end())
            {
                if (EASY_GLOBALS.hex_thread_id)
                    lay->addWidget(new QLabel(QString("0x%1 %2").arg(tid, 0, 16).arg(it->second.name()), widget), row, 1, 1, 2, Qt::AlignLeft);
                else
                    lay->addWidget(new QLabel(QString("%1 %2").arg(tid).arg(it->second.name()), widget), row, 1, 1, 2, Qt::AlignLeft);
            }
            else if (EASY_GLOBALS.hex_thread_id)
                lay->addWidget(new QLabel(QString("0x%1").arg(tid, 0, 16), widget), row, 1, 1, 2, Qt::AlignLeft);
            else
                lay->addWidget(new QLabel(QString::number(tid), widget), row, 1, 1, 2, Qt::AlignLeft);
            ++row;

            lay->addWidget(new QLabel("Process:", widget), row, 0, Qt::AlignRight);
            lay->addWidget(new QLabel(process_name, widget), row, 1, 1, 2, Qt::AlignLeft);
            ++row;

            const auto duration = itemBlock.node->duration();
            lay->addWidget(new QLabel("Duration:", widget), row, 0, Qt::AlignRight);
            lay->addWidget(new QLabel(::profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units, duration, 3), widget), row, 1, 1, 2, Qt::AlignLeft);
            ++row;

            if (itemBlock.per_thread_stats)
            {
                lay->addWidget(new QLabel("Sum:", widget), row, 0, Qt::AlignRight);
                lay->addWidget(new QLabel(::profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units, itemBlock.per_thread_stats->total_duration, 3), widget), row, 1, 1, 2, Qt::AlignLeft);
                ++row;

                lay->addWidget(new EasyBoldLabel("-------- Statistics --------", widget), row, 0, 1, 3, Qt::AlignHCenter);
                lay->addWidget(new QLabel("per ", widget), row + 1, 0, Qt::AlignRight);
                lay->addWidget(new QLabel("This %:", widget), row + 2, 0, Qt::AlignRight);
                lay->addWidget(new QLabel("Sum %:", widget), row + 3, 0, Qt::AlignRight);
                lay->addWidget(new QLabel("N Calls:", widget), row + 4, 0, Qt::AlignRight);

                lay->addWidget(new QLabel("Thread", widget), row + 1, 1, Qt::AlignHCenter);

                auto percent = ::profiler_gui::percentReal(duration, item->root()->profiled_time);
                lay->addWidget(new QLabel(0.005 < percent && percent < 0.5001 ? QString::number(percent, 'f', 2) : QString::number(static_cast<int>(0.5 + percent)), widget), row + 2, 1, Qt::AlignHCenter);

                lay->addWidget(new QLabel(QString::number(::profiler_gui::percent(itemBlock.per_thread_stats->total_duration, item->root()->profiled_time)), widget), row + 3, 1, Qt::AlignHCenter);

                lay->addWidget(new QLabel(QString::number(itemBlock.per_thread_stats->calls_number), widget), row + 4, 1, Qt::AlignHCenter);

                if (itemBlock.per_frame_stats && !::profiler_gui::is_max(itemBlock.per_frame_stats->parent_block))
                {
                    int col = 2;
                    auto frame_duration = blocksTree(itemBlock.per_frame_stats->parent_block).node->duration();

                    lay->addWidget(new QLabel("Frame", widget), row + 1, col, Qt::AlignHCenter);

                    percent = ::profiler_gui::percentReal(duration, frame_duration);
                    lay->addWidget(new QLabel(0.005 < percent && percent < 0.5001 ? QString::number(percent, 'f', 2) : QString::number(static_cast<int>(0.5 + percent)), widget), row + 2, col, Qt::AlignHCenter);

                    percent = ::profiler_gui::percentReal(itemBlock.per_frame_stats->total_duration, frame_duration);
                    lay->addWidget(new QLabel(0.005 < percent && percent < 0.5001 ? QString::number(percent, 'f', 2) : QString::number(static_cast<int>(0.5 + percent)), widget), row + 3, col, Qt::AlignHCenter);

                    lay->addWidget(new QLabel(QString::number(itemBlock.per_frame_stats->calls_number), widget), row + 4, col, Qt::AlignHCenter);
                }
            }

            m_popupWidget = new QGraphicsProxyWidget();
            m_popupWidget->setWidget(widget);

            break;
        }
    }

    if (m_popupWidget != nullptr)
    {
        auto effect = new QGraphicsDropShadowEffect();
        effect->setBlurRadius(5);
        effect->setOffset(3, 3);
        m_popupWidget->setGraphicsEffect(effect);

        scene()->addItem(m_popupWidget);

        auto br = m_popupWidget->boundingRect();
        if (scenePos.y() + br.height() > m_visibleSceneRect.bottom())
            scenePos.setY(::std::max(scenePos.y() - br.height(), m_visibleSceneRect.top()));

        if (scenePos.x() + br.width() > m_visibleSceneRect.right())
            scenePos.setX(::std::max(scenePos.x() - br.width(), m_visibleSceneRect.left()));

        m_popupWidget->setPos(scenePos);
        m_popupWidget->setOpacity(0.95);
    }
}

//////////////////////////////////////////////////////////////////////////

void EasyGraphicsView::onHierarchyFlagChange(bool)
{
    bool changedSelection = false;

    if (!m_selectedBlocks.empty())
    {
        changedSelection = true;
        m_selectedBlocks.clear();
    }

    if (m_chronometerItem->isVisible())
    {
        for (auto item : m_items)
        {
            if (!EASY_GLOBALS.only_current_thread_hierarchy || item->threadId() == EASY_GLOBALS.selected_thread)
                item->getBlocks(m_chronometerItem->left(), m_chronometerItem->right(), m_selectedBlocks);
        }

        if (!m_selectedBlocks.empty())
        {
            changedSelection = true;
        }
    }

    if (changedSelection)
    {
        emit intervalChanged(m_selectedBlocks, m_beginTime, position2time(m_chronometerItem->left()), position2time(m_chronometerItem->right()), m_chronometerItem->reverse());
    }
}

void EasyGraphicsView::onSelectedThreadChange(::profiler::thread_id_t _id)
{
    if (m_pScrollbar == nullptr || m_pScrollbar->hystThread() == _id)
    {
        return;
    }

    if (_id == 0)
    {
        m_pScrollbar->setHistogramSource(0, nullptr);
        return;
    }

    for (auto item : m_items)
    {
        if (item->threadId() == _id)
        {
            m_pScrollbar->setHistogramSource(_id, item->items(0));

            bool changedSelection = false;
            if (EASY_GLOBALS.only_current_thread_hierarchy)
            {
                if (!m_selectedBlocks.empty())
                {
                    changedSelection = true;
                    m_selectedBlocks.clear();
                }

                if (m_chronometerItem->isVisible())
                {
                    item->getBlocks(m_chronometerItem->left(), m_chronometerItem->right(), m_selectedBlocks);
                    if (!m_selectedBlocks.empty())
                        changedSelection = true;
                }
            }

            if (changedSelection)
            {
                emit intervalChanged(m_selectedBlocks, m_beginTime, position2time(m_chronometerItem->left()), position2time(m_chronometerItem->right()), m_chronometerItem->reverse());
            }

            repaintScene();
            return;
        }
    }

    m_pScrollbar->setHistogramSource(0, nullptr);
    repaintScene();
}

//////////////////////////////////////////////////////////////////////////

void EasyGraphicsView::onSelectedBlockChange(unsigned int _block_index)
{
    if (!m_bUpdatingRect)
    {
        if (_block_index < EASY_GLOBALS.gui_blocks.size())
        {
            // Scroll to item

            const auto& guiblock = EASY_GLOBALS.gui_blocks[_block_index];
            const auto thread_item = m_items[guiblock.graphics_item];
            const auto& item = thread_item->items(guiblock.graphics_item_level)[guiblock.graphics_item_index];

            m_flickerSpeedX = m_flickerSpeedY = 0;

            m_bUpdatingRect = true;
            verticalScrollBar()->setValue(static_cast<int>(thread_item->levelY(guiblock.graphics_item_level) - m_visibleSceneRect.height() * 0.5));
            m_pScrollbar->setValue(item.left() + item.width() * 0.5 - m_pScrollbar->sliderHalfWidth());

            if (EASY_GLOBALS.selecting_block_changes_thread && EASY_GLOBALS.selected_thread != thread_item->threadId())
            {
                EASY_GLOBALS.selected_thread = thread_item->threadId();

                m_pScrollbar->lock();
                emit EASY_GLOBALS.events.selectedThreadChanged(EASY_GLOBALS.selected_thread);
                m_pScrollbar->unlock();
            }

            m_pScrollbar->setHistogramSource(EASY_GLOBALS.selected_thread, guiblock.tree.node->id());

            m_bUpdatingRect = false;
        }
        else if (EASY_GLOBALS.selected_thread != 0)
        {
            for (auto item : m_items)
            {
                if (item->threadId() == EASY_GLOBALS.selected_thread)
                {
                    m_pScrollbar->setHistogramSource(EASY_GLOBALS.selected_thread, item->items(0));
                    break;
                }
            }
        }
        else
        {
            m_pScrollbar->setHistogramSource(0, nullptr);
        }

        updateVisibleSceneRect();
        repaintScene();
    }
}

//////////////////////////////////////////////////////////////////////////

void EasyGraphicsView::onRefreshRequired()
{
    if (!m_bUpdatingRect)
    {
        repaintScene();
    }
}

//////////////////////////////////////////////////////////////////////////

EasyGraphicsViewWidget::EasyGraphicsViewWidget(QWidget* _parent)
    : QWidget(_parent)
    , m_scrollbar(new EasyGraphicsScrollbar(this))
    , m_view(new EasyGraphicsView(this))
    , m_threadNamesWidget(new EasyThreadNamesWidget(m_view, m_scrollbar->height(), this))
{
    initWidget();
}

void EasyGraphicsViewWidget::initWidget()
{
    auto lay = new QGridLayout(this);
    lay->setContentsMargins(1, 0, 1, 0);
    lay->addWidget(m_threadNamesWidget, 0, 0, 2, 1);
    lay->setSpacing(1);
    lay->addWidget(m_view, 0, 1);
    lay->setSpacing(1);
    lay->addWidget(m_scrollbar, 1, 1);
    setLayout(lay);

    m_view->setScrollbar(m_scrollbar);
}

EasyGraphicsViewWidget::~EasyGraphicsViewWidget()
{

}

EasyGraphicsView* EasyGraphicsViewWidget::view()
{
    return m_view;
}

void EasyGraphicsViewWidget::clear()
{
    m_scrollbar->clear();
    m_threadNamesWidget->clear();
    m_view->clear();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void EasyThreadNameItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    auto const parentView = static_cast<EasyThreadNamesWidget*>(scene()->parent());
    const auto view = parentView->view();
    const auto& items = view->getItems();
    if (items.empty())
        return;

    const auto visibleSceneRect = view->visibleSceneRect();
    const auto h = visibleSceneRect.height() + TIMELINE_ROW_SIZE - 2;
    const auto w = parentView->width();//parentView->sceneRect().width();

    static const uint16_t OVERLAP = ::profiler_gui::THREADS_ROW_SPACING >> 1;
    static const QBrush brushes[2] = {QColor::fromRgb(BACKGROUND_1), QColor::fromRgb(BACKGROUND_2)};
    int i = -1;

    QRectF rect;

    _painter->resetTransform();

    // Draw thread names
    auto default_font = _painter->font();
    _painter->setFont(BG_FONT);
    for (auto item : items)
    {
        ++i;

        auto br = item->boundingRect();
        auto top = item->y() + br.top() - visibleSceneRect.top() - OVERLAP;
        auto hgt = br.height() + ::profiler_gui::THREADS_ROW_SPACING;
        auto bottom = top + hgt;

        if (top > h || bottom < 0)
            continue;

        if (item->threadId() == EASY_GLOBALS.selected_thread)
            _painter->setBrush(QBrush(QColor::fromRgb(::profiler_gui::SELECTED_THREAD_BACKGROUND)));
        else
            _painter->setBrush(brushes[i & 1]);

        if (top < 0)
        {
            hgt += top;
            top = 0;
        }

        const auto dh = top + hgt - h;
        if (dh > 0)
            hgt -= dh;

        rect.setRect(0, top, w, hgt);

        _painter->setPen(Qt::NoPen);
        _painter->drawRect(rect);

        rect.translate(-5, 0);
        _painter->setPen(QColor::fromRgb(::profiler::colors::Dark));
        _painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, item->threadName());
    }

    const auto rect_bottom = rect.bottom();
    if (rect_bottom < h)
    {
        ++i;
        rect.translate(5, rect.height());
        rect.setHeight(h - rect_bottom);
        _painter->setBrush(brushes[i & 1]);
        _painter->setPen(Qt::NoPen);
        _painter->drawRect(rect);
    }

    // Draw separator between thread names area and information area
    _painter->setPen(Qt::darkGray);
    _painter->drawLine(QLineF(0, h, w, h));
    _painter->drawLine(QLineF(0, h + 2, w, h + 2));

    // Draw information
    _painter->setFont(CHRONOMETER_FONT);
    QFontMetricsF fm(CHRONOMETER_FONT, parentView);
    const qreal th = fm.height(); // Calculate displayed text height
    const qreal time1 = view->chronoTime();
    const qreal time2 = view->chronoTimeAux();

    auto y = h + 2;

    auto drawTimeText = [&rect, &w, &y, &fm, &_painter](qreal time, qreal th, QRgb color)
    {
        if (time > 0)
        {
            const QString text = ::profiler_gui::autoTimeStringReal(time); // Displayed text
            rect.setRect(0, y, w, th);

            _painter->setPen(color);
            _painter->drawText(rect, Qt::AlignCenter, text);

            y += th;
        }
    };

    drawTimeText(time1, th, ::profiler_gui::CHRONOMETER_COLOR.rgb() & 0x00ffffff);
    drawTimeText(time2, th, ::profiler_gui::CHRONOMETER_COLOR2.rgb() & 0x00ffffff);
}

//////////////////////////////////////////////////////////////////////////

EasyThreadNamesWidget::EasyThreadNamesWidget(EasyGraphicsView* _view, int _additionalHeight, QWidget* _parent)
    : Parent(_parent)
    , m_idleTime(0)
    , m_view(_view)
    , m_popupWidget(nullptr)
    , m_maxLength(100)
    , m_additionalHeight(_additionalHeight + 1)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);

    setScene(new QGraphicsScene(this));

    setCacheMode(QGraphicsView::CacheNone);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedWidth(m_maxLength);

    connect(&EASY_GLOBALS.events, &::profiler_gui::EasyGlobalSignals::selectedThreadChanged, [this](::profiler::thread_id_t){ repaintScene(); });
    connect(m_view, &EasyGraphicsView::treeChanged, this, &This::onTreeChange);
    connect(m_view, &EasyGraphicsView::sceneUpdated, this, &This::repaintScene);
    connect(m_view->verticalScrollBar(), &QScrollBar::valueChanged, verticalScrollBar(), &QScrollBar::setValue);
    connect(m_view->verticalScrollBar(), &QScrollBar::rangeChanged, this, &This::setVerticalScrollbarRange);
    connect(&m_idleTimer, &QTimer::timeout, this, &This::onIdleTimeout);
}

EasyThreadNamesWidget::~EasyThreadNamesWidget()
{

}

void EasyThreadNamesWidget::removePopup(bool _removeFromScene)
{
    if (m_popupWidget != nullptr)
    {
        auto widget = m_popupWidget->widget();
        widget->setParent(nullptr);
        m_popupWidget->setWidget(nullptr);
        delete widget;

        if (_removeFromScene)
            scene()->removeItem(m_popupWidget);

        m_popupWidget = nullptr;
    }
}

void EasyThreadNamesWidget::clear()
{
    const QSignalBlocker b(this);
    removePopup();
    scene()->clear();

    m_maxLength = 100;
    setFixedWidth(m_maxLength);

    m_idleTimer.stop();
    m_idleTime = 0;
}

void EasyThreadNamesWidget::setVerticalScrollbarRange(int _minValue, int _maxValue)
{
    verticalScrollBar()->setRange(_minValue, _maxValue + m_additionalHeight);
}

void EasyThreadNamesWidget::onTreeChange()
{
    const QSignalBlocker b(this);
    removePopup();
    scene()->clear();

    m_idleTimer.stop();
    m_idleTime = 0;

    QFontMetricsF fm(BG_FONT, this);
    qreal maxLength = 100;
    const auto& graphicsItems = m_view->getItems();
    for (auto graphicsItem : graphicsItems)
        maxLength = ::std::max(maxLength, (10 + fm.width(graphicsItem->threadName())) * ::profiler_gui::FONT_METRICS_FACTOR);

    auto vbar = verticalScrollBar();
    auto viewBar = m_view->verticalScrollBar();

    setVerticalScrollbarRange(viewBar->minimum(), viewBar->maximum());
    vbar->setSingleStep(viewBar->singleStep());
    vbar->setPageStep(viewBar->pageStep());

    auto r = m_view->sceneRect();
    setSceneRect(0, r.top(), maxLength, r.height() + m_additionalHeight);

    auto item = new EasyThreadNameItem();
    item->setPos(0, 0);
    item->setBoundingRect(sceneRect());
    scene()->addItem(item);

    m_maxLength = static_cast<int>(maxLength);
    setFixedWidth(m_maxLength);

    m_idleTimer.start(IDLE_TIMER_INTERVAL);
}

void EasyThreadNamesWidget::onIdleTimeout()
{
    static const uint16_t OVERLAP = ::profiler_gui::THREADS_ROW_SPACING >> 1;

    m_idleTime += IDLE_TIMER_INTERVAL;

    if (m_idleTime < IDLE_TIME)
    {
        removePopup(true);
        return;
    }

    if (m_popupWidget != nullptr)
        return;

    auto visibleSceneRect = mapToScene(rect()).boundingRect();
    auto scenePos = mapToScene(mapFromGlobal(QCursor::pos()));

    if (scenePos.x() < visibleSceneRect.left() || scenePos.x() > visibleSceneRect.right())
    {
        if (m_idleTime > 3000)
            setFixedWidth(m_maxLength);
        return;
    }

    if (scenePos.y() < visibleSceneRect.top() || scenePos.y() > visibleSceneRect.bottom())
    {
        if (m_idleTime > 3000)
            setFixedWidth(m_maxLength);
        return;
    }

    auto const parentView = static_cast<EasyThreadNamesWidget*>(scene()->parent());
    const auto view = parentView->view();

    if (scenePos.y() > view->visibleSceneRect().bottom())
    {
        if (m_idleTime > 3000)
            setFixedWidth(m_maxLength);
        return;
    }

    const qreal y = scenePos.y() - visibleSceneRect.top();

    const auto& items = view->getItems();
    if (items.empty())
    {
        if (m_idleTime > 3000)
            setFixedWidth(m_maxLength);
        return;
    }

    EasyGraphicsItem* intersectingItem = nullptr;
    for (auto item : items)
    {
        auto br = item->boundingRect();
        auto top = item->y() + br.top() - visibleSceneRect.top() - OVERLAP;
        auto hgt = br.height() + ::profiler_gui::THREADS_ROW_SPACING;
        auto bottom = top + hgt;

        if (bottom < y || y < top)
            continue;

        intersectingItem = item;

        break;
    }

    if (intersectingItem != nullptr)
    {
        auto widget = new QWidget(nullptr, Qt::FramelessWindowHint);
        if (widget == nullptr)
            return;

        widget->setAttribute(Qt::WA_ShowWithoutActivating, true);
        widget->setFocusPolicy(Qt::NoFocus);

        auto lay = new QGridLayout(widget);
        if (lay == nullptr)
            return;

        int row = 0;

        lay->addWidget(new EasyBoldLabel(intersectingItem->threadName(), widget), row, 0, 1, 2, Qt::AlignHCenter);
        ++row;

        ::profiler::timestamp_t duration = 0;
        const auto& root = *intersectingItem->root();
        if (!root.children.empty())
            duration = easyBlock(root.children.back()).tree.node->end() - easyBlock(root.children.front()).tree.node->begin();

        lay->addWidget(new QLabel("Duration:", widget), row, 0, Qt::AlignRight);
        lay->addWidget(new QLabel(::profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units, duration, 3), widget), row, 1, Qt::AlignLeft);
        ++row;

        lay->addWidget(new QLabel("Profiled:", widget), row, 0, Qt::AlignRight);
        if (duration)
        {
            lay->addWidget(new QLabel(QString("%1 (%2%)").arg(::profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units, root.profiled_time, 3))
                .arg(QString::number(100. * (double)root.profiled_time / (double)duration, 'f', 2)), widget), row, 1, Qt::AlignLeft);
        }
        else
        {
            lay->addWidget(new QLabel(::profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units, root.profiled_time, 3), widget), row, 1, Qt::AlignLeft);
        }
        ++row;

        lay->addWidget(new QLabel("Wait:", widget), row, 0, Qt::AlignRight);
        if (duration)
        {
            lay->addWidget(new QLabel(QString("%1 (%2%)").arg(::profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units, root.wait_time, 3))
                .arg(QString::number(100. * (double)root.wait_time / (double)duration, 'f', 2)), widget), row, 1, Qt::AlignLeft);
        }
        else
        {
            lay->addWidget(new QLabel(::profiler_gui::timeStringRealNs(EASY_GLOBALS.time_units, root.wait_time, 3), widget), row, 1, Qt::AlignLeft);
        }
        ++row;

        const auto eventsSize = root.events.size();

        lay->addWidget(new QLabel("Frames:", widget), row, 0, Qt::AlignRight);
        lay->addWidget(new QLabel(QString::number(root.frames_number), widget), row, 1, Qt::AlignLeft);
        ++row;

        lay->addWidget(new QLabel("Blocks:", widget), row, 0, Qt::AlignRight);
        lay->addWidget(new QLabel(QString::number(root.blocks_number - eventsSize), widget), row, 1, Qt::AlignLeft);
        ++row;

        lay->addWidget(new QLabel("Markers:", widget), row, 0, Qt::AlignRight);
        lay->addWidget(new QLabel(QString::number(eventsSize), widget), row, 1, Qt::AlignLeft);
        ++row;

        m_popupWidget = new QGraphicsProxyWidget();
        if (m_popupWidget != nullptr)
        {
            auto effect = new QGraphicsDropShadowEffect();
            effect->setBlurRadius(5);
            effect->setOffset(3, 3);
            m_popupWidget->setGraphicsEffect(effect);

            m_popupWidget->setWidget(widget);
            scene()->addItem(m_popupWidget);

            auto br = m_popupWidget->boundingRect();

            if (maximumWidth() < br.width())
            {
                setFixedWidth(static_cast<int>(br.width()));
                visibleSceneRect.setWidth(br.width());
            }

            if (scenePos.y() + br.height() > visibleSceneRect.bottom())
                scenePos.setY(::std::max(scenePos.y() - br.height(), visibleSceneRect.top()));

            if (scenePos.x() + br.width() > visibleSceneRect.right())
                scenePos.setX(::std::max(scenePos.x() - br.width(), visibleSceneRect.left()));

            m_popupWidget->setPos(scenePos);
            m_popupWidget->setOpacity(0.95);
        }
    }
}

void EasyThreadNamesWidget::repaintScene()
{
    scene()->update();
}

void EasyThreadNamesWidget::mousePressEvent(QMouseEvent* _event)
{
    m_idleTime = 0;

    QMouseEvent e(_event->type(), _event->pos() - QPointF(sceneRect().width(), 0), _event->button(), _event->buttons() & ~Qt::RightButton, _event->modifiers());
    m_view->mousePressEvent(&e);
    _event->accept();
}

void EasyThreadNamesWidget::mouseDoubleClickEvent(QMouseEvent* _event)
{
    static const auto OVERLAP = ::profiler_gui::THREADS_ROW_SPACING >> 1;

    m_idleTime = 0;

    auto y = mapToScene(_event->pos()).y();
    const auto& items = m_view->getItems();
    for (auto item : items)
    {
        auto br = item->boundingRect();
        auto top = item->y() + br.top() - OVERLAP;
        auto bottom = top + br.height() + OVERLAP;

        if (y < top || y > bottom)
            continue;

        const auto thread_id = item->threadId();
        if (thread_id != EASY_GLOBALS.selected_thread)
        {
            EASY_GLOBALS.selected_thread = thread_id;
            emit EASY_GLOBALS.events.selectedThreadChanged(thread_id);
        }

        break;
    }

    _event->accept();
}

void EasyThreadNamesWidget::mouseReleaseEvent(QMouseEvent* _event)
{
    m_idleTime = 0;

    QMouseEvent e(_event->type(), _event->pos() - QPointF(sceneRect().width(), 0), _event->button(), _event->buttons() & ~Qt::RightButton, _event->modifiers());
    m_view->mouseReleaseEvent(&e);
    _event->accept();
}

void EasyThreadNamesWidget::mouseMoveEvent(QMouseEvent* _event)
{
    m_idleTime = 0;

    QMouseEvent e(_event->type(), _event->pos() - QPointF(sceneRect().width(), 0), _event->button(), _event->buttons() & ~Qt::RightButton, _event->modifiers());
    m_view->mouseMoveEvent(&e);
    _event->accept();
}

void EasyThreadNamesWidget::keyPressEvent(QKeyEvent* _event)
{
    m_idleTime = 0;
    m_view->keyPressEvent(_event);
}

void EasyThreadNamesWidget::keyReleaseEvent(QKeyEvent* _event)
{
    m_idleTime = 0;
    m_view->keyReleaseEvent(_event);
}

void EasyThreadNamesWidget::wheelEvent(QWheelEvent* _event)
{
    m_idleTime = 0;

    auto vbar = m_view->verticalScrollBar();
    if (vbar != nullptr)
    {
        _event->accept();
        vbar->setValue(vbar->value() - _event->delta());
    }
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


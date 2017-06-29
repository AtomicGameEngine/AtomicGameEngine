/************************************************************************
* file name         : blocks_graphics_view.h
* ----------------- :
* creation time     : 2016/06/26
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains declaration of GraphicsScene and GraphicsView and
*                   : it's auxiliary classes for displyaing easy_profiler blocks tree.
* ----------------- :
* change log        : * 2016/06/26 Victor Zarubkin: moved sources from graphics_view.h
*                   :       and renamed classes from My* to Prof*.
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

#ifndef EASY_GRAPHICS_VIEW_H
#define EASY_GRAPHICS_VIEW_H

#include <stdlib.h>
#include <unordered_set>

#include <QGraphicsView>
#include <QGraphicsItem>
#include <QPoint>
#include <QRectF>
#include <QTimer>
#include <QLabel>

#include <easy/reader.h>

#include "common_types.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class QGraphicsProxyWidget;
class EasyGraphicsView;
class EasyGraphicsItem;
class EasyGraphicsScrollbar;
class EasyChronometerItem;

//////////////////////////////////////////////////////////////////////////

#define EASY_QGRAPHICSITEM(ClassName) \
class ClassName : public QGraphicsItem { \
    QRectF m_boundingRect; \
public: \
    ClassName() : QGraphicsItem() {} \
    virtual ~ClassName() {} \
    void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget = nullptr) override; \
    QRectF boundingRect() const override { return m_boundingRect; } \
    void setBoundingRect(qreal x, qreal y, qreal w, qreal h) { m_boundingRect.setRect(x, y, w, h); } \
    void setBoundingRect(const QRectF& _rect) { m_boundingRect = _rect; } \
}

EASY_QGRAPHICSITEM(EasyBackgroundItem);
EASY_QGRAPHICSITEM(EasyTimelineIndicatorItem);
EASY_QGRAPHICSITEM(EasyThreadNameItem);

#undef EASY_QGRAPHICSITEM

//////////////////////////////////////////////////////////////////////////

struct EasyBoldLabel : public QLabel {
    EasyBoldLabel(const QString& _text, QWidget* _parent = nullptr);
    virtual ~EasyBoldLabel();
};

//////////////////////////////////////////////////////////////////////////

class EasyGraphicsView : public QGraphicsView
{
    Q_OBJECT

private:

    typedef QGraphicsView Parent;
    typedef EasyGraphicsView This;
    typedef ::std::vector<EasyGraphicsItem*> Items;
    //typedef ::std::unordered_set<int, ::profiler::passthrough_hash<int> > Keys;

    Items                               m_items; ///< Array of all EasyGraphicsItem items
    //Keys                                 m_keys; ///< Pressed keyboard keys
    ::profiler_gui::TreeBlocks m_selectedBlocks; ///< Array of items which were selected by selection zone (EasyChronometerItem)
    QTimer                       m_flickerTimer; ///< Timer for flicking behavior
    QTimer                          m_idleTimer; ///< 
    QRectF                   m_visibleSceneRect; ///< Visible scene rectangle
    ::profiler::timestamp_t         m_beginTime; ///< Begin time of profiler session. Used to reduce values of all begin and end times of profiler blocks.
    qreal                          m_sceneWidth; ///< 
    qreal                               m_scale; ///< Current scale
    qreal                              m_offset; ///< Have to use manual offset for all scene content instead of using scrollbars because QScrollBar::value is 32-bit integer :(
    qreal                        m_timelineStep; ///< 
    uint64_t                         m_idleTime; ///< 
    QPoint                      m_mousePressPos; ///< Last mouse global position (used by mousePressEvent and mouseMoveEvent)
    QPoint                      m_mouseMovePath; ///< Mouse move path between press and release of any button
    Qt::MouseButtons             m_mouseButtons; ///< Pressed mouse buttons
    EasyGraphicsScrollbar*         m_pScrollbar; ///< Pointer to the graphics scrollbar widget
    EasyChronometerItem*      m_chronometerItem; ///< Pointer to the EasyChronometerItem which is displayed when you press right mouse button and move mouse left or right. This item is used to select blocks to display in tree widget.
    EasyChronometerItem*   m_chronometerItemAux; ///< Pointer to the EasyChronometerItem which is displayed when you double click left mouse button and move mouse left or right. This item is used only to measure time.
    QGraphicsProxyWidget*         m_popupWidget; ///< 
    int                         m_flickerSpeedX; ///< Current flicking speed x
    int                         m_flickerSpeedY; ///< Current flicking speed y
    int                       m_flickerCounterX;
    int                       m_flickerCounterY;
    bool                         m_bDoubleClick; ///< Is mouse buttons double clicked
    bool                        m_bUpdatingRect; ///< Stub flag which is used to avoid excess calculations on some scene update (flicking, scaling and so on)
    bool                               m_bEmpty; ///< Indicates whether scene is empty and has no items

public:

    explicit EasyGraphicsView(QWidget* _parent = nullptr);
    virtual ~EasyGraphicsView();

    // Public virtual methods

    void wheelEvent(QWheelEvent* _event) override;
    void mousePressEvent(QMouseEvent* _event) override;
    void mouseDoubleClickEvent(QMouseEvent* _event) override;
    void mouseReleaseEvent(QMouseEvent* _event) override;
    void mouseMoveEvent(QMouseEvent* _event) override;
    void keyPressEvent(QKeyEvent* _event) override;
    void keyReleaseEvent(QKeyEvent* _event) override;
    void resizeEvent(QResizeEvent* _event) override;

    void dragEnterEvent(QDragEnterEvent*) override {}

public:

    // Public non-virtual methods

    qreal sceneWidth() const;
    qreal chronoTime() const;
    qreal chronoTimeAux() const;

    void setScrollbar(EasyGraphicsScrollbar* _scrollbar);
    void clear();

    void setTree(const ::profiler::thread_blocks_tree_t& _blocksTree);

    const Items& getItems() const;

signals:

    // Signals

    void sceneUpdated();
    void treeChanged();
    void intervalChanged(const ::profiler_gui::TreeBlocks& _blocks, ::profiler::timestamp_t _session_begin_time, ::profiler::timestamp_t _left, ::profiler::timestamp_t _right, bool _strict);

private:

    // Private non-virtual methods

    void removePopup(bool _removeFromScene = false);

    EasyChronometerItem* createChronometer(bool _main = true);
    bool moveChrono(EasyChronometerItem* _chronometerItem, qreal _mouseX);
    void initMode();
    int updateVisibleSceneRect();
    void updateTimelineStep(qreal _windowWidth);
    void scaleTo(qreal _scale);
    void scrollTo(const EasyGraphicsItem* _item);
    void onWheel(qreal _mouseX, int _wheelDelta);
    qreal setTree(EasyGraphicsItem* _item, const ::profiler::BlocksTree::children_t& _children, qreal& _height, uint32_t& _maxDepthChild, qreal _y, short _level);

private slots:

    // Private Slots

    void repaintScene();
    void onGraphicsScrollbarWheel(qreal _mouseX, int _wheelDelta);
    void onScrollbarValueChange(int);
    void onGraphicsScrollbarValueChange(qreal);
    void onFlickerTimeout();
    void onIdleTimeout();
    void onHierarchyFlagChange(bool _value);
    void onSelectedThreadChange(::profiler::thread_id_t _id);
    void onSelectedBlockChange(unsigned int _block_index);
    void onRefreshRequired();
    void onThreadViewChanged();

public:

    // Public inline methods

    inline qreal scale() const
    {
        return m_scale;
    }

    inline qreal offset() const
    {
        return m_offset;
    }

    inline const QRectF& visibleSceneRect() const
    {
        return m_visibleSceneRect;
    }

    inline qreal timelineStep() const
    {
        return m_timelineStep;
    }

    inline qreal time2position(const profiler::timestamp_t& _time) const
    {
        return PROF_MICROSECONDS(qreal(_time - m_beginTime));
        //return PROF_MILLISECONDS(qreal(_time - m_beginTime));
    }

    inline ::profiler::timestamp_t position2time(qreal _pos) const
    {
        return PROF_FROM_MICROSECONDS(_pos);
        //return PROF_FROM_MILLISECONDS(_pos);
    }

}; // END of class EasyGraphicsView.

//////////////////////////////////////////////////////////////////////////

class EasyThreadNamesWidget : public QGraphicsView
{
    Q_OBJECT

private:

    typedef QGraphicsView Parent;
    typedef EasyThreadNamesWidget This;

    QTimer                  m_idleTimer; ///< 
    uint64_t                 m_idleTime; ///< 
    EasyGraphicsView*            m_view; ///< 
    QGraphicsProxyWidget* m_popupWidget; ///< 
    int                     m_maxLength; ///< 
    const int        m_additionalHeight; ///< 

public:

    explicit EasyThreadNamesWidget(EasyGraphicsView* _view, int _additionalHeight, QWidget* _parent = nullptr);
    virtual ~EasyThreadNamesWidget();

    void mousePressEvent(QMouseEvent* _event) override;
    void mouseDoubleClickEvent(QMouseEvent* _event) override;
    void mouseReleaseEvent(QMouseEvent* _event) override;
    void mouseMoveEvent(QMouseEvent* _event) override;
    void keyPressEvent(QKeyEvent* _event) override;
    void keyReleaseEvent(QKeyEvent* _event) override;
    void wheelEvent(QWheelEvent* _event) override;

    void dragEnterEvent(QDragEnterEvent*) override {}

    void clear();

    const EasyGraphicsView* view() const
    {
        return m_view;
    }

private:

    void removePopup(bool _removeFromScene = false);

private slots:

    void setVerticalScrollbarRange(int _minValue, int _maxValue);
    void onTreeChange();
    void onIdleTimeout();
    void repaintScene();

}; // END of class EasyThreadNamesWidget.

//////////////////////////////////////////////////////////////////////////

class EasyGraphicsViewWidget : public QWidget
{
    Q_OBJECT

private:

    EasyGraphicsScrollbar*         m_scrollbar;
    EasyGraphicsView*                   m_view;
    EasyThreadNamesWidget* m_threadNamesWidget;

public:

    explicit EasyGraphicsViewWidget(QWidget* _parent = nullptr);
    virtual ~EasyGraphicsViewWidget();

    EasyGraphicsView* view();
    void clear();

private:

    void initWidget();

}; // END of class EasyGraphicsViewWidget.

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#endif // EASY_GRAPHICS_VIEW_H

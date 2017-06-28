/************************************************************************
* file name         : main_window.cpp
* ----------------- :
* creation time     : 2016/06/26
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains implementation of MainWindow for easy_profiler GUI.
* ----------------- :
* change log        : * 2016/06/26 Victor Zarubkin: Initial commit.
*                   :
*                   : * 2016/06/27 Victor Zarubkin: Passing blocks number to EasyTreeWidget::setTree().
*                   :
*                   : * 2016/06/29 Victor Zarubkin: Added menu with tests.
*                   :
*                   : * 2016/06/30 Sergey Yagovtsev: Open file by command line argument
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

#include <chrono>
#include <fstream>

#include <QApplication>
#include <QCoreApplication>
#include <QStatusBar>
#include <QDockWidget>
#include <QFileDialog>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QCloseEvent>
#include <QSettings>
#include <QTextCodec>
#include <QFont>
#include <QProgressDialog>
#include <QSignalBlocker>
#include <QDebug>
#include <QToolBar>
#include <QToolButton>
#include <QWidgetAction>
#include <QSpinBox>
#include <QMessageBox>
#include <QLineEdit>
#include <QLabel>
#include <QDialog>
#include <QVBoxLayout>
#include <QFile>
#include <QFileInfo>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QDateTime>

#include "main_window.h"
#include "blocks_tree_widget.h"
#include "blocks_graphics_view.h"
#include "descriptors_tree_widget.h"
#include "easy_frame_rate_viewer.h"
#include "globals.h"

#include <easy/easy_net.h>

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

//////////////////////////////////////////////////////////////////////////

#define EASY_DEFAULT_WINDOW_TITLE "EasyProfiler"

const int LOADER_TIMER_INTERVAL = 40;
const auto NETWORK_CACHE_FILE = "easy_profiler_stream.cache";

//////////////////////////////////////////////////////////////////////////

inline void clear_stream(std::stringstream& _stream)
{
#if defined(__GNUC__) && __GNUC__ < 5
    // gcc 4 has a known bug which has been solved in gcc 5:
    // std::stringstream has no swap() method :(
    _stream.str(std::string());
#else
    std::stringstream().swap(_stream);
#endif
}

//////////////////////////////////////////////////////////////////////////

EasyMainWindow::EasyMainWindow() : Parent(), m_lastAddress("localhost"), m_lastPort(::profiler::DEFAULT_PORT)
{
    { QIcon icon(":/logo"); if (!icon.isNull()) QApplication::setWindowIcon(icon); }

    setObjectName("ProfilerGUI_MainWindow");
    setWindowTitle(EASY_DEFAULT_WINDOW_TITLE);
    setDockNestingEnabled(true);
    setAcceptDrops(true);
    resize(800, 600);
    
    setStatusBar(nullptr);

    m_graphicsView = new QDockWidget("Diagram", this);
    m_graphicsView->setObjectName("ProfilerGUI_Diagram");
    m_graphicsView->setMinimumHeight(50);
    m_graphicsView->setAllowedAreas(Qt::AllDockWidgetAreas);

    auto graphicsView = new EasyGraphicsViewWidget(this);
    m_graphicsView->setWidget(graphicsView);

    m_treeWidget = new QDockWidget("Hierarchy", this);
    m_treeWidget->setObjectName("ProfilerGUI_Hierarchy");
    m_treeWidget->setMinimumHeight(50);
    m_treeWidget->setAllowedAreas(Qt::AllDockWidgetAreas);

    auto treeWidget = new EasyHierarchyWidget(this);
    m_treeWidget->setWidget(treeWidget);

    m_fpsViewer = new QDockWidget("FPS Monitor", this);
    m_fpsViewer->setObjectName("ProfilerGUI_FPS");
    m_fpsViewer->setWidget(new EasyFrameRateViewer(this));
    m_fpsViewer->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

    addDockWidget(Qt::TopDockWidgetArea, m_graphicsView);
    addDockWidget(Qt::BottomDockWidgetArea, m_treeWidget);
    addDockWidget(Qt::TopDockWidgetArea, m_fpsViewer);

#if EASY_GUI_USE_DESCRIPTORS_DOCK_WINDOW != 0
    auto descTree = new EasyDescWidget();
    m_descTreeWidget = new QDockWidget("Blocks");
    m_descTreeWidget->setObjectName("ProfilerGUI_Blocks");
    m_descTreeWidget->setMinimumHeight(50);
    m_descTreeWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_descTreeWidget->setWidget(descTree);
    addDockWidget(Qt::BottomDockWidgetArea, m_descTreeWidget);
#endif


    loadSettings();


    auto toolbar = addToolBar("FileToolbar");
    toolbar->setIconSize(::profiler_gui::ICONS_SIZE);
    toolbar->setObjectName("ProfilerGUI_FileToolbar");
    toolbar->setContentsMargins(1, 0, 1, 0);

    m_loadActionMenu = new QMenu(this);
    auto action = m_loadActionMenu->menuAction();
    action->setText("Open file");
    action->setIcon(QIcon(":/Open"));
    connect(action, &QAction::triggered, this, &This::onOpenFileClicked);
    toolbar->addAction(action);

    for (const auto& f : m_lastFiles)
    {
        action = new QAction(f, this);
        connect(action, &QAction::triggered, this, &This::onOpenFileClicked);
        m_loadActionMenu->addAction(action);
    }

    m_saveAction = toolbar->addAction(QIcon(":/Save"), tr("Save"), this, SLOT(onSaveFileClicked(bool)));
    m_deleteAction = toolbar->addAction(QIcon(":/Delete"), tr("Clear all"), this, SLOT(onDeleteClicked(bool)));

    m_saveAction->setEnabled(false);
    m_deleteAction->setEnabled(false);



    toolbar = addToolBar("ProfileToolbar");
    toolbar->setIconSize(::profiler_gui::ICONS_SIZE);
    toolbar->setObjectName("ProfilerGUI_ProfileToolbar");
    toolbar->setContentsMargins(1, 0, 1, 0);

    toolbar->addAction(QIcon(":/List"), tr("Blocks"), this, SLOT(onEditBlocksClicked(bool)));
    m_captureAction = toolbar->addAction(QIcon(":/Start"), tr("Capture"), this, SLOT(onCaptureClicked(bool)));
    m_captureAction->setEnabled(false);

    toolbar->addSeparator();
    m_connectAction = toolbar->addAction(QIcon(":/Connection"), tr("Connect"), this, SLOT(onConnectClicked(bool)));

    auto lbl = new QLabel("Address:", toolbar);
    lbl->setContentsMargins(5, 0, 2, 0);
    toolbar->addWidget(lbl);
    m_addressEdit = new QLineEdit();
    m_addressEdit->setToolTip("Enter IP-address or host name");
    //QRegExp rx("^0*(2(5[0-5]|[0-4]\\d)|1?\\d{1,2})(\\.0*(2(5[0-5]|[0-4]\\d)|1?\\d{1,2})){3}$");
    //m_addressEdit->setValidator(new QRegExpValidator(rx, m_addressEdit));
    m_addressEdit->setText(m_lastAddress);
    m_addressEdit->setFixedWidth((m_addressEdit->fontMetrics().width(QString("255.255.255.255")) * 3) / 2);
    toolbar->addWidget(m_addressEdit);

    lbl = new QLabel("Port:", toolbar);
    lbl->setContentsMargins(5, 0, 2, 0);
    toolbar->addWidget(lbl);
    m_portEdit = new QLineEdit();
    m_portEdit->setValidator(new QIntValidator(1, 65535, m_portEdit));
    m_portEdit->setText(QString::number(m_lastPort));
    m_portEdit->setFixedWidth(m_portEdit->fontMetrics().width(QString("000000")) + 10);
    toolbar->addWidget(m_portEdit);

    connect(m_addressEdit, &QLineEdit::returnPressed, [this](){ onConnectClicked(true); });
    connect(m_portEdit, &QLineEdit::returnPressed, [this](){ onConnectClicked(true); });



    toolbar = addToolBar("SetupToolbar");
    toolbar->setIconSize(::profiler_gui::ICONS_SIZE);
    toolbar->setObjectName("ProfilerGUI_SetupToolbar");
    toolbar->setContentsMargins(1, 0, 1, 0);

    toolbar->addAction(QIcon(":/Expand"), "Expand all", this, SLOT(onExpandAllClicked(bool)));
    toolbar->addAction(QIcon(":/Collapse"), "Collapse all", this, SLOT(onCollapseAllClicked(bool)));

    toolbar->addSeparator();
    auto menu = new QMenu("Settings", this);
    menu->setToolTipsVisible(true);

    QToolButton* toolButton = new QToolButton(toolbar);
    toolButton->setIcon(QIcon(":/Settings"));
    toolButton->setMenu(menu);
    toolButton->setPopupMode(QToolButton::InstantPopup);
    toolbar->addWidget(toolButton);

    action = menu->addAction("Statistics enabled");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.enable_statistics);
    connect(action, &QAction::triggered, this, &This::onEnableDisableStatistics);
    if (EASY_GLOBALS.enable_statistics)
    {
        auto f = action->font();
        f.setBold(true);
        action->setFont(f);
        SET_ICON(action, ":/Stats");
    }
    else
    {
        action->setText("Statistics disabled");
        SET_ICON(action, ":/Stats-off");
    }


    action = menu->addAction("Only frames on histogram");
    action->setToolTip("Display only top-level blocks on histogram.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.display_only_frames_on_histogram);
    connect(action, &QAction::triggered, [this](bool _checked)
    {
        EASY_GLOBALS.display_only_frames_on_histogram = _checked;
        emit EASY_GLOBALS.events.displayOnlyFramesOnHistogramChanged();
    });


    menu->addSeparator();
    auto submenu = menu->addMenu("View");
    submenu->setToolTipsVisible(true);
    action = submenu->addAction("Draw items' borders");
    action->setToolTip("Draw borders for blocks on diagram.\nThis reduces performance.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.draw_graphics_items_borders);
    connect(action, &QAction::triggered, [this](bool _checked){ EASY_GLOBALS.draw_graphics_items_borders = _checked; refreshDiagram(); });

    action = submenu->addAction("Overlap narrow children");
    action->setToolTip("Children blocks will be overlaped by narrow\nparent blocks. See also \'Blocks narrow size\'.\nThis improves performance.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.hide_narrow_children);
    connect(action, &QAction::triggered, [this](bool _checked){ EASY_GLOBALS.hide_narrow_children = _checked; refreshDiagram(); });

    action = submenu->addAction("Hide min-size blocks");
    action->setToolTip("Hides blocks which screen size\nis less than \'Min blocks size\'.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.hide_minsize_blocks);
    connect(action, &QAction::triggered, [this](bool _checked){ EASY_GLOBALS.hide_minsize_blocks = _checked; refreshDiagram(); });

    action = submenu->addAction("Build hierarchy only for current thread");
    action->setToolTip("Hierarchy tree will be built\nfor blocks from current thread only.\nThis improves performance\nand saves a lot of memory.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.only_current_thread_hierarchy);
    connect(action, &QAction::triggered, this, &This::onHierarchyFlagChange);

    action = submenu->addAction("Add zero blocks to hierarchy");
    action->setToolTip("Zero duration blocks will be added into hierarchy tree.\nThis reduces performance and increases memory consumption.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.add_zero_blocks_to_hierarchy);
    connect(action, &QAction::triggered, [this](bool _checked)
    {
        EASY_GLOBALS.add_zero_blocks_to_hierarchy = _checked;
        emit EASY_GLOBALS.events.hierarchyFlagChanged(_checked);
    });

    action = submenu->addAction("Enable zero duration blocks on diagram");
    action->setToolTip("If checked then allows diagram to paint zero duration blocks\nwith 1px width on each scale. Otherwise, such blocks will be resized\nto 250ns duration.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.enable_zero_length);
    connect(action, &QAction::triggered, [this](bool _checked){ EASY_GLOBALS.enable_zero_length = _checked; refreshDiagram(); });

    action = submenu->addAction("Highlight similar blocks");
    action->setToolTip("Highlight all visible blocks which are similar\nto the current selected block.\nThis reduces performance.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.highlight_blocks_with_same_id);
    connect(action, &QAction::triggered, [this](bool _checked){ EASY_GLOBALS.highlight_blocks_with_same_id = _checked; refreshDiagram(); });

    action = submenu->addAction("Collapse blocks on tree reset");
    action->setToolTip("This collapses all blocks on diagram\nafter hierarchy tree reset.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.collapse_items_on_tree_close);
    connect(action, &QAction::triggered, this, &This::onCollapseItemsAfterCloseChanged);

    action = submenu->addAction("Expand all on file open");
    action->setToolTip("If checked then all blocks on diagram\nwill be initially expanded.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.all_items_expanded_by_default);
    connect(action, &QAction::triggered, this, &This::onAllItemsExpandedByDefaultChange);

    action = submenu->addAction("Bind diagram and tree expand");
    action->setToolTip("Expanding/collapsing blocks at diagram expands/collapses\nblocks at hierarchy tree and wise versa.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.bind_scene_and_tree_expand_status);
    connect(action, &QAction::triggered, this, &This::onBindExpandStatusChange);

    action = submenu->addAction("Selecting block changes current thread");
    action->setToolTip("Automatically select thread while selecting a block.\nIf not checked then you will have to select current thread\nmanually double clicking on thread name on a diagram.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.selecting_block_changes_thread);
    connect(action, &QAction::triggered, [this](bool _checked){ EASY_GLOBALS.selecting_block_changes_thread = _checked; });

    action = submenu->addAction("Draw event markers");
    action->setToolTip("Display event markers under the blocks\n(even if event-blocks are not visible).\nThis slightly reduces performance.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.enable_event_markers);
    connect(action, &QAction::triggered, [this](bool _checked)
    {
        EASY_GLOBALS.enable_event_markers = _checked;
        refreshDiagram();
    });

    action = submenu->addAction("Automatically adjust histogram height");
    action->setToolTip("You do not need to adjust boundaries manually,\nbut this restricts you from adjusting boundaries at all (zoom mode).\nYou can still adjust boundaries in overview mode though.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.auto_adjust_histogram_height);
    connect(action, &QAction::triggered, [](bool _checked)
    {
        EASY_GLOBALS.auto_adjust_histogram_height = _checked;
        emit EASY_GLOBALS.events.autoAdjustHistogramChanged();
    });

    action = submenu->addAction("Use decorated thread names");
    action->setToolTip("Add \'Thread\' word into thread name if there is no one already.\nExamples: \'Render\' will change to \'Render Thread\'\n\'WorkerThread\' will not change.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.use_decorated_thread_name);
    connect(action, &QAction::triggered, [this](bool _checked)
    {
        EASY_GLOBALS.use_decorated_thread_name = _checked;
        emit EASY_GLOBALS.events.threadNameDecorationChanged();
    });

    action = submenu->addAction("Display hex thread id");
    action->setToolTip("Display hex thread id instead of decimal.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.hex_thread_id);
    connect(action, &QAction::triggered, [this](bool _checked)
    {
        EASY_GLOBALS.hex_thread_id = _checked;
        emit EASY_GLOBALS.events.hexThreadIdChanged();
    });

    submenu->addSeparator();
    auto actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    action = new QAction("Chrono text at top", actionGroup);
    action->setToolTip("Draw duration of selected interval\nat the top of the screen.");
    action->setCheckable(true);
    action->setData(static_cast<int>(::profiler_gui::ChronoTextPosition_Top));
    if (EASY_GLOBALS.chrono_text_position == ::profiler_gui::ChronoTextPosition_Top)
        action->setChecked(true);
    submenu->addAction(action);
    connect(action, &QAction::triggered, this, &This::onChronoTextPosChanged);

    action = new QAction("Chrono text at center", actionGroup);
    action->setToolTip("Draw duration of selected interval\nat the center of the screen.");
    action->setCheckable(true);
    action->setData(static_cast<int>(::profiler_gui::ChronoTextPosition_Center));
    if (EASY_GLOBALS.chrono_text_position == ::profiler_gui::ChronoTextPosition_Center)
        action->setChecked(true);
    submenu->addAction(action);
    connect(action, &QAction::triggered, this, &This::onChronoTextPosChanged);

    action = new QAction("Chrono text at bottom", actionGroup);
    action->setToolTip("Draw duration of selected interval\nat the bottom of the screen.");
    action->setCheckable(true);
    action->setData(static_cast<int>(::profiler_gui::ChronoTextPosition_Bottom));
    if (EASY_GLOBALS.chrono_text_position == ::profiler_gui::ChronoTextPosition_Bottom)
        action->setChecked(true);
    submenu->addAction(action);
    connect(action, &QAction::triggered, this, &This::onChronoTextPosChanged);

    submenu->addSeparator();
    auto w = new QWidget(submenu);
    auto l = new QHBoxLayout(w);
    l->setContentsMargins(33, 1, 1, 1);
    l->addWidget(new QLabel("Min blocks spacing, px", w), 0, Qt::AlignLeft);
    auto spinbox = new QSpinBox(w);
    spinbox->setRange(0, 400);
    spinbox->setValue(EASY_GLOBALS.blocks_spacing);
    spinbox->setFixedWidth(50);
    connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(onSpacingChange(int)));
    l->addWidget(spinbox);
    w->setLayout(l);
    auto waction = new QWidgetAction(submenu);
    waction->setDefaultWidget(w);
    submenu->addAction(waction);

    w = new QWidget(submenu);
    l = new QHBoxLayout(w);
    l->setContentsMargins(33, 1, 1, 1);
    l->addWidget(new QLabel("Min blocks size, px", w), 0, Qt::AlignLeft);
    spinbox = new QSpinBox(w);
    spinbox->setRange(1, 400);
    spinbox->setValue(EASY_GLOBALS.blocks_size_min);
    spinbox->setFixedWidth(50);
    connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(onMinSizeChange(int)));
    l->addWidget(spinbox);
    w->setLayout(l);
    waction = new QWidgetAction(submenu);
    waction->setDefaultWidget(w);
    submenu->addAction(waction);

    w = new QWidget(submenu);
    l = new QHBoxLayout(w);
    l->setContentsMargins(33, 1, 1, 1);
    l->addWidget(new QLabel("Blocks narrow size, px", w), 0, Qt::AlignLeft);
    spinbox = new QSpinBox(w);
    spinbox->setRange(1, 400);
    spinbox->setValue(EASY_GLOBALS.blocks_narrow_size);
    spinbox->setFixedWidth(50);
    connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(onNarrowSizeChange(int)));
    l->addWidget(spinbox);
    w->setLayout(l);
    waction = new QWidgetAction(submenu);
    waction->setDefaultWidget(w);
    submenu->addAction(waction);




    submenu = menu->addMenu("FPS Monitor");
    w = new QWidget(submenu);
    l = new QHBoxLayout(w);
    l->setContentsMargins(33, 1, 1, 1);
    l->addWidget(new QLabel("Request interval, ms", w), 0, Qt::AlignLeft);
    spinbox = new QSpinBox(w);
    spinbox->setRange(1, 600000);
    spinbox->setValue(EASY_GLOBALS.fps_timer_interval);
    spinbox->setFixedWidth(50);
    connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(onFpsIntervalChange(int)));
    l->addWidget(spinbox);
    w->setLayout(l);
    waction = new QWidgetAction(submenu);
    waction->setDefaultWidget(w);
    submenu->addAction(waction);

    w = new QWidget(submenu);
    l = new QHBoxLayout(w);
    l->setContentsMargins(33, 1, 1, 1);
    l->addWidget(new QLabel("Max history size", w), 0, Qt::AlignLeft);
    spinbox = new QSpinBox(w);
    spinbox->setRange(2, 200);
    spinbox->setValue(EASY_GLOBALS.max_fps_history);
    spinbox->setFixedWidth(50);
    connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(onFpsHistoryChange(int)));
    l->addWidget(spinbox);
    w->setLayout(l);
    waction = new QWidgetAction(submenu);
    waction->setDefaultWidget(w);
    submenu->addAction(waction);

    w = new QWidget(submenu);
    l = new QHBoxLayout(w);
    l->setContentsMargins(33, 1, 1, 1);
    l->addWidget(new QLabel("Line width, px", w), 0, Qt::AlignLeft);
    spinbox = new QSpinBox(w);
    spinbox->setRange(1, 6);
    spinbox->setValue(EASY_GLOBALS.fps_widget_line_width);
    spinbox->setFixedWidth(50);
    connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(onFpsMonitorLineWidthChange(int)));
    l->addWidget(spinbox);
    w->setLayout(l);
    waction = new QWidgetAction(submenu);
    waction->setDefaultWidget(w);
    submenu->addAction(waction);




    submenu = menu->addMenu("Units");
    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);
    action = new QAction("Auto", actionGroup);
    action->setCheckable(true);
    action->setData(static_cast<int>(::profiler_gui::TimeUnits_auto));
    if (EASY_GLOBALS.time_units == ::profiler_gui::TimeUnits_auto)
        action->setChecked(true);
    submenu->addAction(action);
    connect(action, &QAction::triggered, this, &This::onUnitsChanged);

    action = new QAction("Milliseconds", actionGroup);
    action->setCheckable(true);
    action->setData(static_cast<int>(::profiler_gui::TimeUnits_ms));
    if (EASY_GLOBALS.time_units == ::profiler_gui::TimeUnits_ms)
        action->setChecked(true);
    submenu->addAction(action);
    connect(action, &QAction::triggered, this, &This::onUnitsChanged);

    action = new QAction("Microseconds", actionGroup);
    action->setCheckable(true);
    action->setData(static_cast<int>(::profiler_gui::TimeUnits_us));
    if (EASY_GLOBALS.time_units == ::profiler_gui::TimeUnits_us)
        action->setChecked(true);
    submenu->addAction(action);
    connect(action, &QAction::triggered, this, &This::onUnitsChanged);

    action = new QAction("Nanoseconds", actionGroup);
    action->setCheckable(true);
    action->setData(static_cast<int>(::profiler_gui::TimeUnits_ns));
    if (EASY_GLOBALS.time_units == ::profiler_gui::TimeUnits_ns)
        action->setChecked(true);
    submenu->addAction(action);
    connect(action, &QAction::triggered, this, &This::onUnitsChanged);


    submenu = menu->addMenu("Remote");
    m_eventTracingEnableAction = submenu->addAction("Event tracing enabled");
    m_eventTracingEnableAction->setCheckable(true);
    m_eventTracingEnableAction->setEnabled(false);
    connect(m_eventTracingEnableAction, &QAction::triggered, this, &This::onEventTracingEnableChange);

    m_eventTracingPriorityAction = submenu->addAction("Low priority event tracing");
    m_eventTracingPriorityAction->setCheckable(true);
    m_eventTracingPriorityAction->setChecked(EASY_OPTION_LOW_PRIORITY_EVENT_TRACING);
    m_eventTracingPriorityAction->setEnabled(false);
    connect(m_eventTracingPriorityAction, &QAction::triggered, this, &This::onEventTracingPriorityChange);


    submenu = menu->addMenu("Encoding");
    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    auto default_codec_mib = QTextCodec::codecForLocale()->mibEnum();
    foreach(int mib, QTextCodec::availableMibs())
    {
        auto codec = QTextCodec::codecForMib(mib)->name();

        action = new QAction(codec, actionGroup);
        action->setCheckable(true);
        if (mib == default_codec_mib)
            action->setChecked(true);

        submenu->addAction(action);
        connect(action, &QAction::triggered, this, &This::onEncodingChanged);
    }

    auto tb_height = toolbar->height() + 4;
    toolbar = addToolBar("FrameToolbar");
    toolbar->setIconSize(::profiler_gui::ICONS_SIZE);
    toolbar->setObjectName("ProfilerGUI_FrameToolbar");
    toolbar->setContentsMargins(1, 0, 1, 0);
    toolbar->setMinimumHeight(tb_height);

    lbl = new QLabel("Expected frame time:", toolbar);
    lbl->setContentsMargins(5, 2, 2, 2);
    toolbar->addWidget(lbl);

    m_frameTimeEdit = new QLineEdit();
    m_frameTimeEdit->setFixedWidth(70);
    auto val = new QDoubleValidator(m_frameTimeEdit);
    val->setLocale(QLocale::c());
    val->setBottom(0);
    m_frameTimeEdit->setValidator(val);
    m_frameTimeEdit->setText(QString::number(EASY_GLOBALS.frame_time * 1e-3));
    connect(m_frameTimeEdit, &QLineEdit::editingFinished, this, &This::onFrameTimeEditFinish);
    connect(&EASY_GLOBALS.events, &::profiler_gui::EasyGlobalSignals::expectedFrameTimeChanged, this, &This::onFrameTimeChanged);
    toolbar->addWidget(m_frameTimeEdit);

    lbl = new QLabel("ms", toolbar);
    lbl->setContentsMargins(5, 2, 1, 1);
    toolbar->addWidget(lbl);


    connect(graphicsView->view(), &EasyGraphicsView::intervalChanged, treeWidget->tree(), &EasyTreeWidget::setTreeBlocks);
    connect(&m_readerTimer, &QTimer::timeout, this, &This::onFileReaderTimeout);
    connect(&m_listenerTimer, &QTimer::timeout, this, &This::onListenerTimerTimeout);
    connect(&m_fpsRequestTimer, &QTimer::timeout, this, &This::onFrameTimeRequestTimeout);
    

    m_progress = new QProgressDialog("Loading file...", "Cancel", 0, 100, this);
    m_progress->setFixedWidth(300);
    m_progress->setWindowTitle(EASY_DEFAULT_WINDOW_TITLE);
    m_progress->setModal(true);
    m_progress->setValue(100);
    //m_progress->hide();
    connect(m_progress, &QProgressDialog::canceled, this, &This::onFileReaderCancel);

    loadGeometry();

    if(QCoreApplication::arguments().size() > 1)
    {
        auto opened_filename = QCoreApplication::arguments().at(1);
        loadFile(opened_filename);
    }

    connect(&EASY_GLOBALS.events, &::profiler_gui::EasyGlobalSignals::blockStatusChanged, this, &This::onBlockStatusChange);
    connect(&EASY_GLOBALS.events, &::profiler_gui::EasyGlobalSignals::blocksRefreshRequired, this, &This::onGetBlockDescriptionsClicked);
}

EasyMainWindow::~EasyMainWindow()
{
    delete m_progress;
}

//////////////////////////////////////////////////////////////////////////

void EasyMainWindow::dragEnterEvent(QDragEnterEvent* drag_event)
{
    if (drag_event->mimeData()->hasUrls())
        drag_event->acceptProposedAction();
}

void EasyMainWindow::dragMoveEvent(QDragMoveEvent* drag_event)
{
    if (drag_event->mimeData()->hasUrls())
        drag_event->acceptProposedAction();
}

void EasyMainWindow::dragLeaveEvent(QDragLeaveEvent* drag_event)
{
    drag_event->accept();
}

void EasyMainWindow::dropEvent(QDropEvent* drop_event)
{
    const auto& urls = drop_event->mimeData()->urls();
    if (!urls.empty())
    {
        if (m_bNetworkFileRegime)
        {
            // Warn user about unsaved network information and suggest to save
            auto result = QMessageBox::question(this, "Unsaved session", "You have unsaved data!\nSave before opening new file?", QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
            if (result == QMessageBox::Yes)
            {
                onSaveFileClicked(true);
            }
            else if (result != QMessageBox::No)
            {
                // User cancelled opening new file
                return;
            }
        }

        loadFile(urls.front().toLocalFile());
    }
}

//////////////////////////////////////////////////////////////////////////

void EasyMainWindow::onOpenFileClicked(bool)
{
    auto action = qobject_cast<QAction*>(sender());
    if (action == nullptr)
        return;

    QString filename;

    if (action == m_loadActionMenu->menuAction())
        filename = QFileDialog::getOpenFileName(this, "Open EasyProfiler File", m_lastFiles.empty() ? QString() : m_lastFiles.front(), "EasyProfiler File (*.prof);;All Files (*.*)");
    else
        filename = action->text();

    if (!filename.isEmpty())
    {
        if (m_bNetworkFileRegime)
        {
            // Warn user about unsaved network information and suggest to save
            auto result = QMessageBox::question(this, "Unsaved session", "You have unsaved data!\nSave before opening new file?", QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
            if (result == QMessageBox::Yes)
            {
                onSaveFileClicked(true);
            }
            else if (result != QMessageBox::No)
            {
                // User cancelled opening new file
                return;
            }
        }

        loadFile(filename);
    }
}

//////////////////////////////////////////////////////////////////////////

void EasyMainWindow::addFileToList(const QString& filename)
{
    m_lastFiles.push_front(filename);

    auto action = new QAction(filename, this);
    connect(action, &QAction::triggered, this, &This::onOpenFileClicked);
    auto fileActions = m_loadActionMenu->actions();
    if (fileActions.empty())
        m_loadActionMenu->addAction(action);
    else
        m_loadActionMenu->insertAction(fileActions.front(), action);

    if (m_lastFiles.size() > 10)
    {
        // Keep 10 files at the list
        m_lastFiles.pop_back();
        m_loadActionMenu->removeAction(fileActions.back());
        delete fileActions.back();
    }

    m_bOpenedCacheFile = filename.contains(NETWORK_CACHE_FILE);

    if (m_bOpenedCacheFile)
        setWindowTitle(QString(EASY_DEFAULT_WINDOW_TITLE " - [%1] - UNSAVED network cache file").arg(m_lastFiles.front()));
    else
        setWindowTitle(QString(EASY_DEFAULT_WINDOW_TITLE " - [%1]").arg(m_lastFiles.front()));
}

void EasyMainWindow::loadFile(const QString& filename)
{
    const auto i = filename.lastIndexOf(QChar('/'));
    const auto j = filename.lastIndexOf(QChar('\\'));
    m_progress->setLabelText(QString("Loading %1...").arg(filename.mid(::std::max(i, j) + 1)));

    m_progress->setValue(0);
    m_progress->show();
    m_readerTimer.start(LOADER_TIMER_INTERVAL);
    m_reader.load(filename);
}

void EasyMainWindow::readStream(::std::stringstream& data)
{
    m_progress->setLabelText(tr("Reading from stream..."));

    m_progress->setValue(0);
    m_progress->show();
    m_readerTimer.start(LOADER_TIMER_INTERVAL);
    m_reader.load(data);
}

//////////////////////////////////////////////////////////////////////////

void EasyMainWindow::onSaveFileClicked(bool)
{
    if (m_serializedBlocks.empty())
        return;

    QString lastFile = m_lastFiles.empty() ? QString() : m_lastFiles.front();

    const auto i = lastFile.lastIndexOf(QChar('/'));
    const auto j = lastFile.lastIndexOf(QChar('\\'));
    auto k = ::std::max(i, j);

    QString dir;
    if (k > 0)
        dir = lastFile.mid(0, ++k);

    if (m_bNetworkFileRegime)
    {
        // Current file is network cache file, use current system time as output file name

        if (!dir.isEmpty())
            dir += QDateTime::currentDateTime().toString("/yyyy-MM-dd_HH-mm-ss.prof");
        else
            dir = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss.prof");
    }
    else if (m_bOpenedCacheFile)
    {
        // Opened old network cache file, use it's last modification time as output file name

        QFileInfo fileInfo(lastFile);
        if (!fileInfo.exists())
        {
            // Can not open the file!

            QMessageBox::warning(this, "Warning", "Can not open source file.\nSaving incomplete.", QMessageBox::Close);

            m_lastFiles.pop_front();
            auto action = m_loadActionMenu->actions().front();
            m_loadActionMenu->removeAction(action);
            delete action;

            return;
        }

        if (!dir.isEmpty())
            dir += fileInfo.lastModified().toString("/yyyy-MM-dd_HH-mm-ss.prof");
        else
            dir = fileInfo.lastModified().toString("yyyy-MM-dd_HH-mm-ss.prof");
    }
    else
    {
        dir = lastFile;
    }

    auto filename = QFileDialog::getSaveFileName(this, "Save EasyProfiler File", dir, "EasyProfiler File (*.prof);;All Files (*.*)");
    if (!filename.isEmpty())
    {
        // Check if the same file has been selected
        {
            QFileInfo fileInfo1(m_bNetworkFileRegime ? QString(NETWORK_CACHE_FILE) : lastFile), fileInfo2(filename);
            if (fileInfo1.exists() && fileInfo2.exists() && fileInfo1 == fileInfo2)
            {
                // Selected the same file - do nothing
                return;
            }
        }

        bool inOk = false, outOk = false;
        int8_t retry1 = -1;
        while (++retry1 < 4)
        {
            ::std::ifstream inFile(m_bNetworkFileRegime ? NETWORK_CACHE_FILE : lastFile.toStdString().c_str(), ::std::fstream::binary);
            if (!inFile.is_open())
            {
                ::std::this_thread::sleep_for(::std::chrono::milliseconds(500));
                continue;
            }

            inOk = true;

            int8_t retry2 = -1;
            while (++retry2 < 4)
            {
                ::std::ofstream outFile(filename.toStdString(), ::std::fstream::binary);
                if (!outFile.is_open())
                {
                    ::std::this_thread::sleep_for(::std::chrono::milliseconds(500));
                    continue;
                }

                outFile << inFile.rdbuf();
                outOk = true;
                break;
            }

            break;
        }

        if (outOk)
        {
            if (m_bNetworkFileRegime)
            {
                // Remove temporary network cahche file
                QFile::remove(QString(NETWORK_CACHE_FILE));
            }
            else if (m_bOpenedCacheFile)
            {
                // Remove old temporary network cahche file

                QFile::remove(lastFile.toStdString().c_str());

                m_lastFiles.pop_front();
                auto action = m_loadActionMenu->actions().front();
                m_loadActionMenu->removeAction(action);
                delete action;
            }

            addFileToList(filename);

            m_bNetworkFileRegime = false;
        }
        else if (inOk)
        {
            QMessageBox::warning(this, "Warning", "Can not open destination file.\nSaving incomplete.", QMessageBox::Close);
        }
        else
        {
            if (m_bNetworkFileRegime)
                QMessageBox::warning(this, "Warning", "Can not open network cache file.\nSaving incomplete.", QMessageBox::Close);
            else
                QMessageBox::warning(this, "Warning", "Can not open source file.\nSaving incomplete.", QMessageBox::Close);
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void EasyMainWindow::clear()
{
    static_cast<EasyHierarchyWidget*>(m_treeWidget->widget())->clear(true);
    static_cast<EasyGraphicsViewWidget*>(m_graphicsView->widget())->clear();

#if EASY_GUI_USE_DESCRIPTORS_DOCK_WINDOW != 0
    static_cast<EasyDescWidget*>(m_descTreeWidget->widget())->clear();
#endif
    if (m_dialogDescTree != nullptr)
        m_dialogDescTree->clear();

    EASY_GLOBALS.selected_thread = 0;
    ::profiler_gui::set_max(EASY_GLOBALS.selected_block);
    ::profiler_gui::set_max(EASY_GLOBALS.selected_block_id);
    EASY_GLOBALS.profiler_blocks.clear();
    EASY_GLOBALS.descriptors.clear();
    EASY_GLOBALS.gui_blocks.clear();

    m_serializedBlocks.clear();
    m_serializedDescriptors.clear();

    m_saveAction->setEnabled(false);
    m_deleteAction->setEnabled(false);

    if (m_bNetworkFileRegime)
        QFile::remove(QString(NETWORK_CACHE_FILE));

    m_bNetworkFileRegime = false;
    m_bOpenedCacheFile = false;

    setWindowTitle(EASY_DEFAULT_WINDOW_TITLE);
}

//////////////////////////////////////////////////////////////////////////

void EasyMainWindow::refreshDiagram()
{
    static_cast<EasyGraphicsViewWidget*>(m_graphicsView->widget())->view()->scene()->update();
}

//////////////////////////////////////////////////////////////////////////

void EasyMainWindow::onDeleteClicked(bool)
{
    int button = QMessageBox::Yes;
    if (m_bNetworkFileRegime)
        button = QMessageBox::question(this, "Clear all profiled data", "All profiled data and network cache file\nare going to be deleted!\nContinue?", QMessageBox::Yes, QMessageBox::No);

    if (button == QMessageBox::Yes)
        clear();
}

//////////////////////////////////////////////////////////////////////////

void EasyMainWindow::onExitClicked(bool)
{
    close();
}

//////////////////////////////////////////////////////////////////////////

void EasyMainWindow::onEncodingChanged(bool)
{
   auto _sender = qobject_cast<QAction*>(sender());
   auto name = _sender->text();
   QTextCodec *codec = QTextCodec::codecForName(name.toStdString().c_str());
   QTextCodec::setCodecForLocale(codec);
}

void EasyMainWindow::onChronoTextPosChanged(bool)
{
    auto _sender = qobject_cast<QAction*>(sender());
    EASY_GLOBALS.chrono_text_position = static_cast<::profiler_gui::ChronometerTextPosition>(_sender->data().toInt());
    refreshDiagram();
}

void EasyMainWindow::onUnitsChanged(bool)
{
    auto _sender = qobject_cast<QAction*>(sender());
    EASY_GLOBALS.time_units = static_cast<::profiler_gui::TimeUnits>(_sender->data().toInt());
}

void EasyMainWindow::onEnableDisableStatistics(bool _checked)
{
    EASY_GLOBALS.enable_statistics = _checked;

    auto action = qobject_cast<QAction*>(sender());
    if (action != nullptr)
    {
        auto f = action->font();
        f.setBold(_checked);
        action->setFont(f);

        if (_checked)
        {
            action->setText("Statistics enabled");
            SET_ICON(action, ":/Stats");
        }
        else
        {
            action->setText("Statistics disabled");
            SET_ICON(action, ":/Stats-off");
        }
    }
}

void EasyMainWindow::onCollapseItemsAfterCloseChanged(bool _checked)
{
    EASY_GLOBALS.collapse_items_on_tree_close = _checked;
}

void EasyMainWindow::onAllItemsExpandedByDefaultChange(bool _checked)
{
    EASY_GLOBALS.all_items_expanded_by_default = _checked;
}

void EasyMainWindow::onBindExpandStatusChange(bool _checked)
{
    EASY_GLOBALS.bind_scene_and_tree_expand_status = _checked;
}

void EasyMainWindow::onHierarchyFlagChange(bool _checked)
{
    EASY_GLOBALS.only_current_thread_hierarchy = _checked;
    emit EASY_GLOBALS.events.hierarchyFlagChanged(_checked);
}

//////////////////////////////////////////////////////////////////////////

void EasyMainWindow::onExpandAllClicked(bool)
{
    for (auto& block : EASY_GLOBALS.gui_blocks)
        block.expanded = true;

    emit EASY_GLOBALS.events.itemsExpandStateChanged();

    auto tree = static_cast<EasyHierarchyWidget*>(m_treeWidget->widget())->tree();
    const QSignalBlocker b(tree);
    tree->expandAll();
}

void EasyMainWindow::onCollapseAllClicked(bool)
{
    for (auto& block : EASY_GLOBALS.gui_blocks)
        block.expanded = false;

    emit EASY_GLOBALS.events.itemsExpandStateChanged();

    auto tree = static_cast<EasyHierarchyWidget*>(m_treeWidget->widget())->tree();
    const QSignalBlocker b(tree);
    tree->collapseAll();
}

//////////////////////////////////////////////////////////////////////////

void EasyMainWindow::onSpacingChange(int _value)
{
    EASY_GLOBALS.blocks_spacing = _value;
    refreshDiagram();
}

void EasyMainWindow::onMinSizeChange(int _value)
{
    EASY_GLOBALS.blocks_size_min = _value;
    refreshDiagram();
}

void EasyMainWindow::onNarrowSizeChange(int _value)
{
    EASY_GLOBALS.blocks_narrow_size = _value;
    refreshDiagram();
}

//////////////////////////////////////////////////////////////////////////

void EasyMainWindow::onFpsIntervalChange(int _value)
{
    EASY_GLOBALS.fps_timer_interval = _value;

    if (m_fpsRequestTimer.isActive())
        m_fpsRequestTimer.stop();

    if (EASY_GLOBALS.connected)
        m_fpsRequestTimer.start(_value);
}

void EasyMainWindow::onFpsHistoryChange(int _value)
{
    EASY_GLOBALS.max_fps_history = _value;
}

void EasyMainWindow::onFpsMonitorLineWidthChange(int _value)
{
    EASY_GLOBALS.fps_widget_line_width = _value;
}

//////////////////////////////////////////////////////////////////////////

void EasyMainWindow::onEditBlocksClicked(bool)
{
    if (m_descTreeDialog != nullptr)
    {
        m_descTreeDialog->raise();
        return;
    }

    m_descTreeDialog = new QDialog();
    m_descTreeDialog->setAttribute(Qt::WA_DeleteOnClose, true);
    m_descTreeDialog->setWindowTitle(EASY_DEFAULT_WINDOW_TITLE);
    m_descTreeDialog->resize(800, 600);
    connect(m_descTreeDialog, &QDialog::finished, this, &This::onDescTreeDialogClose);

    auto l = new QVBoxLayout(m_descTreeDialog);
    m_dialogDescTree = new EasyDescWidget(m_descTreeDialog);
    l->addWidget(m_dialogDescTree);
    m_descTreeDialog->setLayout(l);

    m_dialogDescTree->build();
    m_descTreeDialog->show();
}

void EasyMainWindow::onDescTreeDialogClose(int)
{
    disconnect(m_descTreeDialog, &QDialog::finished, this, &This::onDescTreeDialogClose);
    m_dialogDescTree = nullptr;
    m_descTreeDialog = nullptr;
}

//////////////////////////////////////////////////////////////////////////

void EasyMainWindow::closeEvent(QCloseEvent* close_event)
{
    if (m_bNetworkFileRegime)
    {
        // Warn user about unsaved network information and suggest to save
        if (QMessageBox::Yes == QMessageBox::question(this, "Unsaved session", "You unsaved data!\nSave before exit?", QMessageBox::Yes, QMessageBox::No))
        {
            onSaveFileClicked(true);
        }
    }

    saveSettingsAndGeometry();

    if (m_descTreeDialog != nullptr)
    {
        m_descTreeDialog->reject();
        m_descTreeDialog = nullptr;
        m_dialogDescTree = nullptr;
    }

    Parent::closeEvent(close_event);
}

//////////////////////////////////////////////////////////////////////////

void EasyMainWindow::loadSettings()
{
    QSettings settings(::profiler_gui::ORGANAZATION_NAME, ::profiler_gui::APPLICATION_NAME);
    settings.beginGroup("main");

    auto last_files = settings.value("last_files");
    if (!last_files.isNull())
        m_lastFiles = last_files.toStringList();

    auto last_addr = settings.value("ip_address");
    if (!last_addr.isNull())
        m_lastAddress = last_addr.toString();

    auto last_port = settings.value("port");
    if (!last_port.isNull())
        m_lastPort = (uint16_t)last_port.toUInt();


    auto val = settings.value("chrono_text_position");
    if (!val.isNull())
        EASY_GLOBALS.chrono_text_position = static_cast<::profiler_gui::ChronometerTextPosition>(val.toInt());

    val = settings.value("time_units");
    if (!val.isNull())
        EASY_GLOBALS.time_units = static_cast<::profiler_gui::TimeUnits>(val.toInt());


    val = settings.value("frame_time");
    if (!val.isNull())
        EASY_GLOBALS.frame_time = val.toFloat();

    val = settings.value("blocks_spacing");
    if (!val.isNull())
        EASY_GLOBALS.blocks_spacing = val.toInt();

    val = settings.value("blocks_size_min");
    if (!val.isNull())
        EASY_GLOBALS.blocks_size_min = val.toInt();

    val = settings.value("blocks_narrow_size");
    if (!val.isNull())
        EASY_GLOBALS.blocks_narrow_size = val.toInt();


    auto flag = settings.value("draw_graphics_items_borders");
    if (!flag.isNull())
        EASY_GLOBALS.draw_graphics_items_borders = flag.toBool();

    flag = settings.value("hide_narrow_children");
    if (!flag.isNull())
        EASY_GLOBALS.hide_narrow_children = flag.toBool();

    flag = settings.value("hide_minsize_blocks");
    if (!flag.isNull())
        EASY_GLOBALS.hide_minsize_blocks = flag.toBool();

    flag = settings.value("collapse_items_on_tree_close");
    if (!flag.isNull())
        EASY_GLOBALS.collapse_items_on_tree_close = flag.toBool();

    flag = settings.value("all_items_expanded_by_default");
    if (!flag.isNull())
        EASY_GLOBALS.all_items_expanded_by_default = flag.toBool();

    flag = settings.value("only_current_thread_hierarchy");
    if (!flag.isNull())
        EASY_GLOBALS.only_current_thread_hierarchy = flag.toBool();

    flag = settings.value("enable_zero_length");
    if (!flag.isNull())
        EASY_GLOBALS.enable_zero_length = flag.toBool();

    flag = settings.value("add_zero_blocks_to_hierarchy");
    if (!flag.isNull())
        EASY_GLOBALS.add_zero_blocks_to_hierarchy = flag.toBool();


    flag = settings.value("highlight_blocks_with_same_id");
    if (!flag.isNull())
        EASY_GLOBALS.highlight_blocks_with_same_id = flag.toBool();

    flag = settings.value("bind_scene_and_tree_expand_status");
    if (!flag.isNull())
        EASY_GLOBALS.bind_scene_and_tree_expand_status = flag.toBool();

    flag = settings.value("selecting_block_changes_thread");
    if (!flag.isNull())
        EASY_GLOBALS.selecting_block_changes_thread = flag.toBool();

    flag = settings.value("enable_event_indicators");
    if (!flag.isNull())
        EASY_GLOBALS.enable_event_markers = flag.toBool();

    flag = settings.value("auto_adjust_histogram_height");
    if (!flag.isNull())
        EASY_GLOBALS.auto_adjust_histogram_height = flag.toBool();

    flag = settings.value("display_only_frames_on_histogram");
    if (!flag.isNull())
        EASY_GLOBALS.display_only_frames_on_histogram = flag.toBool();

    flag = settings.value("use_decorated_thread_name");
    if (!flag.isNull())
        EASY_GLOBALS.use_decorated_thread_name = flag.toBool();

    flag = settings.value("hex_thread_id");
    if (!flag.isNull())
        EASY_GLOBALS.hex_thread_id = flag.toBool();

    flag = settings.value("fps_timer_interval");
    if (!flag.isNull())
        EASY_GLOBALS.fps_timer_interval = flag.toInt();

    flag = settings.value("max_fps_history");
    if (!flag.isNull())
        EASY_GLOBALS.max_fps_history = flag.toInt();

    flag = settings.value("fps_widget_line_width");
    if (!flag.isNull())
        EASY_GLOBALS.fps_widget_line_width = flag.toInt();

    flag = settings.value("enable_statistics");
    if (!flag.isNull())
        EASY_GLOBALS.enable_statistics = flag.toBool();

    QString encoding = settings.value("encoding", "UTF-8").toString();
    auto default_codec_mib = QTextCodec::codecForName(encoding.toStdString().c_str())->mibEnum();
    auto default_codec = QTextCodec::codecForMib(default_codec_mib);
    QTextCodec::setCodecForLocale(default_codec);

    settings.endGroup();
}

void EasyMainWindow::loadGeometry()
{
    QSettings settings(::profiler_gui::ORGANAZATION_NAME, ::profiler_gui::APPLICATION_NAME);
    settings.beginGroup("main");

    auto geometry = settings.value("geometry").toByteArray();
    if (!geometry.isEmpty())
        restoreGeometry(geometry);

    auto state = settings.value("windowState").toByteArray();
    if (!state.isEmpty())
        restoreState(state);

    settings.endGroup();
}

void EasyMainWindow::saveSettingsAndGeometry()
{
    QSettings settings(::profiler_gui::ORGANAZATION_NAME, ::profiler_gui::APPLICATION_NAME);
    settings.beginGroup("main");

    settings.setValue("geometry", this->saveGeometry());
    settings.setValue("windowState", this->saveState());
    settings.setValue("last_files", m_lastFiles);
    settings.setValue("ip_address", m_lastAddress);
    settings.setValue("port", (quint32)m_lastPort);
    settings.setValue("chrono_text_position", static_cast<int>(EASY_GLOBALS.chrono_text_position));
    settings.setValue("time_units", static_cast<int>(EASY_GLOBALS.time_units));
    settings.setValue("frame_time", EASY_GLOBALS.frame_time);
    settings.setValue("blocks_spacing", EASY_GLOBALS.blocks_spacing);
    settings.setValue("blocks_size_min", EASY_GLOBALS.blocks_size_min);
    settings.setValue("blocks_narrow_size", EASY_GLOBALS.blocks_narrow_size);
    settings.setValue("draw_graphics_items_borders", EASY_GLOBALS.draw_graphics_items_borders);
    settings.setValue("hide_narrow_children", EASY_GLOBALS.hide_narrow_children);
    settings.setValue("hide_minsize_blocks", EASY_GLOBALS.hide_minsize_blocks);
    settings.setValue("collapse_items_on_tree_close", EASY_GLOBALS.collapse_items_on_tree_close);
    settings.setValue("all_items_expanded_by_default", EASY_GLOBALS.all_items_expanded_by_default);
    settings.setValue("only_current_thread_hierarchy", EASY_GLOBALS.only_current_thread_hierarchy);
    settings.setValue("enable_zero_length", EASY_GLOBALS.enable_zero_length);
    settings.setValue("add_zero_blocks_to_hierarchy", EASY_GLOBALS.add_zero_blocks_to_hierarchy);
    settings.setValue("highlight_blocks_with_same_id", EASY_GLOBALS.highlight_blocks_with_same_id);
    settings.setValue("bind_scene_and_tree_expand_status", EASY_GLOBALS.bind_scene_and_tree_expand_status);
    settings.setValue("selecting_block_changes_thread", EASY_GLOBALS.selecting_block_changes_thread);
    settings.setValue("enable_event_indicators", EASY_GLOBALS.enable_event_markers);
    settings.setValue("auto_adjust_histogram_height", EASY_GLOBALS.auto_adjust_histogram_height);
    settings.setValue("display_only_frames_on_histogram", EASY_GLOBALS.display_only_frames_on_histogram);
    settings.setValue("use_decorated_thread_name", EASY_GLOBALS.use_decorated_thread_name);
    settings.setValue("hex_thread_id", EASY_GLOBALS.hex_thread_id);
    settings.setValue("enable_statistics", EASY_GLOBALS.enable_statistics);
    settings.setValue("fps_timer_interval", EASY_GLOBALS.fps_timer_interval);
    settings.setValue("max_fps_history", EASY_GLOBALS.max_fps_history);
    settings.setValue("fps_widget_line_width", EASY_GLOBALS.fps_widget_line_width);
    settings.setValue("encoding", QTextCodec::codecForLocale()->name());

    settings.endGroup();
}

void EasyMainWindow::setDisconnected(bool _showMessage)
{
    if (m_fpsRequestTimer.isActive())
        m_fpsRequestTimer.stop();

    if (_showMessage)
        QMessageBox::warning(this, "Warning", "Connection has lost", QMessageBox::Close);

    EASY_GLOBALS.connected = false;
    m_captureAction->setEnabled(false);
    SET_ICON(m_connectAction, ":/Connection");
    m_connectAction->setText(tr("Connect"));

    m_eventTracingEnableAction->setEnabled(false);
    m_eventTracingPriorityAction->setEnabled(false);

    m_addressEdit->setEnabled(true);
    m_portEdit->setEnabled(true);

    emit EASY_GLOBALS.events.connectionChanged(false);

}

//////////////////////////////////////////////////////////////////////////

void EasyMainWindow::onFrameTimeRequestTimeout()
{
    if (EASY_GLOBALS.fps_enabled && EASY_GLOBALS.connected && (m_listener.regime() == LISTENER_IDLE || m_listener.regime() == LISTENER_CAPTURE))
    {
        if (m_listener.requestFrameTime())
        {
            QTimer::singleShot(100, this, &This::checkFrameTimeReady);
        }
        else if (!m_listener.connected())
        {
            setDisconnected();
        }
    }
}

void EasyMainWindow::checkFrameTimeReady()
{
    if (EASY_GLOBALS.fps_enabled && EASY_GLOBALS.connected && (m_listener.regime() == LISTENER_IDLE || m_listener.regime() == LISTENER_CAPTURE))
    {
        uint32_t maxTime = 0, avgTime = 0;
        if (m_listener.frameTime(maxTime, avgTime))
        {
            static_cast<EasyFrameRateViewer*>(m_fpsViewer->widget())->addPoint(maxTime, avgTime);
        }
        else if (m_fpsRequestTimer.isActive())
        {
            QTimer::singleShot(100, this, &This::checkFrameTimeReady);
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void EasyMainWindow::onListenerTimerTimeout()
{
    if (!m_listener.connected())
    {
        if (m_listener.regime() == LISTENER_CAPTURE_RECEIVE)
            m_listener.finalizeCapture();
        m_listenerDialog->reject();
    }
    else if (m_listener.regime() == LISTENER_CAPTURE_RECEIVE)
    {
        if (m_listener.captured())
        {
            if (m_listenerTimer.isActive())
                m_listenerTimer.stop();

            m_listener.finalizeCapture();

            m_listenerDialog->accept();
            m_listenerDialog = nullptr;

            if (m_listener.size() != 0)
            {
                readStream(m_listener.data());
                m_listener.clearData();
            }
        }
    }
}

void EasyMainWindow::onListenerDialogClose(int _result)
{
    if (m_listener.regime() != LISTENER_CAPTURE_RECEIVE || !m_listener.connected())
    {
        if (m_listenerTimer.isActive())
            m_listenerTimer.stop();
    }

    disconnect(m_listenerDialog, &QDialog::finished, this, &This::onListenerDialogClose);
    m_listenerDialog = nullptr;

    switch (m_listener.regime())
    {
        case LISTENER_CAPTURE:
        {
            m_listenerDialog = new QMessageBox(QMessageBox::Information, "Receiving data...", "This process may take some time.", QMessageBox::Cancel, this);
            m_listenerDialog->setAttribute(Qt::WA_DeleteOnClose, true);
            m_listenerDialog->show();

            m_listener.stopCapture();

            if (m_listener.regime() != LISTENER_CAPTURE_RECEIVE)
            {
                m_listenerDialog->reject();
                m_listenerDialog = nullptr;
            }
            else
            {
                connect(m_listenerDialog, &QDialog::finished, this, &This::onListenerDialogClose);
                m_listenerTimer.start(250);
            }

            break;
        }

        case LISTENER_CAPTURE_RECEIVE:
        {
            if (!m_listener.captured())
            {
                if (_result == QDialog::Accepted)
                {
                    m_listenerDialog = new QMessageBox(QMessageBox::Information, "Receiving data...", "This process may take some time.", QMessageBox::Cancel, this);
                    connect(m_listenerDialog, &QDialog::finished, this, &This::onListenerDialogClose);
                    m_listenerDialog->setAttribute(Qt::WA_DeleteOnClose, true);
                    m_listenerDialog->show();
                }
                else
                {
                    m_listener.finalizeCapture();
                    m_listener.clearData();

                    if (m_listener.connected())
                    {
                        // make reconnect to clear socket buffers
                        std::string address = m_listener.address();

                        profiler::net::EasyProfilerStatus reply(false, false, false);
                        if (m_listener.connect(address.c_str(), m_listener.port(), reply))
                        {
                            disconnect(m_eventTracingEnableAction, &QAction::triggered, this, &This::onEventTracingEnableChange);
                            disconnect(m_eventTracingPriorityAction, &QAction::triggered, this, &This::onEventTracingPriorityChange);

                            m_eventTracingEnableAction->setChecked(reply.isEventTracingEnabled);
                            m_eventTracingPriorityAction->setChecked(reply.isLowPriorityEventTracing);

                            connect(m_eventTracingEnableAction, &QAction::triggered, this, &This::onEventTracingEnableChange);
                            connect(m_eventTracingPriorityAction, &QAction::triggered, this, &This::onEventTracingPriorityChange);

                            if (reply.isProfilerEnabled)
                            {
                                // Connected application is already profiling.
                                // Show capture dialog immediately
                                onCaptureClicked(true);
                            }
                        }
                    }
                }

                break;
            }

            if (m_listenerTimer.isActive())
                m_listenerTimer.stop();

            m_listener.finalizeCapture();

            if (m_listener.size() != 0)
            {
                readStream(m_listener.data());
                m_listener.clearData();
            }

            break;
        }

        case LISTENER_DESCRIBE:
        {
            break;
        }

        default:
            return;
    }

    if (!m_listener.connected())
    {
        setDisconnected();
    }
}


//////////////////////////////////////////////////////////////////////////

void EasyMainWindow::onFileReaderTimeout()
{
    if (m_reader.done())
    {
        auto nblocks = m_reader.size();
        if (nblocks != 0)
        {
            static_cast<EasyHierarchyWidget*>(m_treeWidget->widget())->clear(true);

            ::profiler::SerializedData serialized_blocks;
            ::profiler::SerializedData serialized_descriptors;
            ::profiler::descriptors_list_t descriptors;
            ::profiler::blocks_t blocks;
            ::profiler::thread_blocks_tree_t threads_map;
            QString filename;
            uint32_t descriptorsNumberInFile = 0;
            uint32_t version = 0;
            m_reader.get(serialized_blocks, serialized_descriptors, descriptors, blocks, threads_map, descriptorsNumberInFile, version, filename);

            if (threads_map.size() > 0xff)
            {
                if (m_reader.isFile())
                    qWarning() << "Warning: file " << filename << " contains " << threads_map.size() << " threads!";
                else
                    qWarning() << "Warning: input stream contains " << threads_map.size() << " threads!";
                qWarning() << "Warning:    Currently, maximum number of displayed threads is 255! Some threads will not be displayed.";
            }

            m_bNetworkFileRegime = !m_reader.isFile();
            if (!m_bNetworkFileRegime)
            {
                auto index = m_lastFiles.indexOf(filename, 0);
                if (index == -1)
                {
                    // This file is totally new. Add it to the list.
                    addFileToList(filename);
                }
                else
                {
                    if (index != 0)
                    {
                        // This file has been already loaded. Move it to the front.
                        m_lastFiles.move(index, 0);
                        auto fileActions = m_loadActionMenu->actions();
                        auto action = fileActions.at(index);
                        m_loadActionMenu->removeAction(action);
                        m_loadActionMenu->insertAction(fileActions.front(), action);
                    }

                    m_bOpenedCacheFile = filename.contains(NETWORK_CACHE_FILE);

                    if (m_bOpenedCacheFile)
                        setWindowTitle(QString(EASY_DEFAULT_WINDOW_TITLE " - [%1] - UNSAVED network cache file").arg(filename));
                    else
                        setWindowTitle(QString(EASY_DEFAULT_WINDOW_TITLE " - [%1]").arg(filename));
                }
            }
            else
            {
                m_bOpenedCacheFile = false;
                setWindowTitle(EASY_DEFAULT_WINDOW_TITLE " - UNSAVED network cache");
            }

            m_serializedBlocks = ::std::move(serialized_blocks);
            m_serializedDescriptors = ::std::move(serialized_descriptors);
            m_descriptorsNumberInFile = descriptorsNumberInFile;
            EASY_GLOBALS.selected_thread = 0;
            EASY_GLOBALS.version = version;
            ::profiler_gui::set_max(EASY_GLOBALS.selected_block);
            ::profiler_gui::set_max(EASY_GLOBALS.selected_block_id);
            EASY_GLOBALS.profiler_blocks.swap(threads_map);
            EASY_GLOBALS.descriptors.swap(descriptors);

            EASY_GLOBALS.gui_blocks.clear();
            EASY_GLOBALS.gui_blocks.resize(nblocks);
            memset(EASY_GLOBALS.gui_blocks.data(), 0, sizeof(::profiler_gui::EasyBlock) * nblocks);
            for (decltype(nblocks) i = 0; i < nblocks; ++i) {
                auto& guiblock = EASY_GLOBALS.gui_blocks[i];
                guiblock.tree = ::std::move(blocks[i]);
#ifdef EASY_TREE_WIDGET__USE_VECTOR
                ::profiler_gui::set_max(guiblock.tree_item);
#endif
            }

            static_cast<EasyGraphicsViewWidget*>(m_graphicsView->widget())->view()->setTree(EASY_GLOBALS.profiler_blocks);

#if EASY_GUI_USE_DESCRIPTORS_DOCK_WINDOW != 0
            static_cast<EasyDescWidget*>(m_descTreeWidget->widget())->build();
#endif
            if (m_dialogDescTree != nullptr)
                m_dialogDescTree->build();

            m_saveAction->setEnabled(true);
            m_deleteAction->setEnabled(true);
        }
        else
        {
            QMessageBox::warning(this, "Warning", QString("Can not read profiled blocks.\n\nReason:\n%1").arg(m_reader.getError()), QMessageBox::Close);

            if (m_reader.isFile())
            {
                auto index = m_lastFiles.indexOf(m_reader.filename(), 0);
                if (index >= 0)
                {
                    // Remove unexisting file from list
                    m_lastFiles.removeAt(index);
                    auto action = m_loadActionMenu->actions().at(index);
                    m_loadActionMenu->removeAction(action);
                    delete action;
                }
            }
        }

        m_reader.interrupt();

        m_readerTimer.stop();
        m_progress->setValue(100);
        //m_progress->hide();

        if (EASY_GLOBALS.all_items_expanded_by_default)
        {
            onExpandAllClicked(true);
        }
    }
    else
    {
        m_progress->setValue(m_reader.progress());
    }
}

void EasyMainWindow::onFileReaderCancel()
{
    m_readerTimer.stop();
    m_reader.interrupt();
    m_progress->setValue(100);
    //m_progress->hide();
}

//////////////////////////////////////////////////////////////////////////

EasyFileReader::EasyFileReader()
{

}

EasyFileReader::~EasyFileReader()
{
    interrupt();
}

const bool EasyFileReader::isFile() const
{
    return m_isFile;
}

bool EasyFileReader::done() const
{
    return m_bDone.load(::std::memory_order_acquire);
}

int EasyFileReader::progress() const
{
    return m_progress.load(::std::memory_order_acquire);
}

unsigned int EasyFileReader::size() const
{
    return m_size.load(::std::memory_order_acquire);
}

const QString& EasyFileReader::filename() const
{
    return m_filename;
}

void EasyFileReader::load(const QString& _filename)
{
    interrupt();

    m_isFile = true;
    m_filename = _filename;
    m_thread = ::std::thread([this](bool _enableStatistics) {
        m_size.store(fillTreesFromFile(m_progress, m_filename.toStdString().c_str(), m_serializedBlocks, m_serializedDescriptors,
            m_descriptors, m_blocks, m_blocksTree, m_descriptorsNumberInFile, m_version, _enableStatistics, m_errorMessage), ::std::memory_order_release);
        m_progress.store(100, ::std::memory_order_release);
        m_bDone.store(true, ::std::memory_order_release);
    }, EASY_GLOBALS.enable_statistics);
}

void EasyFileReader::load(::std::stringstream& _stream)
{
    interrupt();

    m_isFile = false;
    m_filename.clear();

#if defined(__GNUC__) && __GNUC__ < 5 && !defined(__llvm__)
    // gcc 4 has a known bug which has been solved in gcc 5:
    // std::stringstream has no swap() method :(
    // have to copy all contents... Use gcc 5 or higher!
#pragma message "Warning: in gcc 4 and lower std::stringstream has no swap()! Memory consumption may increase! Better use gcc 5 or higher instead."
    m_stream.str(_stream.str());
#else
    m_stream.swap(_stream);
#endif

    m_thread = ::std::thread([this](bool _enableStatistics) {
        ::std::ofstream cache_file(NETWORK_CACHE_FILE, ::std::fstream::binary);
        if (cache_file.is_open()) {
            cache_file << m_stream.str();
            cache_file.close();
        }
        m_size.store(fillTreesFromStream(m_progress, m_stream, m_serializedBlocks, m_serializedDescriptors, m_descriptors,
            m_blocks, m_blocksTree, m_descriptorsNumberInFile, m_version, _enableStatistics, m_errorMessage), ::std::memory_order_release);
        m_progress.store(100, ::std::memory_order_release);
        m_bDone.store(true, ::std::memory_order_release);
    }, EASY_GLOBALS.enable_statistics);
}

void EasyFileReader::interrupt()
{
    m_progress.store(-100, ::std::memory_order_release);
    if (m_thread.joinable())
        m_thread.join();

    m_bDone.store(false, ::std::memory_order_release);
    m_progress.store(0, ::std::memory_order_release);
    m_size.store(0, ::std::memory_order_release);
    m_serializedBlocks.clear();
    m_serializedDescriptors.clear();
    m_descriptors.clear();
    m_blocks.clear();
    m_blocksTree.clear();
    m_descriptorsNumberInFile = 0;
    m_version = 0;

    clear_stream(m_stream);
    clear_stream(m_errorMessage);
}

void EasyFileReader::get(::profiler::SerializedData& _serializedBlocks, ::profiler::SerializedData& _serializedDescriptors,
                         ::profiler::descriptors_list_t& _descriptors, ::profiler::blocks_t& _blocks,
                         ::profiler::thread_blocks_tree_t& _tree, uint32_t& _descriptorsNumberInFile, uint32_t& _version, QString& _filename)
{
    if (done())
    {
        m_serializedBlocks.swap(_serializedBlocks);
        m_serializedDescriptors.swap(_serializedDescriptors);
        ::profiler::descriptors_list_t(::std::move(m_descriptors)).swap(_descriptors);
        m_blocks.swap(_blocks);
        m_blocksTree.swap(_tree);
        m_filename.swap(_filename);
        _descriptorsNumberInFile = m_descriptorsNumberInFile;
        _version = m_version;
    }
}

QString EasyFileReader::getError()
{
    return QString(m_errorMessage.str().c_str());
}

//////////////////////////////////////////////////////////////////////////

void EasyMainWindow::onEventTracingPriorityChange(bool _checked)
{
    if (EASY_GLOBALS.connected)
        m_listener.send(profiler::net::BoolMessage(profiler::net::MESSAGE_TYPE_EVENT_TRACING_PRIORITY, _checked));
}

void EasyMainWindow::onEventTracingEnableChange(bool _checked)
{
    if (EASY_GLOBALS.connected)
        m_listener.send(profiler::net::BoolMessage(profiler::net::MESSAGE_TYPE_EVENT_TRACING_STATUS, _checked));
}

//////////////////////////////////////////////////////////////////////////

void EasyMainWindow::onFrameTimeEditFinish()
{
    auto text = m_frameTimeEdit->text();
    if (text.contains(QChar(',')))
    {
        text.remove(QChar('.')).replace(QChar(','), QChar('.'));
        m_frameTimeEdit->setText(text);
    }

    EASY_GLOBALS.frame_time = text.toFloat() * 1e3f;

    disconnect(&EASY_GLOBALS.events, &::profiler_gui::EasyGlobalSignals::expectedFrameTimeChanged, this, &This::onFrameTimeChanged);
    emit EASY_GLOBALS.events.expectedFrameTimeChanged();
    connect(&EASY_GLOBALS.events, &::profiler_gui::EasyGlobalSignals::expectedFrameTimeChanged, this, &This::onFrameTimeChanged);
}

void EasyMainWindow::onFrameTimeChanged()
{
    m_frameTimeEdit->setText(QString::number(EASY_GLOBALS.frame_time * 1e-3));
}

//////////////////////////////////////////////////////////////////////////

void EasyMainWindow::onConnectClicked(bool)
{
    if (EASY_GLOBALS.connected)
    {
        // Disconnect if already connected
        m_listener.disconnect();
        setDisconnected(false);
        return;
    }

    QString address = m_addressEdit->text();
    const decltype(m_lastPort) port = m_portEdit->text().toUShort();
    const bool isSameAddress = (EASY_GLOBALS.connected && m_listener.port() == port && address.toStdString() == m_listener.address());

    profiler::net::EasyProfilerStatus reply(false, false, false);
    if (!m_listener.connect(address.toStdString().c_str(), port, reply))
    {
        /*if (EASY_GLOBALS.connected && !isSameAddress)
        {
            if (QMessageBox::warning(this, "Warning", QString("Cannot connect to %1\n\nRestore previous connection?").arg(address),
                QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
            {
                if (!m_listener.connect(m_lastAddress.toStdString().c_str(), m_lastPort, reply))
                {
                    QMessageBox::warning(this, "Warning", "Cannot restore previous connection", QMessageBox::Close);
                    setDisconnected(false);
                    m_lastAddress = ::std::move(address);
                    m_lastPort = port;
                }
                else
                {
                    m_addressEdit->setText(m_lastAddress);
                    m_portEdit->setText(QString::number(m_lastPort));
                    //QMessageBox::information(this, "Information", "Previous connection restored", QMessageBox::Close);
                }
            }
            else
            {
                setDisconnected(false);
                m_lastAddress = ::std::move(address);
                m_lastPort = port;
            }
        }
        else*/
        {
            QMessageBox::warning(this, "Warning", QString("Cannot connect to %1").arg(address), QMessageBox::Close);
            if (EASY_GLOBALS.connected)
                setDisconnected(false);

            if (!isSameAddress)
            {
                m_lastAddress = ::std::move(address);
                m_lastPort = port;
            }
        }

        return;
    }

    m_lastAddress = ::std::move(address);
    m_lastPort = port;

    qInfo() << "Connected successfully";
    EASY_GLOBALS.connected = true;
    m_captureAction->setEnabled(true);
    SET_ICON(m_connectAction, ":/Connection-on");
    m_connectAction->setText(tr("Disconnect"));

    if (m_fpsViewer->isVisible())
        static_cast<EasyFrameRateViewer*>(m_fpsViewer->widget())->clear();

    if (!m_fpsRequestTimer.isActive())
        m_fpsRequestTimer.start(EASY_GLOBALS.fps_timer_interval);

    disconnect(m_eventTracingEnableAction, &QAction::triggered, this, &This::onEventTracingEnableChange);
    disconnect(m_eventTracingPriorityAction, &QAction::triggered, this, &This::onEventTracingPriorityChange);

    m_eventTracingEnableAction->setEnabled(true);
    m_eventTracingPriorityAction->setEnabled(true);

    m_eventTracingEnableAction->setChecked(reply.isEventTracingEnabled);
    m_eventTracingPriorityAction->setChecked(reply.isLowPriorityEventTracing);

    connect(m_eventTracingEnableAction, &QAction::triggered, this, &This::onEventTracingEnableChange);
    connect(m_eventTracingPriorityAction, &QAction::triggered, this, &This::onEventTracingPriorityChange);

    m_addressEdit->setEnabled(false);
    m_portEdit->setEnabled(false);

    emit EASY_GLOBALS.events.connectionChanged(true);

    if (reply.isProfilerEnabled)
    {
        // Connected application is already profiling.
        // Show capture dialog immediately
        onCaptureClicked(true);
    }
}

void EasyMainWindow::onCaptureClicked(bool)
{
    if (!EASY_GLOBALS.connected)
    {
        QMessageBox::warning(this, "Warning", "No connection with profiling app", QMessageBox::Close);
        return;
    }

    if (m_listener.regime() != LISTENER_IDLE)
    {
        if (m_listener.regime() == LISTENER_CAPTURE || m_listener.regime() == LISTENER_CAPTURE_RECEIVE)
            QMessageBox::warning(this, "Warning", "Already capturing frames.\nFinish old capturing session first.", QMessageBox::Close);
        else
            QMessageBox::warning(this, "Warning", "Capturing blocks description.\nFinish old capturing session first.", QMessageBox::Close);
        return;
    }

    if (!m_listener.startCapture())
    {
        // Connection lost. Try to restore connection.

        profiler::net::EasyProfilerStatus reply(false, false, false);
        if (!m_listener.connect(m_lastAddress.toStdString().c_str(), m_lastPort, reply))
        {
            setDisconnected();
            return;
        }

        if (!m_listener.startCapture())
        {
            setDisconnected();
            return;
        }
    }

    m_listenerTimer.start(250);

    m_listenerDialog = new QMessageBox(QMessageBox::Information, "Capturing frames...", "Close this dialog to stop capturing.", QMessageBox::NoButton, this);

    auto button = new QToolButton(m_listenerDialog);
    button->setAutoRaise(true);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    button->setIconSize(::profiler_gui::ICONS_SIZE);
    button->setIcon(QIcon(":/Stop"));
    button->setText("Stop");
    m_listenerDialog->addButton(button, QMessageBox::AcceptRole);

    m_listenerDialog->setAttribute(Qt::WA_DeleteOnClose, true);
    connect(m_listenerDialog, &QDialog::finished, this, &This::onListenerDialogClose);
    m_listenerDialog->show();
}

void EasyMainWindow::onGetBlockDescriptionsClicked(bool)
{
    if (!EASY_GLOBALS.connected)
    {
        QMessageBox::warning(this, "Warning", "No connection with profiling app", QMessageBox::Close);
        return;
    }

    if (m_listener.regime() != LISTENER_IDLE)
    {
        if (m_listener.regime() == LISTENER_DESCRIBE)
            QMessageBox::warning(this, "Warning", "Already capturing blocks description.\nFinish old capturing session first.", QMessageBox::Close);
        else
            QMessageBox::warning(this, "Warning", "Capturing capturing frames.\nFinish old capturing session first.", QMessageBox::Close);
        return;
    }

    m_listenerDialog = new QMessageBox(QMessageBox::Information, "Waiting for blocks...", "This may take some time.", QMessageBox::NoButton, this);
    m_listenerDialog->setAttribute(Qt::WA_DeleteOnClose, true);
    m_listenerDialog->show();

    m_listener.requestBlocksDescription();

    m_listenerDialog->reject();
    m_listenerDialog = nullptr;

    if (m_listener.size() != 0)
    {
        // Read descriptions from stream

        decltype(EASY_GLOBALS.descriptors) descriptors;
        decltype(m_serializedDescriptors) serializedDescriptors;
        ::std::stringstream errorMessage;
        if (readDescriptionsFromStream(m_listener.data(), serializedDescriptors, descriptors, errorMessage))
        {
            // Merge old and new descriptions

            bool cancel = false;
            const bool doFlush = m_descriptorsNumberInFile > descriptors.size();
            if (doFlush && !m_serializedBlocks.empty())
            {
                auto button = QMessageBox::question(this, "Information",
                    QString("New blocks description number = %1\nis less than the old one = %2.\nTo avoid possible conflicts\nall profiled data will be deleted.\nContinue?")
                    .arg(descriptors.size())
                    .arg(m_descriptorsNumberInFile),
                    QMessageBox::Yes, QMessageBox::No);

                if (button == QMessageBox::Yes)
                    clear(); // Clear all contents because new descriptors list conflicts with old one
                else
                    cancel = true;
            }

            if (!cancel)
            {
                if (!doFlush && m_descriptorsNumberInFile < EASY_GLOBALS.descriptors.size())
                {
                    // There are dynamically added descriptors, add them to the new list too

                    auto newnumber = static_cast<decltype(m_descriptorsNumberInFile)>(descriptors.size());
                    auto size = static_cast<decltype(m_descriptorsNumberInFile)>(EASY_GLOBALS.descriptors.size());
                    auto diff = newnumber - size;
                    decltype(newnumber) failnumber = 0;

                    descriptors.reserve(descriptors.size() + EASY_GLOBALS.descriptors.size() - m_descriptorsNumberInFile);
                    for (auto i = m_descriptorsNumberInFile; i < size; ++i)
                    {
                        auto id = EASY_GLOBALS.descriptors[i]->id();
                        if (id < newnumber)
                            descriptors.push_back(descriptors[id]);
                        else
                            ++failnumber;
                    }

                    if (failnumber != 0)
                    {
                        // There are some errors...

                        // revert changes
                        descriptors.resize(newnumber);

                        // clear all profiled data to avoid conflicts
                        auto button = QMessageBox::question(this, "Information",
                            "There are errors while merging block descriptions lists.\nTo avoid possible conflicts\nall profiled data will be deleted.\nContinue?",
                            QMessageBox::Yes, QMessageBox::No);

                        if (button == QMessageBox::Yes)
                            clear(); // Clear all contents because new descriptors list conflicts with old one
                        else
                            cancel = true;
                    }

                    if (!cancel && diff != 0)
                    {
                        for (auto& b : EASY_GLOBALS.gui_blocks)
                        {
                            if (b.tree.node->id() >= m_descriptorsNumberInFile)
                                b.tree.node->setId(b.tree.node->id() + diff);
                        }

                        m_descriptorsNumberInFile = newnumber;
                    }
                }

                if (!cancel)
                {
                    EASY_GLOBALS.descriptors.swap(descriptors);
                    m_serializedDescriptors.swap(serializedDescriptors);
                    m_descriptorsNumberInFile = static_cast<uint32_t>(EASY_GLOBALS.descriptors.size());

                    if (m_descTreeDialog != nullptr)
                    {
#if EASY_GUI_USE_DESCRIPTORS_DOCK_WINDOW != 0
                        static_cast<EasyDescWidget*>(m_descTreeWidget->widget())->build();
#endif
                        m_dialogDescTree->build();
                        m_descTreeDialog->raise();
                    }
                    else
                    {
                        onEditBlocksClicked(true);
                    }
                }
            }
        }
        else
        {
            QMessageBox::warning(this, "Warning", QString("Can not read blocks description from stream.\n\nReason:\n%1").arg(errorMessage.str().c_str()), QMessageBox::Close);
        }

        m_listener.clearData();
    }

    if (!m_listener.connected())
    {
        setDisconnected();
    }
}

//////////////////////////////////////////////////////////////////////////

void EasyMainWindow::onBlockStatusChange(::profiler::block_id_t _id, ::profiler::EasyBlockStatus _status)
{
    if (EASY_GLOBALS.connected)
        m_listener.send(profiler::net::BlockStatusMessage(_id, static_cast<uint8_t>(_status)));
}

//////////////////////////////////////////////////////////////////////////

EasySocketListener::EasySocketListener() : m_receivedSize(0), m_port(0), m_regime(LISTENER_IDLE)
{
    m_bInterrupt = ATOMIC_VAR_INIT(false);
    m_bConnected = ATOMIC_VAR_INIT(false);
    m_bStopReceive = ATOMIC_VAR_INIT(false);
    m_bFrameTimeReady = ATOMIC_VAR_INIT(false);
    m_bCaptureReady = ATOMIC_VAR_INIT(false);
    m_frameMax = ATOMIC_VAR_INIT(0);
    m_frameAvg = ATOMIC_VAR_INIT(0);
}

EasySocketListener::~EasySocketListener()
{
    m_bInterrupt.store(true, ::std::memory_order_release);
    if (m_thread.joinable())
        m_thread.join();
}

bool EasySocketListener::connected() const
{
    return m_bConnected.load(::std::memory_order_acquire);
}

bool EasySocketListener::captured() const
{
    return m_bCaptureReady.load(::std::memory_order_acquire);
}

EasyListenerRegime EasySocketListener::regime() const
{
    return m_regime;
}

uint64_t EasySocketListener::size() const
{
    return m_receivedSize;
}

::std::stringstream& EasySocketListener::data()
{
    return m_receivedData;
}

const ::std::string& EasySocketListener::address() const
{
    return m_address;
}

uint16_t EasySocketListener::port() const
{
    return m_port;
}

void EasySocketListener::clearData()
{
    clear_stream(m_receivedData);
    m_receivedSize = 0;
}

void EasySocketListener::disconnect()
{
    if (connected())
    {
        m_bInterrupt.store(true, ::std::memory_order_release);
        if (m_thread.joinable())
            m_thread.join();

        m_bConnected.store(false, ::std::memory_order_release);
        m_bInterrupt.store(false, ::std::memory_order_release);
        m_bCaptureReady.store(false, ::std::memory_order_release);
        m_bStopReceive.store(false, ::std::memory_order_release);
    }

    m_address.clear();
    m_port = 0;

    m_easySocket.flush();
    m_easySocket.init();
}

bool EasySocketListener::connect(const char* _ipaddress, uint16_t _port, profiler::net::EasyProfilerStatus& _reply)
{
    if (connected())
    {
        m_bInterrupt.store(true, ::std::memory_order_release);
        if (m_thread.joinable())
            m_thread.join();

        m_bConnected.store(false, ::std::memory_order_release);
        m_bInterrupt.store(false, ::std::memory_order_release);
        m_bCaptureReady.store(false, ::std::memory_order_release);
        m_bStopReceive.store(false, ::std::memory_order_release);
    }

    m_address.clear();
    m_port = 0;

    //m_easySocket.flush();
    //m_easySocket.init();
    int res = m_easySocket.setAddress(_ipaddress, _port);
    res = m_easySocket.connect();

    const bool isConnected = res == 0;
    if (isConnected)
    {
        static const size_t buffer_size = sizeof(profiler::net::EasyProfilerStatus) << 1;
        char buffer[buffer_size] = {};
        int bytes = 0;
        
        while (true)
        {
            bytes = m_easySocket.receive(buffer, buffer_size);

            if (bytes == -1)
            {
                if (m_easySocket.isDisconnected())
                    return false;
                bytes = 0;
                continue;
            }

            break;
        }

        if (bytes == 0)
        {
            m_address = _ipaddress;
            m_port = _port;
            m_bConnected.store(isConnected, ::std::memory_order_release);
            return isConnected;
        }

        size_t seek = bytes;
        while (seek < sizeof(profiler::net::EasyProfilerStatus))
        {
            bytes = m_easySocket.receive(buffer + seek, buffer_size - seek);

            if (bytes == -1)
            {
                if (m_easySocket.isDisconnected())
                    return false;
                break;
            }

            seek += bytes;
        }

        auto message = reinterpret_cast<const ::profiler::net::EasyProfilerStatus*>(buffer);
        if (message->isEasyNetMessage() && message->type == profiler::net::MESSAGE_TYPE_ACCEPTED_CONNECTION)
            _reply = *message;

        m_address = _ipaddress;
        m_port = _port;
    }

    m_bConnected.store(isConnected, ::std::memory_order_release);
    return isConnected;
}

bool EasySocketListener::startCapture()
{
    //if (m_thread.joinable())
    //{
    //    m_bInterrupt.store(true, ::std::memory_order_release);
    //    m_thread.join();
    //    m_bInterrupt.store(false, ::std::memory_order_release);
    //}

    clearData();

    profiler::net::Message request(profiler::net::MESSAGE_TYPE_REQUEST_START_CAPTURE);
    m_easySocket.send(&request, sizeof(request));

    if (m_easySocket.isDisconnected()) {
        m_bConnected.store(false, ::std::memory_order_release);
        return false;
    }

    m_regime = LISTENER_CAPTURE;
    m_bCaptureReady.store(false, ::std::memory_order_release);
    //m_thread = ::std::thread(&EasySocketListener::listenCapture, this);

    return true;
}

void EasySocketListener::stopCapture()
{
    //if (!m_thread.joinable() || m_regime != LISTENER_CAPTURE)
    //    return;

    if (m_regime != LISTENER_CAPTURE)
        return;

    //m_bStopReceive.store(true, ::std::memory_order_release);
    profiler::net::Message request(profiler::net::MESSAGE_TYPE_REQUEST_STOP_CAPTURE);
    m_easySocket.send(&request, sizeof(request));

    //m_thread.join();

    if (m_easySocket.isDisconnected()) {
        m_bConnected.store(false, ::std::memory_order_release);
        m_bStopReceive.store(false, ::std::memory_order_release);
        m_regime = LISTENER_IDLE;
        m_bCaptureReady.store(true, ::std::memory_order_release);
        return;
    }

    m_regime = LISTENER_CAPTURE_RECEIVE;
    if (m_thread.joinable())
    {
        m_bInterrupt.store(true, ::std::memory_order_release);
        m_thread.join();
        m_bInterrupt.store(false, ::std::memory_order_release);
    }

    m_thread = ::std::thread(&EasySocketListener::listenCapture, this);

    //m_regime = LISTENER_IDLE;
    //m_bStopReceive.store(false, ::std::memory_order_release);
}

void EasySocketListener::finalizeCapture()
{
    if (m_thread.joinable())
    {
        m_bInterrupt.store(true, ::std::memory_order_release);
        m_thread.join();
        m_bInterrupt.store(false, ::std::memory_order_release);
    }

    m_regime = LISTENER_IDLE;
    m_bCaptureReady.store(false, ::std::memory_order_release);
    m_bStopReceive.store(false, ::std::memory_order_release);
}

void EasySocketListener::requestBlocksDescription()
{
    if (m_thread.joinable())
    {
        m_bInterrupt.store(true, ::std::memory_order_release);
        m_thread.join();
        m_bInterrupt.store(false, ::std::memory_order_release);
    }

    clearData();

    profiler::net::Message request(profiler::net::MESSAGE_TYPE_REQUEST_BLOCKS_DESCRIPTION);
    m_easySocket.send(&request, sizeof(request));

    if(m_easySocket.isDisconnected()  ){
        m_bConnected.store(false, ::std::memory_order_release);
    }

    m_regime = LISTENER_DESCRIBE;
    listenDescription();
    m_regime = LISTENER_IDLE;
}

bool EasySocketListener::frameTime(uint32_t& _maxTime, uint32_t& _avgTime)
{
    if (m_bFrameTimeReady.exchange(false, ::std::memory_order_acquire))
    {
        _maxTime = m_frameMax.load(::std::memory_order_acquire);
        _avgTime = m_frameAvg.load(::std::memory_order_acquire);
        return true;
    }

    return false;
}

bool EasySocketListener::requestFrameTime()
{
    if (m_regime != LISTENER_IDLE && m_regime != LISTENER_CAPTURE)
        return false;

    if (m_thread.joinable())
    {
        m_bInterrupt.store(true, ::std::memory_order_release);
        m_thread.join();
        m_bInterrupt.store(false, ::std::memory_order_release);
    }

    profiler::net::Message request(profiler::net::MESSAGE_TYPE_REQUEST_MAIN_FRAME_TIME_MAX_AVG_US);
    m_easySocket.send(&request, sizeof(request));

    if (m_easySocket.isDisconnected())
    {
        m_bConnected.store(false, ::std::memory_order_release);
        return false;
    }

    m_bFrameTimeReady.store(false, ::std::memory_order_release);
    m_thread = ::std::thread(&EasySocketListener::listenFrameTime, this);

    return true;
}

//////////////////////////////////////////////////////////////////////////

void EasySocketListener::listenCapture()
{
    // TODO: Merge functions listenCapture() and listenDescription()

    static const int buffer_size = 8 * 1024 * 1024;
    char* buffer = new char[buffer_size];
    int seek = 0, bytes = 0;
    auto timeBegin = ::std::chrono::system_clock::now();

    bool isListen = true, disconnected = false;
    while (isListen && !m_bInterrupt.load(::std::memory_order_acquire))
    {
        if (m_bStopReceive.load(::std::memory_order_acquire))
        {
            profiler::net::Message request(profiler::net::MESSAGE_TYPE_REQUEST_STOP_CAPTURE);
            m_easySocket.send(&request, sizeof(request));
            m_bStopReceive.store(false, ::std::memory_order_release);
        }

        if ((bytes - seek) == 0)
        {
            bytes = m_easySocket.receive(buffer, buffer_size);

            if (bytes == -1)
            {
                if (m_easySocket.isDisconnected())
                {
                    m_bConnected.store(false, ::std::memory_order_release);
                    isListen = false;
                    disconnected = true;
                }

                seek = 0;
                bytes = 0;

                continue;
            }

            seek = 0;
        }

        if (bytes == 0)
        {
            isListen = false;
            break;
        }

        char* buf = buffer + seek;

        if (bytes > 0)
        {
            auto message = reinterpret_cast<const ::profiler::net::Message*>(buf);
            if (!message->isEasyNetMessage())
                continue;

            switch (message->type)
            {
                case profiler::net::MESSAGE_TYPE_ACCEPTED_CONNECTION:
                {
                    qInfo() << "Receive MESSAGE_TYPE_ACCEPTED_CONNECTION";
                    //m_easySocket.send(&request, sizeof(request));
                    seek += sizeof(profiler::net::Message);
                    break;
                }

                case profiler::net::MESSAGE_TYPE_REPLY_START_CAPTURING:
                {
                    qInfo() << "Receive MESSAGE_TYPE_REPLY_START_CAPTURING";
                    seek += sizeof(profiler::net::Message);
                    break;
                }

                case profiler::net::MESSAGE_TYPE_REPLY_BLOCKS_END:
                {
                    qInfo() << "Receive MESSAGE_TYPE_REPLY_BLOCKS_END";
                    seek += sizeof(profiler::net::Message);

                    const auto dt = ::std::chrono::duration_cast<std::chrono::milliseconds>(::std::chrono::system_clock::now() - timeBegin);
                    const auto bytesNumber = m_receivedData.str().size();
                    qInfo() << "recieved " << bytesNumber << " bytes, " << dt.count() << " ms, average speed = " << double(bytesNumber) * 1e3 / double(dt.count()) / 1024. << " kBytes/sec";

                    seek = 0;
                    bytes = 0;

                    isListen = false;

                    break;
                }

                case profiler::net::MESSAGE_TYPE_REPLY_BLOCKS:
                {
                    qInfo() << "Receive MESSAGE_TYPE_REPLY_BLOCKS";

                    seek += sizeof(profiler::net::DataMessage);
                    profiler::net::DataMessage* dm = (profiler::net::DataMessage*)message;
                    timeBegin = std::chrono::system_clock::now();

                    int neededSize = dm->size;


                    buf = buffer + seek;
                    auto bytesNumber = ::std::min((int)dm->size, bytes - seek);
                    m_receivedSize += bytesNumber;
                    m_receivedData.write(buf, bytesNumber);
                    neededSize -= bytesNumber;

                    if (neededSize == 0)
                        seek += bytesNumber;
                    else
                    {
                        seek = 0;
                        bytes = 0;
                    }


                    int loaded = 0;
                    while (neededSize > 0)
                    {
                        bytes = m_easySocket.receive(buffer, buffer_size);

                        if (bytes == -1)
                        {
                            if (m_easySocket.isDisconnected())
                            {
                                m_bConnected.store(false, ::std::memory_order_release);
                                isListen = false;
                                disconnected = true;
                                neededSize = 0;
                            }

                            break;
                        }

                        buf = buffer;
                        int toWrite = ::std::min(bytes, neededSize);
                        m_receivedSize += toWrite;
                        m_receivedData.write(buf, toWrite);
                        neededSize -= toWrite;
                        loaded += toWrite;
                        seek = toWrite;
                    }

                    if (m_bStopReceive.load(::std::memory_order_acquire))
                    {
                        profiler::net::Message request(profiler::net::MESSAGE_TYPE_REQUEST_STOP_CAPTURE);
                        m_easySocket.send(&request, sizeof(request));
                        m_bStopReceive.store(false, ::std::memory_order_release);
                    }

                    break;
                }

                default:
                    //qInfo() << "Receive unknown " << message->type;
                    break;
            }
        }
    }

    if (disconnected)
        clearData();

    delete [] buffer;

    m_bCaptureReady.store(true, ::std::memory_order_release);
}

void EasySocketListener::listenDescription()
{
    // TODO: Merge functions listenDescription() and listenCapture()

    static const int buffer_size = 8 * 1024 * 1024;
    char* buffer = new char[buffer_size];
    int seek = 0, bytes = 0;

    bool isListen = true, disconnected = false;
    while (isListen && !m_bInterrupt.load(::std::memory_order_acquire))
    {
        if ((bytes - seek) == 0)
        {
            bytes = m_easySocket.receive(buffer, buffer_size);

            if (bytes == -1)
            {
                if (m_easySocket.isDisconnected())
                {
                    m_bConnected.store(false, ::std::memory_order_release);
                    isListen = false;
                    disconnected = true;
                }

                seek = 0;
                bytes = 0;

                continue;
            }

            seek = 0;
        }

        if (bytes == 0)
        {
            isListen = false;
            break;
        }

        char* buf = buffer + seek;

        if (bytes > 0)
        {
            auto message = reinterpret_cast<const ::profiler::net::Message*>(buf);
            if (!message->isEasyNetMessage())
                continue;

            switch (message->type)
            {
                case profiler::net::MESSAGE_TYPE_ACCEPTED_CONNECTION:
                {
                    qInfo() << "Receive MESSAGE_TYPE_ACCEPTED_CONNECTION";
                    seek += sizeof(profiler::net::Message);
                    break;
                }

                case profiler::net::MESSAGE_TYPE_REPLY_BLOCKS_DESCRIPTION_END:
                {
                    qInfo() << "Receive MESSAGE_TYPE_REPLY_BLOCKS_DESCRIPTION_END";
                    seek += sizeof(profiler::net::Message);

                    seek = 0;
                    bytes = 0;

                    isListen = false;

                    break;
                }

                case profiler::net::MESSAGE_TYPE_REPLY_BLOCKS_DESCRIPTION:
                {
                    qInfo() << "Receive MESSAGE_TYPE_REPLY_BLOCKS";

                    seek += sizeof(profiler::net::DataMessage);
                    profiler::net::DataMessage* dm = (profiler::net::DataMessage*)message;
                    int neededSize = dm->size;

                    buf = buffer + seek;
                    auto bytesNumber = ::std::min((int)dm->size, bytes - seek);
                    m_receivedSize += bytesNumber;
                    m_receivedData.write(buf, bytesNumber);
                    neededSize -= bytesNumber;
                    
                    if (neededSize == 0)
                        seek += bytesNumber;
                    else{
                        seek = 0;
                        bytes = 0;
                    }

                    int loaded = 0;
                    while (neededSize > 0)
                    {
                        bytes = m_easySocket.receive(buffer, buffer_size);

                        if (bytes == -1)
                        {
                            if (m_easySocket.isDisconnected())
                            {
                                m_bConnected.store(false, ::std::memory_order_release);
                                isListen = false;
                                disconnected = true;
                                neededSize = 0;
                            }

                            break;
                        }

                        buf = buffer;
                        int toWrite = ::std::min(bytes, neededSize);
                        m_receivedSize += toWrite;
                        m_receivedData.write(buf, toWrite);
                        neededSize -= toWrite;
                        loaded += toWrite;
                        seek = toWrite;
                    }

                    break;
                }

                default:
                    break;
            }
        }
    }

    if (disconnected)
        clearData();

    delete[] buffer;
}

void EasySocketListener::listenFrameTime()
{
    // TODO: Merge functions listenDescription() and listenCapture()

    static const int buffer_size = sizeof(::profiler::net::TimestampMessage) << 2;
    char buffer[buffer_size] = {};
    int seek = 0, bytes = 0;

    bool isListen = true;
    while (isListen && !m_bInterrupt.load(::std::memory_order_acquire))
    {
        if ((bytes - seek) == 0)
        {
            bytes = m_easySocket.receive(buffer, buffer_size);

            if (bytes == -1)
            {
                if (m_easySocket.isDisconnected())
                {
                    m_bConnected.store(false, ::std::memory_order_release);
                    isListen = false;
                }

                seek = 0;
                bytes = 0;

                continue;
            }

            seek = 0;
        }

        if (bytes == 0)
        {
            isListen = false;
            break;
        }

        char* buf = buffer + seek;

        if (bytes > 0)
        {
            auto message = reinterpret_cast<const ::profiler::net::Message*>(buf);
            if (!message->isEasyNetMessage())
                continue;

            switch (message->type)
            {
                case profiler::net::MESSAGE_TYPE_ACCEPTED_CONNECTION:
                case profiler::net::MESSAGE_TYPE_REPLY_START_CAPTURING:
                {
                    seek += sizeof(profiler::net::Message);
                    break;
                }

                case profiler::net::MESSAGE_TYPE_REPLY_MAIN_FRAME_TIME_MAX_AVG_US:
                {
                    //qInfo() << "Receive MESSAGE_TYPE_REPLY_MAIN_FRAME_TIME_MAX_AVG_US";

                    seek += sizeof(profiler::net::TimestampMessage);
                    if (seek <= buffer_size)
                    {
                        profiler::net::TimestampMessage* timestampMessage = (profiler::net::TimestampMessage*)message;
                        m_frameMax.store(timestampMessage->maxValue, ::std::memory_order_release);
                        m_frameAvg.store(timestampMessage->avgValue, ::std::memory_order_release);
                        m_bFrameTimeReady.store(true, ::std::memory_order_release);
                    }

                    isListen = false;
                    break;
                }

                default:
                    break;
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////


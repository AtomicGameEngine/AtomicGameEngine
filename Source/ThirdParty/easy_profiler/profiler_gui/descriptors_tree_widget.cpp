/************************************************************************
* file name         : descriptors_tree_widget.cpp
* ----------------- :
* creation time     : 2016/09/17
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains implementation of EasyDescTreeWidget and it's auxiliary classes
*                   : for displyaing EasyProfiler blocks descriptors tree.
* ----------------- :
* change log        : * 2016/09/17 Victor Zarubkin: initial commit.
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

#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QHeaderView>
#include <QString>
#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QSignalBlocker>
#include <QSettings>
#include <QLabel>
#include <QLineEdit>
#include <QToolBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <thread>
#include "descriptors_tree_widget.h"
#include "globals.h"

#ifdef _WIN32
#include <Windows.h>

#ifdef __MINGW32__
#include <processthreadsapi.h>
#endif

#endif

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

//////////////////////////////////////////////////////////////////////////

enum DescColumns
{
    DESC_COL_FILE_LINE = 0,
    DESC_COL_TYPE,
    DESC_COL_NAME,
    DESC_COL_STATUS,

    DESC_COL_COLUMNS_NUMBER
};

//////////////////////////////////////////////////////////////////////////

::profiler::EasyBlockStatus nextStatus(::profiler::EasyBlockStatus _status)
{
    switch (_status)
    {
        case ::profiler::OFF:
            return ::profiler::ON;

        case ::profiler::ON:
            return ::profiler::FORCE_ON;

        case ::profiler::FORCE_ON:
            return ::profiler::OFF_RECURSIVE;

        case ::profiler::OFF_RECURSIVE:
            return ::profiler::ON_WITHOUT_CHILDREN;

        case ::profiler::ON_WITHOUT_CHILDREN:
            return ::profiler::FORCE_ON_WITHOUT_CHILDREN;

        case ::profiler::FORCE_ON_WITHOUT_CHILDREN:
            return ::profiler::OFF;
    }

    return ::profiler::OFF;
}

const char* statusText(::profiler::EasyBlockStatus _status)
{
    switch (_status)
    {
        case ::profiler::OFF:
            return "OFF";

        case ::profiler::ON:
            return "ON";

        case ::profiler::FORCE_ON:
            return "FORCE_ON";

        case ::profiler::OFF_RECURSIVE:
            return "OFF_RECURSIVE";

        case ::profiler::ON_WITHOUT_CHILDREN:
            return "ON_WITHOUT_CHILDREN";

        case ::profiler::FORCE_ON_WITHOUT_CHILDREN:
            return "FORCE_ON_WITHOUT_CHILDREN";
    }

    return "";
}

::profiler::color_t statusColor(::profiler::EasyBlockStatus _status)
{
    switch (_status)
    {
        case ::profiler::OFF:
            return ::profiler::colors::Red900;

        case ::profiler::ON:
            return ::profiler::colors::LightGreen900;

        case ::profiler::FORCE_ON:
            return ::profiler::colors::LightGreen900;

        case ::profiler::OFF_RECURSIVE:
            return ::profiler::colors::Red900;

        case ::profiler::ON_WITHOUT_CHILDREN:
            return ::profiler::colors::Lime900;

        case ::profiler::FORCE_ON_WITHOUT_CHILDREN:
            return ::profiler::colors::Lime900;
    }

    return ::profiler::colors::Black;
}

//////////////////////////////////////////////////////////////////////////

EasyDescWidgetItem::EasyDescWidgetItem(::profiler::block_id_t _desc, Parent* _parent) : Parent(_parent), m_desc(_desc)
{

}

EasyDescWidgetItem::~EasyDescWidgetItem()
{

}

bool EasyDescWidgetItem::operator < (const Parent& _other) const
{
    const auto col = treeWidget()->sortColumn();

    switch (col)
    {
        case DESC_COL_FILE_LINE:
        {
            if (parent() != nullptr)
                return data(col, Qt::UserRole).toInt() < _other.data(col, Qt::UserRole).toInt();
        }
    }

    return Parent::operator < (_other);
}

//////////////////////////////////////////////////////////////////////////

EasyDescTreeWidget::EasyDescTreeWidget(QWidget* _parent)
    : Parent(_parent)
    , m_lastFound(nullptr)
    , m_lastSearchColumn(-1)
    , m_searchColumn(DESC_COL_NAME)
    , m_bLocked(false)
{
    setAutoFillBackground(false);
    setAlternatingRowColors(true);
    setItemsExpandable(true);
    setAnimated(true);
    setSortingEnabled(false);
    setColumnCount(DESC_COL_COLUMNS_NUMBER);

    auto header_item = new QTreeWidgetItem();
    header_item->setText(DESC_COL_FILE_LINE, "File/Line");
    header_item->setText(DESC_COL_TYPE, "Type");
    header_item->setText(DESC_COL_NAME, "Name");
    header_item->setText(DESC_COL_STATUS, "Status");
    setHeaderItem(header_item);

    connect(&EASY_GLOBALS.events, &::profiler_gui::EasyGlobalSignals::selectedBlockChanged, this, &This::onSelectedBlockChange);
    connect(&EASY_GLOBALS.events, &::profiler_gui::EasyGlobalSignals::blockStatusChanged, this, &This::onBlockStatusChange);
    connect(this, &Parent::itemExpanded, this, &This::onItemExpand);
    connect(this, &Parent::itemDoubleClicked, this, &This::onDoubleClick);
    connect(this, &Parent::currentItemChanged, this, &This::onCurrentItemChange);

    loadSettings();
}

EasyDescTreeWidget::~EasyDescTreeWidget()
{
    if (::profiler_gui::is_max(EASY_GLOBALS.selected_block) && !::profiler_gui::is_max(EASY_GLOBALS.selected_block_id))
    {
        ::profiler_gui::set_max(EASY_GLOBALS.selected_block_id);
        emit EASY_GLOBALS.events.refreshRequired();
    }

    saveSettings();
}

//////////////////////////////////////////////////////////////////////////

void EasyDescTreeWidget::contextMenuEvent(QContextMenuEvent* _event)
{
    _event->accept();

    QMenu menu;
    menu.setToolTipsVisible(true);
    auto action = menu.addAction("Expand all");
    SET_ICON(action, ":/Expand");
    connect(action, &QAction::triggered, this, &This::expandAll);

    action = menu.addAction("Collapse all");
    SET_ICON(action, ":/Collapse");
    connect(action, &QAction::triggered, this, &This::collapseAll);

    menu.addSeparator();
    auto submenu = menu.addMenu("Search by");
    auto header_item = headerItem();
    for (int i = 0; i < DESC_COL_STATUS; ++i)
    {
        if (i == DESC_COL_TYPE)
            continue;

        action = submenu->addAction(header_item->text(i));
        action->setData(i);
        action->setCheckable(true);
        if (i == m_searchColumn)
            action->setChecked(true);
        connect(action, &QAction::triggered, this, &This::onSearchColumnChange);
    }

    auto item = currentItem();
    if (item != nullptr && item->parent() != nullptr && currentColumn() >= DESC_COL_TYPE)
    {
        const auto& desc = easyDescriptor(static_cast<EasyDescWidgetItem*>(item)->desc());

        menu.addSeparator();
        auto submenu = menu.addMenu("Change status");
        submenu->setToolTipsVisible(true);

#define ADD_STATUS_ACTION(NameValue, StatusValue, ToolTipValue)\
        action = submenu->addAction(NameValue);\
        action->setCheckable(true);\
        action->setChecked(desc.status() == StatusValue);\
        action->setData(static_cast<quint32>(StatusValue));\
        action->setToolTip(ToolTipValue);\
        connect(action, &QAction::triggered, this, &This::onBlockStatusChangeClicked)

        ADD_STATUS_ACTION("Off", ::profiler::OFF, "Do not profile this block.");
        ADD_STATUS_ACTION("On", ::profiler::ON, "Profile this block\nif parent enabled children.");
        ADD_STATUS_ACTION("Force-On", ::profiler::FORCE_ON, "Always profile this block even\nif it's parent disabled children.");
        ADD_STATUS_ACTION("Off-recursive", ::profiler::OFF_RECURSIVE, "Do not profile neither this block\nnor it's children.");
        ADD_STATUS_ACTION("On-without-children", ::profiler::ON_WITHOUT_CHILDREN, "Profile this block, but\ndo not profile it's children.");
        ADD_STATUS_ACTION("Force-On-without-children", ::profiler::FORCE_ON_WITHOUT_CHILDREN, "Always profile this block, but\ndo not profile it's children.");
#undef ADD_STATUS_ACTION

        submenu->setEnabled(EASY_GLOBALS.connected);
        if (!EASY_GLOBALS.connected)
            submenu->setTitle(QString("%1 (connection needed)").arg(submenu->title()));
    }

    menu.exec(QCursor::pos());
}

void EasyDescTreeWidget::onSearchColumnChange(bool)
{
    auto action = qobject_cast<QAction*>(sender());
    if (action != nullptr)
        m_searchColumn = action->data().toInt();
}

//////////////////////////////////////////////////////////////////////////

void EasyDescTreeWidget::clearSilent(bool _global)
{
    const QSignalBlocker b(this);

    setSortingEnabled(false);
    m_lastFound = nullptr;
    m_lastSearch.clear();

    m_highlightItems.clear();
    m_items.clear();

    ::std::vector<QTreeWidgetItem*> topLevelItems;
    topLevelItems.reserve(topLevelItemCount());
    for (int i = topLevelItemCount() - 1; i >= 0; --i)
    {
        const bool expanded = !_global && topLevelItem(i)->isExpanded();
        auto item = takeTopLevelItem(i);
        if (expanded)
            m_expandedFilesTemp.insert(item->text(DESC_COL_FILE_LINE).toStdString());
        topLevelItems.push_back(item);
    }

    auto deleter_thread = ::std::thread([](decltype(topLevelItems) _items)
    {
#ifdef _WIN32
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);
#endif

        for (auto item : _items)
            delete item;

    }, ::std::move(topLevelItems));

    deleter_thread.detach();

    //clear();
}

//////////////////////////////////////////////////////////////////////////

struct FileItems
{
    typedef ::std::unordered_map<int, EasyDescWidgetItem*, ::profiler::passthrough_hash<int> > Items;
    Items children;
    QTreeWidgetItem* item = nullptr;
};

void EasyDescTreeWidget::build()
{
    auto f = font();
    f.setBold(true);

    typedef ::std::unordered_map<::std::string, FileItems> Files;
    Files fileItems;

    m_items.resize(EASY_GLOBALS.descriptors.size());
    memset(m_items.data(), 0, sizeof(void*) * m_items.size());

    const QSignalBlocker b(this);
    ::profiler::block_id_t id = 0;
    for (auto desc : EASY_GLOBALS.descriptors)
    {
        if (desc != nullptr)
        {
            auto& p = fileItems[desc->file()];
            if (p.item == nullptr)
            {
                p.item = new QTreeWidgetItem();
                p.item->setText(DESC_COL_FILE_LINE, QString(desc->file()).remove(QRegExp("^(\\.{2}\\\\+|\\/+)+")));
                p.item->setText(DESC_COL_TYPE, "F");
                p.item->setToolTip(DESC_COL_TYPE, "File");
            }

            auto it = p.children.find(desc->line());
            if (it == p.children.end())
            {
                auto item = new EasyDescWidgetItem(desc->id(), p.item);
                item->setText(DESC_COL_FILE_LINE, QString::number(desc->line()));
                item->setData(DESC_COL_FILE_LINE, Qt::UserRole, desc->line());
                item->setText(DESC_COL_NAME, desc->name());

                if (desc->type() == ::profiler::BLOCK_TYPE_BLOCK)
                {
                    item->setText(DESC_COL_TYPE, "B");
                    item->setToolTip(DESC_COL_TYPE, "Block");
                }
                else
                {
                    item->setText(DESC_COL_TYPE, "E");
                    item->setToolTip(DESC_COL_TYPE, "Event");
                }

                item->setFont(DESC_COL_STATUS, f);
                item->setText(DESC_COL_STATUS, statusText(desc->status()));
                item->setForeground(DESC_COL_STATUS, QColor::fromRgba(statusColor(desc->status())));

                m_items[id] = item;
                p.children.insert(::std::make_pair(desc->line(), item));
            }
            else
            {
                m_items[id] = it->second;
            }
        }

        ++id;
    }

    for (auto& p : fileItems)
    {
        addTopLevelItem(p.second.item);
        if (m_expandedFilesTemp.find(p.first) != m_expandedFilesTemp.end())
            p.second.item->setExpanded(true);
    }

    m_expandedFilesTemp.clear();
    setSortingEnabled(true);
    sortByColumn(DESC_COL_FILE_LINE, Qt::AscendingOrder);
    resizeColumnsToContents();
    QTimer::singleShot(100, [this](){ onSelectedBlockChange(EASY_GLOBALS.selected_block); });
}

//////////////////////////////////////////////////////////////////////////

void EasyDescTreeWidget::onItemExpand(QTreeWidgetItem*)
{
    resizeColumnsToContents();
}

//////////////////////////////////////////////////////////////////////////

void EasyDescTreeWidget::onDoubleClick(QTreeWidgetItem* _item, int _column)
{
    if (!EASY_GLOBALS.connected)
        return;

    if (_column >= DESC_COL_TYPE && _item->parent() != nullptr)
    {
        auto item = static_cast<EasyDescWidgetItem*>(_item);
        auto& desc = easyDescriptor(item->desc());
        desc.setStatus(nextStatus(desc.status()));

        item->setText(DESC_COL_STATUS, statusText(desc.status()));
        item->setForeground(DESC_COL_STATUS, QColor::fromRgba(statusColor(desc.status())));

        m_bLocked = true;
        emit EASY_GLOBALS.events.blockStatusChanged(desc.id(), desc.status());
        m_bLocked = false;
    }
}

//////////////////////////////////////////////////////////////////////////

void EasyDescTreeWidget::onCurrentItemChange(QTreeWidgetItem* _item, QTreeWidgetItem* _prev)
{
    if (_prev != nullptr)
    {
        auto f = font();
        for (int i = 0; i < DESC_COL_STATUS; ++i)
            _prev->setFont(i, f);
    }

    if (_item != nullptr)
    {
        auto f = font();
        f.setBold(true);
        for (int i = 0; i < DESC_COL_STATUS; ++i)
            _item->setFont(i, f);

        if (::profiler_gui::is_max(EASY_GLOBALS.selected_block) && _item->parent() != nullptr)
        {
            const auto id = static_cast<EasyDescWidgetItem*>(_item)->desc();
            if (EASY_GLOBALS.selected_block_id != id)
            {
                EASY_GLOBALS.selected_block_id = id;
                emit EASY_GLOBALS.events.selectedBlockIdChanged(id);
            }
        }
    }
    else if (::profiler_gui::is_max(EASY_GLOBALS.selected_block) && !::profiler_gui::is_max(EASY_GLOBALS.selected_block_id))
    {
        ::profiler_gui::set_max(EASY_GLOBALS.selected_block_id);
        emit EASY_GLOBALS.events.selectedBlockIdChanged(EASY_GLOBALS.selected_block_id);
    }
}

//////////////////////////////////////////////////////////////////////////

void EasyDescTreeWidget::onBlockStatusChangeClicked(bool _checked)
{
    if (!_checked || !EASY_GLOBALS.connected)
        return;

    auto item = currentItem();
    if (item == nullptr || item->parent() == nullptr)
        return;

    auto action = qobject_cast<QAction*>(sender());
    if (action != nullptr)
    {
        auto& desc = easyDescriptor(static_cast<EasyDescWidgetItem*>(item)->desc());
        desc.setStatus(static_cast<::profiler::EasyBlockStatus>(action->data().toUInt()));
        item->setText(DESC_COL_STATUS, statusText(desc.status()));
        item->setForeground(DESC_COL_STATUS, QColor::fromRgba(statusColor(desc.status())));

        m_bLocked = true;
        emit EASY_GLOBALS.events.blockStatusChanged(desc.id(), desc.status());
        m_bLocked = false;
    }
}

void EasyDescTreeWidget::onBlockStatusChange(::profiler::block_id_t _id, ::profiler::EasyBlockStatus _status)
{
    if (m_bLocked)
        return;

    auto item = m_items[_id];
    if (item == nullptr)
        return;

    auto& desc = easyDescriptor(item->desc());
    item->setText(DESC_COL_STATUS, statusText(desc.status()));
    item->setForeground(DESC_COL_STATUS, QColor::fromRgba(statusColor(desc.status())));
}

//////////////////////////////////////////////////////////////////////////

void EasyDescTreeWidget::resizeColumnsToContents()
{
    for (int i = 0; i < DESC_COL_COLUMNS_NUMBER; ++i)
        resizeColumnToContents(i);
}

//////////////////////////////////////////////////////////////////////////

void EasyDescTreeWidget::onSelectedBlockChange(uint32_t _block_index)
{
    if (::profiler_gui::is_max(_block_index))
        return;

    auto item = m_items[blocksTree(_block_index).node->id()];
    if (item == nullptr)
        return;

    scrollToItem(item, QAbstractItemView::PositionAtCenter);
    setCurrentItem(item);
}

//////////////////////////////////////////////////////////////////////////

void EasyDescTreeWidget::resetHighlight()
{
    for (auto item : m_highlightItems) {
        for (int i = 0; i < DESC_COL_COLUMNS_NUMBER; ++i)
            item->setBackground(i, Qt::NoBrush);
    }
    m_highlightItems.clear();
}

void EasyDescTreeWidget::loadSettings()
{
    QSettings settings(::profiler_gui::ORGANAZATION_NAME, ::profiler_gui::APPLICATION_NAME);
    settings.beginGroup("desc_tree_widget");

    auto val = settings.value("searchColumn");
    if (!val.isNull())
        m_searchColumn = val.toInt();

    settings.endGroup();
}

void EasyDescTreeWidget::saveSettings()
{
    QSettings settings(::profiler_gui::ORGANAZATION_NAME, ::profiler_gui::APPLICATION_NAME);
    settings.beginGroup("desc_tree_widget");

    settings.setValue("searchColumn", m_searchColumn);

    settings.endGroup();
}

//////////////////////////////////////////////////////////////////////////

int EasyDescTreeWidget::findNext(const QString& _str, Qt::MatchFlags _flags)
{
    if (_str.isEmpty())
    {
        resetHighlight();
        m_lastSearchColumn = m_searchColumn;
        return 0;
    }

    const bool isNewSearch = (m_lastSearchColumn != m_searchColumn || m_lastSearch != _str);
    auto itemsList = findItems(_str, Qt::MatchContains | Qt::MatchRecursive | _flags, m_searchColumn);

    if (!isNewSearch)
    {
        if (!itemsList.empty())
        {
            bool stop = false;
            decltype(m_lastFound) next = nullptr;
            for (auto item : itemsList)
            {
                if (stop)
                {
                    next = item;
                    break;
                }

                stop = item == m_lastFound;
            }

            m_lastFound = next == nullptr ? itemsList.front() : next;
        }
        else
        {
            m_lastFound = nullptr;
        }
    }
    else
    {
        resetHighlight();

        m_lastSearchColumn = m_searchColumn;
        m_lastSearch = _str;
        m_lastFound = !itemsList.empty() ? itemsList.front() : nullptr;

        for (auto item : itemsList)
        {
            m_highlightItems.push_back(item);
            for (int i = 0; i < DESC_COL_COLUMNS_NUMBER; ++i)
                item->setBackgroundColor(i, QColor::fromRgba(0x80000000 | (0x00ffffff & ::profiler::colors::Yellow)));
        }
    }

    if (m_lastFound != nullptr)
    {
        scrollToItem(m_lastFound, QAbstractItemView::PositionAtCenter);
        setCurrentItem(m_lastFound);
    }

    return itemsList.size();
}

int EasyDescTreeWidget::findPrev(const QString& _str, Qt::MatchFlags _flags)
{
    if (_str.isEmpty())
    {
        resetHighlight();
        m_lastSearchColumn = m_searchColumn;
        return 0;
    }

    const bool isNewSearch = (m_lastSearchColumn != m_searchColumn || m_lastSearch != _str);
    auto itemsList = findItems(_str, Qt::MatchContains | Qt::MatchRecursive | _flags, m_searchColumn);

    if (!isNewSearch)
    {
        if (!itemsList.empty())
        {
            decltype(m_lastFound) prev = nullptr;
            for (auto item : itemsList)
            {
                if (item == m_lastFound)
                    break;

                prev = item;
            }

            m_lastFound = prev == nullptr ? itemsList.back() : prev;
        }
        else
        {
            m_lastFound = nullptr;
        }
    }
    else
    {
        resetHighlight();

        m_lastSearchColumn = m_searchColumn;
        m_lastSearch = _str;
        m_lastFound = !itemsList.empty() ? itemsList.front() : nullptr;

        m_highlightItems.reserve(itemsList.size());
        for (auto item : itemsList)
        {
            m_highlightItems.push_back(item);
            for (int i = 0; i < DESC_COL_COLUMNS_NUMBER; ++i)
                item->setBackgroundColor(i, QColor::fromRgba(0x80000000 | (0x00ffffff & ::profiler::colors::Yellow)));
        }
    }

    if (m_lastFound != nullptr)
    {
        scrollToItem(m_lastFound, QAbstractItemView::PositionAtCenter);
        setCurrentItem(m_lastFound);
    }

    return itemsList.size();
}

//////////////////////////////////////////////////////////////////////////

EasyDescWidget::EasyDescWidget(QWidget* _parent) : Parent(_parent)
    , m_tree(new EasyDescTreeWidget(this))
    , m_searchBox(new QLineEdit(this))
    , m_foundNumber(new QLabel("Found 0 matches", this))
    , m_searchButton(nullptr)
    , m_bCaseSensitiveSearch(false)
{
    loadSettings();

    m_searchBox->setFixedWidth(200);
    m_searchBox->setContentsMargins(5, 0, 0, 0);

    auto tb = new QToolBar(this);
    tb->setIconSize(::profiler_gui::ICONS_SIZE);
    auto refreshButton = tb->addAction(QIcon(":/Reload"), tr("Refresh blocks list"));
    refreshButton->setEnabled(EASY_GLOBALS.connected);
    refreshButton->setToolTip(tr("Refresh blocks list.\nConnection needed."));
    connect(refreshButton, &QAction::triggered, &EASY_GLOBALS.events, &::profiler_gui::EasyGlobalSignals::blocksRefreshRequired);



    QMenu* menu = new QMenu(this);
    m_searchButton = menu->menuAction();
    m_searchButton->setText("Find next");
    m_searchButton->setIcon(QIcon(":/Search-next"));
    m_searchButton->setData(true);
    connect(m_searchButton, &QAction::triggered, this, &This::findNext);

    auto actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    auto a = new QAction(tr("Find next"), actionGroup);
    a->setCheckable(true);
    a->setChecked(true);
    connect(a, &QAction::triggered, this, &This::findNextFromMenu);
    menu->addAction(a);

    a = new QAction(tr("Find previous"), actionGroup);
    a->setCheckable(true);
    connect(a, &QAction::triggered, this, &This::findPrevFromMenu);
    menu->addAction(a);

    menu->addSeparator();
    a = menu->addAction("Case sensitive");
    a->setCheckable(true);
    a->setChecked(m_bCaseSensitiveSearch);
    connect(a, &QAction::triggered, [this](bool _checked){ m_bCaseSensitiveSearch = _checked; });
    menu->addAction(a);

    tb->addSeparator();
    tb->addAction(m_searchButton);
    tb->addWidget(m_searchBox);

    auto searchbox = new QHBoxLayout();
    searchbox->setContentsMargins(0, 0, 5, 0);
    searchbox->addWidget(tb);
    searchbox->addStretch(100);
    searchbox->addWidget(m_foundNumber, Qt::AlignRight);

    auto lay = new QVBoxLayout(this);
    lay->setContentsMargins(1, 1, 1, 1);
    lay->addLayout(searchbox);
    lay->addWidget(m_tree);

    connect(m_searchBox, &QLineEdit::returnPressed, this, &This::onSeachBoxReturnPressed);
    connect(&EASY_GLOBALS.events, &::profiler_gui::EasyGlobalSignals::connectionChanged, refreshButton, &QAction::setEnabled);
}

EasyDescWidget::~EasyDescWidget()
{
    saveSettings();
}

void EasyDescWidget::loadSettings()
{
    QSettings settings(::profiler_gui::ORGANAZATION_NAME, ::profiler_gui::APPLICATION_NAME);
    settings.beginGroup("EasyDescWidget");

    auto val = settings.value("case_sensitive");
    if (!val.isNull())
        m_bCaseSensitiveSearch = val.toBool();

    settings.endGroup();
}

void EasyDescWidget::saveSettings()
{
    QSettings settings(::profiler_gui::ORGANAZATION_NAME, ::profiler_gui::APPLICATION_NAME);
    settings.beginGroup("EasyDescWidget");
    settings.setValue("case_sensitive", m_bCaseSensitiveSearch);
    settings.endGroup();
}

void EasyDescWidget::keyPressEvent(QKeyEvent* _event)
{
    if (_event->key() == Qt::Key_F3)
    {
        if (_event->modifiers() & Qt::ShiftModifier)
            findPrev(true);
        else
            findNext(true);
    }

    _event->accept();
}

void EasyDescWidget::contextMenuEvent(QContextMenuEvent* _event)
{
    m_tree->contextMenuEvent(_event);
}

void EasyDescWidget::build()
{
    m_tree->clearSilent(false);
    m_foundNumber->setText(QString("Found 0 matches"));
    m_tree->build();
}

void EasyDescWidget::clear()
{
    m_tree->clearSilent(true);
    m_foundNumber->setText(QString("Found 0 matches"));
}

void EasyDescWidget::onSeachBoxReturnPressed()
{
    if (m_searchButton->data().toBool() == true)
        findNext(true);
    else
        findPrev(true);
}

void EasyDescWidget::findNext(bool)
{
    auto matches = m_tree->findNext(m_searchBox->text(), m_bCaseSensitiveSearch ? Qt::MatchCaseSensitive : Qt::MatchFlags());

    if (matches == 1)
        m_foundNumber->setText(QString("Found 1 match"));
    else
        m_foundNumber->setText(QString("Found %1 matches").arg(matches));
}

void EasyDescWidget::findPrev(bool)
{
    auto matches = m_tree->findPrev(m_searchBox->text(), m_bCaseSensitiveSearch ? Qt::MatchCaseSensitive : Qt::MatchFlags());

    if (matches == 1)
        m_foundNumber->setText(QString("Found 1 match"));
    else
        m_foundNumber->setText(QString("Found %1 matches").arg(matches));
}

void EasyDescWidget::findNextFromMenu(bool _checked)
{
    if (!_checked)
        return;

    if (m_searchButton->data().toBool() == false)
    {
        m_searchButton->setData(true);
        m_searchButton->setText(tr("Find next"));
        m_searchButton->setIcon(QIcon(":/Search-next"));
        disconnect(m_searchButton, &QAction::triggered, this, &This::findPrev);
        connect(m_searchButton, &QAction::triggered, this, &This::findNext);
    }

    findNext(true);
}

void EasyDescWidget::findPrevFromMenu(bool _checked)
{
    if (!_checked)
        return;

    if (m_searchButton->data().toBool() == true)
    {
        m_searchButton->setData(false);
        m_searchButton->setText(tr("Find prev"));
        m_searchButton->setIcon(QIcon(":/Search-prev"));
        disconnect(m_searchButton, &QAction::triggered, this, &This::findNext);
        connect(m_searchButton, &QAction::triggered, this, &This::findPrev);
    }

    findPrev(true);
}

//////////////////////////////////////////////////////////////////////////

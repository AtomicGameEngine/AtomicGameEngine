/************************************************************************
* file name         : tree_widget_loader.h
* ----------------- :
* creation time     : 2016/08/18
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains implementation of EasyTreeWidgetLoader which aim is
*                   : to load EasyProfiler blocks hierarchy in separate thread.
* ----------------- :
* change log        : * 2016/08/18 Victor Zarubkin: moved sources from blocks_tree_widget.h/.cpp
*                   :       and renamed Prof* to Easy*.
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

#include "tree_widget_loader.h"
#include "tree_widget_item.h"
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

EasyTreeWidgetLoader::EasyTreeWidgetLoader()
    : m_bDone(ATOMIC_VAR_INIT(false))
    , m_bInterrupt(ATOMIC_VAR_INIT(false))
    , m_progress(ATOMIC_VAR_INIT(0))
    , m_mode(EasyTreeMode_Full)
{
}

EasyTreeWidgetLoader::~EasyTreeWidgetLoader()
{
    interrupt(true);
}

bool EasyTreeWidgetLoader::done() const
{
    return m_bDone.load();
}

void EasyTreeWidgetLoader::setDone()
{
    m_bDone.store(true);
    //m_progress.store(100);
}

void EasyTreeWidgetLoader::setProgress(int _progress)
{
    m_progress.store(_progress);
}

bool EasyTreeWidgetLoader::interrupted() const
{
    return m_bInterrupt.load();
}

int EasyTreeWidgetLoader::progress() const
{
    return m_progress.load();
}

void EasyTreeWidgetLoader::takeTopLevelItems(ThreadedItems& _output)
{
    if (done())
    {
        _output = ::std::move(m_topLevelItems);
        m_topLevelItems.clear();
    }
}

void EasyTreeWidgetLoader::takeItems(Items& _output)
{
    if (done())
    {
        _output = ::std::move(m_items);
        m_items.clear();
    }
}

void EasyTreeWidgetLoader::interrupt(bool _wait)
{
    m_bInterrupt.store(true);
    if (m_thread.joinable())
        m_thread.join();

    m_bInterrupt.store(false);
    m_bDone.store(false);
    m_progress.store(0);

    if (!_wait)
    {
        auto deleter_thread = ::std::thread([](decltype(m_topLevelItems) _items)
        {
#ifdef _WIN32
            SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);
#endif

            for (auto item : _items)
                delete item.second;

        }, ::std::move(m_topLevelItems));

        deleter_thread.detach();
    }
    else
    {
        for (auto item : m_topLevelItems)
            delete item.second;
    }

    m_items.clear();
    m_topLevelItems.clear();
    m_iditems.clear();
}

void EasyTreeWidgetLoader::fillTree(::profiler::timestamp_t& _beginTime, const unsigned int _blocksNumber, const ::profiler::thread_blocks_tree_t& _blocksTree, bool _colorizeRows, EasyTreeMode _mode)
{
    interrupt();
    m_mode = _mode;
    m_thread = ::std::thread(&EasyTreeWidgetLoader::setTreeInternal1, this,
        ::std::ref(_beginTime), _blocksNumber, ::std::ref(_blocksTree), _colorizeRows,
        EASY_GLOBALS.add_zero_blocks_to_hierarchy, EASY_GLOBALS.use_decorated_thread_name, EASY_GLOBALS.hex_thread_id, EASY_GLOBALS.time_units);
}

void EasyTreeWidgetLoader::fillTreeBlocks(const::profiler_gui::TreeBlocks& _blocks, ::profiler::timestamp_t _beginTime, ::profiler::timestamp_t _left, ::profiler::timestamp_t _right, bool _strict, bool _colorizeRows, EasyTreeMode _mode)
{
    interrupt();
    m_mode = _mode;
    m_thread = ::std::thread(&EasyTreeWidgetLoader::setTreeInternal2, this,
        _beginTime, ::std::ref(_blocks), _left, _right, _strict, _colorizeRows,
        EASY_GLOBALS.add_zero_blocks_to_hierarchy, EASY_GLOBALS.use_decorated_thread_name, EASY_GLOBALS.hex_thread_id, EASY_GLOBALS.time_units);
}

//////////////////////////////////////////////////////////////////////////

void EasyTreeWidgetLoader::setTreeInternal1(::profiler::timestamp_t& _beginTime, const unsigned int _blocksNumber, const ::profiler::thread_blocks_tree_t& _blocksTree, bool _colorizeRows, bool _addZeroBlocks, bool _decoratedThreadNames, bool _hexThreadId, ::profiler_gui::TimeUnits _units)
{
    m_items.reserve(_blocksNumber + _blocksTree.size()); // _blocksNumber does not include Thread root blocks

    ::profiler::timestamp_t finishtime = 0;
    for (const auto& threadTree : _blocksTree)
    {
        const auto node_block = blocksTree(threadTree.second.children.front()).node;
        const auto startTime = node_block->begin();
        const auto endTime = node_block->end();

        if (_beginTime > startTime)
            _beginTime = startTime;

        if (finishtime < endTime)
            finishtime = endTime;
    }

    //const QSignalBlocker b(this);
    const auto u_thread = ::profiler_gui::toUnicode("thread");
    int i = 0;
    const int total = static_cast<int>(_blocksTree.size());
    for (const auto& threadTree : _blocksTree)
    {
        if (interrupted())
            break;

        const auto& root = threadTree.second;
        auto item = new EasyTreeWidgetItem();
        item->setText(COL_NAME, ::profiler_gui::decoratedThreadName(_decoratedThreadNames, root, u_thread, _hexThreadId));

        ::profiler::timestamp_t duration = 0;
        if (!root.children.empty())
            duration = blocksTree(root.children.back()).node->end() - blocksTree(root.children.front()).node->begin();

        item->setTimeSmart(COL_DURATION, _units, duration);
        item->setBackgroundColor(::profiler_gui::SELECTED_THREAD_BACKGROUND);
        item->setTextColor(::profiler_gui::SELECTED_THREAD_FOREGROUND);

        //_items.push_back(item);

        item->setTimeSmart(COL_SELF_DURATION, _units, root.profiled_time);

        ::profiler::timestamp_t children_duration = 0;
        const auto children_items_number = setTreeInternal(root, 0, _beginTime, root.children, item, nullptr, _beginTime, finishtime + 1000000000ULL, false, children_duration, _colorizeRows, _addZeroBlocks, _units);

        if (children_items_number > 0)
        {
            //total_items += children_items_number + 1;
            //addTopLevelItem(item);
            //m_roots[threadTree.first] = item;
            m_topLevelItems.emplace_back(root.thread_id, item);
        }
        else
        {
            //_items.pop_back();
            delete item;
        }

        setProgress((100 * ++i) / total);
    }

    setDone();
    //return total_items;
}

//////////////////////////////////////////////////////////////////////////

// auto calculateTotalChildrenNumber(const ::profiler::BlocksTree& _tree) -> decltype(_tree.children.size())
// {
//     auto children_number = _tree.children.size();
//     for (auto i : _tree.children)
//         children_number += calculateTotalChildrenNumber(blocksTree(i));
//     return children_number;
// }

typedef ::std::unordered_map<::profiler::thread_id_t, ::profiler::block_index_t, ::profiler::passthrough_hash<::profiler::thread_id_t> > BeginEndIndicesMap;

void EasyTreeWidgetLoader::setTreeInternal2(const ::profiler::timestamp_t& _beginTime, const ::profiler_gui::TreeBlocks& _blocks, ::profiler::timestamp_t _left, ::profiler::timestamp_t _right, bool _strict, bool _colorizeRows, bool _addZeroBlocks, bool _decoratedThreadNames, bool _hexThreadId, ::profiler_gui::TimeUnits _units)
{
    //size_t blocksNumber = 0;
    //for (const auto& block : _blocks)
    //    blocksNumber += calculateTotalChildrenNumber(*block.tree);
    //    //blocksNumber += block.tree->total_children_number;
    //m_items.reserve(blocksNumber + _blocks.size()); // blocksNumber does not include root blocks

    BeginEndIndicesMap beginEndMap;
    RootsMap threadsMap;

    auto const setTree = (m_mode == EasyTreeMode_Full) ? &EasyTreeWidgetLoader::setTreeInternal : &EasyTreeWidgetLoader::setTreeInternalPlain;

    const auto u_thread = ::profiler_gui::toUnicode("thread");
    int i = 0, total = static_cast<int>(_blocks.size());
    //const QSignalBlocker b(this);
    for (const auto& block : _blocks)
    {
        if (interrupted())
            break;

        auto& gui_block = easyBlock(block.tree);
        const auto startTime = gui_block.tree.node->begin();
        const auto endTime = gui_block.tree.node->end();
        if (startTime > _right || endTime < _left)
        {
            setProgress((90 * ++i) / total);
            continue;
        }

        ::profiler::timestamp_t duration = 0;
        EasyTreeWidgetItem* thread_item = nullptr;
        ::profiler::block_index_t& firstCswitch = beginEndMap[block.root->thread_id];
        auto thread_item_it = threadsMap.find(block.root->thread_id);
        if (thread_item_it != threadsMap.end())
        {
            thread_item = thread_item_it->second;
        }
        else
        {
            thread_item = new EasyTreeWidgetItem();
            thread_item->setText(COL_NAME, ::profiler_gui::decoratedThreadName(_decoratedThreadNames, *block.root, u_thread, _hexThreadId));

            if (!block.root->children.empty())
                duration = blocksTree(block.root->children.back()).node->end() - blocksTree(block.root->children.front()).node->begin();

            thread_item->setTimeSmart(COL_DURATION, _units, duration);
            thread_item->setBackgroundColor(::profiler_gui::SELECTED_THREAD_BACKGROUND);
            thread_item->setTextColor(::profiler_gui::SELECTED_THREAD_FOREGROUND);

            // Sum of all children durations:
            thread_item->setTimeSmart(COL_SELF_DURATION, _units, block.root->profiled_time);

            threadsMap.insert(::std::make_pair(block.root->thread_id, thread_item));

            firstCswitch = 0;
            auto it = ::std::lower_bound(block.root->sync.begin(), block.root->sync.end(), _left, [](::profiler::block_index_t ind, decltype(_left) _val)
            {
                return EASY_GLOBALS.gui_blocks[ind].tree.node->begin() < _val;
            });

            if (it != block.root->sync.end())
            {
                firstCswitch = it - block.root->sync.begin();
                if (firstCswitch > 0)
                    --firstCswitch;
            }
            else
            {
                firstCswitch = static_cast<::profiler::block_index_t>(block.root->sync.size());
            }
        }

        bool hasContextSwitch = false;
        ::profiler::timestamp_t idleTime = 0;
        for (::profiler::block_index_t ind = firstCswitch, ncs = static_cast<::profiler::block_index_t>(block.root->sync.size()); ind < ncs; ++ind)
        {
            auto cs_index = block.root->sync[ind];
            const auto cs = EASY_GLOBALS.gui_blocks[cs_index].tree.node;

            if (cs->begin() > endTime)
            {
                if (!hasContextSwitch)
                    firstCswitch = ind;
                break;
            }

            if (startTime <= cs->begin() && cs->end() <= endTime)
            {
                if (!hasContextSwitch)
                {
                    firstCswitch = ind;
                    hasContextSwitch = true;
                }

                idleTime += cs->duration();
            }
        }

        auto item = new EasyTreeWidgetItem(block.tree, thread_item);
        duration = endTime - startTime;

        auto name = *gui_block.tree.node->name() != 0 ? gui_block.tree.node->name() : easyDescriptor(gui_block.tree.node->id()).name();
        item->setText(COL_NAME, ::profiler_gui::toUnicode(name));
        item->setTimeSmart(COL_DURATION, _units, duration);

        auto active_time = duration - idleTime;
        auto active_percent = duration == 0 ? 100. : ::profiler_gui::percentReal(active_time, duration);
        item->setTimeSmart(COL_ACTIVE_TIME, _units, active_time);
        item->setText(COL_ACTIVE_PERCENT, QString::number(active_percent, 'g', 3));
        item->setData(COL_ACTIVE_PERCENT, Qt::UserRole, active_percent);

        item->setTimeMs(COL_BEGIN, startTime - _beginTime);
        item->setTimeMs(COL_END, endTime - _beginTime);

        item->setData(COL_PERCENT_PER_FRAME, Qt::UserRole, 0);

        auto percentage_per_thread = ::profiler_gui::percent(duration, block.root->profiled_time);
        item->setData(COL_PERCENT_PER_PARENT, Qt::UserRole, percentage_per_thread);
        item->setText(COL_PERCENT_PER_PARENT, QString::number(percentage_per_thread));

        if (gui_block.tree.per_thread_stats != nullptr) // if there is per_thread_stats then there are other stats also
        {
            const ::profiler::BlockStatistics* per_thread_stats = gui_block.tree.per_thread_stats;
            const ::profiler::BlockStatistics* per_parent_stats = gui_block.tree.per_parent_stats;
            const ::profiler::BlockStatistics* per_frame_stats = gui_block.tree.per_frame_stats;


            if (per_thread_stats->calls_number > 1 || !EASY_GLOBALS.display_only_relevant_stats)
            {
                item->setTimeSmart(COL_MIN_PER_THREAD, _units, easyBlock(per_thread_stats->min_duration_block).tree.node->duration());
                item->setTimeSmart(COL_MAX_PER_THREAD, _units, easyBlock(per_thread_stats->max_duration_block).tree.node->duration());
                item->setTimeSmart(COL_AVERAGE_PER_THREAD, _units, per_thread_stats->average_duration());
                item->setTimeSmart(COL_DURATION_SUM_PER_THREAD, _units, per_thread_stats->total_duration);
            }

            item->setData(COL_NCALLS_PER_THREAD, Qt::UserRole, per_thread_stats->calls_number);
            item->setText(COL_NCALLS_PER_THREAD, QString::number(per_thread_stats->calls_number));

            percentage_per_thread = ::profiler_gui::percent(per_thread_stats->total_duration, block.root->profiled_time);
            item->setData(COL_PERCENT_SUM_PER_THREAD, Qt::UserRole, percentage_per_thread);
            item->setText(COL_PERCENT_SUM_PER_THREAD, QString::number(percentage_per_thread));


            if (per_parent_stats->calls_number > 1 || !EASY_GLOBALS.display_only_relevant_stats)
            {
                item->setTimeSmart(COL_MIN_PER_PARENT, _units, easyBlock(per_parent_stats->min_duration_block).tree.node->duration());
                item->setTimeSmart(COL_MAX_PER_PARENT, _units, easyBlock(per_parent_stats->max_duration_block).tree.node->duration());
                item->setTimeSmart(COL_AVERAGE_PER_PARENT, _units, per_parent_stats->average_duration());
                item->setTimeSmart(COL_DURATION_SUM_PER_PARENT, _units, per_parent_stats->total_duration);
            }

            item->setData(COL_NCALLS_PER_PARENT, Qt::UserRole, per_parent_stats->calls_number);
            item->setText(COL_NCALLS_PER_PARENT, QString::number(per_parent_stats->calls_number));


            if (per_frame_stats->calls_number > 1 || !EASY_GLOBALS.display_only_relevant_stats)
            {
                item->setTimeSmart(COL_MIN_PER_FRAME, _units, easyBlock(per_frame_stats->min_duration_block).tree.node->duration());
                item->setTimeSmart(COL_MAX_PER_FRAME, _units, easyBlock(per_frame_stats->max_duration_block).tree.node->duration());
                item->setTimeSmart(COL_AVERAGE_PER_FRAME, _units, per_frame_stats->average_duration());
                item->setTimeSmart(COL_DURATION_SUM_PER_FRAME, _units, per_frame_stats->total_duration);
            }

            item->setData(COL_NCALLS_PER_FRAME, Qt::UserRole, per_frame_stats->calls_number);
            item->setText(COL_NCALLS_PER_FRAME, QString::number(per_frame_stats->calls_number));
        }
        else
        {
            item->setData(COL_PERCENT_SUM_PER_THREAD, Qt::UserRole, 0);
            item->setText(COL_PERCENT_SUM_PER_THREAD, "");
        }

        const auto color = easyDescriptor(gui_block.tree.node->id()).color();
        //const auto bgColor = ::profiler_gui::fromProfilerRgb(::profiler::colors::get_red(color), ::profiler::colors::get_green(color), ::profiler::colors::get_blue(color));
        const auto fgColor = ::profiler_gui::textColorForRgb(color);//0x00ffffff - bgColor;
        item->setBackgroundColor(color);
        item->setTextColor(fgColor);

#ifdef EASY_TREE_WIDGET__USE_VECTOR
        auto item_index = static_cast<unsigned int>(m_items.size());
        m_items.push_back(item);
#endif

        size_t children_items_number = 0;
        ::profiler::timestamp_t children_duration = 0;
        if (!gui_block.tree.children.empty())
        {
            m_iditems.clear();
            children_items_number = (this->*setTree)(*block.root, firstCswitch, _beginTime, gui_block.tree.children, item, item, _left, _right, _strict, children_duration, _colorizeRows, _addZeroBlocks, _units);
            if (interrupted())
                break;
        }

        int percentage = 100;
        auto self_duration = duration - children_duration;
        if (children_duration > 0 && duration > 0)
        {
            percentage = static_cast<int>(0.5 + 100. * static_cast<double>(self_duration) / static_cast<double>(duration));
        }

        item->setTimeSmart(COL_SELF_DURATION, _units, self_duration);
        item->setData(COL_SELF_DURATION_PERCENT, Qt::UserRole, percentage);
        item->setText(COL_SELF_DURATION_PERCENT, QString::number(percentage));

        if (children_items_number > 0 || !_strict || (startTime >= _left && endTime <= _right))
        {
            //total_items += children_items_number + 1;
#ifdef EASY_TREE_WIDGET__USE_VECTOR
            gui_block.tree_item = item_index;
#endif

            if (_colorizeRows)
                item->colorize(_colorizeRows);

            if (gui_block.expanded)
                item->setExpanded(true);

#ifndef EASY_TREE_WIDGET__USE_VECTOR
            m_items.insert(::std::make_pair(block.tree, item));
#endif
        }
        else
        {
#ifdef EASY_TREE_WIDGET__USE_VECTOR
            m_items.pop_back();
#endif
            delete item;
        }

        setProgress((90 * ++i) / total);
    }

    i = 0;
    total = static_cast<int>(threadsMap.size());
    for (auto& it : threadsMap)
    {
        auto item = it.second;

        if (item->childCount() > 0)
        {
            //addTopLevelItem(item);
            //m_roots[it.first] = item;

            //_items.push_back(item);
            m_topLevelItems.emplace_back(it.first, item);

            //++total_items;
        }
        else
        {
            delete item;
        }

        setProgress(90 + (10 * ++i) / total);
    }

    setDone();
    //return total_items;
}

//////////////////////////////////////////////////////////////////////////

size_t EasyTreeWidgetLoader::setTreeInternal(const ::profiler::BlocksTreeRoot& _threadRoot, ::profiler::block_index_t _firstCswitch, const ::profiler::timestamp_t& _beginTime, const ::profiler::BlocksTree::children_t& _children, EasyTreeWidgetItem* _parent, EasyTreeWidgetItem* _frame, ::profiler::timestamp_t _left, ::profiler::timestamp_t _right, bool _strict, ::profiler::timestamp_t& _duration, bool _colorizeRows, bool _addZeroBlocks, ::profiler_gui::TimeUnits _units)
{
    auto const setTree = m_mode == EasyTreeMode_Full ? &EasyTreeWidgetLoader::setTreeInternal : &EasyTreeWidgetLoader::setTreeInternalPlain;

    size_t total_items = 0;
    for (auto child_index : _children)
    {
        if (interrupted())
            break;

        auto& gui_block = easyBlock(child_index);
        const auto& child = gui_block.tree;
        const auto startTime = child.node->begin();
        const auto endTime = child.node->end();
        const auto duration = endTime - startTime;

        if (duration == 0 && !_addZeroBlocks)
            continue;

        _duration += duration;

        if (startTime > _right || endTime < _left)
            continue;

        bool hasContextSwitch = false;
        ::profiler::timestamp_t idleTime = 0;
        for (::profiler::block_index_t ind = _firstCswitch, ncs = static_cast<::profiler::block_index_t>(_threadRoot.sync.size()); ind < ncs; ++ind)
        {
            auto cs_index = _threadRoot.sync[ind];
            const auto cs = EASY_GLOBALS.gui_blocks[cs_index].tree.node;

            if (cs->begin() > endTime)
            {
                if (!hasContextSwitch)
                    _firstCswitch = ind;
                break;
            }

            if (startTime <= cs->begin() && cs->end() <= endTime)
            {
                if (!hasContextSwitch)
                {
                    _firstCswitch = ind;
                    hasContextSwitch = true;
                }

                idleTime += cs->duration();
            }
        }

        auto item = new EasyTreeWidgetItem(child_index, _parent);

        auto name = *child.node->name() != 0 ? child.node->name() : easyDescriptor(child.node->id()).name();
        item->setText(COL_NAME, ::profiler_gui::toUnicode(name));
        item->setTimeSmart(COL_DURATION, _units, duration);

        auto active_time = duration - idleTime;
        auto active_percent = duration == 0 ? 100. : ::profiler_gui::percentReal(active_time, duration);
        item->setTimeSmart(COL_ACTIVE_TIME, _units, active_time);
        item->setText(COL_ACTIVE_PERCENT, QString::number(active_percent, 'g', 3));
        item->setData(COL_ACTIVE_PERCENT, Qt::UserRole, active_percent);

        item->setTimeMs(COL_BEGIN, startTime - _beginTime);
        item->setTimeMs(COL_END, endTime - _beginTime);
        item->setData(COL_PERCENT_SUM_PER_THREAD, Qt::UserRole, 0);

        if (child.per_thread_stats != nullptr) // if there is per_thread_stats then there are other stats also
        {
            const ::profiler::BlockStatistics* per_thread_stats = child.per_thread_stats;
            const ::profiler::BlockStatistics* per_parent_stats = child.per_parent_stats;
            const ::profiler::BlockStatistics* per_frame_stats = child.per_frame_stats;

            auto parent_duration = _parent->duration();
            auto percentage = duration == 0 ? 0 : ::profiler_gui::percent(duration, parent_duration);
            auto percentage_sum = ::profiler_gui::percent(per_parent_stats->total_duration, parent_duration);
            item->setData(COL_PERCENT_PER_PARENT, Qt::UserRole, percentage);
            item->setText(COL_PERCENT_PER_PARENT, QString::number(percentage));
            item->setData(COL_PERCENT_SUM_PER_PARENT, Qt::UserRole, percentage_sum);
            item->setText(COL_PERCENT_SUM_PER_PARENT, QString::number(percentage_sum));

            if (_frame != nullptr)
            {
                if (_parent != _frame)
                {
                    parent_duration = _frame->duration();
                    percentage = duration == 0 ? 0 : ::profiler_gui::percent(duration, parent_duration);
                    percentage_sum = ::profiler_gui::percent(per_frame_stats->total_duration, parent_duration);
                }

                item->setData(COL_PERCENT_PER_FRAME, Qt::UserRole, percentage);
                item->setText(COL_PERCENT_PER_FRAME, QString::number(percentage));
                item->setData(COL_PERCENT_SUM_PER_FRAME, Qt::UserRole, percentage_sum);
                item->setText(COL_PERCENT_SUM_PER_FRAME, QString::number(percentage_sum));
            }
            else
            {
                item->setData(COL_PERCENT_PER_FRAME, Qt::UserRole, 0);
                item->setData(COL_PERCENT_SUM_PER_FRAME, Qt::UserRole, 0);

                auto percentage_per_thread = ::profiler_gui::percent(duration, _threadRoot.profiled_time);
                item->setData(COL_PERCENT_PER_PARENT, Qt::UserRole, percentage_per_thread);
                item->setText(COL_PERCENT_PER_PARENT, QString::number(percentage_per_thread));
            }


            if (per_thread_stats->calls_number > 1 || !EASY_GLOBALS.display_only_relevant_stats)
            {
                item->setTimeSmart(COL_MIN_PER_THREAD, _units, easyBlock(per_thread_stats->min_duration_block).tree.node->duration());
                item->setTimeSmart(COL_MAX_PER_THREAD, _units, easyBlock(per_thread_stats->max_duration_block).tree.node->duration());
                item->setTimeSmart(COL_AVERAGE_PER_THREAD, _units, per_thread_stats->average_duration());
                item->setTimeSmart(COL_DURATION_SUM_PER_THREAD, _units, per_thread_stats->total_duration);
            }

            item->setData(COL_NCALLS_PER_THREAD, Qt::UserRole, per_thread_stats->calls_number);
            item->setText(COL_NCALLS_PER_THREAD, QString::number(per_thread_stats->calls_number));

            auto percentage_per_thread = ::profiler_gui::percent(per_thread_stats->total_duration, _threadRoot.profiled_time);
            item->setData(COL_PERCENT_SUM_PER_THREAD, Qt::UserRole, percentage_per_thread);
            item->setText(COL_PERCENT_SUM_PER_THREAD, QString::number(percentage_per_thread));


            if (per_parent_stats->calls_number > 1 || !EASY_GLOBALS.display_only_relevant_stats)
            {
                item->setTimeSmart(COL_MIN_PER_PARENT, _units, easyBlock(per_parent_stats->min_duration_block).tree.node->duration());
                item->setTimeSmart(COL_MAX_PER_PARENT, _units, easyBlock(per_parent_stats->max_duration_block).tree.node->duration());
                item->setTimeSmart(COL_AVERAGE_PER_PARENT, _units, per_parent_stats->average_duration());
                item->setTimeSmart(COL_DURATION_SUM_PER_PARENT, _units, per_parent_stats->total_duration);
            }

            item->setData(COL_NCALLS_PER_PARENT, Qt::UserRole, per_parent_stats->calls_number);
            item->setText(COL_NCALLS_PER_PARENT, QString::number(per_parent_stats->calls_number));


            if (per_frame_stats->calls_number > 1 || !EASY_GLOBALS.display_only_relevant_stats)
            {
                item->setTimeSmart(COL_MIN_PER_FRAME, _units, easyBlock(per_frame_stats->min_duration_block).tree.node->duration());
                item->setTimeSmart(COL_MAX_PER_FRAME, _units, easyBlock(per_frame_stats->max_duration_block).tree.node->duration());
                item->setTimeSmart(COL_AVERAGE_PER_FRAME, _units, per_frame_stats->average_duration());
                item->setTimeSmart(COL_DURATION_SUM_PER_FRAME, _units, per_frame_stats->total_duration);
            }

            item->setData(COL_NCALLS_PER_FRAME, Qt::UserRole, per_frame_stats->calls_number);
            item->setText(COL_NCALLS_PER_FRAME, QString::number(per_frame_stats->calls_number));
        }
        else
        {
            if (_frame == nullptr)
            {
                auto percentage_per_thread = ::profiler_gui::percent(duration, _threadRoot.profiled_time);
                item->setData(COL_PERCENT_PER_PARENT, Qt::UserRole, percentage_per_thread);
                item->setText(COL_PERCENT_PER_PARENT, QString::number(percentage_per_thread));
            }
            else
            {
                item->setData(COL_PERCENT_PER_PARENT, Qt::UserRole, 0);
            }

            item->setData(COL_PERCENT_SUM_PER_PARENT, Qt::UserRole, 0);
            item->setData(COL_PERCENT_SUM_PER_THREAD, Qt::UserRole, 0);
        }

        const auto color = easyDescriptor(child.node->id()).color();
        //const auto bgColor = ::profiler_gui::fromProfilerRgb(::profiler::colors::get_red(color), ::profiler::colors::get_green(color), ::profiler::colors::get_blue(color));
        const auto fgColor = ::profiler_gui::textColorForRgb(color);// 0x00ffffff - bgColor;
        item->setBackgroundColor(color);
        item->setTextColor(fgColor);

#ifdef EASY_TREE_WIDGET__USE_VECTOR
        auto item_index = static_cast<uint32_t>(m_items.size());
        m_items.push_back(item);
#endif

        size_t children_items_number = 0;
        ::profiler::timestamp_t children_duration = 0;
        if (!child.children.empty())
        {
            m_iditems.clear();
            children_items_number = (this->*setTree)(_threadRoot, _firstCswitch, _beginTime, child.children, item, _frame ? _frame : item, _left, _right, _strict, children_duration, _colorizeRows, _addZeroBlocks, _units);
            if (interrupted())
                break;
        }

        int percentage = 100;
        auto self_duration = duration - children_duration;
        if (children_duration > 0 && duration > 0)
        {
            percentage = ::profiler_gui::percent(self_duration, duration);
        }

        item->setTimeSmart(COL_SELF_DURATION, _units, self_duration);
        item->setData(COL_SELF_DURATION_PERCENT, Qt::UserRole, percentage);
        item->setText(COL_SELF_DURATION_PERCENT, QString::number(percentage));

        if (children_items_number > 0 || !_strict || (startTime >= _left && endTime <= _right))
        {
            total_items += children_items_number + 1;
#ifdef EASY_TREE_WIDGET__USE_VECTOR
            gui_block.tree_item = item_index;
#endif

            if (_colorizeRows)
                item->colorize(_colorizeRows);

            if (gui_block.expanded)
                item->setExpanded(true);

#ifndef EASY_TREE_WIDGET__USE_VECTOR
            m_items.insert(::std::make_pair(child_index, item));
#endif
        }
        else
        {
#ifdef EASY_TREE_WIDGET__USE_VECTOR
            m_items.pop_back();
#endif
            delete item;
        }
    }

    return total_items;
}

//////////////////////////////////////////////////////////////////////////

::profiler::timestamp_t EasyTreeWidgetLoader::calculateChildrenDurationRecursive(const ::profiler::BlocksTree::children_t& _children, ::profiler::block_id_t _id)
{
    ::profiler::timestamp_t total_duration = 0;

    for (auto child_index : _children)
    {
        if (interrupted())
            break;

        const auto& gui_block = easyBlock(child_index);
        total_duration += gui_block.tree.node->duration();
        if (gui_block.tree.node->id() == _id)
            total_duration += calculateChildrenDurationRecursive(gui_block.tree.children, _id);
    }

    return total_duration;
}

size_t EasyTreeWidgetLoader::setTreeInternalPlain(const ::profiler::BlocksTreeRoot& _threadRoot, ::profiler::block_index_t _firstCswitch, const ::profiler::timestamp_t& _beginTime, const ::profiler::BlocksTree::children_t& _children, EasyTreeWidgetItem*, EasyTreeWidgetItem* _frame, ::profiler::timestamp_t _left, ::profiler::timestamp_t _right, bool _strict, ::profiler::timestamp_t& _duration, bool _colorizeRows, bool _addZeroBlocks, ::profiler_gui::TimeUnits _units)
{
    size_t total_items = 0;
    for (auto child_index : _children)
    {
        if (interrupted())
            break;

        const auto& gui_block = easyBlock(child_index);
        const auto& child = gui_block.tree;
        const auto startTime = child.node->begin();
        const auto endTime = child.node->end();
        const auto duration = endTime - startTime;

        _duration += duration;

        auto it = m_iditems.find(child.node->id());
        if (it != m_iditems.end())
        {
            ++total_items;

            ::profiler::timestamp_t children_duration = 0;
            if (!child.children.empty())
            {
                setTreeInternalPlain(_threadRoot, _firstCswitch, _beginTime, child.children, _frame, _frame, _left, _right, _strict, children_duration, _colorizeRows, _addZeroBlocks, _units);
                if (interrupted())
                    break;
            }

            if (it->second != nullptr && child.per_frame_stats != nullptr)
            {
                auto item = it->second;

                //auto children_duration = calculateChildrenDurationRecursive(child.children, it->first);
                if (children_duration != 0)
                {
                    auto self_duration = item->data(COL_SELF_DURATION, Qt::UserRole).toULongLong() - children_duration;

                    int percentage = 100;
                    if (child.per_frame_stats->total_duration > 0)
                        percentage = ::profiler_gui::percent(self_duration, child.per_frame_stats->total_duration);

                    item->setTimeSmart(COL_SELF_DURATION, _units, self_duration);
                    item->setData(COL_SELF_DURATION_PERCENT, Qt::UserRole, percentage);
                    item->setText(COL_SELF_DURATION_PERCENT, QString::number(percentage));
                }

                bool hasContextSwitch = false;
                ::profiler::timestamp_t idleTime = 0;
                for (::profiler::block_index_t ind = _firstCswitch, ncs = static_cast<::profiler::block_index_t>(_threadRoot.sync.size()); ind < ncs; ++ind)
                {
                    auto cs_index = _threadRoot.sync[ind];
                    const auto cs = EASY_GLOBALS.gui_blocks[cs_index].tree.node;

                    if (cs->begin() > endTime)
                    {
                        if (!hasContextSwitch)
                            _firstCswitch = ind;
                        break;
                    }

                    if (startTime <= cs->begin() && cs->end() <= endTime)
                    {
                        if (!hasContextSwitch)
                        {
                            _firstCswitch = ind;
                            hasContextSwitch = true;
                        }

                        idleTime += cs->duration();
                    }
                }

                auto active_time = item->data(COL_ACTIVE_TIME, Qt::UserRole).toULongLong() - idleTime;
                auto active_percent = child.per_frame_stats->total_duration == 0 ? 100. : ::profiler_gui::percentReal(active_time, child.per_frame_stats->total_duration);
                item->setTimeSmart(COL_ACTIVE_TIME, _units, active_time);
                item->setText(COL_ACTIVE_PERCENT, QString::number(active_percent, 'g', 3));
                item->setData(COL_ACTIVE_PERCENT, Qt::UserRole, active_percent);
            }

            continue;
        }

        if (startTime > _right || endTime < _left)
            continue;

        bool hasContextSwitch = false;
        ::profiler::timestamp_t idleTime = 0;
        for (::profiler::block_index_t ind = _firstCswitch, ncs = static_cast<::profiler::block_index_t>(_threadRoot.sync.size()); ind < ncs; ++ind)
        {
            auto cs_index = _threadRoot.sync[ind];
            const auto cs = EASY_GLOBALS.gui_blocks[cs_index].tree.node;

            if (cs->begin() > endTime)
            {
                if (!hasContextSwitch)
                    _firstCswitch = ind;
                break;
            }

            if (startTime <= cs->begin() && cs->end() <= endTime)
            {
                if (!hasContextSwitch)
                {
                    _firstCswitch = ind;
                    hasContextSwitch = true;
                }

                idleTime += cs->duration();
            }
        }

        auto item = new EasyTreeWidgetItem(child_index, _frame);

        auto name = *child.node->name() != 0 ? child.node->name() : easyDescriptor(child.node->id()).name();
        item->setText(COL_NAME, ::profiler_gui::toUnicode(name));

        if (child.per_thread_stats != nullptr) // if there is per_thread_stats then there are other stats also
        {
            const ::profiler::BlockStatistics* per_thread_stats = child.per_thread_stats;
            if (per_thread_stats->calls_number > 1 || !EASY_GLOBALS.display_only_relevant_stats)
            {
                item->setTimeSmart(COL_MIN_PER_THREAD, _units, easyBlock(per_thread_stats->min_duration_block).tree.node->duration());
                item->setTimeSmart(COL_MAX_PER_THREAD, _units, easyBlock(per_thread_stats->max_duration_block).tree.node->duration());
                item->setTimeSmart(COL_AVERAGE_PER_THREAD, _units, per_thread_stats->average_duration());
            }

            item->setTimeSmart(COL_DURATION_SUM_PER_THREAD, _units, per_thread_stats->total_duration);
            item->setData(COL_NCALLS_PER_THREAD, Qt::UserRole, per_thread_stats->calls_number);
            item->setText(COL_NCALLS_PER_THREAD, QString::number(per_thread_stats->calls_number));

            auto percentage_per_thread = ::profiler_gui::percent(per_thread_stats->total_duration, _threadRoot.profiled_time);
            item->setData(COL_PERCENT_SUM_PER_THREAD, Qt::UserRole, percentage_per_thread);
            item->setText(COL_PERCENT_SUM_PER_THREAD, QString::number(percentage_per_thread));

            const ::profiler::BlockStatistics* per_frame_stats = child.per_frame_stats;
            const auto percentage_sum = ::profiler_gui::percent(per_frame_stats->total_duration, _frame->duration());
            item->setData(COL_PERCENT_PER_FRAME, Qt::UserRole, percentage_sum);
            item->setText(COL_PERCENT_PER_FRAME, QString::number(percentage_sum));

            if (per_frame_stats->calls_number > 1 || !EASY_GLOBALS.display_only_relevant_stats)
            {
                item->setTimeSmart(COL_MIN_PER_FRAME, _units, easyBlock(per_frame_stats->min_duration_block).tree.node->duration());
                item->setTimeSmart(COL_MAX_PER_FRAME, _units, easyBlock(per_frame_stats->max_duration_block).tree.node->duration());
                item->setTimeSmart(COL_AVERAGE_PER_FRAME, _units, per_frame_stats->average_duration());
            }

            item->setTimeSmart(COL_DURATION, _units, per_frame_stats->total_duration);
            item->setData(COL_NCALLS_PER_FRAME, Qt::UserRole, per_frame_stats->calls_number);
            item->setText(COL_NCALLS_PER_FRAME, QString::number(per_frame_stats->calls_number));
        }
        else
        {
            item->setData(COL_PERCENT_SUM_PER_THREAD, Qt::UserRole, 0);
            item->setData(COL_PERCENT_PER_FRAME, Qt::UserRole, 0);
        }

        const auto color = easyDescriptor(child.node->id()).color();
        const auto fgColor = ::profiler_gui::textColorForRgb(color);// 0x00ffffff - bgColor;
        item->setBackgroundColor(color);
        item->setTextColor(fgColor);

#ifdef EASY_TREE_WIDGET__USE_VECTOR
        auto item_index = static_cast<uint32_t>(m_items.size());
        m_items.push_back(item);
#endif
        m_iditems[child.node->id()] = nullptr;

        size_t children_items_number = 0;
        ::profiler::timestamp_t children_duration = 0;
        if (!child.children.empty())
        {
            children_items_number = setTreeInternalPlain(_threadRoot, _firstCswitch, _beginTime, child.children, _frame, _frame, _left, _right, _strict, children_duration, _colorizeRows, _addZeroBlocks, _units);
            if (interrupted())
                break;
        }

        m_iditems[child.node->id()] = item;

        if (child.per_frame_stats != nullptr)
        {
            int percentage = 100;
            auto self_duration = child.per_frame_stats->total_duration - children_duration;
            if (child.per_frame_stats->total_duration > 0)
                percentage = ::profiler_gui::percent(self_duration, child.per_frame_stats->total_duration);

            item->setTimeSmart(COL_SELF_DURATION, _units, self_duration);
            item->setData(COL_SELF_DURATION_PERCENT, Qt::UserRole, percentage);
            item->setText(COL_SELF_DURATION_PERCENT, QString::number(percentage));

            auto active_time = child.per_frame_stats->total_duration - idleTime;
            auto active_percent = child.per_frame_stats->total_duration == 0 ? 100. : ::profiler_gui::percentReal(active_time, child.per_frame_stats->total_duration);
            item->setTimeSmart(COL_ACTIVE_TIME, _units, active_time);
            item->setText(COL_ACTIVE_PERCENT, QString::number(active_percent, 'g', 3));
            item->setData(COL_ACTIVE_PERCENT, Qt::UserRole, active_percent);
        }

        if (children_items_number > 0 || !_strict || (startTime >= _left && endTime <= _right))
        {
            total_items += children_items_number + 1;
#ifdef EASY_TREE_WIDGET__USE_VECTOR
            gui_block.tree_item = item_index;
#endif

            if (_colorizeRows)
                item->colorize(_colorizeRows);

            if (gui_block.expanded)
                item->setExpanded(true);

#ifndef EASY_TREE_WIDGET__USE_VECTOR
            m_items.insert(::std::make_pair(child_index, item));
#endif
        }
        else
        {
#ifdef EASY_TREE_WIDGET__USE_VECTOR
            m_items.pop_back();
#endif
            delete item;
            m_iditems.erase(gui_block.tree.node->id());
        }
    }

    return total_items;
}

//////////////////////////////////////////////////////////////////////////

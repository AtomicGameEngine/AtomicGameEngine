/************************************************************************
* file name         : tree_widget_item.cpp
* ----------------- :
* creation time     : 2016/08/18
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains implementation of EasyTreeWidgetItem.
* ----------------- :
* change log        : * 2016/08/18 Victor Zarubkin: Moved sources from blocks_tree_widget.cpp
*                   :       and renamed classes from Prof* to Easy*.
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

#include "tree_widget_item.h"
#include "globals.h"

//////////////////////////////////////////////////////////////////////////

EasyTreeWidgetItem::EasyTreeWidgetItem(const ::profiler::block_index_t _treeBlock, Parent* _parent)
    : Parent(_parent)
    , m_block(_treeBlock)
    , m_customBGColor(0)
    , m_customTextColor(0)
{

}

EasyTreeWidgetItem::~EasyTreeWidgetItem()
{
}

bool EasyTreeWidgetItem::operator < (const Parent& _other) const
{
    const auto col = treeWidget()->sortColumn();

    switch (col)
    {
        //case COL_UNKNOWN:
        case COL_NAME:
        {
            if (parent() == nullptr)
                return false; // Do not sort topLevelItems by name
            return Parent::operator < (_other);
        }

        case COL_NCALLS_PER_THREAD:
        case COL_NCALLS_PER_PARENT:
        case COL_NCALLS_PER_FRAME:
        {
            return data(col, Qt::UserRole).toUInt() < _other.data(col, Qt::UserRole).toUInt();
        }

        case COL_SELF_DURATION_PERCENT:
        case COL_PERCENT_PER_PARENT:
        case COL_PERCENT_PER_FRAME:
        case COL_PERCENT_SUM_PER_PARENT:
        case COL_PERCENT_SUM_PER_FRAME:
        case COL_PERCENT_SUM_PER_THREAD:
        {
            return data(col, Qt::UserRole).toInt() < _other.data(col, Qt::UserRole).toInt();
        }

        case COL_ACTIVE_PERCENT:
        {
            return data(col, Qt::UserRole).toDouble() < _other.data(col, Qt::UserRole).toDouble();
        }

        default:
        {
            // durations min, max, average
            return data(col, Qt::UserRole).toULongLong() < _other.data(col, Qt::UserRole).toULongLong();
        }
    }

    return false;
}

::profiler::block_index_t EasyTreeWidgetItem::block_index() const
{
    return m_block;
}

::profiler_gui::EasyBlock& EasyTreeWidgetItem::guiBlock()
{
    return easyBlock(m_block);
}

const ::profiler::BlocksTree& EasyTreeWidgetItem::block() const
{
    return blocksTree(m_block);
}

::profiler::timestamp_t EasyTreeWidgetItem::duration() const
{
    if (parent() != nullptr)
        return block().node->duration();
    return data(COL_DURATION, Qt::UserRole).toULongLong();
}

::profiler::timestamp_t EasyTreeWidgetItem::selfDuration() const
{
    return data(COL_SELF_DURATION, Qt::UserRole).toULongLong();
}

void EasyTreeWidgetItem::setTimeSmart(int _column, ::profiler_gui::TimeUnits _units, const ::profiler::timestamp_t& _time, const QString& _prefix)
{
    const ::profiler::timestamp_t nanosecondsTime = PROF_NANOSECONDS(_time);

    setData(_column, Qt::UserRole, (quint64)nanosecondsTime);
    setToolTip(_column, QString("%1 ns").arg(nanosecondsTime));
    setText(_column, QString("%1%2").arg(_prefix).arg(::profiler_gui::timeStringRealNs(_units, nanosecondsTime, 3)));

//     if (_time < 1e3)
//     {
//         setText(_column, QString("%1%2 ns").arg(_prefix).arg(nanosecondsTime));
//     }
//     else if (_time < 1e6)
//     {
//         setText(_column, QString("%1%2 us").arg(_prefix).arg(double(nanosecondsTime) * 1e-3, 0, 'f', 3));
//     }
//     else if (_time < 1e9)
//     {
//         setText(_column, QString("%1%2 ms").arg(_prefix).arg(double(nanosecondsTime) * 1e-6, 0, 'f', 3));
//     }
//     else
//     {
//         setText(_column, QString("%1%2 s").arg(_prefix).arg(double(nanosecondsTime) * 1e-9, 0, 'f', 3));
//     }
}

void EasyTreeWidgetItem::setTimeSmart(int _column, ::profiler_gui::TimeUnits _units, const ::profiler::timestamp_t& _time)
{
    const ::profiler::timestamp_t nanosecondsTime = PROF_NANOSECONDS(_time);

    setData(_column, Qt::UserRole, (quint64)nanosecondsTime);
    setToolTip(_column, QString("%1 ns").arg(nanosecondsTime));
    setText(_column, ::profiler_gui::timeStringRealNs(_units, nanosecondsTime, 3));
}

void EasyTreeWidgetItem::setTimeMs(int _column, const ::profiler::timestamp_t& _time)
{
    const ::profiler::timestamp_t nanosecondsTime = PROF_NANOSECONDS(_time);
    setData(_column, Qt::UserRole, (quint64)nanosecondsTime);
    setToolTip(_column, QString("%1 ns").arg(nanosecondsTime));
    setText(_column, QString::number(double(nanosecondsTime) * 1e-6, 'g', 9));
}

void EasyTreeWidgetItem::setTimeMs(int _column, const ::profiler::timestamp_t& _time, const QString& _prefix)
{
    const ::profiler::timestamp_t nanosecondsTime = PROF_NANOSECONDS(_time);
    setData(_column, Qt::UserRole, (quint64)nanosecondsTime);
    setToolTip(_column, QString("%1 ns").arg(nanosecondsTime));
    setText(_column, QString("%1%2").arg(_prefix).arg(double(nanosecondsTime) * 1e-6, 0, 'g', 9));
}

void EasyTreeWidgetItem::setBackgroundColor(QRgb _color)
{
    m_customBGColor = _color;
}

void EasyTreeWidgetItem::setTextColor(QRgb _color)
{
    m_customTextColor = _color;
}

void EasyTreeWidgetItem::colorize(bool _colorize)
{
    if (_colorize)
    {
        for (int i = 0; i < COL_COLUMNS_NUMBER; ++i)
        {
            setBackground(i, QColor::fromRgb(m_customBGColor));
            setForeground(i, QColor::fromRgb(m_customTextColor));
        }
    }
    else
    {
        const QBrush nobrush;
        for (int i = 0; i < COL_COLUMNS_NUMBER; ++i)
        {
            setBackground(i, nobrush);
            setForeground(i, nobrush);
        }
    }
}

void EasyTreeWidgetItem::collapseAll()
{
    for (int i = 0, childrenNumber = childCount(); i < childrenNumber; ++i)
    {
        static_cast<EasyTreeWidgetItem*>(child(i))->collapseAll();
    }

    setExpanded(false);
    if (parent() != nullptr)
        guiBlock().expanded = false;
}

void EasyTreeWidgetItem::expandAll()
{
    for (int i = 0, childrenNumber = childCount(); i < childrenNumber; ++i)
    {
        static_cast<EasyTreeWidgetItem*>(child(i))->expandAll();
    }

    setExpanded(true);
    if (parent() != nullptr)
        guiBlock().expanded = true;
}

//////////////////////////////////////////////////////////////////////////

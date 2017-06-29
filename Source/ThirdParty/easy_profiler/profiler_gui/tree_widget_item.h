/************************************************************************
* file name         : tree_widget_item.h
* ----------------- : 
* creation time     : 2016/08/18
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- : 
* description       : The file contains declaration of EasyTreeWidgetItem
*                   : for displyaing EasyProfiler blocks tree.
* ----------------- : 
* change log        : * 2016/08/18 Victor Zarubkin: moved sources from blocks_tree_widget.h
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

#ifndef EASY_TREE_WIDGET_ITEM_H
#define EASY_TREE_WIDGET_ITEM_H

#include <stdlib.h>
#include <QTreeWidget>
#include <easy/reader.h>

#include "common_types.h"

//////////////////////////////////////////////////////////////////////////

enum EasyColumnsIndexes
{
    COL_UNKNOWN = -1,

    COL_NAME = 0,

    COL_BEGIN,

    COL_DURATION,
    COL_SELF_DURATION,
    COL_DURATION_SUM_PER_PARENT,
    COL_DURATION_SUM_PER_FRAME,
    COL_DURATION_SUM_PER_THREAD,

    COL_SELF_DURATION_PERCENT,
    COL_PERCENT_PER_PARENT,
    COL_PERCENT_PER_FRAME,
    COL_PERCENT_SUM_PER_PARENT,
    COL_PERCENT_SUM_PER_FRAME,
    COL_PERCENT_SUM_PER_THREAD,

    COL_END,

    COL_MIN_PER_FRAME,
    COL_MAX_PER_FRAME,
    COL_AVERAGE_PER_FRAME,
    COL_NCALLS_PER_FRAME,

    COL_MIN_PER_THREAD,
    COL_MAX_PER_THREAD,
    COL_AVERAGE_PER_THREAD,
    COL_NCALLS_PER_THREAD,

    COL_MIN_PER_PARENT,
    COL_MAX_PER_PARENT,
    COL_AVERAGE_PER_PARENT,
    COL_NCALLS_PER_PARENT,

    COL_ACTIVE_TIME,
    COL_ACTIVE_PERCENT,

    COL_COLUMNS_NUMBER
};

//////////////////////////////////////////////////////////////////////////

class EasyTreeWidgetItem : public QTreeWidgetItem
{
    typedef QTreeWidgetItem    Parent;
    typedef EasyTreeWidgetItem   This;

    const ::profiler::block_index_t         m_block;
    QRgb                            m_customBGColor;
    QRgb                          m_customTextColor;

public:

    using Parent::setBackgroundColor;
    using Parent::setTextColor;

    explicit EasyTreeWidgetItem(const ::profiler::block_index_t _treeBlock = ::profiler_gui::numeric_max<decltype(m_block)>(), Parent* _parent = nullptr);
    virtual ~EasyTreeWidgetItem();

    bool operator < (const Parent& _other) const override;

public:

    ::profiler::block_index_t block_index() const;
    ::profiler_gui::EasyBlock& guiBlock();
    const ::profiler::BlocksTree& block() const;

    ::profiler::timestamp_t duration() const;
    ::profiler::timestamp_t selfDuration() const;

    void setTimeSmart(int _column, ::profiler_gui::TimeUnits _units, const ::profiler::timestamp_t& _time, const QString& _prefix);
    void setTimeSmart(int _column, ::profiler_gui::TimeUnits _units, const ::profiler::timestamp_t& _time);

    void setTimeMs(int _column, const ::profiler::timestamp_t& _time);
    void setTimeMs(int _column, const ::profiler::timestamp_t& _time, const QString& _prefix);

    void setBackgroundColor(QRgb _color);

    void setTextColor(QRgb _color);

    void colorize(bool _colorize);

    void collapseAll();

    void expandAll();

}; // END of class EasyTreeWidgetItem.

//////////////////////////////////////////////////////////////////////////

#endif // EASY_TREE_WIDGET_ITEM_H

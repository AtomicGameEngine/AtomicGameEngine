/************************************************************************
* file name         : common_types.h
* ----------------- :
* creation time     : 2016/07/31
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains declaration of common types for both GraphicsView
*                   : and TreeWidget.
* ----------------- :
* change log        : * 2016/07/31 Victor Zarubkin: initial commit.
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

#ifndef EASY_PROFILER__GUI_COMMON_TYPES_H
#define EASY_PROFILER__GUI_COMMON_TYPES_H

#include <stdlib.h>
#include <vector>
#include <unordered_map>
#include <QRgb>
#include <QString>
#include <QFont>
#include <easy/reader.h>

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#define PROF_MICROSECONDS(timestamp) ((timestamp) * 1e-3)
//#define PROF_MICROSECONDS(timestamp) (timestamp)

#define PROF_FROM_MICROSECONDS(to_timestamp) ((to_timestamp) * 1e3)
//#define PROF_FROM_MICROSECONDS(to_timestamp) (to_timestamp)

#define PROF_MILLISECONDS(timestamp) ((timestamp) * 1e-6)
//#define PROF_MILLISECONDS(timestamp) ((timestamp) * 1e-3)

#define PROF_FROM_MILLISECONDS(to_timestamp) ((to_timestamp) * 1e6)
//#define PROF_FROM_MILLISECONDS(to_timestamp) ((to_timestamp) * 1e3)

#define PROF_NANOSECONDS(timestamp) (timestamp)
//#define PROF_NANOSECONDS(timestamp) ((timestamp) * 1000)

//////////////////////////////////////////////////////////////////////////

inline qreal units2microseconds(qreal _value)
{
    return _value;
    //return _value * 1e3;
}

inline qreal microseconds2units(qreal _value)
{
    return _value;
    //return _value * 1e-3;
}

//////////////////////////////////////////////////////////////////////////

namespace profiler_gui {
    
inline QRgb toRgb(uint32_t _red, uint32_t _green, uint32_t _blue)
{
    return (_red << 16) + (_green << 8) + _blue;
}

inline QRgb fromProfilerRgb(uint32_t _red, uint32_t _green, uint32_t _blue)
{
    if (_red == 0 && _green == 0 && _blue == 0)
        return ::profiler::colors::Default;
    return toRgb(_red, _green, _blue) | 0x00141414;
}

inline bool isLightColor(::profiler::color_t _color)
{
    const auto sum = 255. - (((_color & 0x00ff0000) >> 16) * 0.299 + ((_color & 0x0000ff00) >> 8) * 0.587 + (_color & 0x000000ff) * 0.114);
    return sum < 76.5 || ((_color & 0xff000000) >> 24) < 0x80;
}

inline bool isLightColor(::profiler::color_t _color, qreal _maxSum)
{
    const auto sum = 255. - (((_color & 0x00ff0000) >> 16) * 0.299 + ((_color & 0x0000ff00) >> 8) * 0.587 + (_color & 0x000000ff) * 0.114);
    return sum < _maxSum || ((_color & 0xff000000) >> 24) < 0x80;
}

inline ::profiler::color_t textColorForFlag(bool _is_light)
{
    return _is_light ? ::profiler::colors::Dark : ::profiler::colors::CreamWhite;
}

inline ::profiler::color_t textColorForRgb(::profiler::color_t _color)
{
    return isLightColor(_color) ? ::profiler::colors::Dark : ::profiler::colors::CreamWhite;
}

//////////////////////////////////////////////////////////////////////////

#define EASY_GRAPHICS_ITEM_RECURSIVE_PAINT
//#undef  EASY_GRAPHICS_ITEM_RECURSIVE_PAINT

#pragma pack(push, 1)
struct EasyBlockItem Q_DECL_FINAL
{
    qreal                              x; ///< x coordinate of the item (this is made qreal=double to avoid mistakes on very wide scene)
    float                              w; ///< Width of the item
    ::profiler::block_index_t      block; ///< Index of profiler block

#ifndef EASY_GRAPHICS_ITEM_RECURSIVE_PAINT
    ::profiler::block_index_t neighbours; ///< Number of neighbours (parent.children.size())
    uint32_t              children_begin; ///< Index of first child item on the next sublevel
    int8_t                         state; ///< 0 = no change, 1 = paint, -1 = do not paint
#else
    ::profiler::block_index_t max_depth_child; ///< Index of child with maximum tree depth
    uint32_t              children_begin; ///< Index of first child item on the next sublevel
#endif

    // Possible optimizations:
    // 1) We can save 1 more byte per block if we will use char instead of short + real time calculations for "totalHeight" var;
    // 2) We can save 12 bytes per block if "x" and "w" vars will be removed (all this information exist inside BlocksTree),
    //      but this requires runtime x-coodinate calculation because BlocksTree has x value in nanoseconds.

    inline void setPos(qreal _x, float _w) { x = _x; w = _w; }
    inline qreal left() const { return x; }
    inline qreal right() const { return x + w; }
    inline float width() const { return w; }

}; // END of struct EasyBlockItem.

//#define EASY_TREE_WIDGET__USE_VECTOR
struct EasyBlock Q_DECL_FINAL
{
    ::profiler::BlocksTree       tree;
#ifdef EASY_TREE_WIDGET__USE_VECTOR
    uint32_t                tree_item;
#endif
    uint32_t      graphics_item_index;
    uint8_t       graphics_item_level;
    uint8_t             graphics_item;
    bool                     expanded;

    EasyBlock() = default;

    EasyBlock(EasyBlock&& that)
        : tree(::std::move(that.tree))
#ifdef EASY_TREE_WIDGET__USE_VECTOR
        , tree_item(that.tree_item)
#endif
        , graphics_item_index(that.graphics_item_index)
        , graphics_item_level(that.graphics_item_level)
        , graphics_item(that.graphics_item)
        , expanded(that.expanded)
    {
    }

private:

    EasyBlock(const EasyBlock&) = delete;
};
#pragma pack(pop)

typedef ::std::vector<EasyBlockItem> EasyItems;
typedef ::std::vector<EasyBlock> EasyBlocks;

//////////////////////////////////////////////////////////////////////////

struct EasySelectedBlock Q_DECL_FINAL
{
    const ::profiler::BlocksTreeRoot* root;
    ::profiler::block_index_t         tree;

    EasySelectedBlock() : root(nullptr), tree(0xffffffff)
    {
    }

    EasySelectedBlock(const ::profiler::BlocksTreeRoot* _root, const ::profiler::block_index_t _tree)
        : root(_root)
        , tree(_tree)
    {
    }

}; // END of struct EasySelectedBlock.

typedef ::std::vector<EasySelectedBlock> TreeBlocks;

//////////////////////////////////////////////////////////////////////////

enum TimeUnits : int8_t
{
    TimeUnits_ms = 0,
    TimeUnits_us,
    TimeUnits_ns,
    TimeUnits_auto

}; // END of enum TimeUnits.

inline qreal timeFactor(qreal _interval)
{
    if (_interval < 1) // interval in nanoseconds
        return 1e3;

    if (_interval < 1e3) // interval in microseconds
        return 1;

    if (_interval < 1e6) // interval in milliseconds
        return 1e-3;

    // interval in seconds
    return 1e-6;
}

inline QString autoTimeStringReal(qreal _interval, int _precision = 1)
{
    if (_interval < 1) // interval in nanoseconds
        return QString("%1 ns").arg(static_cast<quint64>(_interval * 1e3));

    if (_interval < 1e3) // interval in microseconds
        return QString("%1 us").arg(_interval, 0, 'f', _precision);

    if (_interval < 1e6) // interval in milliseconds
        return QString("%1 ms").arg(_interval * 1e-3, 0, 'f', _precision);

    // interval in seconds
    return QString("%1 s").arg(_interval * 1e-6, 0, 'f', _precision);
}

inline QString autoTimeStringInt(qreal _interval)
{
    if (_interval < 1) // interval in nanoseconds
        return QString("%1 ns").arg(static_cast<quint64>(_interval * 1e3));

    if (_interval < 1e3) // interval in microseconds
        return QString("%1 us").arg(static_cast<quint32>(_interval));

    if (_interval < 1e6) // interval in milliseconds
        return QString("%1 ms").arg(static_cast<quint32>(_interval * 1e-3));

    // interval in seconds
    return QString("%1 s").arg(static_cast<quint32>(_interval * 1e-6));
}

inline QString autoTimeStringRealNs(::profiler::timestamp_t _interval, int _precision = 1)
{
    if (_interval < 1000) // interval in nanoseconds
        return QString("%1 ns").arg(_interval);

    if (_interval < 1000000) // interval in microseconds
        return QString("%1 us").arg(_interval * 1e-3, 0, 'f', _precision);

    if (_interval < 1000000000U) // interval in milliseconds
        return QString("%1 ms").arg(_interval * 1e-6, 0, 'f', _precision);

    // interval in seconds
    return QString("%1 s").arg(_interval * 1e-9, 0, 'f', _precision);
}

inline QString autoTimeStringIntNs(::profiler::timestamp_t _interval)
{
    if (_interval < 1000) // interval in nanoseconds
        return QString("%1 ns").arg(_interval);

    if (_interval < 1000000) // interval in microseconds
        return QString("%1 us").arg(static_cast<quint32>(_interval * 1e-3));

    if (_interval < 1000000000U) // interval in milliseconds
        return QString("%1 ms").arg(static_cast<quint32>(_interval * 1e-6));

    // interval in seconds
    return QString("%1 s").arg(static_cast<quint32>(_interval * 1e-9));
}

inline QString timeStringReal(TimeUnits _units, qreal _interval, int _precision = 1)
{
    switch (_units)
    {
        case TimeUnits_ms:{
            const char fmt = _interval <= 1 ? 'g' : 'f';
            return QString("%1 ms").arg(_interval * 1e-3, 0, fmt, _precision);
        }

        case TimeUnits_us:
            return QString("%1 us").arg(_interval, 0, 'f', _precision);

        case TimeUnits_ns:
            return QString("%1 ns").arg(static_cast<quint64>(_interval * 1e3));

        case TimeUnits_auto:
        default:
            return autoTimeStringReal(_interval, _precision);
    }

    return QString();
}

inline QString timeStringRealNs(TimeUnits _units, ::profiler::timestamp_t _interval, int _precision = 1)
{
    switch (_units)
    {
        case TimeUnits_ms:{
            const char fmt = _interval <= 1000 ? 'g' : 'f';
            return QString("%1 ms").arg(_interval * 1e-6, 0, fmt, _precision);
        }

        case TimeUnits_us:
            return QString("%1 us").arg(_interval * 1e-3, 0, 'f', _precision);

        case TimeUnits_ns:
            return QString("%1 ns").arg(_interval);

        case TimeUnits_auto:
        default:
            return autoTimeStringRealNs(_interval, _precision);
    }

    return QString();
}

inline QString timeStringInt(TimeUnits _units, qreal _interval)
{
    switch (_units)
    {
        case TimeUnits_ms:
            return QString("%1 ms").arg(static_cast<quint32>(_interval * 1e-3));

        case TimeUnits_us:
            return QString("%1 us").arg(static_cast<quint32>(_interval));

        case TimeUnits_ns:
            return QString("%1 ns").arg(static_cast<quint64>(_interval * 1e3));

        case TimeUnits_auto:
        default:
            return autoTimeStringInt(_interval);
    }

    return QString();
}

inline QString timeStringIntNs(TimeUnits _units, ::profiler::timestamp_t _interval)
{
    switch (_units)
    {
        case TimeUnits_ms:
            return QString("%1 ms").arg(static_cast<quint32>(_interval * 1e-6));

        case TimeUnits_us:
            return QString("%1 us").arg(static_cast<quint32>(_interval * 1e-3));

        case TimeUnits_ns:
            return QString("%1 ns").arg(_interval);

        case TimeUnits_auto:
        default:
            return autoTimeStringIntNs(_interval);
    }

    return QString();
}

//////////////////////////////////////////////////////////////////////////

template <class T> inline T numeric_max() {
    return ::std::numeric_limits<T>::max();
}

template <class T> inline T numeric_max(T) {
    return ::std::numeric_limits<T>::max();
}

template <class T> inline void set_max(T& _value) {
    _value = ::std::numeric_limits<T>::max();
}

template <class T> inline bool is_max(const T& _value) {
    return _value == ::std::numeric_limits<T>::max();
}

//////////////////////////////////////////////////////////////////////////

inline double percentReal(::profiler::timestamp_t _partial, ::profiler::timestamp_t _total)
{
    return _total ? 100. * static_cast<double>(_partial) / static_cast<double>(_total) : 0.;
}

inline int percent(::profiler::timestamp_t _partial, ::profiler::timestamp_t _total)
{
    return static_cast<int>(0.5 + percentReal(_partial, _total));
}

//////////////////////////////////////////////////////////////////////////

inline QFont EFont(QFont::StyleHint _hint, const char* _family, int _size, int _weight = -1)
{
    QFont f;
    f.setStyleHint(_hint, QFont::PreferMatch);
    f.setFamily(_family);
    f.setPointSize(_size);
    f.setWeight(_weight);
    return f;
}

inline QFont EFont(const char* _family, int _size, int _weight = -1)
{
    return EFont(QFont::Helvetica, _family, _size, _weight);
}

//////////////////////////////////////////////////////////////////////////

} // END of namespace profiler_gui.

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#endif // EASY_PROFILER__GUI_COMMON_TYPES_H

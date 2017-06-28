/************************************************************************
* file name         : reader.cpp
* ----------------- : 
* creation time     : 2016/06/19
* authors           : Sergey Yagovtsev, Victor Zarubkin
* emails            : yse.sey@gmail.com, v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains implementation of fillTreesFromFile function
*                   : which reads profiler file and fill profiler blocks tree.
* ----------------- :
* change log        : * 2016/06/19 Sergey Yagovtsev: First fillTreesFromFile implementation.
*                   :
*                   : * 2016/06/25 Victor Zarubkin: Removed unnecessary memory allocation and copy
*                   :       when creating and inserting blocks into the tree.
*                   :
*                   : * 2016/06/26 Victor Zarubkin: Added statistics gathering (min, max, average duration,
*                   :       number of block calls).
*                   : * 2016/06/26 Victor Zarubkin, Sergey Yagovtsev: Added statistics gathering for root
*                   :       blocks in the tree.
*                   :
*                   : * 2016/06/29 Victor Zarubkin: Added calculaton of total children number for blocks.
*                   :
*                   : * 2016/06/30 Victor Zarubkin: Added this header.
*                   :       Added tree depth calculation.
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

#include <easy/reader.h>

#include "hashed_cstr.h"

#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <unordered_map>
#include <thread>

//////////////////////////////////////////////////////////////////////////

typedef uint64_t processid_t;

extern const uint32_t PROFILER_SIGNATURE;
extern const uint32_t EASY_CURRENT_VERSION;

# define EASY_VERSION_INT(v_major, v_minor, v_patch) ((static_cast<uint32_t>(v_major) << 24) | (static_cast<uint32_t>(v_minor) << 16) | static_cast<uint32_t>(v_patch))
const uint32_t MIN_COMPATIBLE_VERSION = EASY_VERSION_INT(0, 1, 0); ///< minimal compatible version (.prof file format was not changed seriously since this version)
const uint32_t EASY_V_100 = EASY_VERSION_INT(1, 0, 0); ///< in v1.0.0 some additional data were added into .prof file
const uint32_t EASY_V_130 = EASY_VERSION_INT(1, 3, 0); ///< in v1.3.0 changed sizeof(thread_id_t) uint32_t -> uint64_t
# undef EASY_VERSION_INT

const uint64_t TIME_FACTOR = 1000000000ULL;

// TODO: use 128 bit integer operations for better accuracy
#define EASY_USE_FLOATING_POINT_CONVERSION

#ifdef EASY_USE_FLOATING_POINT_CONVERSION

// Suppress warnings about double to uint64 conversion
# ifdef _MSC_VER
#  pragma warning(disable:4244)
# elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
# elif defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wsign-conversion"
# endif

# define EASY_CONVERT_TO_NANO(t, freq, factor) t *= factor

#else

# define EASY_CONVERT_TO_NANO(t, freq, factor) t *= TIME_FACTOR; t /= freq

#endif

//////////////////////////////////////////////////////////////////////////

inline bool isCompatibleVersion(uint32_t _version)
{
    return _version >= MIN_COMPATIBLE_VERSION;
}

inline void write(::std::stringstream& _stream, const char* _value, size_t _size)
{
    _stream.write(_value, _size);
}

template <class T>
inline void write(::std::stringstream& _stream, const T& _value)
{
    _stream.write((const char*)&_value, sizeof(T));
}

//////////////////////////////////////////////////////////////////////////

namespace profiler {

    void SerializedData::set(char* _data, uint64_t _size)
    {
        delete [] m_data;
        m_data = _data;
        m_size = _size;
    }

    void SerializedData::set(uint64_t _size)
    {
        if (_size != 0)
            set(new char[_size], _size);
        else
            set(nullptr, 0);
    }

    void SerializedData::extend(uint64_t _size)
    {
        auto olddata = m_data;
        auto oldsize = m_size;

        m_size = oldsize + _size;
        m_data = new char[m_size];

        if (olddata != nullptr) {
            memcpy(m_data, olddata, oldsize);
            delete [] olddata;
        }
    }

    extern "C" PROFILER_API void release_stats(BlockStatistics*& _stats)
    {
        if (_stats == nullptr)
            return;

        if (--_stats->calls_number == 0)
            delete _stats;

        _stats = nullptr;
    }

}

//////////////////////////////////////////////////////////////////////////

#ifdef EASY_PROFILER_HASHED_CSTR_DEFINED

typedef ::std::unordered_map<::profiler::block_id_t, ::profiler::BlockStatistics*, ::profiler::passthrough_hash<::profiler::block_id_t> > StatsMap;

/** \note It is absolutely safe to use hashed_cstr (which simply stores pointer) because std::unordered_map,
which uses it as a key, exists only inside fillTreesFromFile function. */
typedef ::std::unordered_map<::profiler::hashed_cstr, ::profiler::block_id_t> IdMap;

typedef ::std::unordered_map<::profiler::hashed_cstr, ::profiler::BlockStatistics*> CsStatsMap;

#else

// TODO: Create optimized version of profiler::hashed_cstr for Linux too.
typedef ::std::unordered_map<::profiler::block_id_t, ::profiler::BlockStatistics*, ::profiler::passthrough_hash<::profiler::block_id_t> > StatsMap;
typedef ::std::unordered_map<::profiler::hashed_stdstring, ::profiler::block_id_t> IdMap;
typedef ::std::unordered_map<::profiler::hashed_stdstring, ::profiler::BlockStatistics*> CsStatsMap;

#endif

//////////////////////////////////////////////////////////////////////////

/** \brief Updates statistics for a profiler block.

\param _stats_map Storage of statistics for blocks.
\param _current Pointer to the current block.
\param _stats Reference to the variable where pointer to the block statistics must be written.

\note All blocks with similar name have the same pointer to statistics information.

\note As all profiler block keeps a pointer to it's statistics, all similar blocks
automatically receive statistics update.

*/
::profiler::BlockStatistics* update_statistics(StatsMap& _stats_map, const ::profiler::BlocksTree& _current, ::profiler::block_index_t _current_index, ::profiler::block_index_t _parent_index, const ::profiler::blocks_t& _blocks, bool _calculate_children = true)
{
    auto duration = _current.node->duration();
    //StatsMap::key_type key(_current.node->name());
    //auto it = _stats_map.find(key);
    auto it = _stats_map.find(_current.node->id());
    if (it != _stats_map.end())
    {
        // Update already existing statistics

        auto stats = it->second; // write pointer to statistics into output (this is BlocksTree:: per_thread_stats or per_parent_stats or per_frame_stats)

        ++stats->calls_number; // update calls number of this block
        stats->total_duration += duration; // update summary duration of all block calls

        if (_calculate_children)
        {
            for (auto i : _current.children)
                stats->total_children_duration += _blocks[i].node->duration();
        }

        if (duration > _blocks[stats->max_duration_block].node->duration())
        {
            // update max duration
            stats->max_duration_block = _current_index;
            //stats->max_duration = duration;
        }

        if (duration < _blocks[stats->min_duration_block].node->duration())
        {
            // update min duraton
            stats->min_duration_block = _current_index;
            //stats->min_duration = duration;
        }

        // average duration is calculated inside average_duration() method by dividing total_duration to the calls_number

        return stats;
    }

    // This is first time the block appear in the file.
    // Create new statistics.
    auto stats = new ::profiler::BlockStatistics(duration, _current_index, _parent_index);
    //_stats_map.emplace(key, stats);
    _stats_map.emplace(_current.node->id(), stats);

    if (_calculate_children)
    {
        for (auto i : _current.children)
            stats->total_children_duration += _blocks[i].node->duration();
    }

    return stats;
}

::profiler::BlockStatistics* update_statistics(CsStatsMap& _stats_map, const ::profiler::BlocksTree& _current, ::profiler::block_index_t _current_index, ::profiler::block_index_t _parent_index, const ::profiler::blocks_t& _blocks, bool _calculate_children = true)
{
    auto duration = _current.node->duration();
    CsStatsMap::key_type key(_current.node->name());
    auto it = _stats_map.find(key);
    if (it != _stats_map.end())
    {
        // Update already existing statistics

        auto stats = it->second; // write pointer to statistics into output (this is BlocksTree:: per_thread_stats or per_parent_stats or per_frame_stats)

        ++stats->calls_number; // update calls number of this block
        stats->total_duration += duration; // update summary duration of all block calls

        if (_calculate_children)
        {
            for (auto i : _current.children)
                stats->total_children_duration += _blocks[i].node->duration();
        }

        if (duration > _blocks[stats->max_duration_block].node->duration())
        {
            // update max duration
            stats->max_duration_block = _current_index;
            //stats->max_duration = duration;
        }

        if (duration < _blocks[stats->min_duration_block].node->duration())
        {
            // update min duraton
            stats->min_duration_block = _current_index;
            //stats->min_duration = duration;
        }

        // average duration is calculated inside average_duration() method by dividing total_duration to the calls_number

        return stats;
    }

    // This is first time the block appear in the file.
    // Create new statistics.
    auto stats = new ::profiler::BlockStatistics(duration, _current_index, _parent_index);
    _stats_map.emplace(key, stats);

    if (_calculate_children)
    {
        for (auto i : _current.children)
            stats->total_children_duration += _blocks[i].node->duration();
    }

    return stats;
}

//////////////////////////////////////////////////////////////////////////

void update_statistics_recursive(StatsMap& _stats_map, ::profiler::BlocksTree& _current, ::profiler::block_index_t _current_index, ::profiler::block_index_t _parent_index, ::profiler::blocks_t& _blocks)
{
    _current.per_frame_stats = update_statistics(_stats_map, _current, _current_index, _parent_index, _blocks, false);
    for (auto i : _current.children)
    {
        _current.per_frame_stats->total_children_duration += _blocks[i].node->duration();
        update_statistics_recursive(_stats_map, _blocks[i], i, _parent_index, _blocks);
    }
}

//////////////////////////////////////////////////////////////////////////

/*void validate_pointers(::std::atomic<int>& _progress, const char* _oldbase, ::profiler::SerializedData& _serialized_blocks, ::profiler::blocks_t& _blocks, size_t _size)
{
    if (_oldbase == nullptr)
    {
        _progress.store(25, ::std::memory_order_release);
        return;
    }

    for (size_t i = 0; i < _size; ++i)
    {
        auto& tree = _blocks[i];
        auto dist = ::std::distance(_oldbase, reinterpret_cast<const char*>(tree.node));
        tree.node = reinterpret_cast<::profiler::SerializedBlock*>(_serialized_blocks.data() + dist);
        _progress.store(20 + static_cast<int>(5 * i / _size), ::std::memory_order_release);
    }
}

void validate_pointers(::std::atomic<int>& _progress, const char* _oldbase, ::profiler::SerializedData& _serialized_descriptors, ::profiler::descriptors_list_t& _descriptors, size_t _size)
{
    if (_oldbase == nullptr)
    {
        _progress.store(5, ::std::memory_order_release);
        return;
    }

    for (size_t i = 0; i < _size; ++i)
    {
        auto dist = ::std::distance(_oldbase, reinterpret_cast<const char*>(_descriptors[i]));
        _descriptors[i] = reinterpret_cast<::profiler::SerializedBlockDescriptor*>(_serialized_descriptors.data() + dist);
        _progress.store(static_cast<int>(5 * i / _size));
    }
}*/

//////////////////////////////////////////////////////////////////////////

extern "C" {

    PROFILER_API ::profiler::block_index_t fillTreesFromFile(::std::atomic<int>& progress, const char* filename,
                                                             ::profiler::SerializedData& serialized_blocks,
                                                             ::profiler::SerializedData& serialized_descriptors,
                                                             ::profiler::descriptors_list_t& descriptors,
                                                             ::profiler::blocks_t& blocks,
                                                             ::profiler::thread_blocks_tree_t& threaded_trees,
                                                             uint32_t& total_descriptors_number,
                                                             uint32_t& version,
                                                             bool gather_statistics,
                                                             ::std::stringstream& _log)
    {
        auto oldprogress = progress.exchange(0, ::std::memory_order_release);
        if (oldprogress < 0)
        {
            _log << "Reading was interrupted";
            return 0;
        }

        ::std::ifstream inFile(filename, ::std::fstream::binary);
        if (!inFile.is_open())
        {
            _log << "Can not open file " << filename;
            return 0;
        }

        ::std::stringstream str;

        // Replace str buffer to inFile buffer to avoid redundant copying
        typedef ::std::basic_iostream<::std::stringstream::char_type, ::std::stringstream::traits_type> stringstream_parent;
        stringstream_parent& s = str;
        auto oldbuf = s.rdbuf(inFile.rdbuf());
        
        // Read data from file
        auto result = fillTreesFromStream(progress, str, serialized_blocks, serialized_descriptors, descriptors, blocks,
                                          threaded_trees, total_descriptors_number, version, gather_statistics, _log);

        // Restore old str buffer to avoid possible second memory free on stringstream destructor
        s.rdbuf(oldbuf);

        return result;
    }

    //////////////////////////////////////////////////////////////////////////

    PROFILER_API ::profiler::block_index_t fillTreesFromStream(::std::atomic<int>& progress, ::std::stringstream& inFile,
                                                               ::profiler::SerializedData& serialized_blocks,
                                                               ::profiler::SerializedData& serialized_descriptors,
                                                               ::profiler::descriptors_list_t& descriptors,
                                                               ::profiler::blocks_t& blocks,
                                                               ::profiler::thread_blocks_tree_t& threaded_trees,
                                                               uint32_t& total_descriptors_number,
                                                               uint32_t& version,
                                                               bool gather_statistics,
                                                               ::std::stringstream& _log)
    {
        EASY_FUNCTION(::profiler::colors::Cyan);

        auto oldprogress = progress.exchange(0, ::std::memory_order_release);
        if (oldprogress < 0)
        {
            _log << "Reading was interrupted";
            return 0;
        }

        uint32_t signature = 0;
        inFile.read((char*)&signature, sizeof(uint32_t));
        if (signature != PROFILER_SIGNATURE)
        {
            _log << "Wrong signature " << signature << "\nThis is not EasyProfiler file/stream.";
            return 0;
        }

        version = 0;
        inFile.read((char*)&version, sizeof(uint32_t));
        if (!isCompatibleVersion(version))
        {
            _log << "Incompatible version: v" << (version >> 24) << "." << ((version & 0x00ff0000) >> 16) << "." << (version & 0x0000ffff);
            return 0;
        }

        processid_t pid = 0;
        if (version > EASY_V_100)
        {
            if (version < EASY_V_130)
            {
                uint32_t old_pid = 0;
                inFile.read((char*)&old_pid, sizeof(uint32_t));
                pid = old_pid;
            }
            else
            {
                inFile.read((char*)&pid, sizeof(processid_t));
            }
        }

        int64_t file_cpu_frequency = 0LL;
        inFile.read((char*)&file_cpu_frequency, sizeof(int64_t));
        uint64_t cpu_frequency = file_cpu_frequency;
        const double conversion_factor = static_cast<double>(TIME_FACTOR) / static_cast<double>(cpu_frequency);

        ::profiler::timestamp_t begin_time = 0ULL;
        ::profiler::timestamp_t end_time = 0ULL;
        inFile.read((char*)&begin_time, sizeof(::profiler::timestamp_t));
        inFile.read((char*)&end_time, sizeof(::profiler::timestamp_t));
        if (cpu_frequency != 0)
        {
            EASY_CONVERT_TO_NANO(begin_time, cpu_frequency, conversion_factor);
            EASY_CONVERT_TO_NANO(end_time, cpu_frequency, conversion_factor);
        }

        uint32_t total_blocks_number = 0;
        inFile.read((char*)&total_blocks_number, sizeof(uint32_t));
        if (total_blocks_number == 0)
        {
            _log << "Profiled blocks number == 0";
            return 0;
        }

        uint64_t memory_size = 0;
        inFile.read((char*)&memory_size, sizeof(decltype(memory_size)));
        if (memory_size == 0)
        {
            _log << "Wrong memory size == 0 for " << total_blocks_number << " blocks";
            return 0;
        }

        total_descriptors_number = 0;
        inFile.read((char*)&total_descriptors_number, sizeof(uint32_t));
        if (total_descriptors_number == 0)
        {
            _log << "Blocks description number == 0";
            return 0;
        }

        uint64_t descriptors_memory_size = 0;
        inFile.read((char*)&descriptors_memory_size, sizeof(decltype(descriptors_memory_size)));
        if (descriptors_memory_size == 0)
        {
            _log << "Wrong memory size == 0 for " << total_descriptors_number << " blocks descriptions";
            return 0;
        }

        descriptors.reserve(total_descriptors_number);
        //const char* olddata = append_regime ? serialized_descriptors.data() : nullptr;
        serialized_descriptors.set(descriptors_memory_size);
        //validate_pointers(progress, olddata, serialized_descriptors, descriptors, descriptors.size());

        uint64_t i = 0;
        while (!inFile.eof() && descriptors.size() < total_descriptors_number)
        {
            uint16_t sz = 0;
            inFile.read((char*)&sz, sizeof(sz));
            if (sz == 0)
            {
                descriptors.push_back(nullptr);
                continue;
            }

            //if (i + sz > descriptors_memory_size) {
            //    printf("FILE CORRUPTED\n");
            //    return 0;
            //}

            char* data = serialized_descriptors[i];
            inFile.read(data, sz);
            auto descriptor = reinterpret_cast<::profiler::SerializedBlockDescriptor*>(data);
            descriptors.push_back(descriptor);

            i += sz;
            auto oldprogress = progress.exchange(static_cast<int>(15 * i / descriptors_memory_size), ::std::memory_order_release);
            if (oldprogress < 0)
            {
                _log << "Reading was interrupted";
                return 0; // Loading interrupted
            }
        }

        typedef ::std::unordered_map<::profiler::thread_id_t, StatsMap, ::profiler::passthrough_hash<::profiler::thread_id_t> > PerThreadStats;
        PerThreadStats parent_statistics, frame_statistics;
        IdMap identification_table;

        blocks.reserve(total_blocks_number);
        //olddata = append_regime ? serialized_blocks.data() : nullptr;
        serialized_blocks.set(memory_size);
        //validate_pointers(progress, olddata, serialized_blocks, blocks, blocks.size());

        i = 0;
        uint32_t read_number = 0;
        ::profiler::block_index_t blocks_counter = 0;
        ::std::vector<char> name;

        const size_t thread_id_t_size = version < EASY_V_130 ? sizeof(uint32_t) : sizeof(::profiler::thread_id_t);

        while (!inFile.eof() && read_number < total_blocks_number)
        {
            EASY_BLOCK("Read thread data", ::profiler::colors::DarkGreen);

            ::profiler::thread_id_t thread_id = 0;
            inFile.read((char*)&thread_id, thread_id_t_size);

            auto& root = threaded_trees[thread_id];

            uint16_t name_size = 0;
            inFile.read((char*)&name_size, sizeof(uint16_t));
            if (name_size != 0)
            {
                name.resize(name_size);
                inFile.read(name.data(), name_size);
                root.thread_name = name.data();
            }

            CsStatsMap per_thread_statistics_cs;

            uint32_t blocks_number_in_thread = 0;
            inFile.read((char*)&blocks_number_in_thread, sizeof(decltype(blocks_number_in_thread)));
            auto threshold = read_number + blocks_number_in_thread;
            while (!inFile.eof() && read_number < threshold)
            {
                EASY_BLOCK("Read context switch", ::profiler::colors::Green);

                ++read_number;

                uint16_t sz = 0;
                inFile.read((char*)&sz, sizeof(sz));
                if (sz == 0)
                {
                    _log << "Bad CSwitch block size == 0";
                    return 0;
                }

                char* data = serialized_blocks[i];
                inFile.read(data, sz);
                i += sz;
                auto baseData = reinterpret_cast<::profiler::SerializedCSwitch*>(data);
                auto t_begin = reinterpret_cast<::profiler::timestamp_t*>(data);
                auto t_end = t_begin + 1;

                if (cpu_frequency != 0)
                {
                    EASY_CONVERT_TO_NANO(*t_begin, cpu_frequency, conversion_factor);
                    EASY_CONVERT_TO_NANO(*t_end, cpu_frequency, conversion_factor);
                }

                if (*t_end > begin_time)
                {
                    if (*t_begin < begin_time)
                        *t_begin = begin_time;

                    blocks.emplace_back();
                    ::profiler::BlocksTree& tree = blocks.back();
                    tree.cs = baseData;
                    const auto block_index = blocks_counter++;

                    root.wait_time += baseData->duration();
                    root.sync.emplace_back(block_index);

                    if (gather_statistics)
                    {
                        EASY_BLOCK("Gather per thread statistics", ::profiler::colors::Coral);
                        tree.per_thread_stats = update_statistics(per_thread_statistics_cs, tree, block_index, ~0U, blocks);//, thread_id, blocks);
                    }
                }

                auto oldprogress = progress.exchange(20 + static_cast<int>(70 * i / memory_size), ::std::memory_order_release);
                if (oldprogress < 0)
                {
                    _log << "Reading was interrupted";
                    return 0; // Loading interrupted
                }
            }

            if (inFile.eof())
                break;

            StatsMap per_thread_statistics;

            blocks_number_in_thread = 0;
            inFile.read((char*)&blocks_number_in_thread, sizeof(decltype(blocks_number_in_thread)));
            threshold = read_number + blocks_number_in_thread;
            while (!inFile.eof() && read_number < threshold)
            {
                EASY_BLOCK("Read block", ::profiler::colors::Green);

                ++read_number;

                uint16_t sz = 0;
                inFile.read((char*)&sz, sizeof(sz));
                if (sz == 0)
                {
                    _log << "Bad block size == 0";
                    return 0;
                }

                char* data = serialized_blocks[i];
                inFile.read(data, sz);
                i += sz;
                auto baseData = reinterpret_cast<::profiler::SerializedBlock*>(data);
                if (baseData->id() >= total_descriptors_number)
                {
                    _log << "Bad block id == " << baseData->id();
                    return 0;
                }

                auto desc = descriptors[baseData->id()];
                if (desc == nullptr)
                {
                    _log << "Bad block id == " << baseData->id() << ". Description is null.";
                    return 0;
                }

                auto t_begin = reinterpret_cast<::profiler::timestamp_t*>(data);
                auto t_end = t_begin + 1;

                if (cpu_frequency != 0)
                {
                    EASY_CONVERT_TO_NANO(*t_begin, cpu_frequency, conversion_factor);
                    EASY_CONVERT_TO_NANO(*t_end, cpu_frequency, conversion_factor);
                }

                if (*t_end >= begin_time)
                {
                    if (*t_begin < begin_time)
                        *t_begin = begin_time;

                    blocks.emplace_back();
                    ::profiler::BlocksTree& tree = blocks.back();
                    tree.node = baseData;
                    const auto block_index = blocks_counter++;

                    if (*tree.node->name() != 0)
                    {
                        // If block has runtime name then generate new id for such block.
                        // Blocks with the same name will have same id.

                        IdMap::key_type key(tree.node->name());
                        auto it = identification_table.find(key);
                        if (it != identification_table.end())
                        {
                            // There is already block with such name, use it's id
                            baseData->setId(it->second);
                        }
                        else
                        {
                            // There were no blocks with such name, generate new id and save it in the table for further usage.
                            auto id = static_cast<::profiler::block_id_t>(descriptors.size());
                            identification_table.emplace(key, id);
                            if (descriptors.capacity() == descriptors.size())
                                descriptors.reserve((descriptors.size() * 3) >> 1);
                            descriptors.push_back(descriptors[baseData->id()]);
                            baseData->setId(id);
                        }
                    }

                    if (!root.children.empty())
                    {
                        auto& back = blocks[root.children.back()];
                        auto t1 = back.node->end();
                        auto mt0 = tree.node->begin();
                        if (mt0 < t1)//parent - starts earlier than last ends
                        {
                            //auto lower = ::std::lower_bound(root.children.begin(), root.children.end(), tree);
                            /**/
                            EASY_BLOCK("Find children", ::profiler::colors::Blue);
                            auto rlower1 = ++root.children.rbegin();
                            for (; rlower1 != root.children.rend() && !(mt0 > blocks[*rlower1].node->begin()); ++rlower1);
                            auto lower = rlower1.base();
                            ::std::move(lower, root.children.end(), ::std::back_inserter(tree.children));

                            root.children.erase(lower, root.children.end());
                            EASY_END_BLOCK;

                            if (gather_statistics)
                            {
                                EASY_BLOCK("Gather statistic within parent", ::profiler::colors::Magenta);
                                auto& per_parent_statistics = parent_statistics[thread_id];
                                per_parent_statistics.clear();

                                //per_parent_statistics.reserve(tree.children.size());     // this gives slow-down on Windows
                                //per_parent_statistics.reserve(tree.children.size() * 2); // this gives no speed-up on Windows
                                // TODO: check this behavior on Linux

                                for (auto i : tree.children)
                                {
                                    auto& child = blocks[i];
                                    child.per_parent_stats = update_statistics(per_parent_statistics, child, i, block_index, blocks);
                                    if (tree.depth < child.depth)
                                        tree.depth = child.depth;
                                }
                            }
                            else
                            {
                                for (auto i : tree.children)
                                {
                                    const auto& child = blocks[i];
                                    if (tree.depth < child.depth)
                                        tree.depth = child.depth;
                                }
                            }

                            if (tree.depth == 254)
                            {
                                // 254 because we need 1 additional level for root (thread).
                                // In other words: real stack depth = 1 root block + 254 children

                                if (*tree.node->name() != 0)
                                    _log << "Stack depth exceeded value of 254\nfor block \"" << desc->name() << "\"";
                                else
                                    _log << "Stack depth exceeded value of 254\nfor block \"" << desc->name() << "\"\nfrom file \"" << desc->file() << "\":" << desc->line();

                                return 0;
                            }

                            ++tree.depth;
                        }
                    }

                    ++root.blocks_number;
                    root.children.emplace_back(block_index);// ::std::move(tree));
                    if (desc->type() == ::profiler::BLOCK_TYPE_EVENT)
                        root.events.emplace_back(block_index);


                    if (gather_statistics)
                    {
                        EASY_BLOCK("Gather per thread statistics", ::profiler::colors::Coral);
                        tree.per_thread_stats = update_statistics(per_thread_statistics, tree, block_index, ~0U, blocks);//, thread_id, blocks);
                    }
                }

                auto oldprogress = progress.exchange(20 + static_cast<int>(70 * i / memory_size), ::std::memory_order_release);
                if (oldprogress < 0)
                {
                    _log << "Reading was interrupted";
                    return 0; // Loading interrupted
                }
            }
        }

        if (progress.load(::std::memory_order_acquire) < 0)
        {
            _log << "Reading was interrupted";
            return 0; // Loading interrupted
        }

        EASY_BLOCK("Gather statistics for roots", ::profiler::colors::Purple);
        if (gather_statistics)
        {
            ::std::vector<::std::thread> statistics_threads;
            statistics_threads.reserve(threaded_trees.size());

            for (auto& it : threaded_trees)
            {
                auto& root = it.second;
                root.thread_id = it.first;
                //root.tree.shrink_to_fit();

                auto& per_frame_statistics = frame_statistics[root.thread_id];
                auto& per_parent_statistics = parent_statistics[it.first];
                per_parent_statistics.clear();

                statistics_threads.emplace_back(::std::thread([&per_parent_statistics, &per_frame_statistics, &blocks, &descriptors](::profiler::BlocksTreeRoot& root)
                {
                    //::std::sort(root.sync.begin(), root.sync.end(), [&blocks](::profiler::block_index_t left, ::profiler::block_index_t right)
                    //{
                    //    return blocks[left].node->begin() < blocks[right].node->begin();
                    //});

                    ::profiler::block_index_t cs_index = 0;
                    for (auto i : root.children)
                    {
                        auto& frame = blocks[i];

                        if (descriptors[frame.node->id()]->type() == ::profiler::BLOCK_TYPE_BLOCK)
                            ++root.frames_number;

                        frame.per_parent_stats = update_statistics(per_parent_statistics, frame, i, ~0U, blocks);//, root.thread_id, blocks);

                        per_frame_statistics.clear();
                        update_statistics_recursive(per_frame_statistics, frame, i, i, blocks);

                        if (cs_index < root.sync.size())
                        {
                            CsStatsMap frame_stats_cs;
                            do {

                                auto j = root.sync[cs_index];
                                auto& cs = blocks[j];
                                if (cs.node->end() < frame.node->begin())
                                    continue;
                                if (cs.node->begin() > frame.node->end())
                                    break;
                                cs.per_frame_stats = update_statistics(frame_stats_cs, cs, cs_index, i, blocks);

                            } while (++cs_index < root.sync.size());
                        }

                        if (root.depth < frame.depth)
                            root.depth = frame.depth;

                        root.profiled_time += frame.node->duration();
                    }

                    ++root.depth;
                }, ::std::ref(root)));
            }

            int j = 0, n = static_cast<int>(statistics_threads.size());
            for (auto& t : statistics_threads)
            {
                t.join();
                progress.store(90 + (10 * ++j) / n, ::std::memory_order_release);
            }
        }
        else
        {
            int j = 0, n = static_cast<int>(threaded_trees.size());
            for (auto& it : threaded_trees)
            {
                auto& root = it.second;
                root.thread_id = it.first;

                //::std::sort(root.sync.begin(), root.sync.end(), [&blocks](::profiler::block_index_t left, ::profiler::block_index_t right)
                //{
                //    return blocks[left].node->begin() < blocks[right].node->begin();
                //});

                //root.tree.shrink_to_fit();
                for (auto i : root.children)
                {
                    auto& frame = blocks[i];

                    if (descriptors[frame.node->id()]->type() == ::profiler::BLOCK_TYPE_BLOCK)
                        ++root.frames_number;

                    if (root.depth < frame.depth)
                        root.depth = frame.depth;

                    root.profiled_time += frame.node->duration();
                }

                ++root.depth;

                progress.store(90 + (10 * ++j) / n, ::std::memory_order_release);
            }
        }
        // No need to delete BlockStatistics instances - they will be deleted inside BlocksTree destructors

        return blocks_counter;
    }

    //////////////////////////////////////////////////////////////////////////

    PROFILER_API bool readDescriptionsFromStream(::std::atomic<int>& progress, ::std::stringstream& inFile,
                                                 ::profiler::SerializedData& serialized_descriptors,
                                                 ::profiler::descriptors_list_t& descriptors,
                                                 ::std::stringstream& _log)
    {
        EASY_FUNCTION(::profiler::colors::Cyan);

        progress.store(0);

        uint32_t signature = 0;
        inFile.read((char*)&signature, sizeof(uint32_t));
        if (signature != PROFILER_SIGNATURE)
        {
            _log << "Wrong file signature.\nThis is not EasyProfiler file/stream.";
            return false;
        }

        uint32_t version = 0;
        inFile.read((char*)&version, sizeof(uint32_t));
        if (!isCompatibleVersion(version))
        {
            _log << "Incompatible version: v" << (version >> 24) << "." << ((version & 0x00ff0000) >> 16) << "." << (version & 0x0000ffff);
            return false;
        }

        uint32_t total_descriptors_number = 0;
        inFile.read((char*)&total_descriptors_number, sizeof(decltype(total_descriptors_number)));
        if (total_descriptors_number == 0)
        {
            _log << "Blocks description number == 0";
            return false;
        }

        uint64_t descriptors_memory_size = 0;
        inFile.read((char*)&descriptors_memory_size, sizeof(decltype(descriptors_memory_size)));
        if (descriptors_memory_size == 0)
        {
            _log << "Wrong memory size == 0 for " << total_descriptors_number << " blocks descriptions";
            return false;
        }

        descriptors.reserve(total_descriptors_number);
        //const char* olddata = append_regime ? serialized_descriptors.data() : nullptr;
        serialized_descriptors.set(descriptors_memory_size);
        //validate_pointers(progress, olddata, serialized_descriptors, descriptors, descriptors.size());

        uint64_t i = 0;
        while (!inFile.eof() && descriptors.size() < total_descriptors_number)
        {
            uint16_t sz = 0;
            inFile.read((char*)&sz, sizeof(sz));
            if (sz == 0)
            {
                descriptors.push_back(nullptr);
                continue;
            }

            //if (i + sz > descriptors_memory_size) {
            //    printf("FILE CORRUPTED\n");
            //    return 0;
            //}

            char* data = serialized_descriptors[i];
            inFile.read(data, sz);
            auto descriptor = reinterpret_cast<::profiler::SerializedBlockDescriptor*>(data);
            descriptors.push_back(descriptor);

            i += sz;
            auto oldprogress = progress.exchange(static_cast<int>(100 * i / descriptors_memory_size), ::std::memory_order_release);
            if (oldprogress < 0)
            {
                _log << "Reading was interrupted";
                return false; // Loading interrupted
            }
        }

        return !descriptors.empty();
    }

    //////////////////////////////////////////////////////////////////////////

}

#undef EASY_CONVERT_TO_NANO

#ifdef EASY_USE_FLOATING_POINT_CONVERSION
# ifdef _MSC_VER
#  pragma warning(default:4244)
# elif defined(__GNUC__)
#  pragma GCC diagnostic pop
# elif defined(__clang__)
#  pragma clang diagnostic pop
# endif
# undef EASY_USE_FLOATING_POINT_CONVERSION
#endif

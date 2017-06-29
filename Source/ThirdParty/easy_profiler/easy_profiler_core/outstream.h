/************************************************************************
* file name         : outstream.h
* ----------------- :
* creation time     : 2016/09/11
* authors           : Sergey Yagovtsev, Victor Zarubkin
* emails            : yse.sey@gmail.com, v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains definition of output stream helpers.
* ----------------- :
* change log        : * 2016/09/11 Victor Zarubkin: Initial commit. Moved sources from profiler_manager.h/.cpp
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

#ifndef EASY_PROFILER__OUTPUT_STREAM__H_
#define EASY_PROFILER__OUTPUT_STREAM__H_

#include <sstream>
#include <string.h>

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

namespace profiler {

    class OStream
    {
        ::std::stringstream m_stream;

    public:

        explicit OStream() : m_stream(std::ios_base::out | std::ios_base::binary)
        {

        }

        template <typename T> void write(const char* _data, T _size)
        {
            m_stream.write(_data, _size);
        }

        template <class T> void write(const T& _data)
        {
            m_stream.write((const char*)&_data, sizeof(T));
        }

        ::std::stringstream& stream()
        {
            return m_stream;
        }

        const ::std::stringstream& stream() const
        {
            return m_stream;
        }

        void clear()
        {
#if defined(__GNUC__) && __GNUC__ < 5
            // gcc 4 has a known bug which has been solved in gcc 5:
            // std::stringstream has no swap() method :(
            m_stream.str(::std::string());
#else
            ::std::stringstream().swap(m_stream);
#endif
        }

    }; // END of class OStream.

} // END of namespace profiler.

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#endif // EASY_PROFILER__OUTPUT_STREAM__H_

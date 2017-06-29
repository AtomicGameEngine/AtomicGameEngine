/************************************************************************
* file name         : easy_compiler_support.h
* ----------------- :
* creation time     : 2016/09/22
* authors           : Victor Zarubkin, Sergey Yagovtsev
* emails            : v.s.zarubkin@gmail.com, yse.sey@gmail.com
* ----------------- :
* description       : This file contains auxiliary profiler macros for different compiler support.
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

#ifndef EASY_PROFILER_COMPILER_SUPPORT_H
#define EASY_PROFILER_COMPILER_SUPPORT_H

#include <cstddef>

//#define EASY_CODE_WRAP(Code) Code

#if defined(_WIN32) && !defined(EASY_PROFILER_STATIC)
// Visual Studio and MinGW
# ifdef _BUILD_PROFILER
#  define PROFILER_API __declspec(dllexport)
# else
#  define PROFILER_API __declspec(dllimport)
# endif
#endif



#if defined (_MSC_VER)
//////////////////////////////////////////////////////////////////////////
// Visual Studio

# define __func__ __FUNCTION__

# if _MSC_VER <= 1800
// There is no support for C++11 thread_local keyword prior to Visual Studio 2015. Use __declspec(thread) instead.
// There is also no support for C++11 magic statics feature :( So it becomes slightly harder to initialize static vars - additional "if" for each profiler block.
#  define EASY_THREAD_LOCAL __declspec(thread)
#  define EASY_LOCAL_STATIC_PTR(VarType, VarName, VarInitializer)\
                                            __declspec(thread) static VarType VarName = 0;\
                                            if (!VarName)\
                                                VarName = VarInitializer
# endif

#elif defined (__clang__)
//////////////////////////////////////////////////////////////////////////
// Clang Compiler

# if (__clang_major__ == 3 && __clang_minor__ < 3) || (__clang_major__ < 3)
// There is no support for C++11 thread_local keyword prior to clang 3.3. Use __thread instead.
#  define EASY_THREAD_LOCAL __thread
# endif

# if (__clang_major__ == 2 && __clang_minor__ < 9) || (__clang_major__ < 2)
// There is no support for C++11 magic statics feature prior to clang 2.9. It becomes slightly harder to initialize static vars - additional "if" for each profiler block.
#  define EASY_LOCAL_STATIC_PTR(VarType, VarName, VarInitializer)\
                                            EASY_THREAD_LOCAL static VarType VarName = 0;\
                                            if (!VarName)\
                                                VarName = VarInitializer

// There is no support for C++11 final keyword prior to clang 2.9
#  define EASY_FINAL 
# endif

#elif defined(__GNUC__)
//////////////////////////////////////////////////////////////////////////
// GNU Compiler

# if (__GNUC__ == 4 && __GNUC_MINOR__ < 8) || (__GNUC__ < 4)
// There is no support for C++11 thread_local keyword prior to gcc 4.8. Use __thread instead.
#  define EASY_THREAD_LOCAL __thread
# endif

# if (__GNUC__ == 4 && __GNUC_MINOR__ < 3) || (__GNUC__ < 4)
// There is no support for C++11 magic statics feature prior to gcc 4.3. It becomes slightly harder to initialize static vars - additional "if" for each profiler block.
#  define EASY_LOCAL_STATIC_PTR(VarType, VarName, VarInitializer)\
                                            EASY_THREAD_LOCAL static VarType VarName = 0;\
                                            if (!VarName)\
                                                VarName = VarInitializer
# endif

# if (__GNUC__ == 4 && __GNUC_MINOR__ < 7) || (__GNUC__ < 4)
// There is no support for C++11 final keyword prior to gcc 4.7
#  define EASY_FINAL 
# endif

#endif
// END // TODO: Add other compilers support
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
// Default values

#ifndef EASY_THREAD_LOCAL
# define EASY_THREAD_LOCAL thread_local
# define EASY_THREAD_LOCAL_CPP11
#endif

#ifndef EASY_LOCAL_STATIC_PTR
# define EASY_LOCAL_STATIC_PTR(VarType, VarName, VarInitializer) static VarType VarName = VarInitializer
# define EASY_MAGIC_STATIC_CPP11
#endif

#ifndef EASY_FINAL
# define EASY_FINAL final
#endif

#ifndef PROFILER_API
# define PROFILER_API
#endif

//////////////////////////////////////////////////////////////////////////

#endif // EASY_PROFILER_COMPILER_SUPPORT_H

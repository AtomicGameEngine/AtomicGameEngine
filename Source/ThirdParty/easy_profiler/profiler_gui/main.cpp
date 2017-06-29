/************************************************************************
* file name         : main.cpp
* ----------------- :
* creation time     : 2016/04/29
* authors           : Sergey Yagovtsev, Victor Zarubkin
* email             : yse.sey@gmail.com, v.s.zarubkin@gmail.com
* ----------------- :
* description       : Main file for EasyProfiler GUI.
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
#include <QApplication>

#include "main_window.h"

#include <easy/reader.h>


#if defined(_WIN32) && defined (_BUILD_RELEASE_)
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    //QFileSystemModel *model = new QFileSystemModel;
    //model->setRootPath(QDir::currentPath());
//     const char* filename = 0;
//     if(argc > 1 && argv[1]){
//         filename = argv[1];
//     }else{
//         return 255;
//     }

//     QFile file(filename);
//     file.open(QIODevice::ReadOnly);
//     TreeModel model(file.readAll());
//     file.close();


//     QTreeView *tree = new QTreeView();
//     tree->setModel(&model);
// 
//     tree->show();

    auto now = ::std::chrono::duration_cast<std::chrono::seconds>(::std::chrono::system_clock::now().time_since_epoch()).count() >> 1;
    srand((unsigned int)now);

    EasyMainWindow window;
    window.show();

    return app.exec();
}

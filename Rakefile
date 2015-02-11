
include RbConfig

case CONFIG['host_os']
when /mswin|windows|mingw32/i
    $HOST_OS = "windows"
when /darwin/i
    $HOST_OS = "darwin"
else
    abort("Unknown host config: Config::CONFIG['host_os']: #{Config::CONFIG['host_os']}")
end

$RAKE_ROOT = File.dirname(__FILE__)

$ATOMICTILED_BUILD_DIR = "#{$RAKE_ROOT}/Artifacts/AtomicTiled_Build"
$ATOMICTILED_SOURCE_DIR =  "#{$RAKE_ROOT}/../AtomicTiled"


if $HOST_OS == "darwin"
  $QT_BIN_DIR = "/Users/josh/Qt/5.4/clang_64/bin"
else
  $QT_BIN_DIR = "C:\\Qt\\5.4\\msvc2013_64\\bin"
end


namespace :android do

  CMAKE_ANDROID_BUILD_FOLDER = "#{$RAKE_ROOT}/Artifacts/Android_Build"

  task :player do

    if !Dir.exists?("#{CMAKE_ANDROID_BUILD_FOLDER}")
      FileUtils.mkdir_p(CMAKE_ANDROID_BUILD_FOLDER)
    end

    Dir.chdir(CMAKE_ANDROID_BUILD_FOLDER) do
      sh "cmake -DCMAKE_TOOLCHAIN_FILE=#{$RAKE_ROOT}/CMake/Toolchains/android.toolchain.cmake -DCMAKE_BUILD_TYPE=Release ../../"
      sh "make -j8"
    end 
    
  end

end  

namespace :web do

  CMAKE_WEB_BUILD_FOLDER = "#{$RAKE_ROOT}/Artifacts/Web_Build"

  task :player do

    if !Dir.exists?("#{CMAKE_WEB_BUILD_FOLDER}")
      FileUtils.mkdir_p(CMAKE_WEB_BUILD_FOLDER)
    end

    Dir.chdir(CMAKE_WEB_BUILD_FOLDER) do
      sh "cmake -DCMAKE_TOOLCHAIN_FILE=#{$RAKE_ROOT}/CMake/Toolchains/emscripten.toolchain.cmake -DCMAKE_BUILD_TYPE=Release ../../"
      sh "make -j8"
    end 

    Dir.chdir("#{CMAKE_WEB_BUILD_FOLDER}/Source/Tools/AtomicPlayer") do
      sh "mv AtomicPlayer AtomicPlayer.bc"
      sh "emcc -O3 --llvm-lto 1 --memory-init-file 0 -s VERBOSE=0 -s USE_SDL=2 -s ASM_JS=1 -s ASSERTIONS=1 -s OUTLINING_LIMIT=20000 -s TOTAL_MEMORY=520093696 --closure 0 ./AtomicPlayer.bc -o  ./AtomicPlayer.html"
    end

  end

end  


namespace :macosx do

  ARTIFACTS_FOLDER = "#{$RAKE_ROOT}/Artifacts"
  CMAKE_MACOSX_BUILD_FOLDER = "#{$RAKE_ROOT}/Artifacts/MacOSX_Build"
  MACOSX_PACKAGE_FOLDER = "#{$RAKE_ROOT}/Artifacts/MacOSX_Package"

  task :clean do

    folders = ["#{CMAKE_MACOSX_BUILD_FOLDER}", "#{MACOSX_PACKAGE_FOLDER}",
               "#{ARTIFACTS_FOLDER}/Android_Build", "#{ARTIFACTS_FOLDER}/Web_Build",  
               "#{ARTIFACTS_FOLDER}/AtomicExamples", "#{ARTIFACTS_FOLDER}/Docs",
               "#{ARTIFACTS_FOLDER}/Examples",  "#{ARTIFACTS_FOLDER}/AtomicTiled_Build"]

    for index in 0 ... folders.size    

        if Dir.exists?(folders[index])
            puts "rm -rf #{folders[index]}"
            sh "rm -rf #{folders[index]}"
        end

        if Dir.exists?(folders[index])
            abort("Unable to clean #{folders[index]}")
        end
    
    end           

  end

	task :cmake do

    FileUtils.mkdir_p(CMAKE_MACOSX_BUILD_FOLDER)

    Dir.chdir(CMAKE_MACOSX_BUILD_FOLDER) do
      sh "cmake ../../ -DCMAKE_BUILD_TYPE=Release"
    end 

	end

	task :generate_javascript_bindings => "macosx:cmake" do

    Dir.chdir(CMAKE_MACOSX_BUILD_FOLDER) do
      sh "make -j8 JSBind"
      sh "./Source/Tools/JSBind/JSBind #{$RAKE_ROOT}"
    end

	end

  task :generate_docs => "macosx:generate_javascript_bindings" do

    Dir.chdir("#{$RAKE_ROOT}/Docs") do

      if Dir.exists?("out")
        sh "rm -rf out"
      end

      # add the generated JS bindings
      sh "./gendocs.sh"
      sh "cp -r out #{$RAKE_ROOT}/Artifacts/Docs"
    end

  end  

  task :generate_examples do

    Dir.chdir("#{$RAKE_ROOT}/Artifacts") do

      if Dir.exists?("AtomicExamples")
        sh "rm -rf AtomicExamples"
      end

      if Dir.exists?("Examples")
        sh "rm -rf Examples"
      end

      sh "mkdir Examples"

      sh "git clone https://github.com/AtomicGameEngine/AtomicExamples"
      
      Dir.chdir("AtomicExamples") do
        sh "git archive master | tar -x -C #{$RAKE_ROOT}/Artifacts/Examples"
      end

    end

  end  

	task :player => "macosx:generate_javascript_bindings" do

    Dir.chdir(CMAKE_MACOSX_BUILD_FOLDER) do
      # add the generated JS bindings
      sh "cmake ../../ -DCMAKE_BUILD_TYPE=Release"
      sh "make -j8 AtomicPlayer"
    end

	end

  task :editor => [#"android:player",
                   #"web:player",
                   "macosx:player", 
                   "atomictiled:osx",
                   "macosx:generate_examples", 
                   "macosx:generate_docs"
                   ] do

    Dir.chdir(CMAKE_MACOSX_BUILD_FOLDER) do

      
      sh "make -j8 AtomicEditor"

    end

  end

end

namespace :package do

  task :macosx_editor => ['macosx:clean', 
                          'macosx:editor'] do 


      FileUtils.mkdir_p(MACOSX_PACKAGE_FOLDER)                    

      MAC_PLAYER_APP_FOLDER_SRC = "#{CMAKE_MACOSX_BUILD_FOLDER}/Source/Tools/AtomicPlayer/AtomicPlayer.app"
      MAC_EDITOR_APP_FOLDER_SRC = "#{CMAKE_MACOSX_BUILD_FOLDER}/AtomicEditor/AtomicEditor.app"

      COREDATA_FOLDER_SRC = "#{$RAKE_ROOT}/Bin/CoreData"
      DATA_FOLDER_SRC = "#{$RAKE_ROOT}/Bin/Data"
      EDITORRESOURCES_FOLDER_SRC = "#{$RAKE_ROOT}/AtomicEditor/EditorResources"

      PROJECTTEMPLATES_FOLDER_SRC = "#{$RAKE_ROOT}/AtomicEditor/EditorApplicationData/ProjectTemplates"

      #Example info could possibly go in the AtomicExamples repo
      EXAMPLEINFO_FOLDER_SRC = "#{$RAKE_ROOT}/AtomicEditor/EditorApplicationData/ExampleInfo"

      EXAMPLES_FOLDER_SRC = "#{$RAKE_ROOT}/Artifacts/Examples"
      DOCS_FOLDER_SRC = "#{$RAKE_ROOT}/Artifacts/Docs"

      MAC_EDITOR_APP_FOLDER_DST = "#{MACOSX_PACKAGE_FOLDER}/AtomicEditor.app"
      MAC_EDITOR_APP_RESOURCE_FOLDER_DST = "#{MACOSX_PACKAGE_FOLDER}/AtomicEditor.app/Contents/Resources"
      

      sh "cp -r #{MAC_EDITOR_APP_FOLDER_SRC} #{MACOSX_PACKAGE_FOLDER}/AtomicEditor.app"

      DEPLOYMENT_FOLDER_DST = "#{MAC_EDITOR_APP_RESOURCE_FOLDER_DST}/Deployment"

      FileUtils.mkdir_p("#{DEPLOYMENT_FOLDER_DST}")

      FileUtils.mkdir_p(DEPLOYMENT_FOLDER_DST + "/MacOS")
      FileUtils.mkdir_p(DEPLOYMENT_FOLDER_DST + "/Android")

      sh "cp -r #{COREDATA_FOLDER_SRC} #{MAC_EDITOR_APP_RESOURCE_FOLDER_DST}/CoreData"
      sh "cp -r #{DATA_FOLDER_SRC} #{MAC_EDITOR_APP_RESOURCE_FOLDER_DST}/Data"
      sh "cp -r #{EDITORRESOURCES_FOLDER_SRC} #{MAC_EDITOR_APP_RESOURCE_FOLDER_DST}/EditorResources"
      sh "cp -r #{MAC_PLAYER_APP_FOLDER_SRC} #{DEPLOYMENT_FOLDER_DST}/MacOS/AtomicPlayer.app"

      sh "cp -r #{EXAMPLES_FOLDER_SRC} #{MAC_EDITOR_APP_RESOURCE_FOLDER_DST}/Examples"
      sh "cp -r #{DOCS_FOLDER_SRC} #{MAC_EDITOR_APP_RESOURCE_FOLDER_DST}/Docs"

      sh "cp -r #{PROJECTTEMPLATES_FOLDER_SRC} #{MAC_EDITOR_APP_RESOURCE_FOLDER_DST}/ProjectTemplates"
      sh "cp -r #{EXAMPLEINFO_FOLDER_SRC} #{MAC_EDITOR_APP_RESOURCE_FOLDER_DST}/ExampleInfo"

      # DEPLOY TILED
      ATOMICTILED_DEPLOYED_DIR = "#{MAC_EDITOR_APP_FOLDER_DST}/Contents/Applications"

      FileUtils.mkdir_p(ATOMICTILED_DEPLOYED_DIR)

      FileUtils.cp_r("#{$ATOMICTILED_BUILD_DIR}/bin/Tiled.app", "#{ATOMICTILED_DEPLOYED_DIR}/Tiled.app")

      Dir.chdir(ATOMICTILED_DEPLOYED_DIR) do
        sh "#{$QT_BIN_DIR}/macdeployqt #{ATOMICTILED_DEPLOYED_DIR}/Tiled.app"
      end


  end

end

namespace :windows do

  CMAKE_WINDOWS_BUILD_FOLDER = "#{$RAKE_ROOT}/Artifacts/Windows_Build"
  PACKAGE_WINDOWS_FOLDER = "#{$RAKE_ROOT}/Artifacts/Windows_Package"

  task :clean do

    if Dir.exists?("#{CMAKE_WINDOWS_BUILD_FOLDER}")
    	FileUtils.rmtree("#{CMAKE_WINDOWS_BUILD_FOLDER}")
    end

    if Dir.exists?("#{CMAKE_WINDOWS_BUILD_FOLDER}")
        abort("Unable to clean #{CMAKE_WINDOWS_BUILD_FOLDER}")
    end

    if Dir.exists?("#{PACKAGE_WINDOWS_FOLDER}")
      FileUtils.rmtree("#{PACKAGE_WINDOWS_FOLDER}")
    end

    if Dir.exists?("#{PACKAGE_WINDOWS_FOLDER}")
        abort("Unable to clean #{PACKAGE_WINDOWS_FOLDER}")
    end  

  end

	task :cmake do

    FileUtils.mkdir_p(CMAKE_WINDOWS_BUILD_FOLDER)

    Dir.chdir(CMAKE_WINDOWS_BUILD_FOLDER) do
      sh "cmake ../../ -G\"NMake Makefiles\" -DCMAKE_BUILD_TYPE=Release"
    end 

	end

	task :generate_javascript_bindings => "windows:cmake" do

    Dir.chdir(CMAKE_WINDOWS_BUILD_FOLDER) do
      sh "nmake JSBind"
      sh "./Source/Tools/JSBind/JSBind.exe #{$RAKE_ROOT}"
    end

	end

	task :player => "windows:generate_javascript_bindings" do

    Dir.chdir(CMAKE_WINDOWS_BUILD_FOLDER) do
      # add the generated JS bindings
      sh "cmake ../../ -G\"NMake Makefiles\" -DCMAKE_BUILD_TYPE=Release"
      sh "nmake AtomicPlayer"
    end

	end

  task :editor => "windows:player" do

    Dir.chdir(CMAKE_WINDOWS_BUILD_FOLDER) do
      # add the generated JS bindings
      sh "nmake AtomicEditor"

      PLAYER_APP_EXE = "#{CMAKE_WINDOWS_BUILD_FOLDER}/Source/Tools/AtomicPlayer/AtomicPlayer.exe"
      EDITOR_APP_FOLDER = "#{CMAKE_WINDOWS_BUILD_FOLDER}/AtomicEditor"
      DEPLOYMENT_FOLDER = "#{EDITOR_APP_FOLDER}/Deployment/Win32"

      COREDATA_FOLDER = "#{$RAKE_ROOT}/Bin/CoreData"
      DATA_FOLDER = "#{$RAKE_ROOT}/Bin/Data"
      EDITORRESOURCES_FOLDER = "#{$RAKE_ROOT}/AtomicEditor/EditorResources"

      FileUtils.mkdir_p(DEPLOYMENT_FOLDER)

      FileUtils.cp_r("#{COREDATA_FOLDER}", "#{EDITOR_APP_FOLDER}/CoreData")
      FileUtils.cp_r("#{DATA_FOLDER}", "#{EDITOR_APP_FOLDER}/Data")
      FileUtils.cp_r("#{EDITORRESOURCES_FOLDER}", "#{EDITOR_APP_FOLDER}/EditorResources")
      FileUtils.cp("#{PLAYER_APP_EXE}", "#{DEPLOYMENT_FOLDER}/AtomicPlayer.exe")
    end

  end

end

namespace :package_windows do

  task :editor => ['windows:clean', 'windows:editor'] do

  end

end

namespace :atomictiled do

  task :windows do

    QT_CREATOR_BIN_DIR = "C:\\Qt\\Tools\\QtCreator\\bin"  
    ENV['PATH'] = "#{$QT_BIN_DIR};" + ENV['PATH']    

    ATOMICTILED_BUILD_DIR = "#{$RAKE_ROOT}/Artifacts/AtomicTiled_Build"
    ATOMICTILED_SOURCE_DIR =  "#{$RAKE_ROOT}\\..\\AtomicTiled"
    ATOMICTILED_DEPLOYED_DIR = "#{$RAKE_ROOT}\\Artifacts\\AtomicTiled_Deployed"

    FileUtils.mkdir_p(ATOMICTILED_BUILD_DIR)

    Dir.chdir(ATOMICTILED_BUILD_DIR) do
      sh "qmake.exe -r \"#{ATOMICTILED_SOURCE_DIR}\\tiled.pro\" \"CONFIG+=release\" \"QMAKE_CXXFLAGS+=-DBUILD_INFO_VERSION=ATOMIC_BUILD\""
      sh "#{QT_CREATOR_BIN_DIR}\\jom.exe -j8"
    end

    FileUtils.mkdir_p(ATOMICTILED_DEPLOYED_DIR)

    FileUtils.cp("#{ATOMICTILED_BUILD_DIR}\\tiled.exe", "#{ATOMICTILED_DEPLOYED_DIR}")
    FileUtils.cp("#{ATOMICTILED_BUILD_DIR}\\tiled.dll", "#{ATOMICTILED_DEPLOYED_DIR}")

    Dir.chdir(ATOMICTILED_DEPLOYED_DIR) do
      sh "windeployqt.exe --release #{ATOMICTILED_DEPLOYED_DIR}\\tiled.exe"
    end

  end

    task :osx do

      FileUtils.mkdir_p($ATOMICTILED_BUILD_DIR)

      Dir.chdir($ATOMICTILED_BUILD_DIR) do
        sh "#{$QT_BIN_DIR}/qmake -r \"#{$ATOMICTILED_SOURCE_DIR}/tiled.pro\" \"CONFIG+=release\" \"QMAKE_CXXFLAGS+=-DBUILD_INFO_VERSION=ATOMIC_BUILD\""
        sh "make -j8"
      end

  end


end
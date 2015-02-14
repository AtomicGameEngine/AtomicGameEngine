
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

ATOMICTILED_BUILD_DIR = "#{$RAKE_ROOT}/Artifacts/AtomicTiled_Build"
ATOMICTILED_SOURCE_DIR =  "#{$RAKE_ROOT}/../AtomicTiled"


if $HOST_OS == "darwin"
  $QT_BIN_DIR = "#{ENV["QT_SDK"]}/bin"
else
  $QT_BIN_DIR = "C:\\Qt\\5.4\\msvc2013_64\\bin"
  QT_CREATOR_BIN_DIR = "C:\\Qt\\Tools\\QtCreator\\bin"  
  ENV['PATH'] = "#{QT_CREATOR_BIN_DIR};" + ENV['PATH']    
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
      sh "cmake -DEMSCRIPTEN=1 -DCMAKE_TOOLCHAIN_FILE=#{$RAKE_ROOT}/CMake/Toolchains/emscripten.toolchain.cmake -DCMAKE_BUILD_TYPE=Release ../../"
      sh "make -j8"
    end 

    Dir.chdir("#{CMAKE_WEB_BUILD_FOLDER}/Source/AtomicPlayer") do
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
      sh "./Source/Tools/JSBind/JSBind #{$RAKE_ROOT} WEB"
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

  task :editor => ["macosx:player"] do

    Dir.chdir(CMAKE_MACOSX_BUILD_FOLDER) do
      
      sh "make -j8 AtomicEditor"

    end

  end

end

namespace :package do

  task :macosx_preflight => ['macosx:clean',
                          'web:player',
                          'android:player',
                          "atomictiled:osx",
                          'macosx:editor',
                          'macosx:generate_examples', 
                          'macosx:generate_docs'] do

      FileUtils.mkdir_p(MACOSX_PACKAGE_FOLDER)                    

      MAC_PLAYER_APP_FOLDER_SRC = "#{CMAKE_MACOSX_BUILD_FOLDER}/Source/AtomicPlayer/AtomicPlayer.app"
      MAC_EDITOR_APP_FOLDER_SRC = "#{CMAKE_MACOSX_BUILD_FOLDER}/Source/AtomicEditor/AtomicEditor.app"

      # Resources
      COREDATA_FOLDER_SRC = "#{$RAKE_ROOT}/Data/AtomicPlayer/Resources/CoreData"
      DATA_FOLDER_SRC = "#{$RAKE_ROOT}/Data/AtomicPlayer/Resources/Data"
      EDITORRESOURCES_FOLDER_SRC = "#{$RAKE_ROOT}/Data/AtomicEditor/Resources/EditorData"
      EDITORAPPLICATIONDATA_FOLDER_SRC = "#{$RAKE_ROOT}/Data/AtomicEditor"

      # Project Templates
      PROJECTTEMPLATES_FOLDER_SRC = "#{EDITORAPPLICATIONDATA_FOLDER_SRC}/ProjectTemplates"

      #Examples
      #Example info could possibly go in the AtomicExamples repo
      EXAMPLEINFO_FOLDER_SRC = "#{EDITORAPPLICATIONDATA_FOLDER_SRC}/ExampleInfo"
      EXAMPLES_FOLDER_SRC = "#{$RAKE_ROOT}/Artifacts/Examples"

      #Docs
      DOCS_FOLDER_SRC = "#{$RAKE_ROOT}/Artifacts/Docs"

      MAC_EDITOR_APP_FOLDER_DST = "#{MACOSX_PACKAGE_FOLDER}/AtomicEditor.app"
      MAC_EDITOR_APP_RESOURCE_FOLDER_DST = "#{MACOSX_PACKAGE_FOLDER}/AtomicEditor.app/Contents/Resources"      

      # Copy the Editor application
      sh "cp -r #{MAC_EDITOR_APP_FOLDER_SRC} #{MACOSX_PACKAGE_FOLDER}/AtomicEditor.app"

      DEPLOYMENT_FOLDER_DST = "#{MAC_EDITOR_APP_RESOURCE_FOLDER_DST}/Deployment"

      # Make Deployment folder
      FileUtils.mkdir_p("#{DEPLOYMENT_FOLDER_DST}")

      # Copy Resources
      sh "cp -r #{COREDATA_FOLDER_SRC} #{MAC_EDITOR_APP_RESOURCE_FOLDER_DST}/CoreData"
      sh "cp -r #{DATA_FOLDER_SRC} #{MAC_EDITOR_APP_RESOURCE_FOLDER_DST}/Data"
      sh "cp -r #{EDITORRESOURCES_FOLDER_SRC} #{MAC_EDITOR_APP_RESOURCE_FOLDER_DST}/EditorData"

      # Copy Deployment

      # MacOS deployment
      FileUtils.mkdir_p("#{DEPLOYMENT_FOLDER_DST}/MacOS")
      sh "cp -r #{MAC_PLAYER_APP_FOLDER_SRC} #{DEPLOYMENT_FOLDER_DST}/MacOS/AtomicPlayer.app"

      # Android Deployment
      sh "cp -r #{EDITORAPPLICATIONDATA_FOLDER_SRC}/Deployment/Android #{DEPLOYMENT_FOLDER_DST}/Android"
      FileUtils.mkdir_p("#{DEPLOYMENT_FOLDER_DST}/Android/libs/armeabi-v7a")
      sh "cp #{CMAKE_ANDROID_BUILD_FOLDER}/Source/AtomicPlayer/libAtomicPlayer.so #{DEPLOYMENT_FOLDER_DST}/Android/libs/armeabi-v7a/libAtomicPlayer.so"

      # Web Deployment
      sh "cp -r #{EDITORAPPLICATIONDATA_FOLDER_SRC}/Deployment/Web #{DEPLOYMENT_FOLDER_DST}/Web"
      sh "cp #{CMAKE_WEB_BUILD_FOLDER}/Source/AtomicPlayer/AtomicPlayer.js #{DEPLOYMENT_FOLDER_DST}/Web/AtomicPlayer.js"

      sh "cp -r #{EXAMPLES_FOLDER_SRC} #{MAC_EDITOR_APP_RESOURCE_FOLDER_DST}/Examples"
      sh "cp -r #{DOCS_FOLDER_SRC} #{MAC_EDITOR_APP_RESOURCE_FOLDER_DST}/Docs"

      sh "cp -r #{PROJECTTEMPLATES_FOLDER_SRC} #{MAC_EDITOR_APP_RESOURCE_FOLDER_DST}/ProjectTemplates"
      sh "cp -r #{EXAMPLEINFO_FOLDER_SRC} #{MAC_EDITOR_APP_RESOURCE_FOLDER_DST}/ExampleInfo"

      # DEPLOY TILED
      ATOMICTILED_DEPLOYED_DIR = "#{MAC_EDITOR_APP_FOLDER_DST}/Contents/Applications"

      FileUtils.mkdir_p(ATOMICTILED_DEPLOYED_DIR)

      FileUtils.cp_r("#{ATOMICTILED_BUILD_DIR}/bin/Tiled.app", "#{ATOMICTILED_DEPLOYED_DIR}/Tiled.app")

      Dir.chdir(ATOMICTILED_DEPLOYED_DIR) do
        sh "#{$QT_BIN_DIR}/macdeployqt #{ATOMICTILED_DEPLOYED_DIR}/Tiled.app"
      end

  end

  task :macosx_editor do

    EDITOR_APP_FOLDER = "#{MACOSX_PACKAGE_FOLDER}/AtomicEditor.app" 
    if (!File.file?("#{EDITOR_APP_FOLDER}/Contents/Resources/Deployment/Win64/AtomicPlayer.exe"))
      abort("Missing Windows player, please run rake package:windows_editor from Windows")
    end

    Dir.chdir("#{MACOSX_PACKAGE_FOLDER}") do

      if (File.file?("AtomicEditor_MacOSX.zip"))
        sh "rm AtomicEditor_MacOSX.zip"
      end 

      if (File.file?("AtomicEditor_MacOSX.zip"))
        abort ("Couldn't remove editor")
      end       

      sh "zip -r AtomicEditor_MacOSX.zip ./AtomicEditor.app"
      sh "zip -T AtomicEditor_MacOSX.zip"

    end



  end


  task :windows_preflight => ['windows:clean', 
                              'windows:editor', 
                              'atomictiled:windows' ] do
                          

    EDITOR_APP_FOLDER_DST = "#{WINDOWS_PACKAGE_FOLDER}/AtomicEditor" 
    
    FileUtils.mkdir_p(EDITOR_APP_FOLDER_DST)

    PLAYER_APP_EXE_SRC = "#{CMAKE_WINDOWS_BUILD_FOLDER}/Source/AtomicPlayer/AtomicPlayer.exe"
    EDITOR_APP_EXE_SRC = "#{CMAKE_WINDOWS_BUILD_FOLDER}/Source/AtomicEditor/AtomicEditor.exe"

    DEPLOYMENT_FOLDER = "#{EDITOR_APP_FOLDER_DST}/Deployment/Win64"

    # Resources
    COREDATA_FOLDER_SRC = "#{$RAKE_ROOT}/Data/AtomicPlayer/Resources/CoreData"
    DATA_FOLDER_SRC = "#{$RAKE_ROOT}/Data/AtomicPlayer/Resources/Data"
    EDITORRESOURCES_FOLDER_SRC = "#{$RAKE_ROOT}/Data/AtomicEditor/Resources/EditorData"
    EDITORAPPLICATIONDATA_FOLDER_SRC = "#{$RAKE_ROOT}/Data/AtomicEditor"

    # Project Templates
    PROJECTTEMPLATES_FOLDER_SRC = "#{EDITORAPPLICATIONDATA_FOLDER_SRC}/ProjectTemplates"

    #Examples
    #Example info could possibly go in the AtomicExamples repo
    EXAMPLEINFO_FOLDER_SRC = "#{EDITORAPPLICATIONDATA_FOLDER_SRC}/ExampleInfo"    
    
    FileUtils.cp_r("#{COREDATA_FOLDER_SRC}", "#{EDITOR_APP_FOLDER_DST}/CoreData")
    FileUtils.cp_r("#{DATA_FOLDER_SRC}", "#{EDITOR_APP_FOLDER_DST}/Data")
    FileUtils.cp_r("#{EDITORRESOURCES_FOLDER_SRC}", "#{EDITOR_APP_FOLDER_DST}/EditorData")

    FileUtils.cp_r("#{PROJECTTEMPLATES_FOLDER_SRC}", "#{EDITOR_APP_FOLDER_DST}/ProjectTemplates")
    FileUtils.cp_r("#{EXAMPLEINFO_FOLDER_SRC}", "#{EDITOR_APP_FOLDER_DST}/ExampleInfo")
    
    FileUtils.mkdir_p("#{DEPLOYMENT_FOLDER}")
    FileUtils.cp("#{PLAYER_APP_EXE_SRC}", "#{DEPLOYMENT_FOLDER}/AtomicPlayer.exe")
    FileUtils.cp("#{EDITOR_APP_EXE_SRC}", "#{EDITOR_APP_FOLDER_DST}/AtomicEditor.exe")

    # DEPLOY TILED
    ATOMICTILED_DEPLOYED_DIR = "#{EDITOR_APP_FOLDER_DST}/Applications/AtomicTiled"

    FileUtils.mkdir_p(ATOMICTILED_DEPLOYED_DIR)

    FileUtils.cp("#{ATOMICTILED_BUILD_DIR}/tiled.exe", "#{ATOMICTILED_DEPLOYED_DIR}")
    FileUtils.cp("#{ATOMICTILED_BUILD_DIR}/tiled.dll", "#{ATOMICTILED_DEPLOYED_DIR}")

    ENV['PATH'] = "#{$QT_BIN_DIR};" + ENV['PATH'] 
    Dir.chdir(ATOMICTILED_DEPLOYED_DIR) do
      sh "windeployqt.exe --release #{ATOMICTILED_DEPLOYED_DIR}/tiled.exe"
    end

  end

  task :windows_editor do

    MAC_ARTIFACTS_SRC = "Z:/Artifacts/MacOSX_Package/AtomicEditor.app/Contents/Resources"
    EDITOR_APP_FOLDER_DST = "#{WINDOWS_PACKAGE_FOLDER}/AtomicEditor"

     Dir.chdir("#{WINDOWS_PACKAGE_FOLDER}") do

      if (File.exists?("AtomicEditor_Windows.zip"))
        FileUtils.rm("AtomicEditor_Windows.zip")
      end 

      if (File.exists?("AtomicEditor_Windows.zip"))
        abort("Unable to remove AtomicEditor_Windows.zip")
      end 
 
    end  

    DEPLOYMENT_FOLDER = "#{EDITOR_APP_FOLDER_DST}/Deployment"
    FileUtils.cp_r("#{MAC_ARTIFACTS_SRC}/Deployment/Android", "#{EDITOR_APP_FOLDER_DST}/Deployment/Android")
    FileUtils.cp_r("#{MAC_ARTIFACTS_SRC}/Deployment/MacOS", "#{EDITOR_APP_FOLDER_DST}/Deployment/MacOS")

    FileUtils.cp_r("#{MAC_ARTIFACTS_SRC}/Docs", "#{EDITOR_APP_FOLDER_DST}/Docs")
    FileUtils.cp_r("#{MAC_ARTIFACTS_SRC}/Examples", "#{EDITOR_APP_FOLDER_DST}/Examples")

    # copy windows player to mac
    PLAYER_APP_EXE_SRC = "#{CMAKE_WINDOWS_BUILD_FOLDER}/Source/AtomicPlayer/AtomicPlayer.exe"
    FileUtils.mkdir_p("#{MAC_ARTIFACTS_SRC}/Deployment/Win64")
    FileUtils.cp("#{PLAYER_APP_EXE_SRC}", "#{MAC_ARTIFACTS_SRC}/Deployment/Win64/AtomicPlayer.exe")

    Dir.chdir("#{WINDOWS_PACKAGE_FOLDER}") do

      sh "\"C:\\Program Files\\7-Zip\\7z.exe\" a -tzip AtomicEditor_Windows.zip AtomicEditor"
      sh "\"C:\\Program Files\\7-Zip\\7z.exe\" t AtomicEditor_Windows.zip"

    end  

  end


end

namespace :windows do

  CMAKE_WINDOWS_BUILD_FOLDER = "#{$RAKE_ROOT}/Artifacts/Windows_Build"
  WINDOWS_PACKAGE_FOLDER = "#{$RAKE_ROOT}/Artifacts/Windows_Package"

  task :clean do


    folders = ["#{CMAKE_WINDOWS_BUILD_FOLDER}", "#{WINDOWS_PACKAGE_FOLDER}",
               "#{ARTIFACTS_FOLDER}/AtomicExamples", "#{ARTIFACTS_FOLDER}/Docs",
               "#{ARTIFACTS_FOLDER}/Examples",  "#{ARTIFACTS_FOLDER}/AtomicTiled_Build"]

    for index in 0 ... folders.size    

        if Dir.exists?(folders[index])
            FileUtils.rmtree(folders[index])          
        end

        if Dir.exists?(folders[index])
            abort("Unable to clean #{folders[index]}")
        end
    
    end           

  end

	task :cmake do

    FileUtils.mkdir_p(CMAKE_WINDOWS_BUILD_FOLDER)

    Dir.chdir(CMAKE_WINDOWS_BUILD_FOLDER) do
      sh "cmake ../../ -G\"NMake Makefiles JOM\" -DCMAKE_BUILD_TYPE=Release"
    end 

	end

	task :generate_javascript_bindings => "windows:cmake" do

    Dir.chdir(CMAKE_WINDOWS_BUILD_FOLDER) do
      sh "jom -j4 JSBind"
      sh "./Source/Tools/JSBind/JSBind.exe #{$RAKE_ROOT}"
    end

	end

	task :player => "windows:generate_javascript_bindings" do

    Dir.chdir(CMAKE_WINDOWS_BUILD_FOLDER) do
      # add the generated JS bindings
      sh "cmake ../../ -G\"NMake Makefiles JOM\" -DCMAKE_BUILD_TYPE=Release"
      sh "jom -j4 AtomicPlayer"
    end

	end

  task :editor => "windows:player" do

    Dir.chdir(CMAKE_WINDOWS_BUILD_FOLDER) do
      
      sh "jom -j4 AtomicEditor"

    end

  end

end

namespace :atomictiled do

  task :windows do

    ENV['PATH'] = "#{$QT_BIN_DIR};" + ENV['PATH']    

    FileUtils.mkdir_p(ATOMICTILED_BUILD_DIR)

    Dir.chdir(ATOMICTILED_BUILD_DIR) do
      sh "qmake.exe -r \"#{ATOMICTILED_SOURCE_DIR}\\tiled.pro\" \"CONFIG+=release\" \"QMAKE_CXXFLAGS+=-DBUILD_INFO_VERSION=ATOMIC_BUILD\""
      sh "#{QT_CREATOR_BIN_DIR}\\jom.exe -j8"
    end

  end

    task :osx do

      FileUtils.mkdir_p(ATOMICTILED_BUILD_DIR)

      Dir.chdir(ATOMICTILED_BUILD_DIR) do
        sh "#{$QT_BIN_DIR}/qmake -r \"#{ATOMICTILED_SOURCE_DIR}/tiled.pro\" \"CONFIG+=release\" \"QMAKE_CXXFLAGS+=-DBUILD_INFO_VERSION=ATOMIC_BUILD\""
        sh "make -j8"
      end

  end


end

require 'rbconfig'

def get_os
@os ||= (
  host_os = RbConfig::CONFIG['host_os']
  case host_os
  when /mswin|msys|mingw|cygwin|bccwin|wince|emc/
    :windows
  when /darwin|mac os/
    :macosx
  when /linux/
    :linux
  when /solaris|bsd/
    :unix
  else
    raise Error::WebDriverError, "unknown os: #{host_os.inspect}"
  end
)
end

$RAKE_ROOT = File.dirname(__FILE__)

namespace :android do

  CMAKE_ANDROID_BUILD_FOLDER = "#{$RAKE_ROOT}/Artifacts/Android_Build"

  task :build_player do

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

  task :build_player do

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


namespace :build_macosx do

  CMAKE_MACOSX_BUILD_FOLDER = "#{$RAKE_ROOT}/Artifacts/MacOSX_Build"
  MACOSX_PACKAGE_FOLDER = "#{$RAKE_ROOT}/Artifacts/MacOSX_Package"

  task :clean do

    if Dir.exists?("#{CMAKE_MACOSX_BUILD_FOLDER}")
      sh "rm -rf #{CMAKE_MACOSX_BUILD_FOLDER}"
    end

    if Dir.exists?("#{CMAKE_MACOSX_BUILD_FOLDER}")
        abort("Unable to clean #{CMAKE_MACOSX_BUILD_FOLDER}")
    end

    if Dir.exists?("#{MACOSX_PACKAGE_FOLDER}")
      sh "rm -rf #{MACOSX_PACKAGE_FOLDER}"
    end

    if Dir.exists?("#{MACOSX_PACKAGE_FOLDER}")
        abort("Unable to clean #{MACOSX_PACKAGE_FOLDER}")
    end  

  end

	task :cmake do

    FileUtils.mkdir_p(CMAKE_MACOSX_BUILD_FOLDER)

    Dir.chdir(CMAKE_MACOSX_BUILD_FOLDER) do
      sh "cmake ../../ -DCMAKE_BUILD_TYPE=Release"
    end 

	end

	task :generate_javascript_bindings => "build_macosx:cmake" do

    Dir.chdir(CMAKE_MACOSX_BUILD_FOLDER) do
      sh "make -j8 JSBind"
      sh "./Source/Tools/JSBind/JSBind #{$RAKE_ROOT}"
    end

	end

	task :player => "build_macosx:generate_javascript_bindings" do

    Dir.chdir(CMAKE_MACOSX_BUILD_FOLDER) do
      # add the generated JS bindings
      sh "cmake ../../ -DCMAKE_BUILD_TYPE=Release"
      sh "make -j8 AtomicPlayer"
    end

	end

  task :editor => "build_macosx:player" do

    Dir.chdir(CMAKE_MACOSX_BUILD_FOLDER) do
      # add the generated JS bindings
      sh "make -j8 AtomicEditor"

      PLAYER_APP_FOLDER = "#{CMAKE_MACOSX_BUILD_FOLDER}/Source/Tools/AtomicPlayer/AtomicPlayer.app"
      EDITOR_APP_FOLDER = "#{CMAKE_MACOSX_BUILD_FOLDER}/AtomicEditor/AtomicEditor.app"
      DEPLOYMENT_FOLDER = "#{EDITOR_APP_FOLDER}/Contents/Resources/Deployment/MacOS"

      COREDATA_FOLDER = "#{$RAKE_ROOT}/Bin/CoreData"
      DATA_FOLDER = "#{$RAKE_ROOT}/Bin/Data"
      EDITORRESOURCES_FOLDER = "#{$RAKE_ROOT}/AtomicEditor/EditorResources"

      if Dir.exists?("#{EDITOR_APP_FOLDER}/Contents/Resources")
        sh "rm -rf #{EDITOR_APP_FOLDER}/Contents/Resources"
      end

      FileUtils.mkdir_p(DEPLOYMENT_FOLDER)

      sh "cp -r #{COREDATA_FOLDER} #{EDITOR_APP_FOLDER}/Contents/Resources/CoreData"
      sh "cp -r #{DATA_FOLDER} #{EDITOR_APP_FOLDER}/Contents/Resources/Data"
      sh "cp -r #{EDITORRESOURCES_FOLDER} #{EDITOR_APP_FOLDER}/Contents/Resources/EditorResources"
      sh "cp -r #{PLAYER_APP_FOLDER} #{DEPLOYMENT_FOLDER}/AtomicPlayer.app"

    end

  end

end

namespace :package_macosx do

  task :editor => ['build_macosx:clean', 'build_macosx:editor'] do

  end

end

namespace :build_windows do

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

	task :generate_javascript_bindings => "build_windows:cmake" do

    Dir.chdir(CMAKE_WINDOWS_BUILD_FOLDER) do
      sh "nmake JSBind"
      sh "./Source/Tools/JSBind/JSBind.exe #{$RAKE_ROOT}"
    end

	end

	task :player => "build_windows:generate_javascript_bindings" do

    Dir.chdir(CMAKE_WINDOWS_BUILD_FOLDER) do
      # add the generated JS bindings
      sh "cmake ../../ -G\"NMake Makefiles\" -DCMAKE_BUILD_TYPE=Release"
      sh "nmake AtomicPlayer"
    end

	end

  task :editor => "build_windows:player" do

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

  task :editor => ['build_windows:clean', 'build_windows:editor'] do

  end

end

namespace :atomictiled do

  task :build_windows do

    QT_BIN_DIR = "C:\\Qt\\5.4\\msvc2013_64\\bin"
    QT_CREATOR_BIN_DIR = "C:\\Qt\\Tools\\QtCreator\\bin"  
    ENV['PATH'] = "#{QT_BIN_DIR};" + ENV['PATH']    

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

    task :build_osx do

    QT_BIN_DIR = "/Users/josh/Qt/5.4/clang_64/bin"

    ATOMICTILED_BUILD_DIR = "#{$RAKE_ROOT}/Artifacts/AtomicTiled_Build"
    ATOMICTILED_SOURCE_DIR =  "#{$RAKE_ROOT}/../AtomicTiled"
    ATOMICTILED_DEPLOYED_DIR = "#{$RAKE_ROOT}/Artifacts/AtomicTiled_Deployed"

    FileUtils.mkdir_p(ATOMICTILED_BUILD_DIR)

    Dir.chdir(ATOMICTILED_BUILD_DIR) do
      sh "#{QT_BIN_DIR}/qmake -r \"#{ATOMICTILED_SOURCE_DIR}/tiled.pro\" \"CONFIG+=release\" \"QMAKE_CXXFLAGS+=-DBUILD_INFO_VERSION=ATOMIC_BUILD\""
      sh "make -j8"
    end

    FileUtils.mkdir_p(ATOMICTILED_DEPLOYED_DIR)

    FileUtils.cp_r("#{ATOMICTILED_BUILD_DIR}/bin/Tiled.app", "#{ATOMICTILED_DEPLOYED_DIR}/Tiled.app")

    Dir.chdir(ATOMICTILED_DEPLOYED_DIR) do
      sh "#{QT_BIN_DIR}/macdeployqt #{ATOMICTILED_DEPLOYED_DIR}/Tiled.app"
    end

  end


end
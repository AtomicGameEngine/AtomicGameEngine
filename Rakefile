
include RbConfig

case CONFIG['host_os']
when /mswin|windows|mingw32/i
    $HOST_OS = "windows"
when /darwin/i
    $HOST_OS = "darwin"
else
    $HOST_OS = "linux"
end

$RAKE_ROOT = File.dirname(__FILE__)

ARTIFACTS_FOLDER = "#{$RAKE_ROOT}/Artifacts"

CMAKE_WINDOWS_BUILD_FOLDER = "#{ARTIFACTS_FOLDER}/Windows_Build"
CMAKE_MACOSX_BUILD_FOLDER = "#{ARTIFACTS_FOLDER}/MacOSX_Build"
CMAKE_ANDROID_BUILD_FOLDER = "#{ARTIFACTS_FOLDER}/Android_Build"
CMAKE_IOS_BUILD_FOLDER = "#{ARTIFACTS_FOLDER}/IOS_Build"
CMAKE_WEB_BUILD_FOLDER = "#{ARTIFACTS_FOLDER}/Web_Build"
CMAKE_LINUX_BUILD_FOLDER = "#{ARTIFACTS_FOLDER}/Linux_Build"

ATOMICTOOL_BIN_MACOSX = "#{CMAKE_MACOSX_BUILD_FOLDER}/Source/AtomicTool/Release/AtomicTool"

namespace :build  do

  task :macosx_atomictool do

    if !Dir.exists?("#{CMAKE_MACOSX_BUILD_FOLDER}")
      FileUtils.mkdir_p(CMAKE_MACOSX_BUILD_FOLDER)
    end

    Dir.chdir(CMAKE_MACOSX_BUILD_FOLDER) do

      sh "cmake ../../ -G Xcode -DCMAKE_BUILD_TYPE=Release"
      sh "xcodebuild -target AtomicTool -configuration Release"

    end

  end

  task :windows do

    # remove existing build folder
    if Dir.exists?("#{CMAKE_WINDOWS_BUILD_FOLDER}")
      FileUtils.rm_rf("#{CMAKE_WINDOWS_BUILD_FOLDER}")
    end

    # create build folder
    if !Dir.exists?("#{CMAKE_WINDOWS_BUILD_FOLDER}")
      FileUtils.mkdir_p(CMAKE_WINDOWS_BUILD_FOLDER)
    end

    Dir.chdir(CMAKE_WINDOWS_BUILD_FOLDER) do

      sh "cmake ../../ -G \"Visual Studio 12 2013 Win64\""
      sh "msbuild /m Atomic.sln /p:Configuration=Release"

    end

  end

  task :macosx do

    if !Dir.exists?("#{CMAKE_MACOSX_BUILD_FOLDER}")
      FileUtils.mkdir_p(CMAKE_MACOSX_BUILD_FOLDER)
    end

    Dir.chdir(CMAKE_MACOSX_BUILD_FOLDER) do

      sh "cmake ../../ -G Xcode"
      sh "xcodebuild -configuration Release"

    end

  end

  task :linux do

    if !Dir.exists?("#{CMAKE_LINUX_BUILD_FOLDER}")
      FileUtils.mkdir_p(CMAKE_LINUX_BUILD_FOLDER)
    end

    Dir.chdir(CMAKE_LINUX_BUILD_FOLDER) do

      sh "cmake ../../ -DLINUX=1 -G \"Unix Makefiles\""
      sh "make -j2"

    end

  end


  #IOS, dependent on macosx for JSBind
  task :ios =>  "build:macosx_atomictool" do

      if !Dir.exists?("#{CMAKE_IOS_BUILD_FOLDER}")
        FileUtils.mkdir_p(CMAKE_IOS_BUILD_FOLDER)
      end

      Dir.chdir(CMAKE_IOS_BUILD_FOLDER) do
        sh "#{ATOMICTOOL_BIN_MACOSX} bind #{$RAKE_ROOT} Source/AtomicJS/Packages/Atomic/ IOS"
        sh "cmake -DIOS=1 -DCMAKE_BUILD_TYPE=Release -G Xcode ../../"
        # the -s option adds $KEYCHAIN to the search scope, while the -d option adds $KEYCHAIN to the system domain; both are needed
        sh "security -v list-keychains -d system -s /Users/jenkins/Library/Keychains/codesign.keychain"
        sh "security -v unlock-keychain /Users/jenkins/Library/Keychains/codesign.keychain"
        sh "xcodebuild -configuration Release"
      end

  end

  task :android =>  "build:macosx_atomictool" do

      if !Dir.exists?("#{CMAKE_ANDROID_BUILD_FOLDER}")
        FileUtils.mkdir_p(CMAKE_ANDROID_BUILD_FOLDER)
      end

      Dir.chdir(CMAKE_ANDROID_BUILD_FOLDER) do

        sh "#{ATOMICTOOL_BIN_MACOSX} bind #{$RAKE_ROOT} Source/AtomicJS/Packages/Atomic/ ANDROID"
        sh "cmake -DCMAKE_TOOLCHAIN_FILE=#{$RAKE_ROOT}/CMake/Toolchains/android.toolchain.cmake -DCMAKE_BUILD_TYPE=Release ../../"
        sh "make -j4"
      end

  end

  task :web => "build:macosx_atomictool" do

      if !Dir.exists?("#{CMAKE_WEB_BUILD_FOLDER}")
        FileUtils.mkdir_p(CMAKE_WEB_BUILD_FOLDER)
      end

      Dir.chdir(CMAKE_WEB_BUILD_FOLDER) do
        sh "#{ATOMICTOOL_BIN_MACOSX} bind #{$RAKE_ROOT} Source/AtomicJS/Packages/Atomic/ WEB"
        sh "cmake -DEMSCRIPTEN=1 -DATOMIC_BUILD_2D=1 -DCMAKE_TOOLCHAIN_FILE=#{$RAKE_ROOT}/CMake/Toolchains/emscripten.toolchain.cmake -DCMAKE_BUILD_TYPE=Release ../../"
        sh "make -j4"
      end

      Dir.chdir("#{CMAKE_WEB_BUILD_FOLDER}/Source/AtomicPlayer") do
        sh "mv AtomicPlayer AtomicPlayer.bc"
        sh "emcc -O3 -s ASM_JS=1 -s VERBOSE=0 -s USE_SDL=2 -s TOTAL_MEMORY=134217728 -s AGGRESSIVE_VARIABLE_ELIMINATION=1 -s ERROR_ON_UNDEFINED_SYMBOLS=1 -s NO_EXIT_RUNTIME=1 ./AtomicPlayer.bc -o  ./AtomicPlayer.html"
      end

  end


end


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

namespace :build_macosx do

  CMAKE_BUILD_FOLDER = "#{$RAKE_ROOT}/Artifacts/MacOSX_Build"
  PACKAGE_FOLDER = "#{$RAKE_ROOT}/Artifacts/MacOSX_Package"

  task :clean do

    if Dir.exists?("#{CMAKE_BUILD_FOLDER}")
      sh "rm -rf #{CMAKE_BUILD_FOLDER}"
    end

    if Dir.exists?("#{CMAKE_BUILD_FOLDER}")
        abort("Unable to clean #{$CMAKE_BUILD_FOLDER}")
    end

    if Dir.exists?("#{PACKAGE_FOLDER}")
      sh "rm -rf #{PACKAGE_FOLDER}"
    end

    if Dir.exists?("#{PACKAGE_FOLDER}")
        abort("Unable to clean #{$PACKAGE_FOLDER}")
    end  

  end

	task :cmake do

    FileUtils.mkdir_p(CMAKE_BUILD_FOLDER)

    Dir.chdir(CMAKE_BUILD_FOLDER) do
      sh "cmake ../../ -DCMAKE_BUILD_TYPE=Debug"
    end 

	end

	task :generate_javascript_bindings => "build_macosx:cmake" do

    Dir.chdir(CMAKE_BUILD_FOLDER) do
      sh "make -j8 JSBind"
      sh "./Source/Tools/JSBind/JSBind"
    end

	end

	task :player => "build_macosx:generate_javascript_bindings" do

    Dir.chdir(CMAKE_BUILD_FOLDER) do
      # add the generated JS bindings
      sh "cmake ../../ -DCMAKE_BUILD_TYPE=Debug"
      sh "make -j8 AtomicPlayer"
    end

	end

  task :editor => "build_macosx:player" do

    Dir.chdir(CMAKE_BUILD_FOLDER) do
      # add the generated JS bindings
      sh "make -j8 AtomicEditor"

      PLAYER_APP_FOLDER = "#{CMAKE_BUILD_FOLDER}/Source/Tools/AtomicPlayer/AtomicPlayer.app"
      EDITOR_APP_FOLDER = "#{CMAKE_BUILD_FOLDER}/AtomicEditor/AtomicEditor.app"
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
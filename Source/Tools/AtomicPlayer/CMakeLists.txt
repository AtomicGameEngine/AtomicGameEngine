# Define target name
set (TARGET_NAME AtomicPlayer)

include_directories (../../ThirdParty/TurboBadger ../../ThirdParty/Duktape)

# Define source files
define_source_files ()

# Setup target with resource copying
setup_main_executable ()


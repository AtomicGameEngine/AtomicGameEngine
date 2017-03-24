To create the duk_config.h file change into the directory ${DUKTAPE}/tools and run the
following Python command (you can copy/paste it):

python2 configure.py \
    --output-directory=out \
    --omit-removed-config-options --omit-deprecated-config-options --omit-unused-config-options \
    --emit-config-sanity-check \
    --option-file ../config/examples/performance_sensitive.yaml \
    --option-file ../config/examples/timing_sensitive.yaml \
    --option-file ../config/examples/debugger_support.yaml

An 'extern "C"' block has to be added to the headers duk_logging.h and duk_module_duktape.h
(from the directory ${DUKTAPE}/extras) to avoid C++ decoration of the function names.
This is tracked in https://github.com/svaarala/duktape/issues/1405.


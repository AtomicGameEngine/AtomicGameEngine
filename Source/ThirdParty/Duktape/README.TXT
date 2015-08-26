
duk_config.h generated with:

python config/genconfig.py --metadata config --output duk_config.h \
    --option-file config/examples/performance_sensitive.yaml \
    --option-file config/examples/timing_sensitive.yaml \
    -DDUK_USE_FASTINT \
    autodetect-header

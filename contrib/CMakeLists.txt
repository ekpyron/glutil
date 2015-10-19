add_library (lz4 STATIC EXCLUDE_FROM_ALL lz4/lz4.c lz4/lz4hc.c)
set_property (TARGET lz4 PROPERTY POSITION_INDEPENDENT_CODE True)
target_include_directories (lz4 INTERFACE lz4)

idf_component_register(SRCS "ringbuf.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "turbox_adapter"
                    LDFRAGMENTS linker.lf)

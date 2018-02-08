SOURCES += \
    $$PWD/src/main_cpprofiler.cpp \
    $$PWD/src/cpprofiler/conductor.cpp \
    $$PWD/src/cpprofiler/tcp_server.cpp \
    $$PWD/src/cpprofiler/receiver_thread.cpp \
    $$PWD/src/cpprofiler/receiver_worker.cpp \
    $$PWD/src/cpprofiler/execution.cpp \
    $$PWD/src/cpprofiler/builder_thread.cpp \
    $$PWD/src/cpprofiler/execution_list.cpp \
    $$PWD/src/cpprofiler/utils/utils.cpp \
    $$PWD/src/cpprofiler/tree/node.cpp \
    $$PWD/src/cpprofiler/tree/structure.cpp \
    $$PWD/src/cpprofiler/tree/layout.cpp \
    $$PWD/src/cpprofiler/tree/shape.cpp \
    $$PWD/src/cpprofiler/tree/node_tree.cpp \

HEADERS += \
    $$PWD/src/cpprofiler/conductor.hh \
    $$PWD/src/cpprofiler/tcp_server.hh \
    $$PWD/src/cpprofiler/receiver_thread.hh \
    $$PWD/src/cpprofiler/receiver_worker.hh \
    $$PWD/src/cpprofiler/execution.hh \
    $$PWD/src/cpprofiler/builder_thread.hh \
    $$PWD/src/cpprofiler/execution_list.hh \
    $$PWD/src/cpprofiler/utils/utils.hh \
    $$PWD/src/cpprofiler/tree/node.hh \
    $$PWD/src/cpprofiler/tree/structure.hh \
    $$PWD/src/cpprofiler/tree/layout.hh \
    $$PWD/src/cpprofiler/tree/shape.hh \
    $$PWD/src/cpprofiler/tree/node_tree.hh \
    $$PWD/src/cpprofiler/tree/node_id.hh \

SOURCES += \
    $$PWD/src/cpprofiler/tests/tree_test.cpp

HEADERS += \
    $$PWD/src/cpprofiler/tests/tree_test.hh
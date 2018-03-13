SOURCES += \
    $$PWD/src/main_cpprofiler.cpp \
    $$PWD/src/cpprofiler/conductor.cpp \
    $$PWD/src/cpprofiler/tcp_server.cpp \
    $$PWD/src/cpprofiler/receiver_thread.cpp \
    $$PWD/src/cpprofiler/receiver_worker.cpp \
    $$PWD/src/cpprofiler/execution.cpp \
    $$PWD/src/cpprofiler/user_data.cpp \
    $$PWD/src/cpprofiler/builder_thread.cpp \
    $$PWD/src/cpprofiler/execution_list.cpp \
    $$PWD/src/cpprofiler/execution_window.cpp \
    $$PWD/src/cpprofiler/utils/utils.cpp \
    $$PWD/src/cpprofiler/utils/perf_helper.cpp \
    $$PWD/src/cpprofiler/utils/array.cpp \
    $$PWD/src/cpprofiler/utils/std_ext.cpp \
    $$PWD/src/cpprofiler/tree/node.cpp \
    $$PWD/src/cpprofiler/tree/structure.cpp \
    $$PWD/src/cpprofiler/tree/layout.cpp \
    $$PWD/src/cpprofiler/tree/layout_computer.cpp \
    $$PWD/src/cpprofiler/tree/shape.cpp \
    $$PWD/src/cpprofiler/tree/node_tree.cpp \
    $$PWD/src/cpprofiler/tree/node_id.cpp \
    $$PWD/src/cpprofiler/tree/node_info.cpp \
    $$PWD/src/cpprofiler/tree/traditional_view.cpp \
    $$PWD/src/cpprofiler/tree/cursors/node_cursor.cpp \
    $$PWD/src/cpprofiler/tree/cursors/drawing_cursor.cpp \
    $$PWD/src/cpprofiler/tree/cursors/layout_cursor.cpp \
    $$PWD/src/cpprofiler/tree/cursors/nodevisitor.hpp \
    $$PWD/src/cpprofiler/analyses/similar_subtree_window.cpp \

HEADERS += \
    $$PWD/src/cpprofiler/config.hh \
    $$PWD/src/cpprofiler/conductor.hh \
    $$PWD/src/cpprofiler/tcp_server.hh \
    $$PWD/src/cpprofiler/receiver_thread.hh \
    $$PWD/src/cpprofiler/receiver_worker.hh \
    $$PWD/src/cpprofiler/execution.hh \
    $$PWD/src/cpprofiler/user_data.hh \
    $$PWD/src/cpprofiler/builder_thread.hh \
    $$PWD/src/cpprofiler/execution_list.hh \
    $$PWD/src/cpprofiler/execution_window.hh \
    $$PWD/src/cpprofiler/utils/utils.hh \
    $$PWD/src/cpprofiler/utils/perf_helper.hh \
    $$PWD/src/cpprofiler/utils/array.hh \
    $$PWD/src/cpprofiler/utils/std_ext.hh \
    $$PWD/src/cpprofiler/tree/node.hh \
    $$PWD/src/cpprofiler/tree/structure.hh \
    $$PWD/src/cpprofiler/tree/layout.hh \
    $$PWD/src/cpprofiler/tree/layout_computer.hh \
    $$PWD/src/cpprofiler/tree/shape.hh \
    $$PWD/src/cpprofiler/tree/node_tree.hh \
    $$PWD/src/cpprofiler/tree/node_id.hh \
    $$PWD/src/cpprofiler/tree/node_info.hh \
    $$PWD/src/cpprofiler/tree/traditional_view.hh \
    $$PWD/src/cpprofiler/tree/subtree_view.hh \
    $$PWD/src/cpprofiler/tree/cursors/node_cursor.hh \
    $$PWD/src/cpprofiler/tree/cursors/height_cursor.hh \
    $$PWD/src/cpprofiler/tree/cursors/drawing_cursor.hh \
    $$PWD/src/cpprofiler/tree/cursors/layout_cursor.hh \
    $$PWD/src/cpprofiler/tree/cursors/nodevisitor.hh \
    $$PWD/src/cpprofiler/core.hh \
    $$PWD/src/cpprofiler/utils/debug_mutex.hh \
    $$PWD/src/cpprofiler/analyses/similar_subtree_window.hh \
    $$PWD/src/cpprofiler/analyses/subtree_pattern.hh \
    $$PWD/src/cpprofiler/analyses/histogram_scene.hh \

SOURCES += \
    $$PWD/src/cpprofiler/tests/tree_test.cpp \
    $$PWD/src/cpprofiler/tests/execution_test.cpp \

HEADERS += \
    $$PWD/src/cpprofiler/tests/tree_test.hh \
    $$PWD/src/cpprofiler/tests/execution_test.hh \
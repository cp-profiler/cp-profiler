#pragma once

namespace cpprofiler { namespace tree {

namespace layout {
    constexpr int dist_y = 38;
    constexpr int min_dist_x = 16;
}

namespace traditional {
    constexpr int NODE_WIDTH = 20;
    constexpr int FAILED_WIDTH = 14;
    constexpr int HALF_FAILED_WIDTH = FAILED_WIDTH / 2;
    constexpr int HALF_NODE_WIDTH = NODE_WIDTH / 2;
    constexpr int SHADOW_OFFSET = 3.0;
    constexpr int HIDDEN_DEPTH = layout::dist_y + FAILED_WIDTH;
}


}}
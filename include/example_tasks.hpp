#pragma once
#include <string_view>

namespace profit::examples {

constexpr std::string_view TASK1 =
    R"({"width":30,"height":20,"objects":[{"type":"deposit","x":1,"y":1,"subtype":0,"width":5,"height":5},{"type":"deposit","x":1,"y":14,"subtype":1,"width":5,"height":5},{"type":"deposit","x":22,"y":1,"subtype":2,"width":7,"height":7},{"type":"obstacle","x":11,"y":9,"width":19,"height":2},{"type":"obstacle","x":11,"y":1,"width":2,"height":8}],"products":[{"type":"product","subtype":0,"resources":[3,3,3,0,0,0,0,0],"points":10}],"turns":50,"time":300})";
constexpr std::string_view TASK2 =
    R"({"width":26,"height":5,"objects":[{"type":"deposit","x":0,"y":0,"subtype":0,"width":5,"height":5},{"type":"obstacle","x":5,"y":2,"width":16,"height":1}],"products":[{"type":"product","subtype":0,"resources":[10,0,0,0,0,0,0,0],"points":10}],"turns":20,"time":60})";
constexpr std::string_view TASK3 =
    R"({"width":40,"height":40,"objects":[{"type":"deposit","x":1,"y":1,"subtype":0,"width":7,"height":7},{"type":"deposit","x":36,"y":36,"subtype":1,"width":3,"height":3}],"products":[{"type":"product","subtype":0,"resources":[36,3,0,0,0,0,0,0],"points":10}],"turns":14,"time":300})";
constexpr std::string_view TASK4 =
    R"({"width":29,"height":23,"objects":[{"type":"obstacle","x":8,"y":0,"width":4,"height":11},{"type":"obstacle","x":8,"y":12,"width":4,"height":11},{"type":"deposit","x":0,"y":0,"subtype":0,"width":8,"height":9},{"type":"deposit","x":0,"y":14,"subtype":1,"width":8,"height":9},{"type":"deposit","x":21,"y":0,"subtype":2,"width":8,"height":9},{"type":"deposit","x":21,"y":14,"subtype":3,"width":8,"height":9},{"type":"obstacle","x":17,"y":0,"width":4,"height":11},{"type":"obstacle","x":17,"y":12,"width":4,"height":11},{"type":"obstacle","x":14,"y":10,"width":1,"height":3},{"type":"obstacle","x":12,"y":0,"width":5,"height":1},{"type":"obstacle","x":12,"y":22,"width":5,"height":1}],"products":[{"type":"product","subtype":0,"resources":[10,10,0,0,0,0,0,0],"points":10},{"type":"product","subtype":1,"resources":[0,0,10,10,0,0,0,0],"points":10}],"turns":50,"time":120})";

constexpr std::string_view TRICKY_CONVEYOR_CONNECTION =
    R"({"width":30,"height":20,"objects":[{"type":"deposit","x":0,"y":0,"subtype":0,"width":3,"height":3},{"type":"obstacle","x":0,"y":5,"width":6,"height":15},{"type":"obstacle","x":6,"y":0,"width":3,"height":3},{"type":"obstacle","x":6,"y":4,"width":3,"height":13},{"type":"obstacle","x":10,"y":1,"width":3,"height":11},{"type":"obstacle","x":9,"y":13,"width":11,"height":3},{"type":"obstacle","x":14,"y":0,"width":16,"height":12},{"type":"obstacle","x":20,"y":13,"width":5,"height":7},{"type":"obstacle","x":9,"y":16,"width":11,"height":4}],"products":[{"type":"product","subtype":0,"resources":[10,0,0,0,0,0,0,0],"points":10}],"turns":100})";
constexpr std::string_view VERTICALLY_SPLIT_MAP =
    R"({"width":40,"height":20,"objects":[{"type":"obstacle","x":18,"y":0,"width":3,"height":20},{"type":"deposit","x":8,"y":2,"subtype":0,"width":3,"height":3},{"type":"deposit","x":30,"y":2,"subtype":1,"width":3,"height":3}],"products":[{"type":"product","subtype":0,"resources":[10,0,0,0,0,0,0,0],"points":10},{"type":"product","subtype":1,"resources":[0,3,0,0,0,0,0,0],"points":1}],"turns":100})";
constexpr std::string_view VERY_STRONG_CONSTRAINED =
    R"({"width":39,"height":20,"objects":[{"type":"deposit","x":0,"y":0,"subtype":0,"width":33,"height":20},{"type":"obstacle","x":33,"y":7,"width":6,"height":13}],"products":[{"type":"product","subtype":0,"resources":[10,0,0,0,0,0,0,0],"points":10}],"turns":100})";
constexpr std::string_view MULTIPLE_CONNECTED_COMPONENTS =
    R"({"width":20,"height":20,"objects":[{"type":"obstacle","x":9,"y":0,"width":2,"height":20},{"type":"obstacle","x":0,"y":9,"width":9,"height":2},{"type":"obstacle","x":11,"y":9,"width":9,"height":2},{"type":"deposit","x":0,"y":0,"subtype":0,"width":3,"height":3},{"type":"deposit","x":6,"y":6,"subtype":1,"width":3,"height":3},{"type":"deposit","x":14,"y":1,"subtype":2,"width":3,"height":3},{"type":"deposit","x":11,"y":4,"subtype":3,"width":3,"height":3},{"type":"deposit","x":3,"y":11,"subtype":4,"width":3,"height":3},{"type":"deposit","x":3,"y":14,"subtype":5,"width":3,"height":3},{"type":"deposit","x":13,"y":12,"subtype":6,"width":3,"height":3},{"type":"deposit","x":16,"y":16,"subtype":7,"width":3,"height":3}],"products":[{"type":"product","subtype":0,"resources":[10,0,0,0,0,0,0,0],"points":10}],"turns":100})";

constexpr std::string_view CONSTRAINED_TO_MINE_AND_CONVEYOR3 =
    R"({"width":5,"height":17,"objects":[{"type":"deposit","x":0,"y":0,"subtype":0,"width":5,"height":5},{"type":"obstacle","x":1,"y":8,"width":4,"height":4},{"type":"obstacle","x":2,"y":5,"width":3,"height":3}],"products":[{"type":"product","subtype":0,"resources":[10,0,0,0,0,0,0,0],"points":10}],"turns":100,"time":10})";

constexpr std::string_view CONSTRAINED_TO_MINE_AND_CONVEYORS =
    R"({"width":5,"height":18,"objects":[{"type":"deposit","x":0,"y":0,"subtype":0,"width":5,"height":5},{"type":"obstacle","x":0,"y":5,"width":1,"height":5},{"type":"obstacle","x":1,"y":9,"width":3,"height":4},{"type":"obstacle","x":2,"y":7,"width":3,"height":1}],"products":[{"type":"product","subtype":0,"resources":[10,0,0,0,0,0,0,0],"points":10}],"turns":100,"time":10})";

constexpr std::string_view IMPOSSIBLE_DUE_TO_SELF_INTERSECTION =
    R"({"width":5,"height":18,"objects":[{"type":"deposit","x":0,"y":0,"subtype":0,"width":5,"height":5},{"type":"obstacle","x":0,"y":7,"width":4,"height":3}],"products":[{"type":"product","subtype":0,"resources":[10,0,0,0,0,0,0,0],"points":10}],"turns":100,"time":10})";

}  // namespace profit::examples

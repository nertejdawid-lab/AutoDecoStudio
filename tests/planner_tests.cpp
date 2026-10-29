#include "../src/DecoPlanner.hpp"
#include <cassert>
#include <iostream>
#include <set>
using namespace autodeco;
int main() {
    std::vector<Seed> seeds = {{1,15,15},{1,45,15},{31,75,15},{1,15,15}};
    for (int i=0; i<4; ++i) {
        auto theme = static_cast<Theme>(i);
        auto deco = generate(seeds,theme,100,true,true);
        assert(deco.size() >= 4 && deco.size() <= 10);
        assert(toLevelString(deco).find("1,") == 0);
        for (auto& d : deco) {
            assert(d.objectID != 1 && d.objectID != 31);
            assert(d.scale > 0 && d.scale < 2);
        }
        assert(generate(seeds,theme,100,true,true) .size() == deco.size());
    }
    assert(generate(seeds,Theme::Hell,0).empty());
    assert(generate({{31,15,15}},Theme::Hell,100).empty());
    assert(generate({{1,15,15}},Theme::Hell,100,true,true,0).empty());
    assert(generate({{1,15,15},{1,15,15}},Theme::Hell,20).size()==1);
    std::cout << "AutoDeco planner tests: OK\n";
}

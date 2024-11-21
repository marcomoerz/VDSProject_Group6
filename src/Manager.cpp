#include "Manager.h"

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <array>
#include <string>
#include <gvc.h>

extern gvplugin_library_t gvplugin_dot_layout_LTX_library;
extern gvplugin_library_t gvplugin_core_LTX_library;
extern gvplugin_library_t gvplugin_gd_LTX_library;

GVC_t *gvc = gvContext();

namespace ClassProject {

Manager::Manager() {
    uniqueTable.emplace(False(), Node{FalseId, FalseId, FalseId});
    uniqueTable.emplace(True(), Node{TrueId, TrueId, TrueId});
    labelTable[TrueId] = "True";
    reverselabelTable["True"] = TrueId;
    labelTable[FalseId] = "False";
    reverselabelTable["False"] = FalseId;
    reverseTable[Node{TrueId, TrueId, TrueId}] = TrueId;
    reverseTable[Node{FalseId, FalseId, FalseId}] = FalseId;
    nextID = 2;
    gvAddLibrary(gvc, &gvplugin_dot_layout_LTX_library);
    gvAddLibrary(gvc, &gvplugin_core_LTX_library);
    gvAddLibrary(gvc, &gvplugin_gd_LTX_library);
}

BDD_ID Manager::createVar(const std::string &label) {
    auto it = reverselabelTable.find(label);
    if (it == reverselabelTable.end()) {
        uniqueTable.emplace(nextID, Node{nextID, TrueId, FalseId});
        reverseTable[Node{nextID, TrueId, FalseId}] = nextID;
        labelTable[nextID] = label; // label + " ? 1 : 0";
        reverselabelTable[label] = nextID;
        return nextID++;
    } else {
        return it->second;
    }
}

const BDD_ID &Manager::True() {
    return TrueId;
}

const BDD_ID &Manager::False() {
    return FalseId;
}

bool Manager::isConstant(BDD_ID f) {
    return f <= 1;
}

bool Manager::isVariable(BDD_ID x) {
    return uniqueTable.at(x).topVar == x && !isConstant(x);
}

BDD_ID Manager::topVar(BDD_ID f) {
    return uniqueTable.at(f).topVar;
}

BDD_ID Manager::ite(BDD_ID i, BDD_ID t, BDD_ID e) {
    if (i == TrueId) return t;
    else if (i == FalseId) return e;
    else if (t == e) return t;
    else if (t == TrueId && e == FalseId) return i;
    else {
        auto iteIt = iteTable.find(Node{i, t, e});
        if (iteIt != iteTable.end()) {
            return iteIt->second;
        }
        BDD_ID top = topVar(i);
        if (topVar(t) < top && isVariable(topVar(t))) top = topVar(t);
        if (topVar(e) < top && isVariable(topVar(e))) top = topVar(e);
        BDD_ID high = ite(coFactorTrue(i, top), coFactorTrue(t, top), coFactorTrue(e, top));
        BDD_ID low = ite(coFactorFalse(i, top), coFactorFalse(t, top), coFactorFalse(e, top));
        if (high == low) {
            iteTable.emplace(Node{i, t, e}, high);
            return high;
        }
        auto it = reverseTable.find({top, high, low});
        if (it == reverseTable.end()) {
            uniqueTable.emplace(nextID, Node{top, high, low});
            reverseTable[Node{top, high, low}] = nextID;
            auto label = labelTable.at(top) + " ? (" + labelTable.at(high) + ") : (" + labelTable.at(low) + ")";
            labelTable[nextID] = label;
            reverselabelTable[label] = nextID;
            iteTable.emplace(Node{i, t, e}, nextID);
            return nextID++;
        } else {
            iteTable.emplace(Node{i, t, e}, it->second);
            return it->second;
        }
    }
}

BDD_ID Manager::coFactorTrue(BDD_ID f, BDD_ID x) {
    if (topVar(f) > x || topVar(f) <= 1) return f;
    if (topVar(f) == x) return uniqueTable.at(f).high;
    else {
        auto coTrueIt = coTrueTable.find(Node{f, x, 0});
        if (coTrueIt != coTrueTable.end()) {
            return coTrueIt->second;
        }
        BDD_ID high = coFactorTrue(uniqueTable.at(f).high, x);
        BDD_ID low = coFactorTrue(uniqueTable.at(f).low, x);
        if (high == low) {
            coTrueTable.emplace(Node{f, x, 0}, high);
            return high;
        }
        auto it = reverseTable.find(Node{topVar(f), high, low});
        if (it == reverseTable.end()) {
            uniqueTable.emplace(nextID, Node{topVar(f), high, low});
            auto label = labelTable.at(topVar(f)).substr(0, 1) + " ? (" + labelTable.at(high) + ") : (" + labelTable.at(low) + ")";
            labelTable[nextID] = label;
            reverseTable[Node{f, high, low}] = nextID;
            reverseTable[Node{topVar(f), high, low}] = nextID;
            coTrueTable.emplace(Node{f, x, 0}, nextID);
            return nextID++;
        } else {
            coTrueTable.emplace(Node{f, x, 0}, it->second);
            return it->second;
        }
    }
}

BDD_ID Manager::coFactorFalse(BDD_ID f, BDD_ID x) {
    if (topVar(f) > x || topVar(f) <= 1) return f;
    if (topVar(f) == x) return uniqueTable.at(f).low;
    else {
        auto coFalseIt = coFalseTable.find(Node{f, x, 0});
        if (coFalseIt != coFalseTable.end()) {
            return coFalseIt->second;
        }
        BDD_ID high = coFactorFalse(uniqueTable.at(f).high, x);
        BDD_ID low = coFactorFalse(uniqueTable.at(f).low, x);
        if (high == low) {
            coFalseTable.emplace(Node{f, x, 0}, high);
            return high;
        }
        auto it = reverseTable.find({topVar(f), high, low});
        if (it == reverseTable.end()) {
            uniqueTable.emplace(nextID, Node{topVar(f), high, low});
            auto label = labelTable.at(topVar(f)).substr(0, 1) + " ? (" + labelTable.at(high) + ") : (" + labelTable.at(low) + ")";
            labelTable[nextID] = label;
            reverselabelTable[label] = nextID;
            reverseTable[Node{f, high, low}] = nextID;
            coFalseTable.emplace(Node{f, x, 0}, nextID);
            return nextID++;
        } else {
            coFalseTable.emplace(Node{f, x, 0}, it->second);
            return it->second;
        }
    }
}

BDD_ID Manager::coFactorTrue(BDD_ID f) {
    return coFactorTrue(f, topVar(f));
}

BDD_ID Manager::coFactorFalse(BDD_ID f) {
    return coFactorFalse(f, topVar(f));
}

BDD_ID Manager::and2(BDD_ID a, BDD_ID b) {
    return ite(a, b, FalseId);
}

BDD_ID Manager::or2(BDD_ID a, BDD_ID b) {
    return ite(a, TrueId, b);
}

BDD_ID Manager::xor2(BDD_ID a, BDD_ID b) {
    return ite(a, neg(b), b);
}

BDD_ID Manager::neg(BDD_ID a) {
    return ite(a, FalseId, TrueId);
}

BDD_ID Manager::nand2(BDD_ID a, BDD_ID b) {
    return ite(a, neg(b), TrueId);
}

BDD_ID Manager::nor2(BDD_ID a, BDD_ID b) {
    return ite(a, FalseId, neg(b));
}

BDD_ID Manager::xnor2(BDD_ID a, BDD_ID b) {
    return ite(a, b, neg(b));
}

std::string Manager::getTopVarName(const BDD_ID &root) {
    return labelTable.at(topVar(root));
}

void Manager::findNodes(const BDD_ID &root, std::set<BDD_ID> &nodes_of_root) {
    nodes_of_root.insert(root);
    if (root <= 1) return;
    else {
        findNodes(uniqueTable.at(root).high, nodes_of_root);
        findNodes(uniqueTable.at(root).low, nodes_of_root);
        return;
    }
}

void Manager::findVars(const BDD_ID &root, std::set<BDD_ID> &vars_of_root) {
    if (root <= 1) return;
    else {
        vars_of_root.insert(topVar(root));
        findVars(uniqueTable.at(root).high, vars_of_root);
        findVars(uniqueTable.at(root).low, vars_of_root);
        return;
    }
}

size_t Manager::uniqueTableSize() {
    return nextID;
}

void Manager::visualizeBDD(std::string filepath, BDD_ID &root) {
    char name[] = "BDD";
    Agraph_t *g = agopen(name, Agdirected, 0);
    std::set<BDD_ID> nodeSet;
    findNodes(root, nodeSet);
    std::unordered_map<BDD_ID, Agnode_t*> nodeMap;
    for (auto &i : nodeSet) {
        Agnode_t *n = agnode(g, labelTable.at(i).data(), 1);
        nodeMap.emplace(i, n);
    }
    for (auto &i : nodeSet) {
        if (i <= 1) continue;
        BDD_ID high = uniqueTable.at(i).high;
        BDD_ID low = uniqueTable.at(i).low;
        Agedge_t *h = agedge(g, nodeMap.at(i), nodeMap.at(high), 0, 1);
        Agedge_t *l = agedge(g, nodeMap.at(i), nodeMap.at(low), 0, 1);
        char stylename[] = "style";
        char stylevalue[] = "dotted";
        char styledef[] = "";
        agsafeset(l, stylename, stylevalue, styledef);
    }

    gvLayout(gvc, g, "dot");
    std::string png = filepath + "/BDD.png";
    std::string dot = filepath + "/BDD.dot";
    gvRenderFilename(gvc, g, "png", png.c_str());
    gvRenderFilename(gvc, g, "dot", dot.c_str());
    gvFreeLayout(gvc, g);
    agclose(g);
}

} // namespace ClassProject
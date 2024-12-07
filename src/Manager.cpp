#include "Manager.h"

#include <iostream>
#include <set>
#include <fstream>
#include <unordered_map>
#include <array>
#include <string>

#if CLASSPROJECT_VISUALIZE == 1
#include <graphviz/gvc.h>
#include <graphviz/cgraph.h>
#endif

namespace ClassProject {

#if CLASSPROJECT_VISUALIZE == 1
static GVC_t *gvc = gvContext();
#endif

Manager::Manager()
    : nextID(2)
#if CLASSPROJECT_USECACHE == 1
    , iteCache(std::bind(&Manager::ite_impl, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
    , coTrueCache(std::bind(&Manager::coFactorTrue_impl, this, std::placeholders::_1, std::placeholders::_2))
    , coFalseCache(std::bind(&Manager::coFactorFalse_impl, this, std::placeholders::_1, std::placeholders::_2))
#endif
{
    uniqueTable.emplace(True(), Node{True(), True(), True()});
    reverseTable.emplace(Node{True(), True(), True()}, True());
    uniqueTable.emplace(False(), Node{False(), False(), False()});
    reverseTable.emplace(Node{False(), False(), False()}, False());
#if CLASSPROJECT_VISUALIZE == 1
    labelTable.emplace(True(), "True");
    reverselabelTable.emplace("True", True());
    labelTable.emplace(False(), "False");
    reverselabelTable.emplace("False", False());
#endif
}

BDD_ID Manager::createVar(const std::string &label) {
#if CLASSPROJECT_VISUALIZE == 1
    auto it = reverselabelTable.find(label);
    if (it == reverselabelTable.end()) {
        uniqueTable.emplace(nextID, Node{nextID, True(), False()});
        reverseTable.emplace(Node{nextID, True(), False()}, nextID);
        labelTable.emplace(nextID, label); // label + " ? 1 : 0";
        reverselabelTable.emplace(label, nextID);
        return nextID++;
    } else {
        return it->second;
    }
#else
    // TODO without visualization cache variables
    uniqueTable.emplace(nextID, Node{nextID, True(), False()});
    reverseTable.emplace(Node{nextID, True(), False()}, nextID);
    return nextID++;
#endif
}

const BDD_ID &Manager::True() {
    return TrueId;
}

const BDD_ID &Manager::False() {
    return FalseId;
}

bool Manager::isConstant(BDD_ID f) {
    return f <= True();
}

bool Manager::isVariable(BDD_ID x) {
    return uniqueTable.at(x).topVar == x && !isConstant(x);
}

BDD_ID Manager::topVar(BDD_ID f) {
    return uniqueTable.at(f).topVar;
}

BDD_ID Manager::ite_impl(BDD_ID i, BDD_ID t, BDD_ID e) {
    // Find the top variable with the lowest index
    BDD_ID top = topVar(i);
    if (topVar(t) < top && isVariable(topVar(t))){
        top = topVar(t);
    } 
    if (topVar(e) < top && isVariable(topVar(e))) {
        top = topVar(e);
    }

    // Calculate the high and low successors with a recursive call
    BDD_ID high = ite(coFactorTrue(i, top), coFactorTrue(t, top), coFactorTrue(e, top));
    BDD_ID low = ite(coFactorFalse(i, top), coFactorFalse(t, top), coFactorFalse(e, top));
    
    // If the high and low successors are the same. The initial ite call is reduced to the high successor
    if (high == low) {
        return high;
    }

    // Check if the node already exists
    auto it = reverseTable.find(Node{top, high, low});
    if (it == reverseTable.end()) {
        // add node
        uniqueTable.emplace(nextID, Node{top, high, low});
        reverseTable.emplace(Node{top, high, low}, nextID);
#if CLASSPROJECT_VISUALIZE == 1
        // add label
        auto label = labelTable.at(top) + " ? (" + labelTable.at(high) + ") : (" + labelTable.at(low) + ")";
        labelTable.emplace(nextID, label);
        reverselabelTable.emplace(label, nextID);
#endif
        return nextID++;
    } else {
        // node found
        return it->second;
    }
}

BDD_ID Manager::ite(BDD_ID i, BDD_ID t, BDD_ID e) {
    // Terminal cases
    if (i == True()) {
        return t;
    } else if (i == False()) {
        return e;
    } else if (t == e) {
        return t;
    } else if (t == True() && e == False()) {
        return i;
    } else {
#if CLASSPROJECT_USECACHE == 1
        return iteCache(i, t, e);
#else
        return ite_impl(i, t, e);
#endif
    }
}

BDD_ID Manager::coFactorTrue_impl(BDD_ID f, BDD_ID x) {
    BDD_ID high = coFactorTrue(uniqueTable.at(f).high, x);
    BDD_ID low = coFactorTrue(uniqueTable.at(f).low, x);
    if (high == low) {
        return high;
    }
    auto it = reverseTable.find(Node{topVar(f), high, low});
    if (it == reverseTable.end()) {
        uniqueTable.emplace(nextID, Node{topVar(f), high, low});
        reverseTable.emplace(Node{topVar(f), high, low}, nextID);
#if CLASSPROJECT_VISUALIZE == 1
        auto label = labelTable.at(topVar(f)).substr(0, 1) + " ? (" + labelTable.at(high) + ") : (" + labelTable.at(low) + ")";
        labelTable.emplace(nextID, label);
        reverselabelTable.emplace(label, nextID);
#endif
        return nextID++;
    } else {
        return it->second;
    }
}

BDD_ID Manager::coFactorTrue(BDD_ID f, BDD_ID x) {
    if (topVar(f) > x || isConstant(f)) {
        return f;
    } if (topVar(f) == x) {
        return uniqueTable.at(f).high;
    } else {
#if CLASSPROJECT_USECACHE == 1
        return coTrueCache(f, x);
#else
        return coFactorTrue_impl(f, x);
#endif
    }
}

BDD_ID Manager::coFactorFalse_impl(BDD_ID f, BDD_ID x) {
    BDD_ID high = coFactorFalse(uniqueTable.at(f).high, x);
    BDD_ID low = coFactorFalse(uniqueTable.at(f).low, x);
    if (high == low) {
        return high;
    }
    auto it = reverseTable.find({topVar(f), high, low});
    if (it == reverseTable.end()) {
        uniqueTable.emplace(nextID, Node{topVar(f), high, low});
        reverseTable.emplace(Node{topVar(f), high, low}, nextID);
#if CLASSPROJECT_VISUALIZE == 1
        auto label = labelTable.at(topVar(f)).substr(0, 1) + " ? (" + labelTable.at(high) + ") : (" + labelTable.at(low) + ")";
        labelTable.emplace(nextID, label);
        reverselabelTable.emplace(label, nextID);
#endif
        return nextID++;
    } else {
        return it->second;
    }
}

BDD_ID Manager::coFactorFalse(BDD_ID f, BDD_ID x) {
    if (topVar(f) > x || isConstant(f)) {
        return f;
    }
    if (topVar(f) == x) {
        return uniqueTable.at(f).low;
    } else {
#if CLASSPROJECT_USECACHE == 1
        return coFalseCache(f, x);
#else
        return coFactorFalse_impl(f, x);
#endif
    }
}

BDD_ID Manager::coFactorTrue(BDD_ID f) {
    return coFactorTrue(f, topVar(f));
}

BDD_ID Manager::coFactorFalse(BDD_ID f) {
    return coFactorFalse(f, topVar(f));
}

BDD_ID Manager::and2(BDD_ID a, BDD_ID b) {
    return ite(a, b, False());
}

BDD_ID Manager::or2(BDD_ID a, BDD_ID b) {
    return ite(a, True(), b);
}

BDD_ID Manager::xor2(BDD_ID a, BDD_ID b) {
    return ite(a, neg(b), b);
}

BDD_ID Manager::neg(BDD_ID a) {
    return ite(a, False(), True());
}

BDD_ID Manager::nand2(BDD_ID a, BDD_ID b) {
    return ite(a, neg(b), True());
}

BDD_ID Manager::nor2(BDD_ID a, BDD_ID b) {
    return ite(a, False(), neg(b));
}

BDD_ID Manager::xnor2(BDD_ID a, BDD_ID b) {
    return ite(a, b, neg(b));
}

std::string Manager::getTopVarName(const BDD_ID &root) {
#if CLASSPROJECT_VISUALIZE == 1
    return labelTable.at(topVar(root));
#else
    return std::to_string(topVar(root));
#endif
}

void Manager::findNodes(const BDD_ID &root, std::set<BDD_ID> &nodes_of_root) {
    bool inserted = nodes_of_root.insert(root).second;
    if (!inserted || isConstant(root)) {
        // No insertion or constant value
        return; 
    } else {
        findNodes(uniqueTable.at(root).high, nodes_of_root);
        findNodes(uniqueTable.at(root).low, nodes_of_root);
        return;
    }
}

void Manager::findVars(const BDD_ID &root, std::set<BDD_ID> &vars_of_root) {
    std::set<BDD_ID> nodes;
    findNodes(root, nodes);
    for (auto it : nodes) {
        BDD_ID tV = topVar(it);
        if (isVariable(tV)) {
            vars_of_root.insert(tV);
        }
    }
}

size_t Manager::uniqueTableSize() {
    return nextID;
}

void Manager::visualizeBDD(std::string filepath, BDD_ID &root) {
#if CLASSPROJECT_VISUALIZE == 1
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
        if (i <= 1) {
            // Skip terminal nodes
            continue;
        }
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
#else
    std::cerr << "Visualization is disabled. Please enable it by setting the CLASSPROJECT_VISUALIZE to 1" << std::endl;
#endif
}

Manager::Node::Node(BDD_ID topVar, BDD_ID high, BDD_ID low)
    : topVar(topVar)
    , high(high)
    , low(low)
{}

bool Manager::Node::operator==(const Manager::Node &rhs) const {
    return topVar == rhs.topVar && high == rhs.high && low == rhs.low;
}

} // namespace ClassProject
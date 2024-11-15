#include "Manager.h"

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <array>
#include <string>

namespace ClassProject {

Manager::Manager() {
    // TODO
    uniqueTable.emplace(False(), Node{FalseId, FalseId, FalseId});
    uniqueTable.emplace(True(), Node{TrueId, TrueId, TrueId});
    labelTable[TrueId] = "1";
    reverselabelTable["1"] = TrueId;
    labelTable[FalseId] = "0";
    reverselabelTable["0"] = FalseId;
    reverseTable[Node{TrueId, TrueId, TrueId}] = TrueId;
    reverseTable[Node{FalseId, FalseId, FalseId}] = FalseId;
    nextID = 2;
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
        BDD_ID top = topVar(i);
        if (topVar(t) < top && isVariable(topVar(t))) top = topVar(t);
        if (topVar(e) < top && isVariable(topVar(e))) top = topVar(e);
        BDD_ID high = ite(coFactorTrue(i, top), coFactorTrue(t, top), coFactorTrue(e, top));
        BDD_ID low = ite(coFactorFalse(i, top), coFactorFalse(t, top), coFactorFalse(e, top));
        if (high == low) return high;
        auto it = reverseTable.find({top, high, low});
        if (it == reverseTable.end()) {
            uniqueTable.emplace(nextID, Node{top, high, low});
            reverseTable[Node{top, high, low}] = nextID;
            auto label = labelTable.at(top) + " ? (" + labelTable.at(high) + ") : (" + labelTable.at(low) + ")";
            labelTable[nextID] = label;
            reverselabelTable[label] = nextID;
            return nextID++;
        } else {
            return it->second;
        }
    }
}

BDD_ID Manager::coFactorTrue(BDD_ID f, BDD_ID x) {
    if (topVar(f) > x || topVar(f) <= 1) return f;
    if (topVar(f) == x) return uniqueTable.at(f).high;
    else {
        BDD_ID high = coFactorTrue(uniqueTable.at(f).high, x);
        BDD_ID low = coFactorTrue(uniqueTable.at(f).low, x);
        if (high == low) return high;
        auto it = reverseTable.find(Node{topVar(f), high, low});
        if (it == reverseTable.end()) {
            uniqueTable.emplace(nextID, Node{topVar(f), high, low});
            auto label = labelTable.at(topVar(f)).substr(0, 1) + " ? (" + labelTable.at(high) + ") : (" + labelTable.at(low) + ")";
            labelTable[nextID] = label;
            reverseTable[Node{f, high, low}] = nextID;
            reverseTable[Node{topVar(f), high, low}] = nextID;
            return nextID++;
        } else {
            return it->second;
        }
    }
}

BDD_ID Manager::coFactorFalse(BDD_ID f, BDD_ID x) {
    if (topVar(f) > x || topVar(f) <= 1) return f;
    if (topVar(f) == x) return uniqueTable.at(f).low;
    else {
        BDD_ID high = coFactorFalse(uniqueTable.at(f).high, x);
        BDD_ID low = coFactorFalse(uniqueTable.at(f).low, x);
        if (high == low) return high;
        auto it = reverseTable.find({topVar(f), high, low});
        if (it == reverseTable.end()) {
            uniqueTable.emplace(nextID, Node{topVar(f), high, low});
            auto label = labelTable.at(topVar(f)).substr(0, 1) + " ? (" + labelTable.at(high) + ") : (" + labelTable.at(low) + ")";
            labelTable[nextID] = label;
            reverselabelTable[label] = nextID;
            reverseTable[Node{f, high, low}] = nextID;
            return nextID++;
        } else {
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
    vars_of_root.insert(topVar(root));
    if (root <= 1) return;
    else {
        findVars(uniqueTable.at(root).high, vars_of_root);
        findVars(uniqueTable.at(root).low, vars_of_root);
        return;
    }
}

size_t Manager::uniqueTableSize() {
    return nextID;
}

void Manager::visualizeBDD(std::string filepath, BDD_ID &root) {
    // TODO
}

} // namespace ClassProject
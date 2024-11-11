#include "Manager.h"

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <array>
#include <string>

namespace ClassProject {

Manager::Manager() {
    // TODO
    uniqueTable.emplace(False(), Node{FalseId, FalseId, FalseId, "0"});
    uniqueTable.emplace(True(), Node{TrueId, TrueId, TrueId, "1"});
    std::string tag1 = std::to_string(TrueId) + std::to_string(TrueId) + std::to_string(TrueId);
    reverseTable[tag1] = TrueId;
    std::string tag0 = std::to_string(FalseId) + std::to_string(FalseId) + std::to_string(FalseId);
    reverseTable[tag0] = FalseId;
    nextID = 2;
}

BDD_ID Manager::createVar(const std::string &label) {
    auto it = reverseTable.find(label);
    if (it == reverseTable.end()) {
        uniqueTable.emplace(nextID, Node{nextID, TrueId, FalseId, label + " ? 1 : 0"});
        std::string tag = std::to_string(nextID) + std::to_string(TrueId) + std::to_string(FalseId);
        reverseTable[tag] = nextID;
        reverseTable[label] = nextID;
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
        std::string check = std::to_string(top) + std::to_string(high) + std::to_string(low);
        auto it = reverseTable.find(check);
        if (it == reverseTable.end()) {
            uniqueTable.emplace(nextID, Node{top, high, low, uniqueTable.at(top).label.substr(0, 1) + " ? (" + uniqueTable.at(high).label + ") : (" + uniqueTable.at(low).label + ")"});
            reverseTable[check] = nextID;
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
        std::string check = std::to_string(topVar(f)) + std::to_string(high) + std::to_string(low);
        auto it = reverseTable.find(check);
        if (it == reverseTable.end()) {
            uniqueTable.emplace(nextID, Node{topVar(f), high, low, uniqueTable.at(topVar(f)).label.substr(0, 1) + " ? (" + uniqueTable.at(high).label + ") : (" + uniqueTable.at(low).label + ")"});
            reverseTable[check] = nextID;
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
        std::string check = std::to_string(topVar(f)) + std::to_string(high) + std::to_string(low);
        auto it = reverseTable.find(check);
        if (it == reverseTable.end()) {
            uniqueTable.emplace(nextID, Node{topVar(f), high, low, uniqueTable.at(topVar(f)).label.substr(0, 1) + " ? (" + uniqueTable.at(high).label + ") : (" + uniqueTable.at(low).label + ")"});
            reverseTable[check] = nextID;
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
    return uniqueTable.at(topVar(root)).label.substr(0, 1);
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
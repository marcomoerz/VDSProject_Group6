// A minimalistic BDD library, following Wolfgang Kunz lecture slides
//
// Created by Markus Wedler 2014

#ifndef VDSPROJECT_MANAGER_H
#define VDSPROJECT_MANAGER_H

#include "ManagerInterface.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <array>

namespace ClassProject {

static const BDD_ID FalseId = 0;
static const BDD_ID TrueId = 1;
class Manager : public ManagerInterface {
public:
// Constructor
    Manager(); // implemented in Manager.cpp
    Manager(const Manager &mgr) = default; // copy constructor
    Manager(Manager &&mgr) = default; // move constructor
    Manager &operator=(const Manager &mgr) = default; // copy assignment
    Manager &operator=(Manager &&mgr) = default; // move assignment
// Destructor
    ~Manager() /* override */ = default; // destructor // TODO should be virtual in ManagerInterface.h
// ManagerInterface
    BDD_ID createVar(const std::string &label) override;
    const BDD_ID &True() override;
    const BDD_ID &False() override;
    bool isConstant(BDD_ID f) override;
    bool isVariable(BDD_ID x) override;
    BDD_ID topVar(BDD_ID f) override;
    BDD_ID ite(BDD_ID i, BDD_ID t, BDD_ID e) override;
    BDD_ID coFactorTrue(BDD_ID f, BDD_ID x) override;
    BDD_ID coFactorFalse(BDD_ID f, BDD_ID x) override;
    BDD_ID coFactorTrue(BDD_ID f) override;
    BDD_ID coFactorFalse(BDD_ID f) override;
    BDD_ID and2(BDD_ID a, BDD_ID b) override;
    BDD_ID or2(BDD_ID a, BDD_ID b) override;
    BDD_ID xor2(BDD_ID a, BDD_ID b) override;
    BDD_ID neg(BDD_ID a) override;
    BDD_ID nand2(BDD_ID a, BDD_ID b) override;
    BDD_ID nor2(BDD_ID a, BDD_ID b) override;
    BDD_ID xnor2(BDD_ID a, BDD_ID b) override;
    std::string getTopVarName(const BDD_ID &root) override;
    void findNodes(const BDD_ID &root, std::set<BDD_ID> &nodes_of_root) override;
    void findVars(const BDD_ID &root, std::set<BDD_ID> &vars_of_root) override;
    size_t uniqueTableSize() override;
    void visualizeBDD(std::string filepath, BDD_ID &root) override;
protected:
// TODO Protected methods and variables
    struct Node {
        BDD_ID topVar;
        BDD_ID high;
        BDD_ID low;
        std::string label;
        Node(BDD_ID topVar, BDD_ID high, BDD_ID low, std::string label) : topVar(topVar), high(high), low(low), label(label) {}
        Node(BDD_ID topVar, BDD_ID high, BDD_ID low) : topVar(topVar), high(high), low(low), label("") {}
        bool operator==(const Node &rhs) const {
            return topVar == rhs.topVar && high == rhs.high && low == rhs.low;
        }
    };
    std::unordered_map<BDD_ID, Node> uniqueTable;
    std::unordered_map<std::string, BDD_ID> reverseTable;
    BDD_ID nextID = 0;
public:
    Node getNode(BDD_ID f) {return uniqueTable.at(f);}
    void printTable() {
        std::cout << "ID || High | Low | Top-Var | Label" << std::endl;
        std::cout << "----------------------------------" << std::endl;
        for (auto &it : uniqueTable) {
            std::cout << " " << it.first << " ||   " << it.second.high << "  |  " << it.second.low << "  |    " << it.second.topVar << "    | " << it.second.label << std::endl;
        }
    }
};

} // namespace ClassProject

#endif

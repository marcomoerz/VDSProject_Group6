// A minimalistic BDD library, following Wolfgang Kunz lecture slides
//
// Created by Markus Wedler 2014

#ifndef VDSPROJECT_MANAGER_H
#define VDSPROJECT_MANAGER_H

#include "ManagerInterface.h"

#include <vector>
#include <string>

namespace ClassProject {


class Manager : public ManagerInterface {
private:
// Constants
    BDD_ID FalseID = 0;
    BDD_ID TrueID = 1;
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
private:
// TODO Private methods and variables

};

} // namespace ClassProject

#endif

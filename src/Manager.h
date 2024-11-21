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
    Manager(); // default constructor
    Manager(const Manager &mgr) = default; // copy constructor
    Manager(Manager &&mgr) = default; // move constructor
    Manager &operator=(const Manager &mgr) = default; // copy assignment
    Manager &operator=(Manager &&mgr) = default; // move assignment
// Destructor
    ~Manager() override = default; // destructor // TODO should be virtual in ManagerInterface.h
// ManagerInterface
    /**
     * @brief Create a new variable with the given label. The label is used to identify the variable in the BDD.
     * If a variable with the same label already exists, the existing variable is returned.
     * 
     * @param label The label of the variable
     * @return The BDD_ID of the variable
     */
    BDD_ID createVar(const std::string &label) override;
    
    /**
     * @brief Returns the BDD_ID of the constant True
     * @return The BDD_ID of the constant True
     */ 
    const BDD_ID &True() override;

    /**
     * @brief Returns the BDD_ID of the constant False
     * @return The BDD_ID of the constant False
     */
    const BDD_ID &False() override;

    /**
     * @brief Returns true if the BDD node is a constant (True or False)
     * @param f The BDD node
     * @return True if the BDD node is a constant
     */
    bool isConstant(BDD_ID f) override;

    /**
     * @brief Returns true if the BDD node is a variable (not including constants)
     * @param x The BDD node
     * @return True if the BDD node is a variable
     */
    bool isVariable(BDD_ID x) override;

    /**
     * @brief Returns the top variable of the BDD node
     * @param f The BDD node
     * @return The top variable of the BDD node
     */
    BDD_ID topVar(BDD_ID f) override;

    /**
     * @brief Returns the BDD node resulting from the if-then-else operation.
     * This function uses recursion to build the BDD. The BDD is reduced and repetitions are avoided.
     * @param i The if BDD node
     * @param t The then BDD node
     * @param e The else BDD node
     * @return The BDD node resulting from the if-then-else operation
     */
    BDD_ID ite(BDD_ID i, BDD_ID t, BDD_ID e) override;

    /**
     * @brief Returns the BDD node resulting from the co-factor operation with respect to the variable x.
     * @param f The BDD node
     * @param x The variable
     * @return The BDD node resulting from the co-factor operation with respect to the variable x
     */
    BDD_ID coFactorTrue(BDD_ID f, BDD_ID x) override;

    /**
     * @brief Returns the BDD node resulting from the co-factor operation with respect to the variable x.
     * @param f The BDD node
     * @param x The variable
     * @return The BDD node resulting from the co-factor operation with respect to the variable x
     */
    BDD_ID coFactorFalse(BDD_ID f, BDD_ID x) override;

    /**
     * @brief Returns the BDD node resulting from the co-factor operation with respect to the top variable of the BDD node.
     * @param f The BDD node
     * @return The BDD node resulting from the co-factor operation with respect to the top variable of the BDD node
     */
    BDD_ID coFactorTrue(BDD_ID f) override;

    /**
     * @brief Returns the BDD node resulting from the co-factor operation with respect to the top variable of the BDD node.
     * @param f The BDD node
     * @return The BDD node resulting from the co-factor operation with respect to the top variable of the BDD node
     */
    BDD_ID coFactorFalse(BDD_ID f) override;

// Logical operations implemented with ite
    BDD_ID and2(BDD_ID a, BDD_ID b) override;
    BDD_ID or2(BDD_ID a, BDD_ID b) override;
    BDD_ID xor2(BDD_ID a, BDD_ID b) override;
    BDD_ID neg(BDD_ID a) override;
    BDD_ID nand2(BDD_ID a, BDD_ID b) override;
    BDD_ID nor2(BDD_ID a, BDD_ID b) override;
    BDD_ID xnor2(BDD_ID a, BDD_ID b) override;

    /**
     * @brief Returns the label of the top variable of the BDD node
     */
    std::string getTopVarName(const BDD_ID &root) override;

    /**
     * @brief Returns a set with all the BDD nodes reachable from the root node (including the root)
     */
    void findNodes(const BDD_ID &root, std::set<BDD_ID> &nodes_of_root) override;

    /**
     * @brief Returns a set with all the variables used in the BDD nodes reachable from the root node (including the root)
     */
    void findVars(const BDD_ID &root, std::set<BDD_ID> &vars_of_root) override;

    /**
     * @brief Returns the number of nodes in the unique table
     */
    size_t uniqueTableSize() override;

    /**
     * @brief Visualizes the BDD rooted at the given root node
     */
    void visualizeBDD(std::string filepath, BDD_ID &root) override;
protected:
// Protected methods and variables

    /**
     * @brief Node struct
     * Container for the BDD node information
     */
    struct Node {
        BDD_ID topVar;
        BDD_ID high;
        BDD_ID low;
        Node(BDD_ID topVar, BDD_ID high, BDD_ID low) : topVar(topVar), high(high), low(low) {}
        bool operator==(const Node &rhs) const {
            return topVar == rhs.topVar && high == rhs.high && low == rhs.low;
        }
    };
    
    /**
     * @brief NodeHash struct
     * Hash function for the Node struct used for a reverse lookup table
     */
    struct NodeHash {
        std::size_t operator()(const Node &node) const {
            return std::hash<BDD_ID>()(node.topVar) ^ std::hash<BDD_ID>()(node.high) ^ std::hash<BDD_ID>()(node.low);
        }
    };

    // BDD_ID -> Node
    std::unordered_map<BDD_ID, Node> uniqueTable;
    // Node -> BDD_ID
    std::unordered_map<Node, BDD_ID, NodeHash> reverseTable;

    // BDD_ID -> Label
    std::unordered_map<BDD_ID, std::string> labelTable;
    // Label -> BDD_ID
    std::unordered_map<std::string, BDD_ID> reverselabelTable;

    // reverse lookup tables to improve runtime performance
    std::unordered_map<Node, BDD_ID, NodeHash> iteTable;
    std::unordered_map<Node, BDD_ID, NodeHash> coTrueTable;
    std::unordered_map<Node, BDD_ID, NodeHash> coFalseTable;

    // next available BDD_ID
    BDD_ID nextID = 0;
public:
    /**
     * @brief Prints the uniqueTable to the console
     */
    void printTable() {
        std::cout << "ID || High | Low | Top-Var | Label" << std::endl;
        std::cout << "----------------------------------" << std::endl;
        for (auto &it : uniqueTable) {
            std::cout << " " << it.first << " ||   " << it.second.high << "  |  " << it.second.low << "  |    " << it.second.topVar << "    | " << labelTable.at(it.first) << std::endl;
        }
    }
};

} // namespace ClassProject

#endif

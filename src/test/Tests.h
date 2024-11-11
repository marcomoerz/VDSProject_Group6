//
// Created by tobias on 21.11.16.
//

#ifndef VDSPROJECT_TESTS_H
#define VDSPROJECT_TESTS_H

#include <gtest/gtest.h>
#include <set>
#include <string>
#include <unordered_map>
#include <memory>

#include "../Manager.h"

#define EXPECT_NODE_EQ(node1, topvar, hsuc, lsuc) EXPECT_EQ((node1), ::ClassProject::Test::ManagerImpl::Node((topvar), (hsuc), (lsuc)))
#define EXPECT_NODE_NE(node1, topvar, hsuc, lsuc) EXPECT_FALSE(node1 == ::ClassProject::Test::ManagerImpl::Node((topvar), (hsuc), (lsuc)))


namespace ClassProject::Test {

using namespace ClassProject;

/**
 * @brief ManagerImpl class
 * 
 * @details This class implements public getters for the protected members of the Manager class
 */
class ManagerImpl : public Manager {
public:
    using Node = Manager::Node;
    ManagerImpl() : Manager() {}

    /**
     * @brief Returns the uniqueTable member of the Manager class
     * @return std::unordered_map<BDD_ID, Node> uniqueTable
     */
    auto getMap() {
        return uniqueTable;
    }
};

/**
 * @brief Test fixture for Manager class
 * 
 * @details This class provides a ManagerImpl object for testing
 */
class ManagerTest : public ::testing::Test {
public:
    void TearDown() override {
        mgr.reset();
    }

    void SetUp() override {
        mgr = std::make_unique<ManagerImpl>();
    }

    std::unique_ptr<ManagerImpl> mgr;
};

TEST_F(ManagerTest, Node) {
    ManagerImpl::Node node{1, 2, 3};

    // Test Node constructor
    EXPECT_EQ(node.topVar, 1);
    EXPECT_EQ(node.high,   2);
    EXPECT_EQ(node.low,    3);

    // Test Node.operator==
    EXPECT_NODE_EQ(node, 1, 2, 3);
    EXPECT_NODE_NE(node, 1, 2, 0);
    EXPECT_NODE_NE(node, 1, 0, 3);
    EXPECT_NODE_NE(node, 0, 2, 3);
}

TEST_F(ManagerTest, ManagerTerminations) {
    EXPECT_EQ(mgr->True(), 1);
    EXPECT_EQ(mgr->False(), 0);
}

TEST_F(ManagerTest, ManagerConstructor) {
    // Get uniqueTable from ManagerImpl
    auto map = mgr->getMap();

    // Test Default Contructor added termination nodes
    EXPECT_EQ(map.size(), 2);

    // Test termination nodes in uniqueTable
    EXPECT_NE(map.find(mgr->False()), map.end());
    EXPECT_NODE_EQ(map.find(mgr->False())->second, 0, 0, 0);
    EXPECT_NE(map.find(mgr->True()), map.end());
    EXPECT_NODE_EQ(map.find(mgr->True())->second, 1, 1, 1);
}

TEST_F(ManagerTest, createVar) {
    // Table in test
    // Label                | ID | TOP_VAR | H_SUC | L_SUC |
    //______________________|____|_________|_______|_______|
    // False                | 0  | 0       | 0     | 0     |
    // True                 | 1  | 1       | 1     | 1     |
    // a, a2                | 2  | 2       | 1     | 0     |
    // b                    | 3  | 3       | 1     | 0     |

    BDD_ID a = mgr->createVar("a");
    BDD_ID a2 = mgr->createVar("a");
    BDD_ID b = mgr->createVar("b");
    auto map = mgr->getMap();
    
    // Test ids
    EXPECT_EQ(mgr->False(), 0);
    EXPECT_EQ(mgr->True(), 1);
    EXPECT_EQ(a, 2);
    EXPECT_EQ(a, a2);

    // Test map
    EXPECT_EQ(map.size(), 4);
    EXPECT_NODE_EQ(map.at(0), 0, 0, 0);
    EXPECT_NODE_EQ(map.at(1), 1, 1, 1);
    EXPECT_NODE_EQ(map.at(2), 2, 1, 0);
    EXPECT_NODE_EQ(map.at(3), 3, 1, 0);
}

TEST_F(ManagerTest, isConstant) {
    // Table in test f1 = a*b + !a*0 
    // Label                | ID | TOP_VAR | H_SUC | L_SUC |
    //______________________|____|_________|_______|_______|
    // False                | 0  | 0       | 0     | 0     |
    // True                 | 1  | 1       | 1     | 1     |
    // a                    | 2  | 2       | 1     | 0     |
    // b                    | 3  | 3       | 1     | 0     |
    // f1 -> (ite(a,b,0))   | 3  | 2       | 3     | 0     |

    BDD_ID a = mgr->createVar("a");
    BDD_ID b = mgr->createVar("b");
    BDD_ID f1 = mgr->ite(a, b, mgr->False());
    auto map = mgr->getMap();

    // Test isConstant
    EXPECT_EQ(map.size(), 5);
    EXPECT_TRUE(mgr->isConstant(mgr->False()));
    EXPECT_TRUE(mgr->isConstant(mgr->True()));
    EXPECT_FALSE(mgr->isConstant(a));
    EXPECT_FALSE(mgr->isConstant(b));
    EXPECT_FALSE(mgr->isConstant(f1));
}

TEST_F(ManagerTest, isVariable) {
    // Table in test f1 = a*b + !a*0 
    // Label                | ID | TOP_VAR | H_SUC | L_SUC |
    //______________________|____|_________|_______|_______|
    // False                | 0  | 0       | 0     | 0     |
    // True                 | 1  | 1       | 1     | 1     |
    // a                    | 2  | 2       | 1     | 0     |
    // b                    | 3  | 3       | 1     | 0     |
    // f1 -> (ite(a,b,0))   | 3  | 2       | 3     | 0     |

    BDD_ID a = mgr->createVar("a");
    BDD_ID b = mgr->createVar("b");
    BDD_ID f1 = mgr->ite(a, b, mgr->False());
    auto map = mgr->getMap();

    // Test isVariable
    EXPECT_EQ(map.size(), 5);
    EXPECT_FALSE(mgr->isVariable(mgr->False()));
    EXPECT_FALSE(mgr->isVariable(mgr->True()));
    EXPECT_TRUE(mgr->isVariable(a));
    EXPECT_TRUE(mgr->isVariable(b));
    EXPECT_FALSE(mgr->isVariable(f1));
}

TEST_F(ManagerTest, topVar) {
    // Table in test f1 = a*b + !a*0 
    // Label                | ID | TOP_VAR | H_SUC | L_SUC |
    //______________________|____|_________|_______|_______|
    // False                | 0  | 0       | 0     | 0     |
    // True                 | 1  | 1       | 1     | 1     |
    // a                    | 2  | 2       | 1     | 0     |
    // b                    | 3  | 3       | 1     | 0     |
    // f1 -> (ite(a,b,0))   | 3  | 2       | 3     | 0     |

    BDD_ID a = mgr->createVar("a");
    BDD_ID b = mgr->createVar("b");
    BDD_ID f1 = mgr->ite(a, b, mgr->False());
    auto map = mgr->getMap();

    // Test topVar
    EXPECT_EQ(map.size(), 5);
    EXPECT_EQ(mgr->topVar(mgr->False()), 0);
    EXPECT_EQ(mgr->topVar(mgr->True()), 1);
    EXPECT_EQ(mgr->topVar(a), 2);
    EXPECT_EQ(mgr->topVar(b), 3);
    EXPECT_EQ(mgr->topVar(f1), 2);
}

TEST_F(ManagerTest, coFactorTrue) {
    // Table in test f1 = a*b + !a*0 
    // Label                | ID | TOP_VAR | H_SUC | L_SUC |
    //______________________|____|_________|_______|_______|
    // False                | 0  | 0       | 0     | 0     |
    // True                 | 1  | 1       | 1     | 1     |
    // a                    | 2  | 2       | 1     | 0     |
    // b                    | 3  | 3       | 1     | 0     |
    // f1 -> (ite(a,b,0))   | 3  | 2       | 3     | 0     |

    BDD_ID a = mgr->createVar("a");
    BDD_ID b = mgr->createVar("b");
    BDD_ID f1 = mgr->ite(a, b, mgr->False());
    auto map = mgr->getMap();

    // Test coFactorTrue
    EXPECT_EQ(map.size(), 5);
    EXPECT_EQ(mgr->coFactorTrue(mgr->False()), mgr->False());
    EXPECT_EQ(mgr->coFactorTrue(mgr->True()), mgr->True());
    EXPECT_EQ(mgr->coFactorTrue(a), mgr->True());
    EXPECT_EQ(mgr->coFactorTrue(b), mgr->True());
    EXPECT_EQ(mgr->coFactorTrue(f1), b);
}

TEST_F(ManagerTest, coFactorFalse) {
    // Table in test f1 = a*b + !a*0 
    // Label                | ID | TOP_VAR | H_SUC | L_SUC |
    //______________________|____|_________|_______|_______|
    // False                | 0  | 0       | 0     | 0     |
    // True                 | 1  | 1       | 1     | 1     |
    // a                    | 2  | 2       | 1     | 0     |
    // b                    | 3  | 3       | 1     | 0     |
    // f1 -> (ite(a,b,0))   | 3  | 2       | 3     | 0     |

    BDD_ID a = mgr->createVar("a");
    BDD_ID b = mgr->createVar("b");
    BDD_ID f1 = mgr->ite(a, b, mgr->False());
    auto map = mgr->getMap();

    // Test coFactorFalse
    EXPECT_EQ(map.size(), 5);
    EXPECT_EQ(mgr->coFactorFalse(mgr->False()), mgr->False());
    EXPECT_EQ(mgr->coFactorFalse(mgr->True()), mgr->True());
    EXPECT_EQ(mgr->coFactorFalse(a), mgr->False());
    EXPECT_EQ(mgr->coFactorFalse(b), mgr->False());
    EXPECT_EQ(mgr->coFactorFalse(f1), mgr->False());
}

TEST_F(ManagerTest, coFactorTrueWithRespectToID) {
    // Table in test f1 = a*b + !a*0 
    // Label                | ID | TOP_VAR | H_SUC | L_SUC |
    //______________________|____|_________|_______|_______|
    // False                | 0  | 0       | 0     | 0     |
    // True                 | 1  | 1       | 1     | 1     |
    // a                    | 2  | 2       | 1     | 0     |
    // b                    | 3  | 3       | 1     | 0     |
    // f1 -> (ite(a,b,0))   | 3  | 2       | 3     | 0     |

    BDD_ID a = mgr->createVar("a");
    BDD_ID b = mgr->createVar("b");
    BDD_ID f1 = mgr->ite(a, b, mgr->False());
    auto map = mgr->getMap();

    // Test coFactorTrue with respect to an id
    EXPECT_EQ(map.size(), 5);
    EXPECT_EQ(mgr->coFactorTrue(mgr->True(), a), mgr->True());
    EXPECT_EQ(mgr->coFactorTrue(mgr->False(), a), mgr->False());
    EXPECT_EQ(mgr->coFactorTrue(a, a), mgr->True());
    EXPECT_EQ(mgr->coFactorTrue(b, a), mgr->True());
    EXPECT_EQ(mgr->coFactorTrue(f1, a), b);
    EXPECT_EQ(mgr->coFactorTrue(f1, b), a);
}

TEST_F(ManagerTest, coFactorFalseWithRespectToID) {
    // Table in test f1 = a*b + !a*0 
    // Label                | ID | TOP_VAR | H_SUC | L_SUC |
    //______________________|____|_________|_______|_______|
    // False                | 0  | 0       | 0     | 0     |
    // True                 | 1  | 1       | 1     | 1     |
    // a                    | 2  | 2       | 1     | 0     |
    // b                    | 3  | 3       | 1     | 0     |
    // f1 -> (ite(a,b,0))   | 3  | 2       | 3     | 0     |

    BDD_ID a = mgr->createVar("a");
    BDD_ID b = mgr->createVar("b");
    BDD_ID f1 = mgr->ite(a, b, mgr->False());
    auto map = mgr->getMap();

    // Test coFactorFalse with respect to an id
    EXPECT_EQ(map.size(), 5);
    EXPECT_EQ(mgr->coFactorFalse(mgr->True(), a), mgr->True());
    EXPECT_EQ(mgr->coFactorFalse(mgr->False(), a), mgr->False());
    EXPECT_EQ(mgr->coFactorFalse(a, a), mgr->False());
    EXPECT_EQ(mgr->coFactorFalse(b, a), mgr->False());
    EXPECT_EQ(mgr->coFactorFalse(f1, a), mgr->False());
    EXPECT_EQ(mgr->coFactorFalse(f1, b), mgr->False());
}

TEST_F(ManagerTest, ITE_SIMPLE) {
    // Table in test
    // Label                | ID | TOP_VAR | H_SUC | L_SUC |
    //______________________|____|_________|_______|_______|
    // False                | 0  | 0       | 0     | 0     |
    // True                 | 1  | 1       | 1     | 1     |
    // a                    | 2  | 2       | 1     | 0     |
    // b                    | 3  | 3       | 1     | 0     |
    // f1: ite(a, b, False) | 4  | 2       | 3     | 0     |

    BDD_ID a = mgr->createVar("a");
    BDD_ID b = mgr->createVar("b");
    BDD_ID f1 = mgr->ite(a, b, mgr->False());
    auto map = mgr->getMap();

    // Test created ids
    EXPECT_EQ(map.size(),  5);
    EXPECT_EQ(mgr->False(), 0);
    EXPECT_EQ(mgr->True(),  1);
    EXPECT_EQ(a,           2);
    EXPECT_EQ(b,           3);
    EXPECT_EQ(f1,          4);

    // Test map
    EXPECT_NODE_EQ(map.at(0), 0, 0, 0);
    EXPECT_NODE_EQ(map.at(1), 1, 1, 1);
    EXPECT_NODE_EQ(map.at(2), 2, 1, 0);
    EXPECT_NODE_EQ(map.at(3), 3, 1, 0);
    EXPECT_NODE_EQ(map.at(4), 2, 3, 0);
}

TEST_F(ManagerTest, ITE_REPETITION) {
    // Table in test
    // Label                | ID | TOP_VAR | H_SUC | L_SUC |
    //______________________|____|_________|_______|_______|
    // False                | 0  | 0       | 0     | 0     |
    // True                 | 1  | 1       | 1     | 1     |
    // a                    | 2  | 2       | 1     | 0     |
    // b                    | 3  | 3       | 1     | 0     |
    // f1: ite(a, b, False) | 4  | 2       | 3     | 0     |
    //(f2: ite(a, b, False) | 4  | 2       | 3     | 0     |) -> entry points to f1

    BDD_ID a = mgr->createVar("a");
    BDD_ID b = mgr->createVar("b");
    BDD_ID f1 = mgr->ite(a, b, mgr->False());
    BDD_ID f2 = mgr->ite(a, b, mgr->False());
    auto map = mgr->getMap();

    // Test created ids
    EXPECT_EQ(map.size(),  5);
    EXPECT_EQ(mgr->False(), 0);
    EXPECT_EQ(mgr->True(),  1);
    EXPECT_EQ(a,           2);
    EXPECT_EQ(b,           3);
    EXPECT_EQ(f1,          4);
    EXPECT_EQ(f1,          f2);

    // Test map
    EXPECT_NODE_EQ(map.at(0), 0, 0, 0);
    EXPECT_NODE_EQ(map.at(1), 1, 1, 1);
    EXPECT_NODE_EQ(map.at(2), 2, 1, 0);
    EXPECT_NODE_EQ(map.at(3), 3, 1, 0);
    EXPECT_NODE_EQ(map.at(4), 2, 3, 0);
}

TEST_F(ManagerTest, ITE_REDUCTION) {
    // Table in test
    // Label                | ID | TOP_VAR | H_SUC | L_SUC |
    //______________________|____|_________|_______|_______|
    // False                | 0  | 0       | 0     | 0     |
    // True                 | 1  | 1       | 1     | 1     |
    // a                    | 2  | 2       | 1     | 0     |
    // b                    | 3  | 3       | 1     | 0     |
    // f1: ite(a, b, False) | 4  | 2       | 3     | 0     |
    //(f2: ite(a, b, b)     | 3  | 3       | 1     | 0     |) -> entry points to b

    BDD_ID a = mgr->createVar("a");
    BDD_ID b = mgr->createVar("b");
    BDD_ID f1 = mgr->ite(a, b, mgr->False());
    BDD_ID f2 = mgr->ite(a, b, b);
    auto map = mgr->getMap();

    // Test created ids
    EXPECT_EQ(map.size(), 5);
    EXPECT_EQ(mgr->False(), 0);
    EXPECT_EQ(mgr->True(),  1);
    EXPECT_EQ(a,           2);
    EXPECT_EQ(b,           3);
    EXPECT_EQ(f1,          4);
    EXPECT_EQ(f2,          b);

    // Test map
    EXPECT_NODE_EQ(map.at(0), 0, 0, 0);
    EXPECT_NODE_EQ(map.at(1), 1, 1, 1);
    EXPECT_NODE_EQ(map.at(2), 2, 1, 0);
    EXPECT_NODE_EQ(map.at(3), 3, 1, 0);
    EXPECT_NODE_EQ(map.at(4), 2, 3, 0);
}

TEST_F(ManagerTest, ITE_RECURSION) {
    // Table in test
    // Label                  | ID | TOP_VAR | H_SUC | L_SUC |
    //________________________|____|_________|_______|_______|
    // False                  | 0  | 0       | 0     | 0     |
    // True                   | 1  | 1       | 1     | 1     |
    // a                      | 2  | 2       | 1     | 0     |
    // b                      | 3  | 3       | 1     | 0     |
    // c                      | 4  | 4       | 1     | 0     |
    // d                      | 5  | 5       | 1     | 0     |
    // f1: ite(a, True, b)    | 6  | 2       | 1     | 3     | // a + b
    // f2: ite(c, d, False)   | 7  | 4       | 5     | 0     | // c * d
    // f3: ite(b,f2,False)    | 8  | 3       | 7     | 0     | // b * f2 = b * c * d
    // f4: ite(f1, f2, False) | 9  | 2       | 7     | 8     | // (a + b) * c * d

    BDD_ID a = mgr->createVar("a");
    BDD_ID b = mgr->createVar("b");
    BDD_ID c = mgr->createVar("c");
    BDD_ID d = mgr->createVar("d");
    BDD_ID f1 = mgr->ite(a, mgr->True(), b);
    BDD_ID f2 = mgr->ite(c, d, mgr->False());
    // BDD_ID f3 = mgr->ite(b, f2, mgr->False()); // Created by f4
    BDD_ID f4 = mgr->ite(f1, f2, mgr->False());
    auto map = mgr->getMap();


    // Test created ids
    EXPECT_EQ(map.size(),  10);
    EXPECT_EQ(mgr->False(), 0);
    EXPECT_EQ(mgr->True(),  1);
    EXPECT_EQ(a,           2);
    EXPECT_EQ(b,           3);
    EXPECT_EQ(c,           4);
    EXPECT_EQ(d,           5);
    EXPECT_EQ(f1,          6);
    EXPECT_EQ(f2,          7);
    // EXPECT_EQ(f3,          8);
    EXPECT_EQ(f4,          9);

    // Test map
    EXPECT_NODE_EQ(map.at(0), 0, 0, 0);
    EXPECT_NODE_EQ(map.at(1), 1, 1, 1);
    EXPECT_NODE_EQ(map.at(2), 2, 1, 0);
    EXPECT_NODE_EQ(map.at(3), 3, 1, 0);
    EXPECT_NODE_EQ(map.at(4), 4, 1, 0);
    EXPECT_NODE_EQ(map.at(5), 5, 1, 0);
    EXPECT_NODE_EQ(map.at(6), 2, 1, 3);
    EXPECT_NODE_EQ(map.at(7), 4, 5, 0);
    EXPECT_NODE_EQ(map.at(8), 3, 7, 0);
    EXPECT_NODE_EQ(map.at(9), 2, 7, 8);
}

TEST_F(ManagerTest, OR2) {
    // Table in test
    // Label                | ID | TOP_VAR | H_SUC | L_SUC |
    //______________________|____|_________|_______|_______|
    // False                | 0  | 0       | 0     | 0     |
    // True                 | 1  | 1       | 1     | 1     |
    // a                    | 2  | 2       | 1     | 0     |
    // b                    | 3  | 3       | 1     | 0     |
    // f1: or2(a, b)        | 4  | 2       | 1     | 3     | // ite(a, True, b)

    BDD_ID a = mgr->createVar("a");
    BDD_ID b = mgr->createVar("b");
    BDD_ID f1 = mgr->or2(a, b);
    auto map = mgr->getMap();

    // Test created ids
    EXPECT_EQ(map.size(), 5);
    EXPECT_EQ(mgr->False(), 0);
    EXPECT_EQ(mgr->True(),  1);
    EXPECT_EQ(a,           2);
    EXPECT_EQ(b,           3);
    EXPECT_EQ(f1,          4);

    // Test map
    EXPECT_NODE_EQ(map.at(0), 0, 0, 0);
    EXPECT_NODE_EQ(map.at(1), 1, 1, 1);
    EXPECT_NODE_EQ(map.at(2), 2, 1, 0);
    EXPECT_NODE_EQ(map.at(3), 3, 1, 0);
    EXPECT_NODE_EQ(map.at(4), 2, 1, 3);
}

TEST_F(ManagerTest, AND2) {
    // Table in test
    // Label                | ID | TOP_VAR | H_SUC | L_SUC |
    //______________________|____|_________|_______|_______|
    // False                | 0  | 0       | 0     | 0     |
    // True                 | 1  | 1       | 1     | 1     |
    // a                    | 2  | 2       | 1     | 0     |
    // b                    | 3  | 3       | 1     | 0     |
    // f1: and2(a, b)       | 4  | 2       | 3     | 0     | // ite(a, b, False)

    BDD_ID a = mgr->createVar("a");
    BDD_ID b = mgr->createVar("b");
    BDD_ID f1 = mgr->and2(a, b);
    auto map = mgr->getMap();

    // Test created ids
    EXPECT_EQ(map.size(), 5);
    EXPECT_EQ(mgr->False(), 0);
    EXPECT_EQ(mgr->True(),  1);
    EXPECT_EQ(a,           2);
    EXPECT_EQ(b,           3);
    EXPECT_EQ(f1,          4);

    // Test map
    EXPECT_NODE_EQ(map.at(0), 0, 0, 0);
    EXPECT_NODE_EQ(map.at(1), 1, 1, 1);
    EXPECT_NODE_EQ(map.at(2), 2, 1, 0);
    EXPECT_NODE_EQ(map.at(3), 3, 1, 0);
    EXPECT_NODE_EQ(map.at(4), 2, 3, 0);
}

TEST_F(ManagerTest, NEG) {
    EXPECT_EQ(mgr->neg(mgr->False()), mgr->True());
    EXPECT_EQ(mgr->neg(mgr->True()), mgr->False());
}

TEST_F(ManagerTest, XOR2) {
    EXPECT_EQ(mgr->xor2(mgr->False(), mgr->False()), mgr->False());
    EXPECT_EQ(mgr->xor2(mgr->False(), mgr->True()), mgr->True());
    EXPECT_EQ(mgr->xor2(mgr->True(), mgr->False()), mgr->True());
    EXPECT_EQ(mgr->xor2(mgr->True(), mgr->True()), mgr->False());
}


TEST_F(ManagerTest, NAND2) {
    // Truth table for NAND2
    EXPECT_EQ(mgr->nand2(mgr->False(), mgr->False()), mgr->True());
    EXPECT_EQ(mgr->nand2(mgr->False(), mgr->True()), mgr->True());
    EXPECT_EQ(mgr->nand2(mgr->True(), mgr->False()), mgr->True());
    EXPECT_EQ(mgr->nand2(mgr->True(), mgr->True()), mgr->False());
}

TEST_F(ManagerTest, NOR2) {
    // Truth table for NOR2
    EXPECT_EQ(mgr->nor2(mgr->False(), mgr->False()), mgr->True());
    EXPECT_EQ(mgr->nor2(mgr->False(), mgr->True()), mgr->False());
    EXPECT_EQ(mgr->nor2(mgr->True(), mgr->False()), mgr->False());
    EXPECT_EQ(mgr->nor2(mgr->True(), mgr->True()), mgr->False());
}

TEST_F(ManagerTest, XNOR2) {
    // Truth table for XNOR2
    EXPECT_EQ(mgr->xnor2(mgr->False(), mgr->False()), mgr->True());
    EXPECT_EQ(mgr->xnor2(mgr->False(), mgr->True()), mgr->False());
    EXPECT_EQ(mgr->xnor2(mgr->True(), mgr->False()), mgr->False());
    EXPECT_EQ(mgr->xnor2(mgr->True(), mgr->True()), mgr->True());
}

TEST_F(ManagerTest, getTopVarName) {
    // Table in test
    // Label                | ID | TOP_VAR | H_SUC | L_SUC |
    //______________________|____|_________|_______|_______|
    // False                | 0  | 0       | 0     | 0     |
    // True                 | 1  | 1       | 1     | 1     |
    // a                    | 2  | 2       | 1     | 0     |
    // b                    | 3  | 3       | 1     | 0     |
    // f1: ite(a,b,False)   | 4  | ....    | ....  | ....  |

    BDD_ID a = mgr->createVar("a");
    BDD_ID b = mgr->createVar("b");
    BDD_ID f1 = mgr->ite(a, b, mgr->False());
    auto map = mgr->getMap();
    EXPECT_EQ(map.size(), 5);

    // Test getTopVarName
    EXPECT_EQ(mgr->getTopVarName(mgr->True()), "True");
    EXPECT_EQ(mgr->getTopVarName(mgr->False()), "False");
    EXPECT_EQ(mgr->getTopVarName(a), "a");
    EXPECT_EQ(mgr->getTopVarName(b), "b");
    EXPECT_EQ(mgr->getTopVarName(f1), "a");
}

TEST_F(ManagerTest, findNodes) {
    // Table in test
    // Label                | ID | TOP_VAR | H_SUC | L_SUC |
    //______________________|____|_________|_______|_______|
    // False                | 0  | 0       | 0     | 0     |
    // True                 | 1  | 1       | 1     | 1     |
    // a                    | 2  | 2       | 1     | 0     |
    // b                    | 3  | 3       | 1     | 0     |
    // c                    | 4  | 4       | 1     | 0     |
    // f1: ite(a,b,False)   | 5  | ....    | ....  | ....  |

    BDD_ID a = mgr->createVar("a");
    BDD_ID b = mgr->createVar("b");
    BDD_ID c = mgr->createVar("c");
    BDD_ID f1 = mgr->ite(a, b, mgr->False());
    EXPECT_EQ(mgr->getMap().size(), 6);

    std::set<BDD_ID> nodes_of_root;
    mgr->findNodes(f1, nodes_of_root);

    // Test findNodes
    EXPECT_EQ(nodes_of_root.size(), 5);

    EXPECT_NE(nodes_of_root.find(f1), nodes_of_root.end());
    EXPECT_NE(nodes_of_root.find(a), nodes_of_root.end());
    EXPECT_NE(nodes_of_root.find(b), nodes_of_root.end());
    EXPECT_NE(nodes_of_root.find(mgr->True()), nodes_of_root.end());
    EXPECT_NE(nodes_of_root.find(mgr->False()), nodes_of_root.end());

    EXPECT_EQ(nodes_of_root.find(c), nodes_of_root.end());
}

TEST_F(ManagerTest, findVars) {
    BDD_ID a = mgr->createVar("a");
    BDD_ID b = mgr->createVar("b");
    BDD_ID c = mgr->createVar("c");
    BDD_ID f1 = mgr->ite(a, b, mgr->False());
    std::set<BDD_ID> vars_of_root;
    mgr->findVars(f1, vars_of_root);

    // Test findVars
    EXPECT_EQ(vars_of_root.size(), 2);
    EXPECT_NE(vars_of_root.find(a), vars_of_root.end());
    EXPECT_NE(vars_of_root.find(b), vars_of_root.end());
    EXPECT_EQ(vars_of_root.find(c), vars_of_root.end());
}

TEST_F(ManagerTest, uniqueTableSize) {
    EXPECT_EQ(mgr->uniqueTableSize(), 2);
    EXPECT_EQ(mgr->uniqueTableSize(), mgr->getMap().size());

    BDD_ID a = mgr->createVar("a");
    BDD_ID b = mgr->createVar("b");
    BDD_ID f1 = mgr->ite(a, b, mgr->False());

    // Test uniqueTableSize
    EXPECT_EQ(mgr->uniqueTableSize(), 5);
    EXPECT_EQ(mgr->uniqueTableSize(), mgr->getMap().size());
}

TEST_F(ManagerTest, visualizeBDD) {
    // TODO: Implement test for visualizeBDD
    EXPECT_EQ(true, false); // Fail test if not implemented
}

} // namespace ClassProject::Test

#endif

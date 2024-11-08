//
// Created by tobias on 21.11.16.
//

#ifndef VDSPROJECT_TESTS_H
#define VDSPROJECT_TESTS_H

#include <gtest/gtest.h>
#include "../Manager.h"

using namespace ClassProject;

/**
 * @brief ManagerImpl class
 * 
 * @details This class implements public getters for the protected members of the Manager class
 */
class ManagerImpl : public Manager {
public:
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
    ManagerTest() : mgr() {}

    ManagerImpl mgr;
};

TEST_F(ManagerTest, BasicTest) {
    auto map = mgr.getMap();
    EXPECT_EQ(map.size(), 0);
}

#endif

#ifndef VDSPROJECT_REACHABILITY_TESTS_H
#define VDSPROJECT_REACHABILITY_TESTS_H

#include <iostream>
#include <string>
#include <gtest/gtest.h>
#include "Reachability.h"

using namespace ClassProject;

struct ReachabilityTest : ::testing::Test {

    std::unique_ptr<ClassProject::ReachabilityInterface> fsm2 = std::make_unique<ClassProject::Reachability>(2);

    std::vector<BDD_ID> stateVars2 = fsm2->getStates();
    std::vector<BDD_ID> transitionFunctions;

};

TEST_F(ReachabilityTest, ReachableTestSimple) { /* NOLINT */

    BDD_ID s0 = stateVars2.at(0);
    BDD_ID s1 = stateVars2.at(1);

    transitionFunctions.push_back(fsm2->neg(s0)); // s0' = not(s0)
    transitionFunctions.push_back(fsm2->neg(s1)); // s1' = not(s1)
    fsm2->setTransitionFunctions(transitionFunctions);

    fsm2->setInitState({false,false});

    EXPECT_TRUE(fsm2->isReachable({false, false}));
    EXPECT_FALSE(fsm2->isReachable({false, true}));
    EXPECT_FALSE(fsm2->isReachable({true, false}));
    EXPECT_TRUE(fsm2->isReachable({true, true}));
}

TEST_F(ReachabilityTest, DistanceTestSimple) { /* NOLINT */

    BDD_ID s0 = stateVars2.at(0);
    BDD_ID s1 = stateVars2.at(1);

    transitionFunctions.push_back(fsm2->neg(s0)); // s0' = not(s0)
    transitionFunctions.push_back(fsm2->neg(s1)); // s1' = not(s1)
    fsm2->setTransitionFunctions(transitionFunctions);

    fsm2->setInitState({false,false});

    EXPECT_EQ(fsm2->stateDistance({false, false}), 0);
    EXPECT_EQ(fsm2->stateDistance({false, true}), -1);
    EXPECT_EQ(fsm2->stateDistance({true, false}), -1);
    EXPECT_EQ(fsm2->stateDistance({true, true}), 1);
}

TEST_F(ReachabilityTest, DistanceTestSimple2) { /* NOLINT */

    BDD_ID s0 = stateVars2.at(0);
    BDD_ID s1 = stateVars2.at(1);

    transitionFunctions.push_back(fsm2->neg(s1)); // s0' = not(s1)
    transitionFunctions.push_back(s0); // s1' = s0
    fsm2->setTransitionFunctions(transitionFunctions);

    fsm2->setInitState({false,false});

    EXPECT_EQ(fsm2->stateDistance({false, false}), 0);
    EXPECT_EQ(fsm2->stateDistance({false, true}), 3);
    EXPECT_EQ(fsm2->stateDistance({true, false}), 1);
    EXPECT_EQ(fsm2->stateDistance({true, true}), 2);
}

struct ReachabilityInputTest : ::testing::Test {

    std::unique_ptr<ClassProject::ReachabilityInterface> fsm2 = std::make_unique<ClassProject::Reachability>(1,1);

    std::vector<BDD_ID> stateVars2 = fsm2->getStates();
    std::vector<BDD_ID> inputs2 = fsm2->getInputs();
    std::vector<BDD_ID> transitionFunctions;

};

TEST_F(ReachabilityInputTest, InputTestActive) { /* NOLINT */

    BDD_ID s0 = stateVars2.at(0);
    BDD_ID i0 = inputs2.at(0);

    transitionFunctions.push_back(i0); // s0' = i0
    fsm2->setTransitionFunctions(transitionFunctions);

    fsm2->setInitState({false});

    EXPECT_EQ(fsm2->stateDistance({false}), 0);
    EXPECT_EQ(fsm2->stateDistance({true}), 1);
}
TEST_F(ReachabilityInputTest, InputTestFlipFlop) { /* NOLINT */

    BDD_ID s0 = stateVars2.at(0);
    BDD_ID i0 = inputs2.at(0);

    transitionFunctions.push_back(fsm2->or2(fsm2->and2(fsm2->neg(s0), i0), fsm2->and2(s0, fsm2->neg(i0)))); // s0' = !s0 & i0 | s0 & !i0
    fsm2->setTransitionFunctions(transitionFunctions);

    fsm2->setInitState({false});

    EXPECT_EQ(fsm2->stateDistance({false}), 0);
    EXPECT_EQ(fsm2->stateDistance({true}), 1);
}

#endif

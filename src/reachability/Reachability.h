#ifndef VDSPROJECT_REACHABILITY_H
#define VDSPROJECT_REACHABILITY_H

#include "ReachabilityInterface.h"
#include <unordered_map>

namespace ClassProject {

class Reachability : public ReachabilityInterface {
// Constructors, destructors, and assignment operators
public:
    Reachability(unsigned int stateSize, unsigned int inputSize);
    Reachability(unsigned int stateSize) : Reachability(stateSize, 0) {};
    // Copy constructor
    Reachability(const Reachability &other) = default;
    // Move constructor
    Reachability(Reachability &&other) noexcept = default;
    // Default constructor
    Reachability() = delete;
    // Destructor
    ~Reachability() override = default;
    // Copy assignment operator
    Reachability &operator=(const Reachability &other) = default;
    // Move assignment operator
    Reachability &operator=(Reachability &&other) noexcept = default;

// Public member functions
public:
    const std::vector<BDD_ID> &getStates() const override;
    const std::vector<BDD_ID> &getInputs() const override;
    bool isReachable(const std::vector<bool> &stateVector) override;
    int stateDistance(const std::vector<bool> &stateVector) override;
    void setTransitionFunctions(const std::vector<BDD_ID> &transitionFunctions) override;
    void setInitState(const std::vector<bool> &stateVector) override;

// Protected member functions
protected:

// Private member functions
private:
void compute();
void computeTransitionRelation();

// Public member variables
public:

// Protected member variables
protected:

// Private member variables
private:
std::vector<BDD_ID> statebits;
std::vector<BDD_ID> statebitsF;
std::vector<BDD_ID> inputbits;
std::vector<BDD_ID> initialState;
std::vector<BDD_ID> transitionFunctions;
BDD_ID transitionRelation;

BDD_ID computedReach;
std::vector<BDD_ID> computedReachSteps;
bool computed = false;

// END OF Reachability class

};

} // namespace ClassProject
#endif

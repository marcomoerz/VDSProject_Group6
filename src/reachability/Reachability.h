#ifndef VDSPROJECT_REACHABILITY_H
#define VDSPROJECT_REACHABILITY_H

#include "ReachabilityInterface.h"
#include <unordered_map>

namespace ClassProject {

class Reachability : public ReachabilityInterface {
// Constructors, destructors, and assignment operators
public:
    Reachability(unsigned int stateSize, unsigned int inputSize) : ReachabilityInterface(stateSize, inputSize) {
        if (stateSize == 0) {
            throw std::runtime_error("stateSize must be greater than 0");
        }
        statebits.resize(stateSize);
        statebitsF.resize(stateSize);
        inputbits.resize(inputSize);
        initialState.resize(stateSize);
        transitionFunctions.resize(stateSize);
        for (int i = 0; i < stateSize; i++) {
            BDD_ID id = createVar("s_" + std::to_string(i));
            statebits[i] = id;
            statebitsF[i] = createVar("s_" + std::to_string(i) + "'");
            initialState[i] = False();
            transitionFunctions[i] = id;
        }
        for (int i = 0; i < inputSize; i++) {
            inputbits[i] = createVar("i_" + std::to_string(i));
        }
    };
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

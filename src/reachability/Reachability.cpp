#include "Reachability.h"

namespace ClassProject {

    const std::vector<BDD_ID> &Reachability::getStates() const {
        return statebits;
    }

    const std::vector<BDD_ID> &Reachability::getInputs() const {
        return inputbits;
    }

    bool Reachability::isReachable(const std::vector<bool> &stateVector) {
        return false;
    }

    int Reachability::stateDistance(const std::vector<bool> &stateVector) {
        return -1;
    }

    void Reachability::setTransitionFunctions(const std::vector<BDD_ID> &transitionFunction) {
        ;
    }

    void Reachability::setInitState(const std::vector<bool> &stateVector) {
        ;
    }

} // namespace ClassProject
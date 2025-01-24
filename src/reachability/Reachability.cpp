#include "Reachability.h"
#include <unordered_map>

namespace ClassProject {

    Reachability::Reachability(unsigned int stateSize, unsigned int inputSize)
    : ReachabilityInterface(stateSize, inputSize) {
        if (stateSize == 0) {
            throw std::runtime_error("stateSize must be greater than 0");
        }
        statebits.resize(stateSize);
        statebitsF.resize(stateSize);
        inputbits.resize(inputSize);
        initialState.resize(stateSize);
        transitionFunctions.resize(stateSize);
        for (int i = 0; i < stateSize; i++) {
            BDD_ID id = Manager::createVar("s_" + std::to_string(i));
            statebits[i] = id;
            statebitsF[i] = Manager::createVar("s_" + std::to_string(i) + "'");
            initialState[i] = Manager::False();
            transitionFunctions[i] = id;
        }
        for (int i = 0; i < inputSize; i++) {
            inputbits[i] = Manager::createVar("i_" + std::to_string(i));
        }
    }

    const std::vector<BDD_ID> &Reachability::getStates() const {
        return statebits;
    }

    const std::vector<BDD_ID> &Reachability::getInputs() const {
        return inputbits;
    }

    bool Reachability::isReachable(const std::vector<bool> &stateVector) {
        if (stateVector.size() != statebits.size()) {
            throw std::runtime_error("Size mismatch on isReachable");
        }
        if (!computed) {
            compute();
        }
        BDD_ID temp = computedReach;
        for (int i = statebits.size() - 1; i >= 0; i--) {
            if (stateVector[i]) {
                temp = Manager::coFactorTrue(temp, statebits[i]);
            } else {
                temp = Manager::coFactorFalse(temp, statebits[i]);
            }
        }
        //for (int i = inputbits.size() - 1; i >= 0; i--) {
        //    temp = or2(coFactorTrue(temp, inputbits[i]), coFactorFalse(temp, inputbits[i]));
        //}
        if (temp == Manager::True()) {
            return true;
        } else if (temp == Manager::False()) {
            return false;
        } else {
            throw std::runtime_error("Error in isReachable");
        }
    }

    int Reachability::stateDistance(const std::vector<bool> &stateVector) {
        if (stateVector.size() != statebits.size()) {
            throw std::runtime_error("Size mismatch on stateDistance");
        }
        if (!isReachable(stateVector)) {
            return -1;
        }
        for (int n = 0; n < computedReachSteps.size(); n++) {
            BDD_ID temp = computedReachSteps[n];
            for (int i = statebits.size() - 1; i >= 0; i--) {
                if (stateVector[i]) {
                    temp = Manager::coFactorTrue(temp, statebits[i]);
                } else {
                    temp = Manager::coFactorFalse(temp, statebits[i]);
                }
            }
            //for (int i = inputbits.size() - 1; i >= 0; i--) {
            //    temp = or2(coFactorTrue(temp, inputbits[i]), coFactorFalse(temp, inputbits[i]));
            //}
            if (temp == Manager::True()) {
                return n;
            }
        }
        throw std::runtime_error("Error in stateDistance");
    }

    void Reachability::setTransitionFunctions(const std::vector<BDD_ID> &transitionFunctions) {
        if (transitionFunctions.size() != Reachability::transitionFunctions.size()) {
            throw std::runtime_error("Size mismatch on setTransitionFunctions");
        }
        for (auto it : transitionFunctions) {
            auto it2 = uniqueTable.find(it);
            if (it2 == uniqueTable.end()) {
                throw std::runtime_error("transitionFunctions contains unknown BDD_ID");
            }
        }
        Reachability::transitionFunctions = transitionFunctions;
        transitionRelation = Manager::True();
        for (int i = 0; i < statebits.size(); i++) {
            transitionRelation = Manager::and2(transitionRelation, Manager::or2(Manager::and2(statebitsF[i], transitionFunctions[i]), Manager::and2(Manager::neg(statebitsF[i]), Manager::neg(transitionFunctions[i]))));
        }
        computed = false;
    }

    void Reachability::setInitState(const std::vector<bool> &stateVector) {
        if (stateVector.size() != initialState.size()) {
            throw std::runtime_error("Size mismatch on setInitState");
        }
        for (int i = 0; i < statebits.size(); i++) {
            if (stateVector[i]) {
                initialState[i] = Manager::True();
            } else {
                initialState[i] = Manager::False();
            }
        }
        computed = false;
    }

    void Reachability::compute() {
        computedReachSteps.clear();
        BDD_ID c_S = Manager::True();
        BDD_ID c_SS = Manager::True();
        for (int i = 0; i < statebits.size(); i++) {
            c_S = Manager::and2(c_S, Manager::xnor2(statebits[i], initialState[i]));
            c_SS = Manager::and2(c_SS, Manager::xnor2(statebits[i], statebitsF[i]));
        }
        BDD_ID c_R_it = c_S;
        BDD_ID c_R, temp;
        computedReachSteps.push_back(c_S);
        do {
            c_R = c_R_it;
            temp = Manager::and2(c_R, transitionRelation);
            for (int i = inputbits.size() - 1; i >= 0; i--) {
                temp = Manager::or2(Manager::coFactorTrue(temp, inputbits[i]), Manager::coFactorFalse(temp, inputbits[i]));
            }
            for (int i = statebits.size() - 1; i >= 0; i--) {
                temp = Manager::or2(Manager::coFactorTrue(temp, statebits[i]), Manager::coFactorFalse(temp, statebits[i]));
            }
            temp = Manager::and2(c_SS, temp);
            for (int i = statebits.size() - 1; i >= 0; i--) {
                temp = Manager::or2(Manager::coFactorTrue(temp, statebitsF[i]), Manager::coFactorFalse(temp, statebitsF[i]));
            }
            computedReachSteps.push_back(temp);
            c_R_it = Manager::or2(c_R, temp);
        } while(c_R_it != c_R);
        computedReach = c_R;
        computed = true;
    }

} // namespace ClassProject
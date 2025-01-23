#include "Reachability.h"
#include <format>
#include <unordered_map>

namespace ClassProject {

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
                temp = coFactorTrue(temp, statebits[i]);
            } else {
                temp = coFactorFalse(temp, statebits[i]);
            }
        }
        //for (int i = inputbits.size() - 1; i >= 0; i--) {
        //    temp = or2(coFactorTrue(temp, inputbits[i]), coFactorFalse(temp, inputbits[i]));
        //}
        if (temp == True()) {
            return true;
        } else if (temp == False()) {
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
                    temp = coFactorTrue(temp, statebits[i]);
                } else {
                    temp = coFactorFalse(temp, statebits[i]);
                }
            }
            //for (int i = inputbits.size() - 1; i >= 0; i--) {
            //    temp = or2(coFactorTrue(temp, inputbits[i]), coFactorFalse(temp, inputbits[i]));
            //}
            if (temp == True()) {
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
        transitionRelation = True();
        for (int i = 0; i < statebits.size(); i++) {
            transitionRelation = and2(transitionRelation, or2(and2(statebitsF[i], transitionFunctions[i]), and2(neg(statebitsF[i]), neg(transitionFunctions[i]))));
        }
        computed = false;
    }

    void Reachability::setInitState(const std::vector<bool> &stateVector) {
        if (stateVector.size() != initialState.size()) {
            throw std::runtime_error("Size mismatch on setInitState");
        }
        for (int i = 0; i < statebits.size(); i++) {
            if (stateVector[i]) {
                initialState[i] = True();
            } else {
                initialState[i] = False();
            }
        }
        computed = false;
    }

    void Reachability::compute() {
        computedReachSteps.clear();
        BDD_ID c_S = True();
        BDD_ID c_SS = True();
        for (int i = 0; i < statebits.size(); i++) {
            c_S = and2(c_S, xnor2(statebits[i], initialState[i]));
            c_SS = and2(c_SS, xnor2(statebits[i], statebitsF[i]));
        }
        BDD_ID c_R_it = c_S;
        BDD_ID c_R, temp;
        computedReachSteps.push_back(c_S);
        do {
            c_R = c_R_it;
            temp = and2(c_R, transitionRelation);
            for (int i = inputbits.size() - 1; i >= 0; i--) {
                temp = or2(coFactorTrue(temp, inputbits[i]), coFactorFalse(temp, inputbits[i]));
            }
            for (int i = statebits.size() - 1; i >= 0; i--) {
                temp = or2(coFactorTrue(temp, statebits[i]), coFactorFalse(temp, statebits[i]));
            }
            temp = and2(c_SS, temp);
            for (int i = statebits.size() - 1; i >= 0; i--) {
                temp = or2(coFactorTrue(temp, statebitsF[i]), coFactorFalse(temp, statebitsF[i]));
            }
            computedReachSteps.push_back(temp);
            c_R_it = or2(c_R, temp);
        } while(c_R_it != c_R);
        computedReach = c_R;
        computed = true;
    }

} // namespace ClassProject
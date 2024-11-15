//
// Created by Carolina P. Nogueira 2016
//

#include <iostream>
#include <string>
#include "Manager.h"

using namespace ClassProject;


int main(int argc, char* argv[])
{
    Manager mn = Manager();
    BDD_ID a = mn.createVar("a");
    BDD_ID b = mn.createVar("b");
    BDD_ID c = mn.createVar("c");
    BDD_ID d = mn.createVar("d");
    BDD_ID f1 = mn.or2(a, b);
    BDD_ID f2 = mn.and2(c, d);
    BDD_ID f3 = mn.and2(f1, f2);
    //BDD_ID an = mn.neg(a);
    //BDD_ID a0 = mn.or2(a, an);
    mn.printTable();
    std::set<BDD_ID> nodes;
    mn.findNodes(f3, nodes);
    std::cout << "List of Nodes from Root:";
    for (auto &it : nodes) {
        std::cout << " " << it;
    }
    std::cout << std::endl;
    mn.visualizeBDD("BDD.png", f3);
}

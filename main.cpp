#include <iostream>
#include <vector>
#include <string>

using namespace std;

#include "dataStructures.h"

vector <Predicate> Clause; // Clause
vector <vector <Predicate>> KB; // Knowledge Base (vector of clauses)

int main()
{
    // string s = "P(x,y)";
    // string s = "~P(x,y,z) | ~Q(z) & R(y,z) => S(x,y)";
    // string s = "A(x) & B(x) | C(x) & D(x)";
    string s = "A(x) & B(x) | C(x) | D(x)";



    FOL *fol = stringToFOL(s);
    printFOL(fol); cout<<endl; // debug
    FOLtoCNF(fol);

    return 0;
}
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
    string s = "~P(x,y,z) | ~Q(z) & R(y,z) => Q(x,y)";


    FOL *fol = stringToFOL(s);
    printFOL(fol);

    return 0;
}
#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>

using namespace std;

#include "dataStructures.h"

vector <unordered_set<Predicate, PredicateHash>> KB; // Knowledge Base (vector of clauses)
unordered_map <Predicate, vector<int>, PredicateHash> KBMap; // Knowledge Base (map of clauses)

int main()
{
    // string s = "P(x,y)";
    string s = "~P(x,y,z) | ~Q(z) & R(y,z) => S(x,y)";
    // string s = "A(x) & B(x) | C(x) & D(x)";
    // string s = "A(x) & B(x) | C(x) | D(x)";



    FOL *fol = stringToFOL(s);
    printFOL(fol); cout<<endl<<endl; // debug
    fol = FOLtoCNF(fol);
    buildKB(fol);
    cout<<endl; printKB(); cout<<endl; // debug

    return 0;
}
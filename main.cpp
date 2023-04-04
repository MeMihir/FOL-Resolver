#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <cstdlib>

using namespace std;

#include "dataStructures.h"

vector <unordered_set<Predicate, PredicateHash>> KB; // Knowledge Base (vector of clauses)
unordered_map <Predicate, vector<int>, PredicateHash> KBMap; // Knowledge Base (map of clauses)

vector<string>input;

string readInput()
{
    string s, target;
    freopen("inputs/input4.txt", "r", stdin);
    cin>>target;

    int n;
    cin>>n;

    getline(cin, s);

    for (int i = 0; i < n; i++)
    {
        getline(cin, s);
        input.push_back(s);
    }

    return target;
}

int main()
{
    string target = readInput();

    Predicate query(target);
    query.sign = !query.sign;
    printPredicate(query); cout<<endl<<endl; // debug

    for(int i = 0; i < input.size(); i++)
    {
        FOL *fol = stringToFOL(input[i]);
        fol = FOLtoCNF(fol);
        buildKB(fol);
    }

    cout<<endl; printKB(); cout<<endl; // debug

    return 0;
}
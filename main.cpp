#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <cstdlib>

using namespace std;
#include "KBTell.h"
#include "KBAsk.h"

vector <Clause> KB; // Knowledge Base (vector of clauses)
unordered_map <Predicate, unordered_set<int>, PredicateHash> KBMap; // Knowledge Base (map of clauses)
vector <Clause> visited; // visited clauses vector
unordered_map <Predicate, vector<int>, PredicateHash> visitedMap; // visited clauses HashMap

vector<string>input;

string readInput()
{
    string s, target;
    freopen("inputs/input.txt", "r", stdin);
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

    auto start = chrono::high_resolution_clock::now();
    for(int i = 0; i < input.size(); i++)
    {
        FOL *fol = stringToFOL(input[i]);
        fol = FOLtoCNF(fol);
        buildKB(fol);
    }
    standardizeKB();
    cout<<endl; printKB(); cout<<endl; // debug

    bool ans = queryKB(query);
    cout<<"FINAL ANSWER \t: "<<ans<<endl<<endl<<endl; 
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    int minutes = chrono::duration_cast<chrono::minutes>(duration).count();
    int seconds = chrono::duration_cast<chrono::seconds>(duration).count() - minutes * 60;
    int milliseconds = chrono::duration_cast<chrono::milliseconds>(duration).count() - minutes * 60 * 1000 - seconds * 1000;
    cout << "Time taken: " << minutes << " minutes " << seconds << " seconds " << milliseconds << " milliseconds" << endl;
    cout<<"=============================================================\n";

    return 0;
}
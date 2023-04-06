#include <unordered_set>
#include <queue>

using namespace std;
#include "KBTell.h"
#include "KBAsk.h"

struct ClauseCompare {
  bool operator()(Clause c1, Clause c2) const {
    return c1.size() > c2.size();
  }
};


bool queryKB(Predicate target)
{
    Clause query; // clause
    query.insert(target);

    // priority queue of clauses
    priority_queue<Clause, vector<Clause>, ClauseCompare> resolver;
    resolver.push(query);

    while(!resolver.empty())
    {
        query = resolver.top();
        resolver.pop();
        cout<<"\n\nQuery\t: "; printClause(query); cout<<endl; // debug

        if (query.empty()) return true;

        for(auto it = query.begin(); it != query.end(); it++)
        {
            Predicate p = *it;
            cout<<"Pred\t: "; printPredicate(p); cout<<endl; // debug
            vector <Clause> clauses = getKBClauses(p);
            printClauses(clauses); // debug

        }
    }

    // vector <Clause> clauses = getKBClauses(target);
    // printClauses(clauses); // debug

    return false;
}

vector <Clause> getKBClauses(Predicate target)
{
    vector <Clause> clauses;
    target.sign = !target.sign;
    

    vector <int> clauseIndices = KBMap[target];
    cout<<"Clauses\t: "; // debug
    for(int i=0; i<clauseIndices.size(); i++) {
        clauses.push_back(KB[clauseIndices[i]]);
        cout<<clauseIndices[i]<<" "; // debug
    }
    cout<<endl; // debug
    return clauses;
}

void printClauses(vector<Clause> clauses) 
{
    // print KB
    cout << "Clauses" << endl;
    for (int i = 0; i < clauses.size(); i++)
    {
        cout << "clause " << i << " : ";
        for (auto it = clauses[i].begin(); it != clauses[i].end(); it++)
        {   
            if(it != clauses[i].begin())
                cout << " | ";
            if (it->sign == false)
                cout << "~";
            cout << it->name << "(";
            for (int j = 0; j < it->arity; j++)
            {
                cout << it->arguments[j];
                if (j != it->arity - 1)
                    cout << ",";
            }
            cout << ") ";
        }
        cout << endl;
    }
}

void printClause(Clause clause) 
{
    for (auto it = clause.begin(); it != clause.end(); it++)
    {   
        if(it != clause.begin())
            cout << " | ";
        if (it->sign == false)
            cout << "~";
        cout << it->name << "(";
        for (int j = 0; j < it->arity; j++)
        {
            cout << it->arguments[j];
            if (j != it->arity - 1)
                cout << ",";
        }
        cout << ") ";
    }
    cout << endl;
}
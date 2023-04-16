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
        if(isClauseVisited(query))
            continue;
        addClauseToVisited(query);
        cout<<"\n\nQuery\t: "; query.print(); // debug

        if (query.empty()) return true;

        for(auto it = query.clause.begin(); it != query.clause.end(); it++)
        {
            Predicate p = *it;
            cout<<"Pred\t: "; printPredicate(p); cout<<endl; // debug
            vector <Clause> clauses = getKBClauses(p);
            for (int i=0; i<clauses.size(); i++)
            {
                Clause unified = unifyClauses(query, clauses[i], p);
                cout<<"Clause\t: "; clauses[i].print(); // debug
                cout<<"Unified\t: "; unified.print(); // debug
                if(unified.size() != 0) {
                    resolver.push(unified);
                }
                else 
                    return true;
                cout<<"Visited\t: "<< visited.size() <<endl<<endl; // debug
            }
            // printClauses(clauses); // debug

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
        ClauseVect::iterator it = KB[clauseIndices[i]].find(target);
        if(it == KB[clauseIndices[i]].clause.end()) {
            cout<<clauseIndices[i]<<":CU "; // debug
            continue;
        }
        Predicate p = *it;
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
        cout << "clause " << i << " : "; clauses[i].print();
    }
}


Clause unifyClauses(Clause query, Clause clause, Predicate target)
{
    Clause result;
    unordered_map<string, string> substitution;

    // unify query and clause
    for(auto it = query.clause.begin(); it != query.clause.end(); it++)
    {
        Predicate queryPredicate = *it;
        for(auto it2 = clause.clause.begin(); it2 != clause.clause.end(); it2++)
        {
            Predicate clausePredicate = *it2;
            if(queryPredicate.name == clausePredicate.name && queryPredicate.arity == clausePredicate.arity)
            {
                for(int i=0; i<queryPredicate.arity; i++)
                {
                    if(queryPredicate.arguments[i] != clausePredicate.arguments[i])
                    {
                        if(substitution.find(queryPredicate.arguments[i]) == substitution.end())
                        {
                            if(isVariable(queryPredicate.arguments[i]) && isVariable(clausePredicate.arguments[i]))
                                substitution[clausePredicate.arguments[i]] = queryPredicate.arguments[i];
                            else if(isVariable(queryPredicate.arguments[i]))
                                substitution[queryPredicate.arguments[i]] = clausePredicate.arguments[i];
                            else if(isVariable(clausePredicate.arguments[i]))
                                substitution[clausePredicate.arguments[i]] = queryPredicate.arguments[i];
                        }
                        // !CHECK THIS
                        // else
                        // {
                        //     if substitution[clausePredicate.arguments[i]] = substitution[queryPredicate.arguments[i]];
                        // }
                    }
                }
            }
        }
    }

    cout<<"Sub\t: "; // debug
    for(auto it = substitution.begin(); it != substitution.end(); it++)
    {
        cout<<it->first<<"->"<<it->second<<" "; // debug
    }
    cout<<endl; // debug
    // substitute
    for(auto it = query.clause.begin(); it != query.clause.end(); it++)
    {
        Predicate queryPredicate = *it;
        for(int i=0; i<queryPredicate.arity; i++)
        {
            if(substitution.find(queryPredicate.arguments[i]) != substitution.end())
            {
                queryPredicate.arguments[i] = substitution[queryPredicate.arguments[i]];
            }
        }
        // !CHANGE RESULT.INSET TO RESULT.INSERT
        result.insert(queryPredicate);
    }

    for(auto it = clause.clause.begin(); it != clause.clause.end(); it++)
    {
        Predicate clausePredicate = *it;
        for(int i=0; i<clausePredicate.arity; i++)
        {
            if(substitution.find(clausePredicate.arguments[i]) != substitution.end())
            {
                clausePredicate.arguments[i] = substitution[clausePredicate.arguments[i]];
            }
        }
        result.insert(clausePredicate);
    }

    return result;
}

void addClauseToVisited(Clause clause)
{
    visited.push_back(clause);
    for(auto it = clause.clause.begin(); it != clause.clause.end(); it++)
    {
        Predicate p = *it;
        visitedMap[p].push_back(visited.size()-1);
    }
}

bool isClauseVisited(Clause clause)
{
    Predicate p = *clause.clause.begin();

    vector <int> clauseIndices = visitedMap[p];
    for(int i=0; i<clauseIndices.size(); i++)
    {
        if(clause.compare(visited[clauseIndices[i]]))
            return true;
    }
    return false;
}

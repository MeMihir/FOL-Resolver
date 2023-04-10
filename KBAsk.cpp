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
        cout<<"\n\nQuery\t: "; printClause(query); cout<<endl; // debug

        if (query.empty()) return true;

        for(auto it = query.begin(); it != query.end(); it++)
        {
            Predicate p = *it;
            cout<<"Pred\t: "; printPredicate(p); cout<<endl; // debug
            vector <Clause> clauses = getKBClauses(p);
            for (int i=0; i<clauses.size(); i++)
            {
                Clause unified = unifyClauses(query, clauses[i], p);
                cout<<"Clause\t: "; printClause(clauses[i]); // debug
                cout<<"Unified\t: "; printClause(unified); // debug
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
        Clause::iterator it = KB[clauseIndices[i]].find(target);
        Predicate p = *it;
        bool flag = true;
        for(int j=0; j<p.arity; j++)
        {
            if(
                !isVariable(p.arguments[j]) && 
                !isVariable(target.arguments[j]) && 
                (p.arguments[j] != target.arguments[j])
            ) {
                cout<<"CU ";
                flag = false;
                break;
            }
        }

        if(!flag) continue;
            
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

Clause unifyClauses(Clause query, Clause clause, Predicate target)
{
    Clause result;
    unordered_map<string, string> substitution;

    // unify query and clause
    for(auto it = query.begin(); it != query.end(); it++)
    {
        Predicate queryPredicate = *it;
        for(auto it2 = clause.begin(); it2 != clause.end(); it2++)
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
                            if(!isVariable(clausePredicate.arguments[i]))
                                substitution[queryPredicate.arguments[i]] = clausePredicate.arguments[i];
                            else
                                substitution[clausePredicate.arguments[i]] = queryPredicate.arguments[i];
                        }
                        // !CHECK THIS
                        // else
                        // {
                        //     substitution[clausePredicate.arguments[i]] = substitution[queryPredicate.arguments[i]];
                        // }
                    }
                }
            }
        }
    }


    // substitute
    for(auto it = query.begin(); it != query.end(); it++)
    {
        Predicate queryPredicate = *it;
        if(queryPredicate.name != target.name)
        {
            for(int i=0; i<queryPredicate.arity; i++)
            {
                if(substitution.find(queryPredicate.arguments[i]) != substitution.end())
                {
                    queryPredicate.arguments[i] = substitution[queryPredicate.arguments[i]];
                }
            }
            // !CHANGE RESULT.INSET TO RESULT.INSERT
            // result.insert(queryPredicate);
            result = insertPredicate(queryPredicate, result);
        }
    }

    for(auto it = clause.begin(); it != clause.end(); it++)
    {
        Predicate clausePredicate = *it;
        if(clausePredicate.name != target.name)
        {
            for(int i=0; i<clausePredicate.arity; i++)
            {
                if(substitution.find(clausePredicate.arguments[i]) != substitution.end())
                {
                    clausePredicate.arguments[i] = substitution[clausePredicate.arguments[i]];
                }
            }
            result = insertPredicate(clausePredicate, result);
            // result.insert(clausePredicate);
        }
    }

    return result;
}

void addClauseToVisited(Clause clause)
{
    visited.push_back(clause);
    for(auto it = clause.begin(); it != clause.end(); it++)
    {
        Predicate p = *it;
        visitedMap[p].push_back(visited.size()-1);
    }
}

bool isClauseVisited(Clause clause)
{
    Predicate p = *clause.begin();

    vector <int> clauseIndices = visitedMap[p];
    for(int i=0; i<clauseIndices.size(); i++)
    {
        if(clauseCompare(clause, visited[clauseIndices[i]]))
            return true;
    }
    return false;
}

bool clauseCompare(Clause a, Clause b)
{
    if(a.size() != b.size())
        return false;
    for(auto it = a.begin(); it != a.end(); it++)
    {
        Predicate p1 = *it;
        Clause::iterator it2 = b.find(p1);
        if(it2 == b.end())
            return false;
        Predicate p2 = *it2;
        for(int i=0; i<p1.arity; i++)
        {
            if(!isVariable(p1.arguments[i]) && !isVariable(p2.arguments[i]) && p1.arguments[i] != p2.arguments[i])
                return false;
        }
    }
    return true;
}
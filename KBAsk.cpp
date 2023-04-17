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

Clause standardizeQuery(Clause query) 
{
    Clause result;
    for(auto it = query.clause.begin(); it != query.clause.end(); it++)
    {
        Predicate p = *it;
        for(int i=0; i<p.arity; i++)
        {
            if(isVariable(p.arguments[i]))
            {
                p.arguments[i] = p.arguments[i][0] + to_string(KB.size());
            }
        }
        result.insert(p);
    }

    return result;
}

void addQueryToKB(Clause query)
{
    KB.push_back(query);
    for(auto it = query.clause.begin(); it != query.clause.end(); it++)
    {
        Predicate p = *it;
        KBMap[p].insert(KB.size()-1);
    }
}

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

        query = standardizeQuery(query);
        cout<<"\n\nQuery\t: "; query.print(); // debug

        if (query.empty()) return true;

        for(auto it = query.clause.begin(); it != query.clause.end(); it++)
        {
            Predicate p = *it;
            cout<<"Pred\t: "; printPredicate(p); cout<<endl; // debug
            vector <Clause> clauses = getKBClauses(p);
            for (int i=0; i<clauses.size(); i++)
            {
                cout<<"Clause\t: "; clauses[i].print(); // debug
                vector<Clause> unifiedResults = unifyClauses(query, clauses[i], p);
                cout<<"Unified\n ";  // debug
                for(int j=0; j<unifiedResults.size(); j++) {
                    cout<<"\t"; unifiedResults[j].print(); // debug
                    if(unifiedResults[j].size() != 0)
                       resolver.push(unifiedResults[j]);
                    else 
                        return true;
                }
                cout<<"Visited\t: "<< visited.size() <<endl; // debug
                cout<<"Resolve\t: "<< resolver.size() <<endl<<endl; // debug
            }
            // printClauses(clauses); // debug

        }

        insertClause(query);
    }

    // vector <Clause> clauses = getKBClauses(target);
    // printClauses(clauses); // debug

    return false;
}

vector <Clause> getKBClauses(Predicate target)
{
    vector <Clause> clauses;
    target.sign = !target.sign;
    
    // vector <int> clauseIndices = KBMap[target];
    unordered_set <int> clauseIndices = KBMap[target];
    cout<<"Clauses\t: "<<clauseIndices.size()<<" : ";  // debug

    for(auto it = clauseIndices.begin(); it != clauseIndices.end(); it++)
    {
        ClauseVect::iterator it2 = KB[*it].find(target);
        if(it2 == KB[*it].clause.end()) {
            // cout<<*it<<":CU "; // debug
            continue;
        }
        Predicate p = *it2;
        clauses.push_back(KB[*it]);
        cout<<*it<<" "; // debug
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


vector<Clause> unifyClauses(Clause query, Clause clause, Predicate target)
{
    vector<Clause> result;

    for(auto it = clause.clause.begin(); it != clause.clause.end(); it++)
    {
        Predicate p = *it;
        if(p.name == target.name && p.arity == target.arity  && p.sign == !target.sign)
        {
            unordered_map<string, string> substitution;
            bool canUnify = true;

            // create substitution map
            for(int i=0; i<p.arity; i++)
            {
                if(p.arguments[i] != target.arguments[i])
                {
                    if(!isVariable(p.arguments[i]) && !isVariable(target.arguments[i]) && p.arguments[i] != target.arguments[i]) {
                        canUnify = false;
                        break;
                    }
                    else if(isVariable(p.arguments[i]) && isVariable(target.arguments[i])) {
                        if(substitution.find(p.arguments[i]) != substitution.end() || substitution.find(target.arguments[i]) != substitution.end()) 
                        {
                            canUnify = false;
                            break;
                        }
                        substitution[p.arguments[i]] = target.arguments[i];
                    }
                    else if(isVariable(p.arguments[i])) {
                        if(substitution.find(p.arguments[i]) != substitution.end() && substitution[p.arguments[i]] != target.arguments[i]) {
                            canUnify = false;
                            break;
                        }
                        substitution[p.arguments[i]] = target.arguments[i];
                    }
                    else if(isVariable(target.arguments[i])) {
                        if(substitution.find(target.arguments[i]) != substitution.end() && substitution[target.arguments[i]] != p.arguments[i]) {
                            canUnify = false;
                            break;
                        }
                        substitution[target.arguments[i]] = p.arguments[i];
                    }
                }
            }

            // unify query and clause
            if(!canUnify) continue;
            Clause unified;

            for(int i=0; i<target.arity; i++)
                if(substitution.find(target.arguments[i]) != substitution.end())
                    target.arguments[i] = substitution[target.arguments[i]];

            for(auto it = query.clause.begin(); it != query.clause.end(); it++)
            {
                Predicate queryPredicate = *it;
                for(int i=0; i<queryPredicate.arity; i++)
                {
                    if(substitution.find(queryPredicate.arguments[i]) != substitution.end())
                        queryPredicate.arguments[i] = substitution[queryPredicate.arguments[i]];
                }
                if(!queryPredicate.compare(target))
                    unified.insert(queryPredicate);
            }
            target.sign = !target.sign;
            for(auto it = clause.clause.begin(); it != clause.clause.end(); it++)
            {
                Predicate clausePredicate = *it;
                for(int i=0; i<clausePredicate.arity; i++)
                {
                    if(substitution.find(clausePredicate.arguments[i]) != substitution.end())
                        clausePredicate.arguments[i] = substitution[clausePredicate.arguments[i]];
                }
                if(!clausePredicate.compare(target))
                    unified.insert(clausePredicate);
            }

            if(unified.checkTautology()) continue;
            result.push_back(unified);
        }
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

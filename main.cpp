#include <iostream>
#include <vector>
#include <string>

using namespace std;

#include "dataStructures.h"

int main()
{
    // string s = "P(x,y)";
    string s = "P(x,y) | Q(z,y) & R(y,z) => Q(x,y)";


    FOL *fol = stringToFOL(s);
    printFOL(fol);

    return 0;
}
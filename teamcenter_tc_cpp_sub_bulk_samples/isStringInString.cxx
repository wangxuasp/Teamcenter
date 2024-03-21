
#include <iostream>
#include <fclasses/tc_string.h>

using namespace std;

/*   Input Example:
        string haystack = "object_name";
        string needle = "name";
        isStringInString(haystack, needle);
*/
bool isStringInString(string str1, string str2)
{
    bool bTorF = false;

    if (tc_strstr(str1.c_str(), str2.c_str()) != NULL) bTorF = true;

    return bTorF;
}

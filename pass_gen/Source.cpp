#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <fstream>

#include <ctime>
#include <cassert>

using namespace std;

void build_statistics (istream &input, const unsigned statlen, unordered_map<string, unsigned> &stat)
{
    string s;

    random_device rd;
    mt19937 mt (rd ());

    while (!input.eof ())
    {
        char t;

        input >> t;

        if ('A' <= t && t <= 'Z')
            t += 'a' - 'A';

        if ('a' <= t && t <= 'z')
        {
            s += t;

            if (s.size () > statlen)
                s.erase (s.begin ());

            for (unsigned i = 0; i < s.size (); i++)
                stat[s.substr (i)]++;
        }
        else
            s = "";
    }
}

string pass_gen (const unordered_map<string, unsigned> &stat, const unsigned statlen, const unsigned passlen)
{
    string s;

    random_device rd;
    mt19937 mt (rd ());

    while (s.size () < passlen)
    {
        string temp = s.substr (s.size () >= statlen ? s.size () - statlen : 0);

        int sum = 0;
        vector<unsigned> arr (26);

        do
        {
            temp.erase (temp.begin ());

            for (char c = 'a'; c <= 'z'; c++)
            {
                arr[c - 'a'] = sum;
                if (stat.count (temp + c))
                    sum += stat.at (temp + c);
            }

        } while (sum == 0);

        uniform_int_distribution<unsigned> dist (0, sum - 1);

        s += (--upper_bound (arr.begin (), arr.end (), dist (mt)) - arr.begin ()) + 'a';
    }

    return s;
}

int parse_mod (const string &arg, string &mod, unsigned &val)
{
    auto it = find (arg.begin (), arg.end (), '=');

    try
    {
        mod = arg.substr (0, it - arg.begin ());
        val = stoi (arg.substr (it - arg.begin () + 1));
    }
    catch (...)
    {
        return 0;
    }

    return 1;
}

int main (int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "Arguments format: [filename] [[modificator]=[val]]..." << endl
            << "\t n \t Number of passwords to generate." << endl
            << "\t pl\t Passwords length. 10 by default." << endl
            << "\t sl\t Maximum length of suffix used to build statisics. The default is 3." << endl;

        return 0;
    }

    unsigned statlen = 3, passlen = 10, passnum = 1;

    unordered_map<string, unsigned *> hmod;
    hmod["n"] = &passnum;
    hmod["pl"] = &passlen;
    hmod["sl"] = &statlen;

    for (int i = 2; i < argc; i++)
    {
        string mod;
        unsigned val;

        if (!parse_mod (argv[i], mod, val))
        {
            const char *endings[] = {"", "st", "nd", "rd"};
            cout << "Wrong modificator format. Check " << i << (i < 4 ? endings[i] : "th") << " argument" << endl;

            return 1;
        }

        if (hmod[mod] == NULL)
        {
            cout << "Unknown modificator \"" << mod << "\"." << endl;

            return 1;
        }

        *hmod[mod] = val;
    }

    ifstream fin (argv[1], ifstream::in);

    if (!fin.is_open ())
    {
        cout << "Error reading file" << endl;

        return 1;
    }

    ios_base::sync_with_stdio (false);
    fin.unsetf (ios_base::skipws);
    fin.tie (NULL);

    unordered_map<string, unsigned> stat;
    build_statistics (fin, statlen, stat);

    fin.close ();
    cout << "Parsed in " << clock () / double (CLOCKS_PER_SEC) << " seconds" << endl;

    for (unsigned i = 0; i < passnum; i++)
        cout << pass_gen (stat, statlen, passlen).c_str () << endl;

    cout.flush ();

    return 0;
}

#include <iostream>
#include <set>
#include <vector>
#include "seq_buffer.h"

using namespace std;

#define LEFTUP 0
#define LEFT 1
#define UP 2

const char* SOURCEFILE = "/home/ubuntu/c++/data/test.txt";
const int WINDOW_SIZE = 8;
const int MIN_LCS_LENGTH = 6;

int Max(int a, int b, int c, int &max);
int getLCSequenceLength(const string &str1, const string &str2);

int main()
{
	int buffer_size = 60000000;

	ifstream ifs(SOURCEFILE);

	if(!ifs)
	{
		cout << "Could not open file " << SOURCEFILE << ".\n";
		exit(1);
	}

	SeqStreamSource seq_source(ifs);

	if(seq_source.eof())
	{
		cout << "Error reading sequence.\n";
		exit(1);
	}

	SeqIterator seq_it(&seq_source, buffer_size);

	set<string> grams;

	for(;seq_it.hasData(WINDOW_SIZE); ++seq_it)
	{
		string windowstr = "";
		for(int i = 0; i < WINDOW_SIZE; ++i)
		{
			windowstr += seq_it[i];
		}
		grams.insert(windowstr);
	}
	
	ofstream out("/home/ubuntu/c++/output/index1.txt");
	vector<int> locations[WINDOW_SIZE-MIN_LCS_LENGTH+1];
	for(set<string>::iterator iter = grams.begin(); iter != grams.end(); ++iter)
	{
		for(int i = 0; i < WINDOW_SIZE-MIN_LCS_LENGTH+1; ++i)
		{
			locations[i].clear();
		}	
		out << *iter << "->";
		seq_it.reset();
		for(; seq_it.hasData(WINDOW_SIZE); ++seq_it)
		{
			string windowstr = "";
			for(int i = 0; i < WINDOW_SIZE; ++i)
			{
				windowstr += seq_it[i];
			}
			int lsc_L = getLCSequenceLength(windowstr,*iter);
			if(lsc_L >= MIN_LCS_LENGTH)
			{
				locations[lsc_L - MIN_LCS_LENGTH].push_back(seq_it.position());
			}
		}

		for(int i = 0; i < WINDOW_SIZE-MIN_LCS_LENGTH+1; ++i)
		{
			out << "<";
			for(vector<int>::iterator iter1 = locations[i].begin(); iter1 != locations[i].end(); ++iter1 )
			{
				out << *iter1;
				if(iter1 + 1 != locations[i].end())
				{
					out << ",";
				}
			}
			out << ">";
		}
		out << endl;
	}
	out.close();

	return 0;
}

int getLCSequenceLength(const string &str1, const string &str2)
{
    int xlen = str1.size();
    int ylen = str2.size();

    if ( xlen == 0 || ylen == 0)
        return 0;
    pair<int,int> arr[ylen+1][xlen+1];  

    for ( int i = 0; i <= xlen; ++i )
        arr[0][i].first = 0;
    for ( int j = 0; j <= ylen; ++j )
		arr[j][0].first = 0;

    for ( int i = 1; i <= ylen; ++i )
    {
        char s = str2.at(i-1);
        for ( int j = 1; j <= xlen; ++j )
        {
            int leftup = arr[i-1][j-1].first;
            int left = arr[i][j-1].first;
            int up = arr[i-1][j].first;

            if (str1.at(j-1) == s)        //c1==c2
            {
                leftup++;
            }
            arr[i][j].second = Max(leftup,left,up,arr[i][j].first);

        }
    }
    int LCS_Length = 0;
    int i = ylen,j = xlen;

    while ( i>=0 && j>=0 )
    {
        if(arr[i][j].second == LEFTUP)
        {
            if(arr[i][j].first == arr[i-1][j-1].first+1)
                LCS_Length++;
            --i;
            --j;
        }
        else if(arr[i][j].second == LEFT)
            --j;
        else if(arr[i][j].second == UP)
            --i;
    }


    return LCS_Length;
}

int Max(int a, int b, int c, int &max)
{
    int res = 0;
    max = a;
    if(b > max)
    {
        max = b;
        res = 1;
    }
    if(c > max)
    {
        max = c;
        res = 2;
    }

    return res;
}

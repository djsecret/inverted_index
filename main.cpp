#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <string.h>

#include "seq_buffer.h"

using namespace std;

//const char* sourcefile = "DNA5M.txt";
const char* sourcefile = "test.txt";
const int gramLength = 8;

int main()
{
    int buffer_size = 60000000;


    ifstream ifs(sourcefile);
    if(!ifs)
    {
    	cout <<"Could not open file "<<sourcefile<<".\n";
    	exit(1);
    }
    SeqStreamSource seq_source(ifs);
    if (seq_source.eof())
    {
        cout << "Error reading sequence.\n";
        exit(1);
    }
    SeqIterator seq_it(&seq_source, buffer_size);

    set<string> grams;
    map<string,vector<int*> > index;

    int num = 0;
    for(;seq_it.hasData(gramLength); ++seq_it)
    {
        string windowstr = "";
        for(int i = 0; i < gramLength; ++i)
        {
            windowstr += seq_it[i];
        }
        grams.insert(windowstr);
        num++;
    }

    ofstream out("index.txt");
    vector<int*> positions;//save every <pos,offsets...>
    for(set<string>::iterator iter = grams.begin(); iter != grams.end(); ++iter)//for every gram
    {
        //cout << endl << *iter << endl;
        out << *iter << "->";
        positions.clear();
        //cout << "positions.size()" << positions.size() << endl;
        seq_it.reset();
        char firstCH = (*iter).at(0);
        for(;seq_it.hasData(gramLength); ++seq_it)//find every pos
        {
            //cout << seq_it.position() << endl;

            if(*seq_it == firstCH)//for a pos
            {
                int* offsets = new int[gramLength];
                offsets[0] = seq_it.position();
                //cout << firstCH << ":" << offsets[0] << endl;
				char* c_pos = seq_it.getC_pos();
				c_pos++;

				int offset = 1;
                int gramIndex = 1;
                while(gramIndex < gramLength && *c_pos != '\0')
                {
                    if(*c_pos == (*iter).at(gramIndex))
                    {
						//cout << gramIndex << ":" << offset << endl;
                        offsets[gramIndex++] = offset;
                    }
                    ++offset;
                    ++c_pos;
                    //cout << "c_pos:" << c_pos << endl;
                }
                if(gramIndex == gramLength)
                {
                    positions.push_back(offsets);
                }

            }
            //cout << seq_it.position() << endl;
        }

        for(vector<int*>::iterator iter1= positions.begin(); iter1 != positions.end(); ++iter1)
        {
            out << "<";
            for(int i = 0; i < gramLength; ++i)
            {
                out << (*iter1)[i];
                if(i != gramLength-1)
                {
                    out << ",";
                }
            }
            out << ">";
        }
        out << endl;

        //index[*iter] = positions;
        //cout << "positions.size()" << positions.size() << endl;
        //cout << index.size() << endl;
        //grams.erase(iter);
    }
    out.close();

    cout << num << endl;
    cout << grams.size() << endl;

    return 0;
}

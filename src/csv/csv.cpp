#include "csv.h"

using namespace std;

long int string2timestamp(const string &str)
{
    struct tm tm{};
    if(strptime(str.c_str(), "%Y-%m-%d %H:%M:%S",&tm) == NULL)
          return -1;

    return mktime(&tm);
}

void read_record()
{

    // File pointer
    fstream fin;

    // Open an existing file
    fin.open("danmaku.csv", ios::in);

    // Read the Data from the file
    // as String Vector
    vector<string> row;
    string line, word, temp;

    while (fin >> temp) {

        row.clear();

        // read an entire row and
        // store it in a string variable 'line'
        getline(fin, line);

        // used for breaking words
        stringstream s(line);

        // read every column data of a row and
        // store it in a string variable, 'word'
        while (getline(s, word, '|')) {

            // add all the column data
            // of a row to a vector
            row.push_back(word);
        }

        cout << "Time:" << row[0] << "Timestamp" << string2timestamp("2022-01-08" + row[0])  << endl;
    }
}

int main() {
    read_record();
}

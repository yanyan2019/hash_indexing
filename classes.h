#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <bitset>
using namespace std;

class Record {
public:
    int id, manager_id;
    std::string bio, name;

    Record(vector<std::string> fields) {
        id = stoi(fields[0]);
        name = fields[1];
        bio = fields[2];
        manager_id = stoi(fields[3]);
    }

    void print() {
        cout << "\tID: " << id << "\n";
        cout << "\tNAME: " << name << "\n";
        cout << "\tBIO: " << bio << "\n";
        cout << "\tMANAGER_ID: " << manager_id << "\n";
    }
};


class LinearHashIndex {

private:
    const int PAGE_SIZE = 4096;

    vector<int> pageDirectory;  // Where pageDirectory[h(id)] gives page index of block
                                // can scan to pages using index*PAGE_SIZE as offset (using seek function)
    int numBlocks; // n
    int i;
    int numRecords; // Records in index
    int nextFreePage; // Next page to write to
    string fName;
    int block_size;

    // Insert new record into index
    void insertRecord(Record record) {

        // No records written to index yet
        if (numRecords == 0) {
            // Initialize index with first blocks (start with 2)

        }

        // Add record to the index in the correct block, creating overflow block if necessary


        // Take neccessary steps if capacity is reached


    }

public:
    LinearHashIndex(string indexFileName) {
        numBlocks = 0;
        i = 0;
        numRecords = 0;
        block_size = 0;
        fName = indexFileName;
    }

    // Read csv file and add records to the index
    void createFromFile(string csvFName) {
        // variables
        int j = 0;
        string line, token;
        vector<std::string> fields;

        // open csv file
        ifstream infile;
        infile.open(csvFName);

        // split data
        while(getline(infile, line)){
            stringstream line_str(line);
            for(int i = 0; i < 4; i++){
                getline(line_str, token, ',');
                fields.insert(fields.begin()+i, token);
                if(i == 3){
                    // create record
                    Record record(fields);
                    record.print();
                }
            }
        }

        // add to index file
    }

    // Given an ID, find the relevant record and print it
    Record findRecordById(int id) {

    }
};

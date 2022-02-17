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
    int i;          // bits to select
    int numRecords; // Records in index
    int nextFreePage; // Next page to write to

    string fName; // index file name
    int block_size; // total occupied bytes

    // Insert new record into index
    void insertRecord(Record record) {
        ofstream index_file;
        index_file.open(fName, ofstream::app);

        // No records written to index yet
        if (numRecords == 0) {
            // Initialize index with first blocks (start with 2)
            pageDirectory.resize(2);
            numBlocks = 2;
            i++;
        }

        // Add record to the index in the correct block, creating overflow block if necessary
        // hash id and convert to binary
        long unsigned int h_value = record.id%(2^16);
        bitset<8> all_bits(h_value);
        bitset<8> b_key;
// i=8;
        // insert and convert i bits key to find bucket
        for(int j = 0; j < i; j++){
            b_key.set(j, all_bits[j]);
        }
//        cout << " all key bits: "<< all_bits.to_string() << " " << i << " bits key: "<< b_key.to_string() << endl;
        long unsigned int i_key = b_key.to_ulong();
        cout <<  "index key: "<< i_key << endl;


        // insert record to bucket
        index_file.seekp(i_key * PAGE_SIZE);
        //cout << pageDirectory[i_key] * PAGE_SIZE << endl;
        index_file << record.id << "," << record.name << "," << record.bio << "," << record.manager_id << "&";
        pageDirectory[i_key] =  index_file.tellp();
        //cout << "after pos: " << pageDirectory[i_key] << endl;


        // Take neccessary steps if capacity is reached


        numRecords++;

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
        string line, token;
        vector<std::string> fields;

        // open files
        ifstream infile;
        //ofstream index_file;
        //index_file.open(fName);
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

                    // add to index file
                    insertRecord(record);
                }
            }
        }
    }

    // Given an ID, find the relevant record and print it
    Record findRecordById(int id) {

    }
};

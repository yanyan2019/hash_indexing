#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <bitset>
#include <math.h>
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

    // hash id and convert to binary and return the integer key
    long unsigned int create_key(int id){
        long unsigned int h_value = id%(2^16);
        bitset<8> all_bits(h_value);
        bitset<8> b_key;
        for(int j = 0; j < i; j++){
             b_key.set(j, all_bits[j]);
        }
        //cout << " all key bits: "<< all_bits.to_string() << " " << i << " bits key: "<< b_key.to_string() << endl;

        long unsigned int i_key = b_key.to_ulong();
        cout <<  "index key: "<< i_key << endl;
        return i_key;
    }

    // find available size of single block
    int find_space(int start_index){
        ifstream file;
        file.open(fName, ifstream::in);
        int index = -1;

        for(int i = start_index; i < start_index + PAGE_SIZE; i++){
            //cout << "i: "<< i << endl;
            file.seekg(i);
            //cout << file.tellg() << endl;
            char c;
            file.get(c);

            cout << "["<< (int)c << "]" << endl;
            //
            if(c == '\0' || c == '$'){
                cout << " found empty space at "<< i << endl;
                index = i;
                break;
            }
        }
        file.close();
        return index;
    }


    // Insert new record into index
    void insertRecord(Record record) {
        ofstream index_file;
        index_file.open(fName);

        // No records written to index yet
        if (numRecords == 0) {
            // Initialize index with first blocks (start with 2)
            pageDirectory.resize(2, 0);
            pageDirectory[1] = 4097;
            numBlocks = 2;

            // set up record space
            for(int j = 0; j < PAGE_SIZE*2 ;j++){
                index_file << '$';
            }
        }
        // calculate bits to select for creating key
        i = ceil(log(numBlocks));
        long unsigned int i_key = create_key(record.id);

        // available page in page directory
        if(i_key <= numBlocks - 1){

            // loop through record to find empty pos
            int space = find_space(pageDirectory[i_key]);
            cout <<  "space return: "<< space << endl;

            // 4 symbols(4 bytes) + total size of record
            int record_size = 20 + record.bio.length() + record.name.length();

            // check for overflow condition
            if(space != -1 && (PAGE_SIZE + pageDirectory[i_key] - space) >= record_size){
                cout << "insert directly" << endl;
                // insert record
                index_file.seekp(0);

                //cout << pageDirectory[i_key] * PAGE_SIZE << endl;
                index_file << record.id << "," << record.name << "," << record.bio << "," << record.manager_id << "&";

                cout << index_file.tellp();
                // point to next page to write on
                nextFreePage++;

            // page exist but overflow in the bucket
            }else{
                cout << "write to new block" << endl;
                exit(-1);
            }
        }
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

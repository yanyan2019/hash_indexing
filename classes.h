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
/*
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

            //cout << "["<< (int)c << "]" << endl;
            //
            if(c == '\0'){
                cout << " found empty space at "<< i << endl;
                index = i;
                break;
            }
        }
        file.close();
        return index;
    }
*/


    //  flip bit
    int flip_bit(int k){
            cout << "bit flipped at" << endl;
            bitset<8> k2_key(k);
            k2_key.flip(0);
            int k2 = k2_key.to_ulong();
        return k2;
    }

    // pritn main memory
    void print_mm(string ** mm){
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < mm[i]->length(); j++){
                cout << mm[i][j];
            }
            cout << endl;
        }
        cout << endl;
    }


    // Insert new record into index
    void insertRecord(Record record, string ** main_memory, vector<vector<int>> overflow) {
        ofstream index_file;
        index_file.open(fName, ofstream::app);
        int space, record_size, pos = -1;
        string sym = ",", sym2 = "&", sym3 = "&";
        string r_string, str, str2;
        bool relocate = false;

        // 4 symbols(4 bytes) + total size of record
        record_size = sym.length() + sym2.length() + 16 + record.name.length() + record.bio.length();

        // No records written to index yet
        if (numRecords == 0) {
            // Initialize index with first blocks (start with 2)
            pageDirectory.resize(2, -1);
            numBlocks = 2;
        }
        // calculate bits to select for creating key
        i = ceil(log(numBlocks));
        long unsigned int i_key = create_key(record.id);

        // available page in page directory
        // flip bit if need to
        if(i_key > numBlocks - 1){
            cout << "bit flipped." << endl;
            bitset<8> b_key(i_key);
            b_key.flip(0);
            i_key = b_key.to_ulong();
            cout << i_key << endl;
        }
insert:
        // insert to main memory
        if(relocate == false){
        r_string = to_string(record.id) + sym + record.name + sym + record.bio + sym + to_string(record.manager_id) + sym2;
        }else{
            r_string = str + sym2;
            record_size = sym.length() + sym2.length() + 16 + record.name.length() + record.bio.length();
        }
        // overflow (page is full)
        if(PAGE_SIZE - main_memory[i_key]->length() -  sizeof(int) - sizeof('\n') < record_size){
            cout << "overflow" << endl;

            // ipush full block to index file
            index_file.seekp(PAGE_SIZE * nextFreePage);

            cout << "check:" << nextFreePage << endl;

            for(int j = 0; j < PAGE_SIZE; j++){
                if(j >= main_memory[i_key]->length()-1){
                    index_file << sym3;
                    j+= sym3.length();
                }else{
                    index_file << main_memory[i_key][j];
                }
            }

            cout << "done push" << endl;
            pageDirectory[i_key] = PAGE_SIZE * nextFreePage;

            // update overflow location
            cout << overflow.size() << endl;
            if(overflow.size() < (i_key + 1)){
                cout << "pushing new vec" << endl;
                vector<int> v1;
                overflow.push_back(v1);
                cout << "v1 done" << endl;
            }
            overflow[i_key].push_back(nextFreePage);
            nextFreePage++;

            cout << "after update" << endl;
            // clear full block in mm
            main_memory[i_key]->erase(0, main_memory[i_key]->length());

            cout << "erase done" << endl;
        }
        pos = i_key;

        // insert to mm
        cout << "pos: " << pos << endl;
        main_memory[pos]->append(r_string);

        // Take neccessary steps if capacity is reached
        if(relocate == false){
            block_size += record_size;
        }

//print_mm(main_memory);

        // Occupied size divide by total page size
        bool split = (block_size/(numBlocks * PAGE_SIZE)) > 0.70;
        if(relocate){
            split = true;
        }
        if(split){
            cout << "capacity is reached" << endl;
            print_mm(main_memory);
            // add n
            numBlocks += 1;
            cout << numBlocks << endl;
            i = ceil(log(numBlocks));

            pageDirectory.push_back(-1);
            // re-locate records
            // check file
    /*
            ofstream tempfile;
            ifstream infile;
            infile.open(fName);
            tempfile.open("tempfile", ofstream::app);

            while(!infile.eof()){
                getline(infile, str, '&');
                if(str.length() > 1){
                    cout << str << endl;
                    int k = stoi(str.substr(0, 8));
                    long unsigned int k2 = create_key(k);
                    cout << k2 << endl;
                    if(k2 > numBlocks-1){
                        k2 = flip_bit(k2);
                    }

                   if(k2 == numBlocks - 1){
                        // insert to new temp file
                        // update page dir
                        int pos = tempfile.tellp();
                        tempfile << str << sym2;
                        if(pageDirectory[numBlocks-1] == -1){
                            pageDirectory[numBlocks-1] = pos;
                        }
                   }else{
                       cout << "going to insert" << endl;
                      relocate = true;
                      goto insert;

                   }

                }
            }


            relocate = false;
            remove(fName.c_str());
            rename("tempfile", fName.c_str());
            cout << "done relocate" << endl;
*/


            // check mm and relocate
            int s_pos; string s2;
            for(int j = 0; j < numBlocks; j++){
                string s;
                int total = main_memory[j]->length() ,start = 0, found = 0;
                cout << endl << j << endl<< endl;
                while(found < main_memory[j]->length()){
                    s.clear();
                    //cout << start;
                    found = main_memory[j]->find(sym2, start);
                    if(found != -1){

                    cout << start << " to " << found<< endl;
                    s =  main_memory[j]->substr(start, found+1-start);
                    cout << "checking on : "<< s << endl;

                    // hash key
                    int k = stoi(s.substr(0, 8));
                    long unsigned int k2 = create_key(k);

                    print_mm(main_memory);
                    if(k2 > numBlocks - 1){
                        k2 = flip_bit(k2);
                    }

                    // insert if match
                    if(k2 != j){
                        main_memory[k2]->append(s);
                        //print_mm(main_memory);
                        cout << "found: " << found << "length: " << main_memory[j]->length() << endl;
                        s2 =  main_memory[j]->substr(found+1, main_memory[j]->length()-1);
                        //cout << s2 << endl;
                        main_memory[j]->replace(start, s2.length(), s2);
                        main_memory[j]->erase(start+s2.length(), main_memory[j]->length());
                        s2.clear();
                        //start = 0;
                    }else{
                        start = found+1;
                        cout << "new start: " << start << endl;
                    }
                        cout << "length: " <<  main_memory[j]->length()<< endl;
                    }
                    //print_mm(main_memory);
                }

            }
/*            // read index file to mm to relocate
            ofstream tempfile;
            ifstream infile;
            infile.open(fName);
            tempfile.open("tempfile", ofstream::app);
*/
            //while

            // output to new index file(rename new file)
        // print_mm(main_memory);
        }
        //print_mm(main_memory);

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
        string ** main_memory = new string * [3];
        vector<vector<int>> overflow;
        vector<std::string> fields;
         vector<int> v1;
        overflow.resize(2, v1);

        // open files
        ifstream infile;
        infile.open(csvFName);

        for(int j = 0; j < 3; j++){
            main_memory[j] = new string[PAGE_SIZE];
        }

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
                    insertRecord(record, main_memory, overflow);
                }
            }
        }

        // copy all temp file data to index file
        // clear temp files
    }

    // Given an ID, find the relevant record and print it
    Record findRecordById(int id) {

    }
};

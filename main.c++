#include<iostream>
#include<cmath>
#include<sstream>
#include<fstream>
#include<algorithm>
#include<vector>
#include<string>


using namespace std;
struct Node{
    int row, col,value;
    Node* nextRow;
    Node* nextCol;

    Node(int r,int c, int v):row(r),col(c),value(v),nextRow(NULL),nextCol(NULL){}
};

class SparseMatrix{
    private:
    int rows,cols;
    vector<Node*> rowHeader;
    vector<Node*> colHeader;
    
    public:
    SparseMatrix(int m,int n):rows(m),cols(n),rowHeader(m,NULL),colHeader(n,NULL){}


    string trim(const std::string& str) {
        size_t first = str.find_first_not_of(' ');
        if (first == std::string::npos) return ""; // No content

        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }
    void read(const string & filename){
        ifstream file(filename);
        if(!file.is_open()){
            cerr << "Error: file not present\n";
            return;
        }
        cout << "File opened successfully" << endl;

        string line;

        // For dimensions
        while (getline(file, line)) {

            if (line[0] == '#') continue; 
            if (line.find("m =") != string::npos && line.find("n =") != string::npos) {
                size_t mPos = line.find("m =") + 4;
                size_t nPos = line.find("n =") + 4;

                string rowsStr = trim(line.substr(mPos, line.find(" ", mPos) - mPos));
                string colsStr = trim(line.substr(nPos, line.find(" ", nPos) - nPos));

                rows = stoi(rowsStr);
                cols = stoi(colsStr);
                
                break; // Exit after reading dimensions
            }
        }

        rowHeader.resize(rows, nullptr);
        colHeader.resize(cols, nullptr);

        // For values (row, col = value)
        while (getline(file, line)) {
           
            if (line[0] == '#') continue;
            int r, c, value;

            size_t commaPos = line.find(',');
            size_t equalsPos = line.find('=');

            if (commaPos == string::npos || equalsPos == string::npos) {
                cerr << "Invalid format in line: " << line << endl;
                continue;
            }

            r = stoi(line.substr(0, commaPos));                        
            c = stoi(line.substr(commaPos + 1, equalsPos - commaPos)); 
            value = stoi(line.substr(equalsPos + 1));                  

            
            insert(r, c, value);  // Insert into the matrix
        }
    }

    void insert(int row,int col,int value){
        Node* newNode = new Node(row,col,value);

         // Insert int row
        if (!rowHeader[row]) {
            rowHeader[row] = newNode;
        } else {
            Node* current = rowHeader[row];
            while (current->nextCol && current->col < col) {
                current = current->nextCol;
            }
            newNode->nextCol = current->nextCol;
            current->nextCol = newNode;
        }

        // Insert in column
        if (!colHeader[col]) {
            colHeader[col] = newNode;
        } else {
            Node* current = colHeader[col];
            while (current->nextRow && current->row < row) {
                current = current->nextRow;
            }
            newNode->nextRow = current->nextRow;
            current->nextRow = newNode;
        }
    }

    SparseMatrix mult(SparseMatrix& A,int c){
        for(int i=0;i<A.rows;++i){
            Node* curr = A.rowHeader[i];
            while(curr){
                curr->value *= c;
                curr =curr->nextCol;

            }
        }
        return A; 
    }

    SparseMatrix add(SparseMatrix& A,SparseMatrix& B){
        if (A.cols != B.cols && B.rows!=A.rows) {
            cerr << "Matrix dimensions are not compatible for multiplication.\n";
            return SparseMatrix(0, 0); 
        }
        SparseMatrix result(rows,cols);

        for(int i=0;i<rows;++i){
            Node* currA = A.rowHeader[i];
            Node* currB = B.rowHeader[i];

            while(currA ||currB){
                if(currA && (!currB||currA->col<currB->col)){
                    result.insert(currA->row,currA->col,currA->value);
                    currA =currA->nextCol;
                }else if (currB && (!currA || currB->col < currA->col)) {
                    result.insert(currB->row, currB->col, currB->value);
                    currB = currB->nextCol;
                } else { 
                    result.insert(currA->row, currA->col, currA->value + currB->value);
                    currA = currA->nextCol;
                    currB = currB->nextCol;
                }
            }

        }
        return result;
    }
    
    SparseMatrix mult(SparseMatrix&A,SparseMatrix& B) {

        if (A.cols != B.rows) {
            cerr << "Matrix dimensions are not compatible for multiplication.\n";
            return SparseMatrix(0, 0); 
        }
        SparseMatrix result(A.rows, B.cols);
        
        for (int i = 0; i < A.rows; ++i) {
            for (int j = 0; j < B.cols; ++j) {
                int sum = 0;
                Node* currA = A.rowHeader[i];
                Node* currB = B.colHeader[j];

                while (currA && currB) {
                    if (currA->col == currB->row) {
                        sum += currA->value * currB->value;
                        currA = currA->nextCol;
                        currB = currB->nextRow;
                    } else if (currA->col < currB->row) {
                        currA = currA->nextCol;
                    } else {
                        currB = currB->nextRow;
                    }
                }

                if (sum != 0) {
                    result.insert(i, j, sum);
                }
            }
        }

        return result;
    }

    void write(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Could not open file.\n";
            return;
        }

        file << "# Sparse Matrix Output\n";
        file <<"m = "<< rows << " " << "n ="<< cols << "\n";

        for (int i = 0; i < rows; ++i) {
            Node* curr = rowHeader[i];
            while (curr) {
                file << curr->row << "," << curr->col << "=" << curr->value << "\n";
                curr = curr->nextCol;
            }
        }
    }
};

int main() {
    
    SparseMatrix matrixA(0, 0);
    matrixA.read("A_.txt");
    matrixA.write("result_A.txt");

    SparseMatrix matrixB(0, 0);
    matrixB.read("B_.txt");
    matrixB.write("outputB.txt");

    SparseMatrix point(5, 5);

    // Test multiply by constant
    // SparseMatrix matrixC = point.mult(matrixA,3);
    // matrixA.write("Const_multiply.txt");

    // Test add
    SparseMatrix matrixD = point.add(matrixA,matrixB);
    matrixD.write("Addition.txt");

    // // Test multiplication
    // SparseMatrix matrixE = point.mult(matrixA,matrixB);
    // matrixE.write("output_mult_matrix.txt");

    return 0;
}
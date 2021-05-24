#include <cstdlib>

#define IO ios_base::sync_with_stdio(0); cin.tie(0); cout.tie(0)
#include <queue>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>  
#include <string>
#include <sstream>
#include <bitset>
#include <chrono>
#include <cassert>

//#include <Windows.h> //if Windows

using namespace std;
using namespace std::chrono;

string orgFileName = "", inFile = "", outFile = "";

string charToBinaryString(unsigned char ch) {
	string ret = "";
	for (int i = 7; i >= 0; i--)
		if (1 << i & ch)
			ret = ret + "1";
		else
			ret = ret + "0";
	return ret;
}

string* stringToBinaryString(string* s) {
	string* ret = new string();
	for (int i = 0; i < (int)s->size(); ++i) {
		*ret += charToBinaryString((*s)[i]);
	}
	return ret;
}

string digitToBinaryString(int dig, int lim = 3) {
	string ret = "";
	for (int i = lim - 1; i >= 0; i--)
		if (1 << i & dig)
			ret = ret + "1";
		else
			ret = ret + "0";
	return ret;
}

int decimalFromBinary(int len, int ind, string* s) {
	int n = 0;
	for (int i = len - 1; i >= 0; i--) {
		n += (1 << i) * ((*s)[ind + len - 1 - i] - '0');
	}
	return n;
}

class Node {
public:
	Node* left;
	Node* right;
	int freq;
	unsigned char c;
	string code;
	Node() {
		this->left = this->right = NULL;
	}

	Node(int freq, char c) {
		this->freq = freq;
		this->c = c;
		this->left = this->right = NULL;
	}

	//Returns 1 if a node is leaf
	int isLeaf() {
		return (left == NULL && right == NULL);
	}
};

struct Comp {
	bool operator()(Node* a, Node* b) {
		return a->freq > b->freq;
	}
};

class HuffmanTreeEncoding {
private:
	string* input;
	vector<int> freq;
	vector<string> code;
	vector <Node*> nodes;
	Node* root;
	int size, h;
	long long startSize, endSize;
	string* compressed;

	//A function to calculate the height of the huffman tree
	int calculateHeight(Node* root) {
		return(root == NULL) ? -1 : 1 + max(calculateHeight(root->left), calculateHeight(root->right));
	}

	//Traverse the tree to get the encoded tree
	void dfsTree(Node* root, string* ret) {
		if (root == nullptr)
			return;
		if (root->isLeaf()) {
			(*ret) += "1" + charToBinaryString(root->c);
			return;
		}
		(*ret) += "0";
		dfsTree(root->left, ret);
		dfsTree(root->right, ret);
	}

	//Fills frequency array, and creates a vector of nodes to be used in building the tree
	void initialize() {
		for (const unsigned char& c : *input) {
			freq[c]++;
		}

		for (int i = 0; i < 256; i++) {
			if (!freq[i])
				continue;
			char ch = i;
			nodes.push_back(new Node(freq[i], ch));
		}
		this->size = (int)nodes.size();
	}

	//Building the tree
	void build() {
		priority_queue <Node*, vector<Node*>, Comp> minHeap;
		int n = this->size;
		for (int i = 0; i < n; ++i) {
			minHeap.push(nodes[i]);
		}
		for (int i = 0; i < n - 1; ++i) {
			Node* node = new Node();
			node->left = minHeap.top();
			minHeap.pop();
			node->right = minHeap.top();
			minHeap.pop();
			node->freq = node->left->freq + node->right->freq;
			minHeap.push(node);
		}
		this->root = minHeap.top();
		this->h = calculateHeight(this->root);
	}

	//Encoding
	void encode(Node* root, string str = "") {
		if (root == nullptr)
			return;
		// Assign 0 to left edge and recur
		if (root->left)
			encode(root->left, str + "0");

		// Assign 1 to right edge and recur
		if (root->right)
			encode(root->right, str + "1");

		/* If this is a leaf node, then
		   it contains one of the input characters*/
		if (root->isLeaf()) {
			if (str == "")
				str = "0"; // To handle case of only 1 character in the file
			code[root->c] = str;
			root->code = str;
		}
	}

	void printLeaves(Node* root) {
		if (root->left && root->right) {
			printLeaves(root->left);
			printLeaves(root->right);
		}
		if (root->isLeaf()) {
			cout << "\t\t" << root->c << "\t\t " << charToBinaryString(root->c) << "\t\t" << root->code << endl << endl;
		}
	}

public:
	HuffmanTreeEncoding(string* input) {
		this->input = input;
		freq.resize(256, 0);
		code.resize(256); //new codes of chars
		startSize = input->size();
		endSize = -1;
		root = nullptr;
	}

	//Gets the encoded tree as binary
	string* getEncodedTree() {
		string* ret = new string();
		dfsTree(this->root, ret);
		return ret;
	}

	//Gets the input encoded string as binary
	string* getEncodedString() {
		string* all = new string();
		for (const unsigned char& c : *input) {
			*all += code[c];
		}
		return all;
	}

	void print() {
		cout << "\t\tThe codes of each character: \n\n";
		cout << "\t     Character \t\t Code \t\t        New Code\n\n";
		printLeaves(root);
	}

	//A function that collects all parts of the compressed string and returns it
	string* getCompressedText(string* tree, string* text) {
		int len = (*tree).length() + (*text).length() + 3; //The first 3 bits that represent the number 'extra'.
		int more = 8 - len % 8;
		if (more == 8)
			more = 0;
		string* extra = new string(more, '0');
		string* extraLength = new string(digitToBinaryString(more));
		string* newString = new string();
		*newString = *extraLength + *tree + *text + *extra;

		stringstream sstream(*newString);
		string* output = new string();

		while (sstream.good()) {
			bitset<8> bits;
			sstream >> bits;
			char c = char(bits.to_ulong());
			*output += c;
		}
		(*output).pop_back();

		delete (extra);
		delete (extraLength);
		delete (newString);

		return output;
	}

	//Function that writes the encoded string to the file
	void writeToFile(string* compressedText, string* filename) {
		ofstream MyFile(*filename,ios::out | ios::binary);
		//MyFile << *compressedText;
		string text = *compressedText;
		MyFile.write(text.c_str(), text.size());
		MyFile.close();
	}

	//Main compression function
	void compress(string* fileName) {
		//filename is the output filename
		initialize();
		build();
		encode(this->root);

		string* encodedString = getEncodedString();
		string* encodedTree = getEncodedTree();
		string* compressedText = getCompressedText(encodedTree, encodedString);
		this->endSize = compressedText->size();

		//print the new compressed text to the file
		writeToFile(compressedText, fileName);

		delete(encodedString);
		delete(encodedTree);
		delete(compressedText);
		delete(input);
	}

	//Function that calculates the compression ratio
	double getCompressionRatio() {
		return startSize * 1.0 / endSize;
	}

};

//decoding and decompressing
class HuffmanTreeDecoding {

private:
	Node* root;
	string* encoded;

public:
	HuffmanTreeDecoding(string* encoded) {
		this->root = new Node();
		this->encoded = encoded;
	}

	//see if the node is 0 (Not a leaf node), or 1 (Leaf node)
	pair<int, Node*>* make_node(int ind, string* binencode) {
		Node* n = new Node();
		pair<int, Node*> p = make_pair(0, n);
		if ((*binencode)[ind] == '1') {
			p.first = 1;
			n->c = decimalFromBinary(8, ind + 1, binencode);
		}
		return new pair<int, Node*>(p);
	}

	//Building decoded tree
	int buildTree(Node* root, string* binencode, int ind = 4) {
		pair<int, Node*>* left_node = make_node(ind, binencode);
		root->left = left_node->second;
		if (left_node->first) {
			ind += 9;
		}
		else {
			ind++;
			ind = buildTree(root->left, binencode, ind);
		}

		pair<int, Node*>* right_node = make_node(ind, binencode);
		root->right = right_node->second;
		if (right_node->first) {
			ind += 9;
		}
		else {
			ind++;
			ind = buildTree(root->right, binencode, ind);
		}
		return ind;
	}

	//Removes extra padding at the end of the compressed string
	void rempadding(string* s) {
		assert(s->size() > 3);
		int x = decimalFromBinary(3, 0, s);
		assert((int)s->size() > x);
		while (x--) {
			s->pop_back();
		}
	}

	//Decodes the actual input and writes it back to file
	void decode_and_write(string* binencode, int textStart, string* fileName) {
		Node* temp = root;
		ofstream write;
		write.open(*fileName);
		for (int i = textStart; i < (int)binencode->size(); i++) {
			if ((*binencode)[i] == '0')
				temp = temp->left;
			else {
				temp = temp->right;
			}
			if (temp->isLeaf()) {
				write << temp->c;
				temp = root;
			}
		}
		write.close();
		return;
	}

	//Main decompression function
	void decode(string* filename) {
		string* binencode = stringToBinaryString(encoded);
		rempadding(binencode);
		int start_ind = buildTree(root, binencode);
		decode_and_write(binencode, start_ind, filename);
		delete(binencode);
	}
};

//Reads input from a given file
string* readFile(string fileName) {
	ifstream in(fileName, ios::in | ios::binary);
	std::ostringstream contents;
	contents << in.rdbuf();
	in.close();
	string* ret = new string();
	*ret = contents.str();
	return ret;
}


//Validates the input file if it exists in the same directory
int invalid(string filename) {
	ifstream f(filename.c_str());
	if (!f.good()) {
		cout << "File does not exist, please enter a valid file name" << endl;
		return 1;
	}

	int last_dot = -1, n = filename.length();
	for (int i = 0; i < n; i++)
		if (filename[i] == '.') last_dot = i;
	return (last_dot == n - 1 || last_dot == -1 || last_dot == 0);
}

//Parses the input file name to the file name and extension
pair<string, string> get_extension(string& filename) {
	while (invalid(filename)) {
		cout << "Please enter a correct filename, in the form filename.extension" << endl;
		cin >> filename;
	}
	int last_dot = -1, n = filename.length();
	for (int i = 0; i < n; i++)
		if (filename[i] == '.') last_dot = i;
	string fname = filename.substr(0, last_dot);
	string ext = filename.substr(last_dot, n - last_dot + 1);
	return { fname, ext };
}

//Function returns -1 if compression/decompression did not happen, 1 if the files are identical and 0 otherwise
int compare_files(string filename1, string filename2) {
	cout << "Comparing '" << filename1 << "' and '" << filename2 << "' ..." << endl << endl;
	if (filename1 == "" || filename2 == "") return -1;
	string* ip = readFile(filename1);
	string* op = readFile(filename2);
	if ((int)(*ip).size() != (int)(*op).size()) return 0;
	for (int i = 0; i < (int)(*ip).size(); i++) {
		if ((*ip)[i] != (*op)[i])
			return 0;
	}
	return 1;
}

//Main menu function
void show_menu() {
	int choice;
	cout << "Please choose what would like to do:\n\
    1) Compress a file\n\
    2) Decompress a file\n\
    3) Compare the compressed and decompressed files\n\
    4) Exit\n";
	cin >> choice;
	while (choice < 1 || choice > 4) {
		cout << "Please enter a valid choice!..." << endl;
		cin >> choice;
	}
	if (choice == 4)
		exit(0);
	//system("cls");  //Windows
	//system("clear"); //Linux
	pair<string, string> p;
	if (choice == 1 || choice == 2) {
		cout << "Please enter the name of the file you want to compress/decompress : " << endl << endl;
		cin >> inFile;
		if (choice == 1) orgFileName = inFile;
		p = get_extension(inFile);
	}
	cout << "\nWorking...\n\n";
	switch (choice) {
	case 1: {
		HuffmanTreeEncoding huffman(readFile(inFile));
		outFile = p.first + "-compressed.bin";
		auto start = high_resolution_clock::now();
		huffman.compress(new string(outFile));
		auto stop = high_resolution_clock::now();
		duration<double, milli> time = stop - start;
		cout << "\nDone!\n\nThe compressed file '" << outFile << "' has been saved" << endl << endl;
		cout << "Compression Ratio = " << huffman.getCompressionRatio() << endl << endl;
		if ((int)time.count() > 1000)
			cout << "Time taken for compression = " << time.count() / 1000.0 << " seconds" << endl << endl;
		else
			cout << "Time taken for compression = " << time.count() << " milliseconds" << endl << endl;
		huffman.print();
		cout << "Press enter key to continue..." << endl;
		cin.ignore();
		cin.get();
		break;
	}
	case 2: {
		HuffmanTreeDecoding huffman(readFile(inFile));
		outFile = p.first + "-decompressed.txt";
		auto start = high_resolution_clock::now();
		huffman.decode(new string(outFile));
		auto stop = high_resolution_clock::now();
		duration<double, milli> time = stop - start;
		cout << "Done!\n\nThe decompressed file '" << outFile << "' has been saved." << endl << endl;
		if ((int)time.count() > 1000)
			cout << "Time taken for compression = " << time.count() / 1000.0 << " seconds" << endl << endl;
		else
			cout << "Time taken for compression = " << time.count() << " milliseconds" << endl << endl;
		cout << "Press enter key to continue..." << endl;
		cin.ignore();
		cin.get();
		break;
	}
	case 3: {
		cout << "Please enter the name of the files you want to compare : " << endl << endl;
		cout << "First File : " << endl;
		cin >> orgFileName;
		cout << "Second File : " << endl;
		cin >> outFile;
		int res = compare_files(orgFileName, outFile);
		if (res == -1)
			cout << "You have to compress and decompress first!" << endl << endl;

		else if (res == 0)
			cout << "Files are not identical!" << endl << endl;

		else
			cout << "Files are identical!" << endl << endl;

		cout << "Press enter key to continue..." << endl;
		cin.ignore();
		cin.get();
	}
	}
	//system("cls"); //Windows
	//system("clear"); //Linux
	show_menu();
}

int main() {
	show_menu();
	return 0;
}

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <queue>
using namespace std;

map <string, string*> permutations;

typedef struct node { 
    string op = "";
    string exp = ""; 
    string letter1, letter2;
    int level, count;
    bool inv = false, end = false;//, parent_inv=false;
    struct node *left, *right; 

    node(){
        letter1="*";
        letter2 = "*";
    }
    node( const node &obj);  // copy constructor
    ~node();                     // destructor
} node; 

string getLetter(string prev, int i, bool inv){
    //  A = CBC’B’ ; A’ = BCB’C’
    //  B = CDC’D’ ; B’ = DCD’C’
    //  C = DED’E’ ; C’ = EDE’D’
    //  D = EBE’B’ ; D’ = BEB’E’
    //  E = DAD’A’ ; E’ = ADA’D’
    if (inv){
        if (prev.size() ==2){
            prev = prev.substr(0, 1);
        }else{
            prev += "'";
        }
    }
    if (!prev.compare("A")){
        string arr[4] = {"C", "B", "C'", "B'"};
        return arr[i];
    }
    if (!prev.compare("B")){
        string arr[4] = {"C", "D", "C'", "D'"};
        return arr[i];
    }
    if (!prev.compare("C")){
        string arr[4] = {"D", "E", "D'", "E'"};
        return arr[i];
    }
    if (!prev.compare("D")){
        string arr[4] = {"E", "B", "E'", "B'"};
        return arr[i];
    }
    if (!prev.compare("E")){
        string arr[4] = {"D", "A", "D'", "A'"};
        return arr[i];
    }
    if (!prev.compare("A'")){
        string arr[4] = {"B", "C", "B'", "C'"};
        return arr[i];
    }
    if (!prev.compare("B'")){
        string arr[4] = {"D", "C", "D'", "C'"};
        return arr[i];
    }
    if (!prev.compare("C'")){
        string arr[4] = {"E", "D", "E'", "D'"};
        return arr[i];
    }
    if (!prev.compare("D'")){
        string arr[4] = {"B", "E", "B'", "E'"};
        return arr[i];
    }
    if (!prev.compare("E'")){
        string arr[4] = {"A", "D", "A'", "D'"};
        return arr[i];
    }
    return "X";
}

class Decomposition {
	public:
        int max_depth = 0;
        node* tree;

        Decomposition(){};

        bool isOperator(string str);
        node* parse(string func, int lvl); 
        void permute(node* tree);
        node* traverse(node* tree);
        node* convert(node* tree);
        node* set_exp(node* tree);
};


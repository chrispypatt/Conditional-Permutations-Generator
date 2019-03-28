#include "bool2cp.h"
/* Test cases: this will grow as program is extended
    F(x, y, z) = (x z)’ (x’y’)’
    // AND(NAND(x,z),NAND(x',y')) - check nomial example
    // AND(NAND(x,z),NAND(AND(r,s),y')) - to check nested
    // NAND(AND(x,y),AND(t,s)) - to check for correct expansion when expanding {xy:B'C,C} like options
*/

/* 
    Use ( ) to specify order of operations
    AND == AND
    NAND == NAND
    OR == OR
    NOR == NOR
    XOR == XOR
    ' == NOT
*/
int main (int argc, char *argv[]){
    string bool_func;
    node* exp_tree = NULL;
    Decomposition dBool;


    cout << "Please enter a boolean function: ";
    getline(cin,bool_func); 
    node* tree = dBool.parse(bool_func, 1);
    dBool.traverse(tree);
    return 0;
}

bool Decomposition::isOperator(string str){ 
    //check if this is an AND, NAND, OR, NOR, or XOR operation
    string two = str.substr(0, 2);
    string three = str.substr(0, 3);
    string four = str.substr(0, 4);
    return (!three.compare("AND") || !four.compare("NAND") || !three.compare("XOR") || !two.compare("OR") || !three.compare("NOR") || !three.compare("NOT"));
} 

node* Decomposition::parse(string func, int lvl){ 
    node* nn = (node*)malloc(sizeof(node));
    nn->level = lvl;
    nn->left = NULL; 
    nn->right = NULL;
    nn->exp = func;
    if (max_depth < lvl){
        max_depth = lvl;
    }
    if (isOperator(func)){//recursively build simple tree
        int j = 0;
        string left = "", right = "";
        int i = 0;
        while (func[j]!='('){
            nn->op += func[j++];
        }
        if (nn->op == "NAND"){
            nn->op = "AND";
            nn->inv = true;
        }
        string inner = func.substr(j+1, func.length() - (j+2));
        if (isOperator(inner)){//get the left chunk as a boolean expression
            while (inner[i] != '('){
                left += inner[i++];
            }
            int open = 1;
            left += inner[i++];
            while (open>0){
                if (inner[i]=='('){//this should always be true
                    open++;
                }else if (inner[i]==')'){//this should always be true
                    open--;
                }
                left += inner[i++];
            } //this should get us to the inner[i] == ','
            i++; //increment over ','
        }else{//left chunk is just one variable
            while (inner[i]!=','){
                left += inner[i++];
            } //iterate to ','
            i++; //increment over ','
        }
        //right should be the second part of inner: inner[i] to inner.end
        right = inner.substr(i, inner.size() - (i));
        nn->left = parse(left,lvl+1);
        nn->right = parse(right, lvl+1);
        if (nn->inv){
            nn->left->parent_inv = true;
            nn->right->parent_inv = true;
        }
    }else{
        nn->end = true;
    }

    if (nn->exp.size() == 2){//If this is a leaf and it is an inversion, note it
        nn->inv = true;
    }
    // cout << func << endl;
    // cout << lvl << " : " << nn->exp << endl;
    return nn;
} 

void Decomposition::traverse(node* tree){
    queue <node*> Q;
    Q.push(tree);
    int count;
    tree->letter1 = "A";
    tree->letter2 = "*";
    cout << "You entered: " << tree->exp << endl;
    int lvl = 0;
    while(!Q.empty()){
        node* n = Q.front();

        if (lvl < n->level){//tell us what step we are on
            lvl = n->level;
            count = 1;
            cout << endl << "Step: " << lvl << endl;
        }

        // put fringe into queue

        // propogate leaf nodes down. These have no children
        if (n->end && n->level < max_depth && lvl < max_depth){
            node* next = (node*)malloc(sizeof(node));
            *next = *n;
            next->level = lvl + 1;
            Q.push(next);
        }

        // if not a leaf node
        // expand line (4 or 5 new lines)  
        if (n->left != NULL){
            node* next1 = (node*)malloc(sizeof(node));
            *next1 = *n->left;
            next1->letter1 = getLetter(n->letter1, 0, n->inv);
            next1->letter2 = "*";
            next1->count = 1;
            Q.push(next1);
        }            
        if (n->right != NULL){
            node* next2 = (node*)malloc(sizeof(node));
            *next2 = *n->right;
            next2->letter1 = getLetter(n->letter1, 1, n->inv);
            next2->letter2 = "*";
            next2->count = 2;
            Q.push(next2);
        }
        if (n->left != NULL){
            node* next3 = (node*)malloc(sizeof(node));
            *next3 = *n->left;
            next3->letter1 = getLetter(n->letter1, 2, n->inv);
            next3->letter2 = "*";
            next3->count = 3;
            Q.push(next3);
        }
        if (n->right != NULL){
            node* next4 = (node*)malloc(sizeof(node));
            *next4 = *n->right;
            next4->letter1 = getLetter(n->letter1, 3, n->inv);
            next4->letter2 = "*";
            next4->count = 4;
            Q.push(next4);
            if (n->inv){ //we don't fold in the inversion line, instead, add it as a new node
                node* nexts = (node*)malloc(sizeof(node));
                nexts->letter1 = n->letter1;
                nexts->letter2 = n->letter1;
                nexts->level = next4->level;
                nexts->end = true;
                nexts->exp = "*";
                nexts->count = 5;
                Q.push(nexts);
            }
        }

        //print permutation
        if (max_depth == n->level && n->inv){
            cout << count << ": " << "{" << n->exp.substr(0, 1) << ": " << n->letter2 << ", " << n->letter1 << "}" << endl;
        }else{
            cout << count << ": " <<  "{" << n->exp << ": " << n->letter1 << ", " << n->letter2 << "}" << endl;
        }
        count++;
        Q.pop();
    }
}
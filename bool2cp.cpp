#include "bool2cp.h"
/* Test cases: this will grow as program is extended
    F(x, y, z) = (x z)’ (x’y’)’ 
        AND(NAND(x,z),NAND(x',y')) - check nomial example

    Yadu's Test Cases:
    F(x,y) = x+y = (x'y')'
        OR(x,y) = NAND(x',y')

    F(x,y,z) = (x'y'z'),
        NAND(AND(x',y'),z')

    OR(OR(x,y),OR(x',y'))
    
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
    dBool.permute(tree);
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
        int j = 0, i = 0;
        string left = "", right = "";
        string op = "";
        while (func[j]!='('){
            op += func[j++];
        }

        nn->op = op;
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

        //If this is NAND, flag it as an inversion
        if (!op.compare("NAND")){
            nn->inv = true;
        }
    }else{//this is a leaf node, a single variable
        nn->end = true;
        if (nn->exp.size() == 2){//If this variable is inverted, flag it for later
            nn->inv = true;
        }
    }
    return nn;
} 

node* Decomposition::convert(node* tree){
    //we need to traverse through and convert each non-AND 
    //and non-NAND op from bottom up
    //then we must traverse through again and update expressions
    return set_exp(traverse(tree));
}

node* Decomposition::traverse(node* tree){
    //this is very verbose and probably bad but it works and 
    //time constraints don't allow for refactoring before due date
    if(!tree->op.compare("")){
        return tree;
    }

    traverse(tree->left);
    traverse(tree->right);
    // if this is a special case, convert to AND and NAND
    if(!tree->op.compare("OR") || !tree->op.compare("NOR")){
        //convert this node
        if(!tree->op.compare("OR")){//OR(x,y) -> NAND(x',y')
            tree->op = "NAND";
            tree->inv = true;
        }else{//NOR(x,y) -> AND(x',y')
            tree->op = "AND";
            tree->inv = false;       
        }
        //convert left child
        if (!tree->left->op.compare("")){
            //invert variable
            if (tree->left->exp.size() == 2){//variable in form of x' (inverted)
                tree->left->exp = tree->left->exp.substr(0,1);
                tree->left->inv = false;
            }else{//variable in form of x (noninverted)
                tree->left->exp += "'";
                tree->left->inv = true;
            }
        }else{
            //invert child expression
            //children should be in NAND and AND form by now or something bigger is wrong!
            if (!tree->left->op.compare("NAND")){
                tree->left->inv = false;
                tree->left->op = "AND";
            }else{
                tree->left->inv = true;
                tree->left->op = "NAND";
            }
        }
        //convert right child
        if (!tree->right->op.compare("")){
            //invert variable
            if (tree->right->exp.size() == 2){//variable in form of x' (inverted)
                tree->right->exp = tree->right->exp.substr(0,1);
                tree->right->inv = false;
            }else{//variable in form of x (noninverted)
                tree->right->exp += "'";
                tree->right->inv = true;
            }
        }else{
            //invert child expression
            //children should be in NAND and AND form by now or something bigger is wrong!
            if (!tree->right->op.compare("NAND")){
                tree->right->inv = false;
                tree->right->op = "AND";
            }else{
                tree->right->inv = true;
                tree->right->op = "NAND";
            }
        }
    }else if(!tree->op.compare("XOR")){
        //XOR(x,y) -> NAND(NAND(x,NAND(x,y)),NAND(y,NAND(x,y)))
        tree->op = "NAND";
        tree->inv = true;
        node* A = tree->left;
        node* B = tree->right;

        A->level += 2;
        B->level += 2;

        node* left = (node*)malloc(sizeof(node));
        node* leftleft = (node*)malloc(sizeof(node));
        node* leftright = (node*)malloc(sizeof(node));
        node* right = (node*)malloc(sizeof(node));
        node* rightleft = (node*)malloc(sizeof(node));
        node* rightright = (node*)malloc(sizeof(node));

        *leftleft = *A;
        *rightleft = *B;
        leftleft->level--;
        rightleft->level--;

        //build NAND(x,y)
        leftright->left = A;
        leftright->right = B;
        leftright->op = "NAND";
        leftleft->level = tree->level + 2;
        //build NAND(x,y)
        rightright->left = A;
        rightright->right = B;
        rightright->op = "NAND";
        rightright->level = tree->level + 2;
        //build NAND(AND(x,x),NAND(x,y))
        left->left = leftleft;
        left->right = leftright;
        left->op = "NAND";
        left->level = tree->level + 1;
        //build NAND(AND(x,y),NAND(y,y))
        right->left = rightleft;
        right->right = rightright;
        right->op = "NAND";
        right->level = tree->level + 1;
        //build NAND(NAND(AND(x,x),NAND(x,y)),NAND(AND(y,y),NAND(x,y)))
        tree->left = left;
        tree->right = right;

        if (max_depth < A->level){
            max_depth = A->level;
        }
    }
    return tree;
}

node* Decomposition::set_exp(node* tree){
    //we have to traverse back through to update all stored expressions
    if(!tree->op.compare("")){
        return tree;
    }
    set_exp(tree->left);
    set_exp(tree->right);
    tree->exp = tree->op + "(" + tree->left->exp + "," + tree->right->exp + ")";
    return tree;
}

void Decomposition::permute(node* tree){
    queue <node*> Q;
    Q.push(tree);
    int count;
    tree->letter1 = "A";
    tree->letter2 = "*";
    cout << "You entered: " << tree->exp << endl;
    tree = convert(tree);
    cout << "Converted: " << tree->exp << endl;
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
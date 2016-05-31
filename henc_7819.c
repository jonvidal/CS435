//Jonathan. Vidal MP 7819: henc_7819.c

#include <stdio.h>
#include <stdlib.h>

int freq[256]; // store freq of byte
int code[256]; // store encode of byte
int length[256]; // store encode length of byte
int inputlength;

struct HeapNode{ // heap node and huffman tree node
    unsigned char data;
    unsigned freq;
    struct HeapNode *left,*right;
};
typedef struct HeapNode Node;
Node *node[1000];
int heapSize = 0;

Node* createNode(unsigned char d, unsigned f);
void heapify_7819(int x);
Node* extractMin();
int p(int i);
void insert(Node* newNode);
void printHuffmanTree2File_7819(FILE *f,Node *node); // print huffman tree to file
void getEncode_7819(Node *node,int l,int encode); // get encode of data
void huffman_7819(int *freq);
void calFreq(FILE *f,int *freq); // calculated word/binary frequency of the input file

void usage_7819(const char* cmd, const char* errMsg) { //prints error message and shows how to run program
    printf("%s\n", errMsg);
    printf("Usage is: %s filename\n", cmd);
}
void writeInt(int n,FILE *f){
    int i;
    for(i=0;i<4;++i){
        unsigned char c = n%256;
        n/=256;
        putc(c,f);
    }
}


int main(int argc,char **argv){
    int i;
    if(argc!=2){
        usage_7819(argv[0], "hdec filename\n");
        return 1;
    }else if (argc==1){
        usage_7819(argv[0], "too many file name arguments specified.");
        return 1;
    }
    FILE *f = fopen(argv[1],"rb");
    
    if(f==NULL){
        usage_7819(argv[0],"Cant open input file\n");
        return 1;
    }

    
    calFreq(f,freq);
    huffman_7819(freq);
    getEncode_7819(node[0],0,0);
    
    char foutname[500];
    strcpy(foutname,argv[1]);
    strcat(foutname,".huf");
    FILE *fout = fopen(foutname,"wb");
    printHuffmanTree2File_7819(fout,node[0]);

    inputlength = 0;
    for(i=0;i<256;++i){
        inputlength+= freq[i];
    }
    writeInt(inputlength,fout);
    fseek(f,0,SEEK_SET);
    int remain = 0;
    int nbitRemain = 0;

    while(!feof(f)){ // for each byte write it encode to output file
        unsigned char c;
        if(fread(&c,1,1,f)<=0){
            break;
        }
        int k = code[c];
        int l = length[c];
        for(i=0;i<l;++i){
            remain = remain*2 + k %2;
            k/=2;
            nbitRemain++;
            if(nbitRemain==8){
                c = remain;
                fwrite(&c,1,1,fout);
                nbitRemain = 0;
                remain = 0;
            }
        }
    }
    if(nbitRemain > 0){
        remain = remain<<(8-nbitRemain);
        unsigned char c = remain;
        fwrite(&c,1,1,fout);
    }
    fclose(fout);
    return 0;
}

Node* createNode(unsigned char d, unsigned f){ // creare new node with data = d and freq = f
    Node* node = (Node*) malloc(sizeof(Node));
    node->data = d;
    node->freq = f;
    node->left = NULL;
    node->right = NULL;
    return node;
}

void heapify_7819(int x){
    int smallest = x;
    int l = 2 * x + 1; // left node
    int r = 2 * x + 2; // right node

    if (l < heapSize && node[l]->freq < node[smallest]->freq)
        smallest = l;

    if (r < heapSize && node[r]->freq < node[smallest]->freq)
        smallest = r;

    if (smallest != x){
        Node * tmp = node[x];
        node[x] = node[smallest];
        node[smallest] = tmp;
        heapify_7819(smallest);
    }
}


Node* extractMin(){
    Node* tempnode = node[0];
    node[0] = node[heapSize - 1];
    heapSize --;
    heapify_7819(0);
    return tempnode;
}


int p(int i){ //parent of building heap
    return (i-1)/2;
}

void insert(Node* newNode){
    heapSize++;
    int i = heapSize - 1;
    while (i && newNode->freq < node[p(i)]->freq){
        node[i] = node[p(i)];
        i = p(i);
    }
    node[i] = newNode;
}

// print huffman tree to file
void printHuffmanTree2File_7819(FILE *f,Node *node){
    unsigned char one = 1;
    unsigned char zero = 0;
    
    fwrite(&node->data,1,1,f);// write data of node
    if(node->left!=NULL)// if left != NULL print 1 to file else print 0 to file
        fwrite(&one,1,1,f);
    else
        fwrite(&zero,1,1,f);
    
    if(node->right !=NULL)
        fwrite(&one,1,1,f);
    else
        fwrite(&zero,1,1,f);
    
    if(node->left!=NULL)
        printHuffmanTree2File_7819(f,node->left);
    if(node->right!=NULL)
        printHuffmanTree2File_7819(f,node->right);
}


void getEncode_7819(Node *node,int l,int encode){ // get encode of data
    if(node->left == NULL&&node->right==NULL){
        unsigned char cc = node->data;
        length[cc] = l;
        code[cc] = encode;
        }
    if(node->left!=NULL){
        getEncode_7819(node->left,l+1,encode);
    }
    if(node->right!=NULL){
        getEncode_7819(node->right,l+1,encode+(1<<l));
    }
}

/*
 HUFFMAN(C)
 1 n = |C|
 2 Q = C
 3 for i = 1 to n - 1
 4      allocate a new node z
 5      z.left = x = EXTRACT-MIN(Q)
 6      z.right = y = EXTRACT-MIN(Q)
 7      z.freq = x.freq + y.freq
 8      INSERT(Q) ´/
 9 return EXTRACT-MIN(Q)/
 */

void huffman_7819(int *freq){
   heapSize = 0;
   int i;
   for(i=0;i<256;++i){
        insert(createNode(i,freq[i]));
   }
    while (heapSize > 1){
        Node *left = extractMin();
        Node *right = extractMin();

        Node *p = createNode(0, left->freq + right->freq);
        p->left = left;
        p->right = right;
        insert(p);
    }
    //return extractMin();
}

void calFreq(FILE *f,int *freq){ // cal freq of input file
    unsigned char c;
    int i;
    for(i=0;i<256;++i)
        freq[i] = 0;
    int cnt = 0;
    while(!feof(f)){
        if(fread(&c,1,1,f)<=0)
            break;
        cnt++;
        freq[c]++;
    }
}
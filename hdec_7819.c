//Jonathan. Vidal MP 7819: hdec_7819.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
// input length
int inputlength;
// to store heap node
struct HeapNode{
    unsigned char data; // data to this node
    struct HeapNode *left,*right; // left and right huffman tree node
};
typedef struct HeapNode Node;
Node *node[1000];
int heapSize = 0;
// create new node
Node* createNode_7819(){
    Node* node = (Node*) malloc(sizeof(Node));
    node->data = 0;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// read huffman tree from encoded file
Node * readHuffmanTree(FILE *f){
    Node *node = createNode_7819();
    // read data of node
    fread(&node->data,1,1,f);
    unsigned char l,r;
    // read l and r mark.
    // if l = 1 then this node have left node else if l = 0 then this node doesnt have left node
    // same for r
    fread(&l,1,1,f);
    fread(&r,1,1,f);
    if(l==1){
        node->left = readHuffmanTree(f);
    }
    if(r==1)
        node->right = readHuffmanTree(f);
    return node;
}

void usage_7819(const char* cmd, const char* errMsg) { //prints error message and shows how to run program
    printf("%s\n", errMsg);
    printf("Usage is: %s filename\n", cmd);
}
int readInt(FILE *f){
    int n = 0;
    int i;
    for(i=0;i < 4;++i){
        unsigned char c = getc(f);
        n = n*256 + c;
    }
    return n;
}

int main(int argc,char **argv){
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

    
    char buf[100];
    int i;
    for(i=0;i<strlen(argv[1])-4;++i){
        buf[i] = argv[1][i];
    }
    buf[i] = '\0';

    FILE *fout = fopen(buf,"wb");
    if(fout==NULL){
        return -1;
    }
    // read huffman tree
    Node *rootNode = readHuffmanTree(f);
    // read input length
    inputlength = readInt(f);
    Node * p = rootNode;
    while(!feof(f)&&inputlength > 0){
        unsigned char c;
        // read next byte
        if(fread(&c,1,1,f)<=0)
            break;
        int t;
        // for each bit in byte
        for(t = 128;t>0;t/=2){
            if((c&t)!=0)
                p = p->right;
            else
                p = p->left;
            // if current node is leaf print data of this node to output file
            if(p->left==NULL&&p->right==NULL){
                inputlength--;
                fwrite(&p->data,1,1,fout);
                p = rootNode;
            }
        }
    }
    fclose(f);
    fclose(fout);
    
    return 0;
}
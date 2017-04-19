#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sys/stat.h>
#include <vector>
using namespace std;

int GetFileSize(char*); 
int CT[128]={0}; // initialize Count table

/*
 * build Rank Table and Count Table
 */
void CreateTables(char* f, int** RT, FILE* fin, int buffer_size)
{
    int c;
    bool flag = true;
    int push_time=0;
    if(!feof(fin)){
        // first RT[0]
        for(int i=0; i<buffer_size; i++){
            if (c == EOF) {
                flag = false;
                break;
            }
            c = getc(fin);
            RT[0][c]++;
	    CT[c]++;
        }
    }
    
    while(!feof(fin)){
        push_time++;
        for(int i=0; i<buffer_size; i++){
            if (c == EOF) {
                flag = false;
                break;
            }
            c = getc(fin);
            RT[push_time][c]++;
 	    CT[c]++;
        }
        if(!flag){break;}
    }

    int temp = 0; 
    int n = 0;
    for(int i=0; i<128; i++){
        if(CT[i]!=0){
            temp = CT[i];
            CT[i] = n;
            n += temp;
        }
    }
}

/*
 * calculate rank
 * first we look at previous buckets and add them up
 * then we calculate the remain part
 */
int Rank(int n, char c, char* f, int** RT, FILE* fin, int buffer_size)
{
    int result=0;
    int pre = 0;
    for(int i=0; i<n/buffer_size; i++){
        pre += RT[i][(int)c];
    }
    result += pre;
    char cc;
    int pos = n/buffer_size * buffer_size;
    if(n%buffer_size!=0 && n%buffer_size <= buffer_size/2){
        
        fseek(fin,pos,SEEK_SET);
        for(int i=0; i<n%buffer_size; i++){
            cc = getc(fin);
            if((char)cc==c)
                result++;
        }
     }
     else if(n%buffer_size!=0 && n%buffer_size > buffer_size/2){
        pos += n%buffer_size;
	fseek(fin,pos,SEEK_SET);
        for(int i=0; i<buffer_size-(n%buffer_size);i++){
       	    cc = getc(fin);
            if((char)cc==c)
		result--;
        }
        result += RT[n/buffer_size][(int)c];
    }
    return  result;
}

/*
 * backward search through updaing First and Last
 */
int First;
int Last;
int backward_search(char* f, string Pattern, int sz, int** RT, FILE* fin, int bs)
{
    int len = Pattern.length();
    int i = len-1;
    char c = Pattern[i];
    First = CT[(int)c] + 1;
    Last = CT[(int)c] + Rank(sz,c,f,RT,fin,bs);
    
    while(First<=Last && i>=1){
        c = Pattern[i-1];
        First = CT[(int)c] + Rank(First-1, c, f, RT, fin, bs) + 1;
        Last = CT[(int)c] + Rank(Last, c, f, RT, fin, bs);
        i--;
    }
    if(First > Last)
        return -1;  //not existed
    else
        return Last-First+1;
}

/*
 *  for every stuff between First and Last, we search by updating LF function(ceil value)
 *  duplicate flag is used to ovid duplicate searching under same offset
 */
int uniqueRecord(char* f, int sz,string Pattern, bool aFlag, bool rFlag, int** RT, FILE* fin, int bs)
{
    backward_search(f, Pattern, sz, RT, fin, bs);
    if(First>Last){return -1;}
   
    vector<int> offset;
    int ceil;
    int c;
    int n=0;
        
    for(int i=0; i<Last-First+1; i++){
        ceil = First+i;
        fseek(fin,ceil-1, SEEK_SET);
        c = getc(fin);
        bool start = false;
	int k = 0;
	n = 0;
        bool duplicate = false;
        
	while(c!='['){
 	    if(c==']'){
		start = true;
            }
    	    ceil = CT[c] + Rank(ceil, c, f, RT, fin, bs);
            if(ceil>=First && ceil<=Last){
            	duplicate = true;
   		break;
            }
            fseek(fin,ceil-1,SEEK_SET);
            c = getc(fin);
           
    	    if(c!='[' && c!=']' && start){
    		n += int((c-48)*pow(10,k));
		k++;
            }
	}
        if(duplicate) continue;
        offset.push_back(n);
        	
    } 
    
     
    if(rFlag){
        cout<<offset.size()<<endl;
    }
    
    if(aFlag){
        sort(offset.begin(), offset.end());
        
        for(int i=0; i<(int)offset.size(); i++){
            cout<<"["<<offset[i]<<']'<<endl;
        }
    }
    
    return 1;
}

/*
 * count the file size
 */
int GetFileSize(char* f)
{
    int sz = -1;
    struct stat st;
    if(stat(f, &st) < 0){
        return sz;
    }else{
        sz = st.st_size;
    }
    return sz;

}

int main(int argc, char* argv[]) {
    /** build table **/
    FILE* fin;
    fin = fopen(argv[2], "r");
 
    int sz = GetFileSize(argv[2]);
    //cout<<"sz: "<<sz<<endl;  
    	
    int bufferSize = 10000;
    int numBuckets = ceil(float(sz)/bufferSize);
    
    int **RT; 
    RT = (int**) calloc(numBuckets, sizeof(int*));
    for(int i=0; i<numBuckets; i++)
	RT[i] = (int*)calloc(128, sizeof(int));
    
    CreateTables(argv[2], RT, fin, bufferSize);
	
    /** operations **/
    char n_model[2]={'-','n'}; //total#
    char r_model[2]={'-','r'}; //unique#
    char a_model[2]={'-','a'}; //display all uniques

	

    if(argc==5 && argv[1][0]==n_model[0] && argv[1][1]==n_model[1]){
        int n;
        n = backward_search(argv[2], argv[4], sz, RT, fin, bufferSize);
        cout<<n<<endl;	
    }

    bool rFlag=false;
    bool aFlag=false;
    
    if(argc==5 && argv[1][0]==r_model[0] && argv[1][1]==r_model[1]){
	rFlag=true;
        aFlag=false;
  	uniqueRecord(argv[2], sz, argv[4], aFlag, rFlag, RT, fin, bufferSize);
    }

    if(argc==5 && argv[1][0]==a_model[0] && argv[1][1]==a_model[1]){
        rFlag=false;
        aFlag=true;
  	uniqueRecord(argv[2], sz, argv[4], aFlag, rFlag, RT, fin, bufferSize);
    }

    fclose(fin);

    return 0;
}




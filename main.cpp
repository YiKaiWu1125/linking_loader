#include<bits/stdc++.h>
#define endl '\n'
#define $_$ pair<int,int>
using namespace std;

map<string,int>ESTAB;
vector<char>memorycontent;
string getmem(int begin,int end){
    string ss="";
    for(int i=begin;i<end;i++){
        ss=ss+memorycontent[i];
    }
    //cout<<"ss-->"<<ss<<endl;
    return ss;
}
int hextoint(string s){
    int n = 0;
    for(int i=0;i<s.length();i++){
        n *= 16;
        n += (s[i]>='0'&&s[i]<='9')?(s[i]-'0'):((s[i]-'A')+10);
    }
    return n;
}
string inttohex(int n){
    string s = "";
    int k;
    while(n!=0){
        k=n%16;
        if(k<10){
            s=(char)('0'+k)+s;
        }
        else{
            s=(char)('A'+(k-10))+s;
        }
        n=n/16;
    }
    return s;
}
void prin(vector<string> ss){
    for(int i=0;i<ss.size();i++){
        cout<<ss[i]<<endl;
    }
}
string cop(string s,int begin,int end ){
    string rs="";
    for(int i=begin;i<end;i++){
        if(s[i]==32)continue;
        rs=rs+(char)(s[i]);
    }
    return rs;
}
vector<string> readfile(string s){
    cout<<"read file:"<<s<<endl;
    fstream file;
    file.open(s,ios::in);
    if(!file){
        cout<<"file open error\n";
        exit(0);
    }
    string str;
    vector<string> arr;
    while (getline(file , str)){
        arr.push_back(str);
        //cout<<str<<endl;
    } 
    file.close(); 
    //prin(arr);
    return arr;
}
string getNameWithoutSpace(string s){
    string ss="";
    for(int i=0;i<s.length();i++){
        if(s[i]=='\n')break;
        ss = ss + s[i];
    }    
    return ss;
}
string getHex(int value,int bits){
    int n=1;
    for(int i=0;i<bits;i++){
        n=n*2;
    }
    if(value<0){return inttohex(n+value);}
    return inttohex(value);
}
int getDec(string hex,int bits){
    int n=1;
    for(int i=1;i<bits;i++){
        n=n*2;
    }
    //cout<<hextoint(hex)<<" "<<n<<endl;
    if(hextoint(hex)>n){
        return -1*(n*2-hextoint(hex));
    }
    return hextoint(hex);
}

string hexToString(string hexstr,int length){
    int k=hexstr.length();
    for(int i=k;i<length;i++){
        hexstr = "0" + hexstr;
    }
    return hexstr;
}
void processTRecord(string line, int csaddr, int progaddr){
    int addr = hextoint(cop(line,1,7));
    addr += csaddr;
    addr -= progaddr;
    addr *= 2;
    
    int len = hextoint(cop(line,7,9));
    for(int i=0;i<len*2;i++){
        memorycontent[addr] = line[9+i];
        addr += 1;
    }
}
void processMRecord(string line, int csaddr, int progaddr){
    int addr = hextoint(cop(line,1,7));
    addr += csaddr;
    addr -= progaddr;
    addr *= 2;
    
    int l = hextoint(cop(line,7,9));
    
    if (l == 5){
        addr += 1;
    }
        
    string current = getmem(addr,addr+l);
    //cout<<addr<<" ***** "<<addr+l<<endl;
    //cout<<current<<endl;
    int value = getDec(current, l*4);
    string token = cop(line,10,line.length());
    //cout<<"--->"<<token<<endl;
    //cout<<ESTAB[token]<<endl;
    if(line[9] == '+'){
        value += ESTAB[token];
    }
    else{
        value -= ESTAB[token];
    }
    string result = hexToString(getHex(value, l*4), l);
    //cout<<result<<endl;
    for(int i=0;i<l;i++){
        memorycontent[addr] = result[i];
        addr += 1;
    }
}




$_$ pass1(int argc, char *argv[]){
    int PROGADDR = hextoint(argv[1]);
    int CSADDR = PROGADDR;
    int memoryspace = 0;
    vector<string>lines;
    for(int i=2;i<argc;i++){
        lines = readfile(argv[i]);
        //prin(lines);
        string csname = getNameWithoutSpace(cop(lines[0],1,7));
        //cout<<"----->> csname:"<<cop(lines[0],1,7)<<endl;
        int cslength = hextoint(cop(lines[0],13,19));
        memoryspace += cslength;
        ESTAB[csname] = CSADDR;
        for(int i=1;i<lines.size();i++){
            if(lines[i][0]=='D'){// D Record
                int n = int(((lines[i].length())-1)/12);
                //cout<<"n is :"<<lines[i].length()<<endl;
                for(int j=0;j<n;j++){
                    string name =  getNameWithoutSpace(cop(lines[1],1+(12*j),7+(12*j)));
                    int addr = hextoint(cop(lines[i],7+(12*j),13+(12*j)));
                    ESTAB[name] = CSADDR + addr;
                    //cout<<"-->addr:"<<addr<<" "<<inttohex(addr)<<" "<<cop(lines[i],7+(12*j),13+(12*j))<<endl;
                }
            }
        }
        CSADDR += cslength;
    }
    return make_pair(memoryspace,PROGADDR);
}
void pass2(int argc, char *argv[]){
    int PROGADDR = hextoint(argv[1]);
    int CSADDR = PROGADDR;
    vector<string>lines;

    for(int i=2;i<argc;i++){
        lines = readfile(argv[i]);
        // Header Record
        //csname = objreader.getNameWithoutSpace(lines[0][1:7])
        string csname = getNameWithoutSpace(cop(lines[0],1,7));
        //cslength = int("%s" % lines[0][13:19], 16)
        int cslength = hextoint(cop(lines[0],13,19));

        for(int j=1;j<lines.size();j++){
            if(lines[j][0] == 'T'){
                processTRecord(lines[j], CSADDR, PROGADDR);
            }
            if(lines[j][0] == 'M'){
                processMRecord(lines[j], CSADDR, PROGADDR);
            }
        }
        CSADDR += cslength;
    }
}

int main(int argc, char *argv[]){
    //cin.tie(0);
    //ios::sync_with_stdio(false);
    ESTAB.clear();
    $_$ pa=pass1(argc,argv);
    int memoryspace=pa.first;
    int PROGADDR=pa.second;
//pass 1 print------------------------------------------------------------------------
    cout<<"================\n     PASS1\n\nESTAB:\n";
    for(map<string,int>::iterator it=ESTAB.begin();it!=ESTAB.end();it++ ){
        cout<<it->first<<" -> 0x"<<inttohex(it->second)<<endl;
    }
    cout<<"\nmemoryspace:"<<memoryspace;
    cout<<"\n================\n";
//pass 1 print end--------------------------------------------------------------------
    memorycontent.clear();
    memorycontent.resize(2*memoryspace,'.');
    //for(int i=0;i<memorycontent.size();i++)cout<<memorycontent[i];
    //cout<<endl;
    pass2(argc,argv);
//answer print________________________________________________________________________
    for(int i=0;i<2*memoryspace;i++){
        if(i%32 == 0){
            cout<<endl<<(inttohex(PROGADDR+(int)(i/2)))<<" ";
        }
        cout<<memorycontent[i]<<" ";
    }
    cout<<endl;
//answer print end____________________________________________________________________
    return 0;   
}
#include<iostream>
#include<fstream>
using namespace std;

class OSCP{
    private:
        char M[100][4]; //Memory of 100x4
        char IR[4];     //Instruction Register of 4  bytes for storing instructions
        char R[4];      //General purpose Register of 4 bytes
        int IC;         //Instruction Counter Register of 2 bytes;
        bool C;         //Toggle 1 byte
        int SI;         //Interrupt
        char buffer[40];

    //functions
    public:
        void init(); 
        void load();
        void Execute();
        void MOS();
    
        fstream infile;
        fstream outfile;

};

void OSCP::init(){
    //Initialise everything to 0
    for(int i=0;i<100;i++){
        for(int j=0;j<4;j++){
            M[i][j]=0;
        }
    }
    IR[0] = {0};
    R[0] = {0};
    C = false;
}


void OSCP::MOS(){
    cout<<endl<<IR[0]<<IR[1]<<IR[2]<<IR[3];
    if(SI==1){          //For GD therefore READ MODE 
        for(int i=0;i<=39;i++){
            buffer[i]='\0';
        }
        infile.getline(buffer,40);
        int k=0;
        int i=IR[2]-48;
        i=i*10;
        for(int l=0;l<10;l++){
            for(int j=0;j<4;j++){
                M[i][j]=buffer[k];
                k++;
            }
            if(k==40){
                break;
            }
            i++;
        }     

    }
    else if(SI==2)        //For PD therefore WRITE MODE
    {
        for(int i=0;i<=39;i++)
           buffer[i]='\0';
        int k = 0;
            int i = IR[2]-48;
            i = i*10;
        for( int l=0 ; l<10 ;  ++l)
        {
            for(int j = 0 ; j<4; ++j)
            {
                buffer[k]=M[i][j];
                outfile<<buffer[k];

                k++;
            }
            if(k == 40)
            {
                break;
            }
            i++;
        }

        outfile<<"\n";
    }
    else if(SI == 3)        //Terminate
    {

        outfile<<"\n";
        outfile<<"\n";
    }
}

void OSCP::Execute(){
    while(true){
        for(int i=0;i<4;i++){
            IR[i] = M[IC][i];
        }
        IC++;
        if(IR[0] == 'G' && IR[1] == 'D')    //For Get Data
        {
            SI = 1;
            MOS();
        }
        else if(IR[0] == 'P' && IR[1] == 'D')    // For Print Data
        {
            SI = 2;
            MOS();
        }
        else if(IR [0] == 'H')      //For Hault
        {
            SI = 3;
            MOS();
            break;
        }
        else if(IR[0] == 'L' && IR[1] == 'R')       //for load Register
        {
            int i = IR[2]-48;               //here we take input in string for eg LR20 so to convert 20 into integer ie ascii of 2 is 50 - 48(ascii of 0)
            i = i*10 + ( IR[3]-48);     

            for(int j=0;j<=3;j++)           //loading the contents of memory into register(general purpose)
                R[j]=M[i][j];

            //for(int j=0;j<=3;j++)
              // cout<<R[j];

            cout<<endl;
        }
        else if(IR[0] == 'S' && IR[1] == 'R')       //for Store Register 
        {
            int i = IR[2]-48;               //same as above
            i = i*10 +( IR[3]-48) ;
            //cout<<i;
            for(int j=0;j<=3;j++)           //loading from register to memory
                M[i][j]=R[j];

            cout<<endl;
        }
        else if(IR[0] == 'C' && IR[1] == 'R')       //for Compare Register
        {
            int i = IR[2]-48;                   //same as above
            i = i*10 + (IR[3] - 48);
            //cout<<i;
            int count=0;

            for(int j=0;j<=3;j++)       //for comparing contents at given pos with contents present in R
                if(M[i][j] == R[j])
                    count++;

            if(count==4)
                C=true;

            //cout<<C;
        }
        else if(IR[0] == 'B' && IR[1] == 'T')       //for Branch on True
        {
            if(C == true)                       //if comparing is true jump to given location using IC
            {
                int i = IR[2]-48;
                i = i*10 + (IR[3] - 48);

                IC = i;

            }
        }
    }
}
void OSCP::load(){
    //For loading the contents of buffer into memory
    int x=0;
    do{
        for(int i=0;i<=39;i++){   //clearing the buffer
            buffer[i]='\0';
        }

        infile.getline(buffer,40);
        
        //reading from buffer
        if(buffer[0]== '$' && buffer[1]=='A' && buffer[2]=='M' && buffer[3]=='J'){
            init();
        }
        else if(buffer[0]== '$' && buffer[1]=='D' && buffer[2]=='T' && buffer[3]=='A'){
            IC=0;
            Execute();
        }
        else if(buffer[0]== '$' && buffer[1]=='E' && buffer[2]=='N' && buffer[3]=='D'){
            x=0;
            continue;
        }
        else{
            int k=0;

            for( ; x<100 ; ++x){
                for(int j=0;j<4;++j){
                    M[x][j] = buffer[k];
                    k++;
                }

                if(k==40 || buffer[k]== ' ' || buffer[k]=='\n'){
                    break;
                }
            }
            for(int i=0;i<10;i++){    //here 10 bcz of memory is in block of 10
                cout<<"M["<<i<<"] :";
                for(int j=0;j<4;j++){
                    cout<<M[i][j];
                }
                cout<<endl;
            }
        }

    }
    while(!infile.eof());   //Till end of the file    
}

int main(){
    OSCP os;
    os.infile.open("input.txt", ios::in);
    os.outfile.open("output.txt", ios::out);

    if(!os.infile)
    {
        cout<<"Failure"<<endl;
    }
    else
    {
        cout<<"File Exist"<<endl;
    }

    os.load();
    return 0;

}
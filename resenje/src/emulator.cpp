#include "../inc/emulator.hpp"

Emulator::Emulator(string ulazniFajl){
  this->ulazniFajl=ulazniFajl;
  memorija=nullptr;
}

int Emulator::inicijalizujMemorijuIRegistre(){
  //inicijalizavija registra;
  for(int i=0;i<16;i++){
    if(i<3){
      creg[i]=0;
    }
    if(i==15){
      reg[i]=0x40000000;
    }else{
      reg[i]=0;
    }
  }
  size_t length=1UL<<32;
  //inicijalizacija i punjenje memorije
  //mozda treba i fleg MAP_NORESERVE
  memorija=(unsigned char*)mmap(nullptr,length,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_NORESERVE,-1,0);
  if(memorija==MAP_FAILED){
    cout<<"EMULATOR: GRESKA, NEMOGUCE JE INICIJALIZOVATI MEMORIJU"<<endl;
    return -1;
  }
  ifstream ulaz;
  ulaz.open(ulazniFajl);
  if(ulaz.fail()){
    cout<<"EMULATOR: GRESKA, FAJL NE POSTOJI"<<endl;
    return -1;
  }
  string linija;
  vector<string> reci;
  while(getline(ulaz,linija)){
    parsirajLiniju(linija,reci);
    string s=reci.front();
    s.pop_back();//skida znak : sa kraja adrese
    unsigned int addr=(unsigned int)stol(s,nullptr,16);
    //krecemo od reci.begin()+1, jer je na nultom mestu adresa:
    for(auto bajt=reci.begin()+1;bajt!=reci.end();bajt++){
      memorija[addr]=(unsigned char)stoi((*bajt),nullptr,16);
      addr++;
    }
  }
  return 0;
}

void Emulator::izvrsi(){
  bool end=false;
  while(!end){
    //citamo instrukicju sa vrednosti PC i obradjujemo
    unsigned char c1=memorija[(unsigned int)reg[PC]];
    unsigned char c2=memorija[(unsigned int)(reg[PC]+1)];
    unsigned char c3=memorija[(unsigned int)(reg[PC]+2)];
    unsigned char c4=memorija[(unsigned int)(reg[PC]+3)];
    reg[PC]+=4;
    unsigned char opC=(c1>>4)&0x0F;
    unsigned char mod=c1&0x0F;
    unsigned char a=(c2>>4)&0x0F;
    unsigned char b=c2&0x0F;
    unsigned char c=(c3>>4)&0x0F;
    int displ;
    unsigned char d1=(c3>>3)&0x01;//da vidimo da li je bit najvise tezine 1, ako jeste->neg br, ako nije->poz br
    if(d1==1){
      //negativan broj, moramo prosiriti F
      displ=0xFFFFF000|((c3&0x0F)<<8)|c4;
    }
    else{
      //pozitivan broj, prosirujemo sa 0
      displ=(((c3&0x0F)<<8)|c4)&0x00000FFF;
    }
    switch (opC)
    {
    case 0:{
      //halt
      end=true;
      break;
    }
    case 1:{
      //int
      reg[SP]-=4;
      memorija[((unsigned int)(reg[SP]))]=(unsigned char)(creg[0]&0xFF);
      memorija[((unsigned int)(reg[SP]+1))]=(unsigned char)((creg[0]>>8)&0xFF);
      memorija[((unsigned int)(reg[SP]+2))]=(unsigned char)((creg[0]>>16)&0xFF);
      memorija[((unsigned int)(reg[SP]+3))]=(unsigned char)((creg[0]>>24)&0xFF);
      reg[SP]-=4;
      memorija[((unsigned int)(reg[SP]))]=(unsigned char)(reg[PC]&0xFF);
      memorija[((unsigned int)(reg[SP]+1))]=(unsigned char)((reg[PC]>>8)&0xFF);
      memorija[((unsigned int)(reg[SP]+2))]=(unsigned char)((reg[PC]>>16)&0xFF);
      memorija[((unsigned int)(reg[SP]+3))]=(unsigned char)((reg[PC]>>24)&0xFF);
      creg[2]=4;
      creg[0]=creg[0]&(~0x1);
      reg[PC]=creg[1];
      break;
    }
    case 2:{
      //call
      switch (mod)
      {
      case 0:{
        reg[SP]-=4;
        memorija[((unsigned int)(reg[SP]))]=(unsigned char)(reg[PC]&0xFF);
        memorija[((unsigned int)(reg[SP]+1))]=(unsigned char)((reg[PC]>>8)&0xFF);
        memorija[((unsigned int)(reg[SP]+2))]=(unsigned char)((reg[PC]>>16)&0xFF);
        memorija[((unsigned int)(reg[SP]+3))]=(unsigned char)((reg[PC]>>24)&0xFF);
        reg[PC]=reg[a]+reg[b]+displ;
        break;
      }
      case 1:{
        reg[SP]-=4;
        memorija[((unsigned int)(reg[SP]))]=(unsigned char)(reg[PC]&0xFF);
        memorija[((unsigned int)(reg[SP]+1))]=(unsigned char)((reg[PC]>>8)&0xFF);
        memorija[((unsigned int)(reg[SP]+2))]=(unsigned char)((reg[PC]>>16)&0xFF);
        memorija[((unsigned int)(reg[SP]+3))]=(unsigned char)((reg[PC]>>24)&0xFF);
        unsigned int lok=(unsigned int)(reg[a]+reg[b]+displ);
        unsigned char c11=memorija[lok];
        lok++;
        unsigned char c22=memorija[lok];
        lok++;
        unsigned char c33=memorija[lok];
        lok++;
        unsigned char c44=memorija[lok];
        reg[PC]=(c44<<24)|(c33<<16)|(c22<<8)|c11;
        break;
      }
      default:{
        neispravnaInstrukcija();
        break;
      }
      }
      break;
    }
    case 3:{
      switch (mod)
      {
      case 0:{
        reg[PC]=reg[a]+displ;
        break;
      }
      case 1:{
        if(reg[b]==reg[c]){
          reg[PC]=reg[a]+displ;
        }
        break;
      }
      case 2:{
        if(reg[b]!=reg[c]){
          reg[PC]=reg[a]+displ;
        }
        break;
      }
      case 3:{
        if(reg[b]>reg[c]){
          reg[PC]=reg[a]+displ;
        }
        break;
      }
      case 8:{
        unsigned int lok=(unsigned int)(reg[a]+displ);
        unsigned char c11=memorija[lok];
        lok++;
        unsigned char c22=memorija[lok];
        lok++;
        unsigned char c33=memorija[lok];
        lok++;
        unsigned char c44=memorija[lok];
        reg[PC]=(c44<<24)|(c33<<16)|(c22<<8)|c11;
        break;
      }
      case 9:{
        if(reg[b]==reg[c]){
          unsigned int lok=(unsigned int)(reg[a]+displ);
          unsigned char c11=memorija[lok];
          lok++;
          unsigned char c22=memorija[lok];
          lok++;
          unsigned char c33=memorija[lok];
          lok++;
          unsigned char c44=memorija[lok];
          reg[PC]=(c44<<24)|(c33<<16)|(c22<<8)|c11;
        }
        break;
      }
      case 10:{
        if(reg[b]!=reg[c]){
          unsigned int lok=(unsigned int)(reg[a]+displ);
          unsigned char c11=memorija[lok];
          lok++;
          unsigned char c22=memorija[lok];
          lok++;
          unsigned char c33=memorija[lok];
          lok++;
          unsigned char c44=memorija[lok];
          reg[PC]=(c44<<24)|(c33<<16)|(c22<<8)|c11;
        }
        break;
      }
      case 11:{
        if(reg[b]>reg[c]){
          unsigned int lok=(unsigned int)(reg[a]+displ);
          unsigned char c11=memorija[lok];
          lok++;
          unsigned char c22=memorija[lok];
          lok++;
          unsigned char c33=memorija[lok];
          lok++;
          unsigned char c44=memorija[lok];
          reg[PC]=(c44<<24)|(c33<<16)|(c22<<8)|c11;
        }
        break;
      }
      default:{
        neispravnaInstrukcija();
        break;
      }
      }
      break;
    }
    case 4:{
      int temp=reg[b];
      reg[b]=reg[c];
      reg[c]=temp;
      break;
    }
    case 5:{
      switch (mod)
      {
      case 0:{
        reg[a]=reg[b]+reg[c];
        break;
      }
      case 1:{
        reg[a]=reg[b]-reg[c];
        break;
      }
      case 2:{
        reg[a]=reg[b]*reg[c];
        break;
      }
      case 3:{
        double f=reg[b]/reg[c];
        reg[a]=(int)(reg[b]/reg[c]);
        break;
      }
      default:{
        neispravnaInstrukcija();
        break;
      }
      }
      break;
    }
    case 6:{
      switch (mod)
      {
      case 0:{
        reg[a]=~reg[b];
        break;
      }
      case 1:{
        reg[a]=reg[b]&reg[c];
        break;
      }
      case 2:{
        reg[a]=reg[b]|reg[c];
        break;
      }
      case 3:{
        reg[a]=reg[b]^reg[c];
        break;
      }
      default:{
        neispravnaInstrukcija();
        break;
      }
      }
      break;
    }
    case 7:{
      switch (mod)
      {
      case 0:{
        reg[a]=reg[b]<<reg[c];
        break;
      }
      case 1:{
        reg[a]=reg[b]>>reg[c];
        break;
      }
      default:{
        neispravnaInstrukcija();
        break;
      }
      }
      break;
    }
    case 8:{
      switch (mod)
      {
      case 0:{
        unsigned int lok=(unsigned int)(reg[a]+reg[b]+displ);
        memorija[lok]=(unsigned char)(reg[c]&0xFF);
        lok++;
        memorija[lok]=(unsigned char)((reg[c]>>8)&0xFF);
        lok++;
        memorija[lok]=(unsigned char)((reg[c]>>16)&0xFF);
        lok++;
        memorija[lok]=(unsigned char)((reg[c]>>24)&0xFF);
        break;
      }
      case 2:{
        unsigned int lok=(unsigned int)(reg[a]+reg[b]+displ);
        unsigned char c11=memorija[lok];
        lok++;
        unsigned char c22=memorija[lok];
        lok++;
        unsigned char c33=memorija[lok];
        lok++;
        unsigned char c44=memorija[lok];
        lok=(unsigned int)((c44<<24)|(c33<<16)|(c22<<8)|c11);
        memorija[lok]=(unsigned char)(reg[c]&0xFF);
        lok++;
        memorija[lok]=(unsigned char)((reg[c]>>8)&0xFF);
        lok++;
        memorija[lok]=(unsigned char)((reg[c]>>16)&0xFF);
        lok++;
        memorija[lok]=(unsigned char)((reg[c]>>24)&0xFF);
        break;
      }
      case 1:{
        reg[a]=reg[a]+displ;
        unsigned int lok=(unsigned int)(reg[a]);
        memorija[lok]=(unsigned char)(reg[c]&0xFF);
        lok++;
        memorija[lok]=(unsigned char)((reg[c]>>8)&0xFF);
        lok++;
        memorija[lok]=(unsigned char)((reg[c]>>16)&0xFF);
        lok++;
        memorija[lok]=(unsigned char)((reg[c]>>24)&0xFF);
        break;
      }
      default:{
        neispravnaInstrukcija();
        break;
      }
      }
      break;
    }
    case 9:{
      switch (mod)
      {
      case 0:{
        reg[a]=creg[b];
        break;
      }
      case 1:{
        reg[a]=reg[b]+displ;
        break;
      }
      case 2:{
        unsigned int lok=(unsigned int)(reg[b]+reg[c]+displ);
        unsigned char c11=memorija[lok];
        lok++;
        unsigned char c22=memorija[lok];
        lok++;
        unsigned char c33=memorija[lok];
        lok++;
        unsigned char c44=memorija[lok];
        reg[a]=(c44<<24)|(c33<<16)|(c22<<8)|c11;
        break;
      }
      case 3:{
        unsigned int lok=(unsigned int)(reg[b]);
        unsigned char c11=memorija[lok];
        lok++;
        unsigned char c22=memorija[lok];
        lok++;
        unsigned char c33=memorija[lok];
        lok++;
        unsigned char c44=memorija[lok];
        reg[a]=(c44<<24)|(c33<<16)|(c22<<8)|c11;
        reg[b]=reg[b]+displ;
        break;
      }
      case 4:{
        creg[a]=reg[b];
        break;
      }
      case 5:{
        creg[a]=creg[b]|displ;
        break;
      }
      case 6:{
        unsigned int lok=(unsigned int)(reg[b]+reg[c]+displ);
        unsigned char c11=memorija[lok];
        lok++;
        unsigned char c22=memorija[lok];
        lok++;
        unsigned char c33=memorija[lok];
        lok++;
        unsigned char c44=memorija[lok];
        creg[a]=(c44<<24)|(c33<<16)|(c22<<8)|c11;
        break;
      }
      case 7:{
        unsigned int lok=(unsigned int)(reg[b]);
        unsigned char c11=memorija[lok];
        lok++;
        unsigned char c22=memorija[lok];
        lok++;
        unsigned char c33=memorija[lok];
        lok++;
        unsigned char c44=memorija[lok];
        creg[a]=(c44<<24)|(c33<<16)|(c22<<8)|c11;
        reg[b]=reg[b]+displ;
        break;
      }
      default:{
        neispravnaInstrukcija();
        break;
      }
      }
      break;
    }
    default:{
      neispravnaInstrukcija();
      break;
    }
    }
    //na kraju svaki instrukcije samo r0 moramo vratiti na 0
    reg[0]=0;
  }
}

void Emulator::neispravnaInstrukcija(){
  //treba da se generise softverski prekid sa statusom 1
  reg[SP]-=4;
  memorija[((unsigned int)(reg[SP]))]=(unsigned char)(creg[0]&0xFF);
  memorija[((unsigned int)(reg[SP]+1))]=(unsigned char)((creg[0]>>8)&0xFF);
  memorija[((unsigned int)(reg[SP]+2))]=(unsigned char)((creg[0]>>16)&0xFF);
  memorija[((unsigned int)(reg[SP]+3))]=(unsigned char)((creg[0]>>24)&0xFF);
  reg[SP]-=4;
  memorija[((unsigned int)(reg[SP]))]=(unsigned char)(reg[PC]&0xFF);
  memorija[((unsigned int)(reg[SP]+1))]=(unsigned char)((reg[PC]>>8)&0xFF);
  memorija[((unsigned int)(reg[SP]+2))]=(unsigned char)((reg[PC]>>16)&0xFF);
  memorija[((unsigned int)(reg[SP]+3))]=(unsigned char)((reg[PC]>>24)&0xFF);
  creg[2]=1;
  creg[0]=creg[0]&(~0x1);
  reg[PC]=creg[1];
}

void Emulator::ispisiUlaz(){
  unsigned int i=0xF0000118;
  printf("%02X",memorija[i]);
  printf("\n");
  i++;
  printf("%02X",memorija[i]);
  printf("\n");
}

void Emulator::parsirajLiniju(string linija,vector<string>& parsirano){
    parsirano.clear();
    stringstream neprasirana(linija);
    string rec;
    while (neprasirana >> rec) {
      parsirano.push_back(rec);
    }
}

void Emulator::ispisiStanjeRegistara(){
  cout<<"--------------------------------------------------"<<endl;
  cout<<"Emulated processor executed halt instruction"<<endl;
  cout<<"Emulated processor state:"<<endl;
  for(int i=0;i<16;i+=4){
    cout<<"r"<<i<<"=0x";
    printf("%08x",reg[i]);
    printf("\t");
    cout<<"r"<<i+1<<"=0x";
    printf("%08x",reg[i+1]);
    printf("\t");
    cout<<"r"<<i+2<<"=0x";
    printf("%08x",reg[i+2]);
    printf("\t");
    cout<<"r"<<i+3<<"=0x";
    printf("%08x",reg[i+3]);
    printf("\n");
  }
}

int Emulator:: pokreniEmulaciju(){
  int i=inicijalizujMemorijuIRegistre();
  if(i==-1){
    return -1;
  }
  izvrsi();
  ispisiStanjeRegistara();
  return 0;
}

Emulator::~Emulator(){
  size_t length=1UL<<32;
  munmap(memorija,length);
}
#include "../inc/tabelaSekcija.hpp"

Sekcija::Sekcija(){
  tabelaLiterala=new TabelaLiterala();
  tabelaRealokacija=new TabelaRealokacija();
}
void TabelaSekcija::dodajSekciju(Sekcija* sekcija){
  tabelaSekcija.push_back(sekcija);
}

void TabelaSekcija::zatvoriSekciju(int index, int length){
  for(auto sek=tabelaSekcija.begin(); sek!=tabelaSekcija.end();sek++){
    if((*sek)->index==index){
      (*sek)->length=length;
    }
  }
}

bool TabelaSekcija::postojiSekcija(char * nazivSekcije){
  for(auto sek=tabelaSekcija.begin(); sek!=tabelaSekcija.end();sek++){
    if(strcmp((*sek)->naziv,nazivSekcije)==0){
      return true;
    }
  }
  return false;
}

vector<Sekcija*>& TabelaSekcija::dohvatiTabeluSekcija(){
  return tabelaSekcija;
}

Sekcija* TabelaSekcija::dohvatiSekciju(int index){
  for(auto sek=tabelaSekcija.begin(); sek!=tabelaSekcija.end();sek++){
    if((*sek)->index==index){
      return (*sek);
    }
  }
  return NULL;
}

void TabelaSekcija::popuniLokazijeUBazenuLiterala(){
  for(auto sek=tabelaSekcija.begin(); sek!=tabelaSekcija.end();sek++){
    (*sek)->tabelaLiterala->postaviLokacijeSimbolima((*sek)->length);
  }
}

void TabelaSekcija::ispisiTabeluSekcija(){
  printf("TABELA SEKCIJA\n");
  printf("INDEX");
  printf("\t\t");
  printf("NAZIV");
  printf("\t\t");
  printf("BASE");
  printf("\t\t");
  printf("LBZ");
  printf("\t\t");
  printf("LENGTH\n");
  printf("=======================================================\n");
  for(auto sek=tabelaSekcija.begin(); sek!=tabelaSekcija.end();sek++){
    printf("%d",(*sek)->index);
    printf("\t\t");
    printf("%s",(*sek)->naziv);
    printf("\t\t");
    printf("%08X",(*sek)->base);
    printf("\t\t");
    printf("%08X",(*sek)->lengthBZ);
    printf("\t\t");
    printf("%08X",(*sek)->length);
    printf("\n");
  }
}

void TabelaSekcija::ispisiTabeleLiterala(){
  for(auto sek=tabelaSekcija.begin(); sek!=tabelaSekcija.end();sek++){
    printf("#");
    printf("tabela literala: ");
    printf("%s",(*sek)->naziv);
    printf("\n");
    (*sek)->tabelaLiterala->ispisiTabeluLiterala();
  }
}

void Sekcija::ispisiMasinskiKod(){
  int i=1;
  int progCounter=0;
  printf("%08X",progCounter);
  printf(": ");
  for(auto bajt=masinskiKod.begin(); bajt!=masinskiKod.end();bajt++){
    printf("%02X",(*bajt));
    if(i%4!=0){
      printf(" ");
    }
    else{
      printf("\n");
      progCounter+=4;
      printf("%08X",progCounter);
      printf(": ");
    }
    i++;
  }
  printf("\n");
}

void TabelaSekcija::ispisiTabeleRealokacija(){
  for(auto sek=tabelaSekcija.begin(); sek!=tabelaSekcija.end();sek++){
    printf("#");
    printf(".rel.");
    printf("%s",(*sek)->naziv);
    printf("\n");
    printf("SIMBOL");
    printf("\t\t");
    printf("SEKCIJA");
    printf("\t\t");
    printf("OFFSET");
    printf("\t\t");
    printf("ADDEND\n");
    printf("=======================================================\n");
    (*sek)->tabelaRealokacija->ispisiTabeluRealokacija();
  }
}

void TabelaSekcija::ispisiMasinskeKodove(){
  for(auto sek=tabelaSekcija.begin(); sek!=tabelaSekcija.end();sek++){
    printf("#");
    printf("%s",(*sek)->naziv);
    printf("\n");
    (*sek)->ispisiMasinskiKod();
  }
}

void Sekcija::kreirajMKodIBazenaLit(){
  int size=tabelaLiterala->dohvatiBrojLiterala();
  for(int i=0;i<size;i++){
    //dodaje se samo na kraj sekcije, tako da ne treba da sepcificramo tacnu lokaciju
    //samo pushujemo na kraj masinskog koda
    Literal* lit=tabelaLiterala->dohvatiLiteral(i);
    if(lit->literal==false){
      //radi se o simbolu->moramo kreirati relokacioni zapis
      //u masinski kod mozemo ugraditi 0, i kreirati rel zapis
      unsigned char c1=0x00;
      for(int j=0;j<4;j++){
        masinskiKod.push_back(c1);
      }
    }
    else{
      unsigned char c1=(unsigned char)(lit->vrednost&0xFF);
      unsigned char c2=(unsigned char)((lit->vrednost>>8)&0xFF);
      unsigned char c3=(unsigned char)((lit->vrednost>>16)&0xFF);
      unsigned char c4=(unsigned char)((lit->vrednost>>24)&0xFF);
      masinskiKod.push_back(c1);
      masinskiKod.push_back(c2);
      masinskiKod.push_back(c3);
      masinskiKod.push_back(c4);
    }
  }
}

void TabelaSekcija::popuniVelicinuBazenaLiterala(){
  for(auto sek=tabelaSekcija.begin(); sek!=tabelaSekcija.end();sek++){
    (*sek)->lengthBZ=(*sek)->masinskiKod.size()-(*sek)->length;
  }
}

int TabelaSekcija::dohvBrojSekcija(){
  return tabelaSekcija.size();
}

Sekcija::~Sekcija(){
  delete tabelaLiterala;
  delete tabelaRealokacija;
}

TabelaSekcija::~TabelaSekcija(){
  for(auto sek=tabelaSekcija.begin(); sek!=tabelaSekcija.end();sek++){
    delete (*sek);
  }
}
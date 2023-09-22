#include "../inc/tabelaLiterala.hpp"
//TABELA LITERALA PREDSTAVLJA USTVARI BAZEN LITERALA
void TabelaLiterala::dodajKonstantu(Literal* lit){
  tabelaLiterala.push_back(lit);
}

void TabelaLiterala::postaviLokaciju(int vrednost, int lokacija){
  for(auto lit=tabelaLiterala.begin(); lit!=tabelaLiterala.end();lit++){
    if((*lit)->vrednost==vrednost){
      (*lit)->lokacija=lokacija;
    }
  }
}

void TabelaLiterala::postaviLokaciju(char* simbol, int lokacija){
  for(auto lit=tabelaLiterala.begin(); lit!=tabelaLiterala.end();lit++){
    if((*lit)->literal==false&&strcmp((*lit)->simbol,simbol)==0){
      (*lit)->lokacija=lokacija;
    }
  }
}

bool TabelaLiterala::postojiVrednost(int vrednost){
  for(auto lit=tabelaLiterala.begin(); lit!=tabelaLiterala.end();lit++){
    if((*lit)->literal==true&&(*lit)->vrednost==vrednost){
      return true;
    }
  }
  return false;
}

bool TabelaLiterala::postojiLabela(char * simbol){
  for(auto lit=tabelaLiterala.begin(); lit!=tabelaLiterala.end();lit++){
    if((*lit)->literal==false&&strcmp((*lit)->simbol,simbol)==0){
      return true;
    }
  }
  return false;
}

int TabelaLiterala::dohvatiLokaciju(int vrednost){
  for(auto lit=tabelaLiterala.begin(); lit!=tabelaLiterala.end();lit++){
    if((*lit)->literal==true&&(*lit)->vrednost==vrednost){
      return (*lit)->lokacija;
    }
  }
  //ukoliko nema povratna vrednost je -1
  return -1;
}

int TabelaLiterala::dohvatiLokaciju(char* simbol){
  for(auto lit=tabelaLiterala.begin(); lit!=tabelaLiterala.end();lit++){
    if((*lit)->literal==false&&strcmp((*lit)->simbol,simbol)==0){
      return (*lit)->lokacija;
    }
  }
  //ukoliko nema povratna vrednost je -1
  return -1;
}

void TabelaLiterala::postaviLokacijeSimbolima(int krajnjaAdrSekcije){
  //nakon prvog prolaza kada znamo velicinu sekcije mozemo popuniti lokacije literalima
  //bazen literala se alocira odmah nakon masinskog koda za sekciju
  int pocAdr=krajnjaAdrSekcije;
  for(auto lit=tabelaLiterala.begin(); lit!=tabelaLiterala.end();lit++){
    (*lit)->lokacija=pocAdr;
    pocAdr+=4;//svaki literal ce zauzimati 4B u bazenu literala;
  }
}

int TabelaLiterala::dohvatiBrojLiterala(){
  return tabelaLiterala.size();
}

Literal*::TabelaLiterala::dohvatiLiteral(int index){
  return tabelaLiterala[index];
}

void TabelaLiterala::ispisiTabeluLiterala(){
  printf("VREDNOST");
  printf("\t\t");
  printf("LOKACIJA\n");
  printf("=======================================================\n");
  for(auto lit=tabelaLiterala.begin(); lit!=tabelaLiterala.end();lit++){
    if((*lit)->literal){
      printf("%d",(*lit)->vrednost);
      printf("\t\t");
    }else{
      printf("%s",(*lit)->simbol);
      printf("\t\t");  
    }
    printf("%d",(*lit)->lokacija);
    printf("\n");
  }
}
TabelaLiterala::~TabelaLiterala(){
  for(auto lit=tabelaLiterala.begin(); lit!=tabelaLiterala.end();lit++){
    delete (*lit);
  }
}
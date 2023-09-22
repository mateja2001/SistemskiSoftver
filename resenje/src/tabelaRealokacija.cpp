#include "../inc/tabelaRealokacija.hpp"

void TabelaRealokacija::dodajZapis(RelZapis * relZ){
  tabelaRealokacija.push_back(relZ);
}

TabelaRealokacija::~TabelaRealokacija(){
  for(auto rz=tabelaRealokacija.begin(); rz!=tabelaRealokacija.end();rz++){
    delete (*rz);
  }
}

void TabelaRealokacija::ispisiTabeluRealokacija(){
  for(auto rz=tabelaRealokacija.begin(); rz!=tabelaRealokacija.end();rz++){
    printf("%d",(*rz)->simbol);
    printf("\t\t");
    printf("%d",(*rz)->sekcija);
    printf("\t\t");
    printf("%d",(*rz)->offset);
    printf("\t\t");
    printf("%d",(*rz)->addend);
    printf("\n");
  }
}

int TabelaRealokacija::dohvBrojRelZapisa(){
  return tabelaRealokacija.size();
}

RelZapis* TabelaRealokacija::dohvRelZapis(int index){
  return tabelaRealokacija[index];
}
vector<RelZapis*>& TabelaRealokacija:: dohvatiTabeluRealokacija(){
  return tabelaRealokacija;
}
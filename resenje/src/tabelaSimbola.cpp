#include "../inc/tabelaSimbola.hpp"

void TabelaSimbola::dodajSimbol(Simbol* simbol){
  tabelaSim.push_back(simbol);
}

bool TabelaSimbola::proveriNedefinisaneSimbole(){
  for(auto sim=tabelaSim.begin(); sim!=tabelaSim.end();sim++){
    if((*sim)->sekcija==-1&&(*sim)->externDef==false){
      return true;
    }
  }
  return false;
}

bool TabelaSimbola::globalExternSimbol(){
  for(auto sim=tabelaSim.begin(); sim!=tabelaSim.end();sim++){
    if((*sim)->globalDef==true&&(*sim)->externDef==true){
      return true;
    }
  }
  return false;
}

vector<Simbol *> &TabelaSimbola::dohvTabeluSimbola()
{
  return tabelaSim;
}

Simbol* TabelaSimbola::dohvatiSimbol(char* nazivSimbola){
  for(auto sim=tabelaSim.begin(); sim!=tabelaSim.end();sim++){
    if(strcmp((*sim)->naziv,nazivSimbola)==0){
      return *sim;
    }
  }
  return NULL;
}

void TabelaSimbola::ispisiTabelu(){
  printf("TABELA SIMBOLA\n");
  printf("INDEX");
  printf("\t\t");
  printf("NAZIV");
  printf("\t\t");
  printf("VREDNOST");
  printf("\t\t");
  printf("TIP");
  printf("\t\t");
  printf("POVEZIVANJE");
  printf("\t\t");
  printf("SEKCIJA");
  printf("\t\t");
  printf("GLOBALDEF");
  printf("\t\t");
  printf("EXTERNDEF\n");
  printf("=======================================================\n");
  for(auto sim=tabelaSim.begin(); sim!=tabelaSim.end();sim++){
    printf("%d",(*sim)->index);
    printf("\t\t");
    printf("%s",(*sim)->naziv);
    printf("\t\t");
    printf("%08X",(*sim)->vrednost);
    printf("\t\t");
    printf("%d",(*sim)->tip);
    printf("\t\t");
    printf("%d",(*sim)->povezivanje);
    printf("\t\t");
    printf("%d",(*sim)->sekcija);
    printf("\t\t");
    printf("%d",(*sim)->globalDef);
    printf("\t\t");
    printf("%d",(*sim)->externDef);
    printf("\n");
  }
}

int TabelaSimbola::dohvBrojSimbola(){
  return tabelaSim.size();
}

Simbol* TabelaSimbola::dohvatiSimbol(int index){
  return tabelaSim[index];
}

TabelaSimbola::~TabelaSimbola(){
  for(auto sim=tabelaSim.begin(); sim!=tabelaSim.end();sim++){
    delete (*sim);
  }
}
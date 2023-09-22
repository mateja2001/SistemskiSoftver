#ifndef TABELASIMBOLA_HPP
#define TABELASIMBOLA_HPP
#include <stdio.h>
#include <vector>
#include <cstring>
using namespace std;

enum Tip{NOTYP,SCTN};

enum Povezivanje{GLOBAL,LOCAL};

class Simbol{
  public:
    unsigned int vrednost;// predstavlja ustari offset od pocetka sekcije u kojoj se on nalazi
    Tip tip;
    bool globalDef; //pokazuje da li je simbol definisan u global direktivi
    bool externDef; //pokazuje da li je simbol definisan u extern direktivi
    Povezivanje povezivanje;
    char* naziv;
    int sekcija;// -1 oznacava da je UND inace oznacava index sekcije
    int index;
};

class TabelaSimbola{
  vector<Simbol*> tabelaSim;
  public:
  void dodajSimbol(Simbol* simbol);
  vector<Simbol*>& dohvTabeluSimbola();
  Simbol* dohvatiSimbol(char* nazivSimbola);
  Simbol* dohvatiSimbol(int index);
  void ispisiTabelu();
  int dohvBrojSimbola();
  bool proveriNedefinisaneSimbole();//vraca true ako postoji simbol koji nije definisan(a nije extern)
  bool globalExternSimbol();//vraca true ako postoji simbol koji je deklarisan i kao global i kao extern
  ~TabelaSimbola();
};
#endif
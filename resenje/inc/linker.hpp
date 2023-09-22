#ifndef LINKER_HPP
#define LINKER_HPP
#include "linkerFajl.hpp"

class PlaceSekcije{
  public:
  string imeSekcije;
  unsigned int mesto;
};

class Linker
{
  private:
  unsigned int counter;//govori koji je naredno slobodno mesto za smestanje sekcija
  int smestiSekcije();//vrsi mapiranje sekcije
  bool obradjenaSekcija(string nazivSekcije, string nazivFajla);
  void prepraviSimbole();//za svaki simbol se upisuje tacna vrednost na koju ce biti smestena
  void resiRelokacioneZapise();//resava realokacione zapise
  void prepraviMKod(unsigned int vrednost, unsigned int offset,vector<unsigned char>&mKod);
  void ispisiHexFajl();//na standardnom izlazu ispisuje
  void ispisiHexFajlOutput();
  public:
  vector<LinkerFajl*>ulazniFajlovi;
  vector<PlaceSekcije*> placeSekcije;
  int proveriSimbole();
  TabelaSekcija* novaTabelaSekcija;
  string nazivIzlazongFajla;
  int linkujFajlove();
  Linker(vector<LinkerFajl*>&fajlovi,vector<PlaceSekcije*>&psekcije,string nazivIzlaza);
  ~Linker();
};


#endif 
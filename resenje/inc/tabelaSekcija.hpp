#ifndef TABELASEKCIJA_HPP
#define TABELASEKCIJA_HPP
#include <stdio.h>
#include <vector>
#include <cstring>
#include <string>
#include "tabelaLiterala.hpp"
#include "tabelaRealokacija.hpp"
using namespace std;

class Sekcija{
  public:
  char* naziv;
  unsigned int base;
  unsigned int length;
  int index; // INDEX SEKCIJE U TABELI SEKCIJA I TABELI SIMBOLA SE RAZLIKUJU, tablea simb se referise na tabelu sekcija
  unsigned int lengthBZ;
  string nazivFajla;//koristi se kod linkera
  TabelaLiterala* tabelaLiterala;
  TabelaRealokacija* tabelaRealokacija;
  vector<unsigned char> masinskiKod;//svaki cvor liste ima po 1B
  void kreirajMKodIBazenaLit();
  void ispisiMasinskiKod();
  Sekcija();
  ~Sekcija();
};

class TabelaSekcija
{
  vector<Sekcija*> tabelaSekcija;
  public:
  void dodajSekciju(Sekcija * sek);
  void zatvoriSekciju(int index, int length);
  Sekcija* dohvatiSekciju(int index);
  bool postojiSekcija(char * nazivSekcije);
  vector<Sekcija*>& dohvatiTabeluSekcija();
  void ispisiTabeluSekcija();
  void ispisiTabeleLiterala();
  void ispisiMasinskeKodove();
  void ispisiTabeleRealokacija();
  void popuniLokazijeUBazenuLiterala();
  void popuniVelicinuBazenaLiterala();
  int dohvBrojSekcija();
  ~TabelaSekcija();
};

#endif
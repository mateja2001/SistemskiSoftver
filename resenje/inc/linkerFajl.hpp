#ifndef LINKERFAJL_HPP
#define LINKERFAJL_HPP
#include "tabelaRealokacija.hpp"
#include "tabelaSimbola.hpp"
#include "tabelaSekcija.hpp"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;
class LinkerFajl
{
private:
  TabelaSimbola *tabelaSimbola;
  TabelaSekcija* tabelaSekcija;
  string nazivFajla;
  void parsirajLiniju(string linija,vector<string>& parsirano);
public:
  LinkerFajl(string nazivFajla);
  TabelaSimbola* dohvatiTabeluSimbola();
  TabelaSekcija* dohvatiTabeluSekcija();
  int napraviStrukture();
  string dohvatiNazivFajla();
  ~LinkerFajl();
};


#endif
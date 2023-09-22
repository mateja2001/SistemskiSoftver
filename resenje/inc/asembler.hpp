#ifndef ASEMBLER_HPP
#define ASEMBLER_HPP
#include "instrukcije.hpp"
#include "../misc/parser.hpp"
#include "../misc/lexer.hpp"
#include "tabelaSimbola.hpp"
#include "tabelaSekcija.hpp"
#include "tabelaLiterala.hpp"
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

class Asembler
{
private:
  struct instrukcija* prvaInstrukcija;
  int locationCounter;
  int indexTrenSekcije;
  int indexTrenSimbola;
  TabelaSimbola * tabelaSimbola;
  TabelaSekcija * tabelaSekcija;
  //obrada prvi prolaz
  int obradiDirektivuPP(struct instrukcija* ins);
  int obradiLabeluPP(struct instrukcija* ins);
  int obradiInstrukcijuPP(struct instrukcija* ins);
  //obrada drugi prolaz
  int obradiDirektivuDP(struct instrukcija* ins);
  int obradiInstrukcijuDP(struct instrukcija* ins);
  int napraviRelZapZaBazenLit();
  int proveriVelicinuSekcija();
public:
  Asembler();
  int prviProlaz();
  int drugiProlaz();
  void ispisiTabeluSimbola();
  void ispisiTabeluSekcija();
  void ispisiTabeleLiterala();
  void ispisiMasinskeKodove();
  void ispisiTabeleRealokacije();
  void upisiSadrzaj(string nazivFajla);
  ~Asembler();
};



#endif
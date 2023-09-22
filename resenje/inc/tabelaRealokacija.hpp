#ifndef TABELAREALOKACIJA_HPP
#define TABELAREALOKACIJA_HPP
#include <stdio.h>
#include <vector>
using namespace std;

enum TipRel {R_X86_64_32};

class RelZapis
{
  public:
  unsigned int offset;//offest od trenutne sekcije na kojoj treba da se izvrsi izmena
  int simbol;//index simbola, -1 ako je simbol lokalni u toj sekciji pa se mora dodati addend na pocetak sekcije da bi se doslo do simbola
  int sekcija;//-1 kaze da nije def u toj sekciji
  unsigned int addend;//dodajse se kao offset na simbol(kad su lokalni podaci u pitanju pa se za simbol stavlja sekcija, pa addent do lokalnog simbola u toj sekciji)
  TipRel tipRelokacije;
};
class TabelaRealokacija
{
private:
vector<RelZapis*> tabelaRealokacija;
public:
  void dodajZapis(RelZapis* relZ);
  ~TabelaRealokacija();
  void ispisiTabeluRealokacija();
  int dohvBrojRelZapisa();
  RelZapis* dohvRelZapis(int index);
  vector<RelZapis*>& dohvatiTabeluRealokacija();
};

#endif
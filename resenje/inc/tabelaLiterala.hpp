#ifndef TABELALITERALA_HPP
#define TABELALITERALA_HPP
#include <stdio.h>
#include <vector>
#include <cstring>
using namespace std;
class Literal
{
public:
  int vrednost;
  char * simbol; //jer i labela moze da se koristi u nekim instrukcijama i moramo je staviti u bazen literaa
  bool literal; // govori da li se u datom ulazu nalazi literal ( ako je true tada gledamo vrednost) ili simbol(labela) 
  //int velicina-> nema potrebe podrazumevano ce biti 4B
  int lokacija;//lokacija od koje je smesten literal u bazenu
};
//PREDSTAVLJA USTVARI BAZEN LITERALA
class TabelaLiterala
{
private:
  vector<Literal *> tabelaLiterala;
public:
  void dodajKonstantu(Literal * lit);
  int dohvatiLokaciju(int vrednost);//ukoliko ne postoji takva vrednost literala povratna vrednost funkcije je -1
  int dohvatiLokaciju(char *simbol);
  void postaviLokaciju(int vrednost, int lokacija);
  void postaviLokaciju(char* simbol, int lokacija);
  void postaviLokacijeSimbolima(int krajnjaAdrSekcije);
  bool postojiVrednost(int vrednost);
  bool postojiLabela(char * naziv);
  void ispisiTabeluLiterala();
  int dohvatiBrojLiterala();
  Literal * dohvatiLiteral(int index);//da bi mogli da iteriramo kroz tabelu, van klase TabelaLiterala
  ~TabelaLiterala();
};



#endif
#ifndef EMULATOR_HPP
#define EMULATOR_HPP
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <sys/mman.h>
#include <fstream>
#include <sstream>
using namespace std;
class Emulator
{
private:
  string ulazniFajl;
  int reg[16];//opstenamenski registri 0-15, pocetna vrednost pc-a(15)=0x40000000, pocetna vrednost sp(14)=0, prva lokacija gde se stavlja na stek je 0xFFFFFFFF
  int creg[3];//kontrolni i statusni registri, status->0, handler->1, cause->2
  unsigned char * memorija;
  int inicijalizujMemorijuIRegistre();
  void parsirajLiniju(string linija,vector<string>& parsirano);
  void izvrsi();
  void ispisiStanjeRegistara();
  void neispravnaInstrukcija();// u ovom slucaju izazivamo softverski prekid sa statusom 1
  const int PC=15;
  const int SP=14;
public:
  int pokreniEmulaciju();
  void ispisiUlaz();
  Emulator(string ulazniFajl);
  ~Emulator();
};

#endif
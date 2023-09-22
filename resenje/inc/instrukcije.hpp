#ifndef INSTRUKCIJE_HPP
#define INSTRUKCIJE_HPP

//mozda ce ovaj naziv TIP da pravi problem u smislu naziva
enum TipOperanda{LITERAL,SIMBOL,REGISTAR,CREGISTAR};
enum Adresiranje{IMMED,MEMDIR,REGDIR,REGINDPOM,REGIND};
struct instrukcija{
  char *naziv; // moze biti i labela i instrukcija i direktiva
  struct operand* operandi; //mogu biti simboli direktive ili operandi
  struct instrukcija *next;
  bool labela;
  bool direktiva;
  Adresiranje adresiranje;
};
struct operand{
  int vrednost; //pretstavlja vrednost literala
  int registar;
  char* simbol; //u slucjau direktive section, global,word,...ascii tu se nalaze simboli koji nisu literali, TAKODJE TU SE NALAZI I POMERAJ KOJI JE SIMBOL TJ NIJE LITERAL
  char* cRegistar;
  int pomeraj;
  TipOperanda tip;
  Adresiranje adr;
  struct operand* next;
};

//postaviti na headInstrukcija->next==NULL
extern struct instrukcija* headInstrukcija;
extern struct instrukcija* tailInstrukcija; // pokazivac na poslednju instrukciju
extern struct instrukcija* currInstrukcija; // pokazivac na trenutnu istrukciju-- koristi se u slucaju direktiva
struct instrukcija* napraviInstrukciju(char * naziv, struct operand * argumenti, Adresiranje adr);
struct instrukcija* napraviDirektivu(char * naziv, struct operand * argumenti);
struct instrukcija* napraviLabelu(char * naziv);
struct instrukcija* napraviTrenutnuDirektivu(char * naziv); //sluzi da se napravi direktiva koja ima trenutni
void dodajTrenutnuInstrukciju();
void dodajOperandTrenInstrukciji(struct operand * op);
struct operand * registar(int registar, Adresiranje a);
struct operand * cRegistar(char * registar, Adresiranje a);
struct operand * registarPomLit(int registar, int pomeraj); //kada je pomeraj literal
struct operand * registarPomSim(int registar, char * simbol); //kada je pomeraj literal
struct operand * literal(int vrednost, Adresiranje a);
struct operand * simbol(char * naziv, Adresiranje a);
void oslobodiProstor();
void ispisiInstrukcije();
int brojInstrukcija();
#endif
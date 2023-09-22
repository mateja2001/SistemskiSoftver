#include "../inc/asembler.hpp"


int main(int argc, char * argv[]){
  if(argc!=4||strcmp(argv[1],"-o")!=0){
    printf("GRESKA, POGRESNO NAVODJENJE ARGUMENATA ASEMBLIRANJA");
    return -1;
  }
  FILE *myfile = fopen(argv[3], "r");
  if (!myfile) {
    printf("TRAZENI FAJL NE POSTOJI ILI SE NE MOZE OTVORITI");
    return -1;
  }
  yyin = myfile;
  int i=yyparse();
  if(i!=0){
    printf("PARSIRANJE NIJE OK\n");
    return -1;
  }
  if(headInstrukcija==NULL){
    printf("FAJL PRAZAN,NE POSTOJI NI JEDNA INSTRUKCIJA\n");
    return -1;
  }
  Asembler* asembler=new Asembler();
  int j=asembler->prviProlaz();
  if(j==-1){
    return -1;
  }
  int k=asembler->drugiProlaz();
  if(k==-1){
    return -1;
  }
  asembler->upisiSadrzaj(argv[2]);
  delete asembler;
  oslobodiProstor();
  fclose(myfile);
  return 0;
}
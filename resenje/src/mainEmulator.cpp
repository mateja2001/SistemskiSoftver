#include "../inc/emulator.hpp"


int main(int argc, char * argv[]){
  if(argc!=2){
    cout<<"EMULATOR: GRESKA, POGRESNO NAVODJENJE ARGUMENATA EMULIRANJA"<<endl;
  }
  Emulator* emulator=new Emulator(argv[1]);
  emulator->pokreniEmulaciju();
  delete emulator;
}
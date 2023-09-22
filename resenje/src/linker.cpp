#include "../inc/linker.hpp"

Linker::Linker(vector<LinkerFajl*>&fajlovi,vector<PlaceSekcije*>&psekcije,string nazivIzlaza){
  ulazniFajlovi=fajlovi;
  nazivIzlazongFajla=nazivIzlaza;
  placeSekcije=psekcije;
  counter=0;
  novaTabelaSekcija=new TabelaSekcija();
}

int Linker::proveriSimbole(){
  for(auto ul=ulazniFajlovi.begin();ul!=ulazniFajlovi.end();ul++){
    LinkerFajl *uf=(*ul);
    for(auto sim=uf->dohvatiTabeluSimbola()->dohvTabeluSimbola().begin();sim!=uf->dohvatiTabeluSimbola()->dohvTabeluSimbola().end();sim++){
      Simbol *s=(*sim);
      if(s->globalDef==true){
        //proveri da li se kao globalni smobol javlja jos u nekom fajlu->ako da, visestruka definicija simbola
        //asembler je resio visestruke definicije na nivou jednog fajla
        for(auto ul2=ulazniFajlovi.begin();ul2!=ulazniFajlovi.end();ul2++){
          if((*ul2)!=uf){
            for(auto sim2=(*ul2)->dohvatiTabeluSimbola()->dohvTabeluSimbola().begin();sim2!=(*ul2)->dohvatiTabeluSimbola()->dohvTabeluSimbola().end();sim2++){
              Simbol *ss=(*sim2);
              if(ss->globalDef&&strcmp(ss->naziv,s->naziv)==0){
                printf("LINKER: GRESKA, VISESTRUKA DEFINICIJA SIMBOLA: ");
                printf("%s",ss->naziv);
                printf("\n");
                return -1;
              }
            }
          } 
        }
      }else if(s->externDef==true){
        //proveri da li za svaki externi definisan simbol postoji simbol koji je globalni
        bool postoji=false;
        for(auto ul2=ulazniFajlovi.begin();ul2!=ulazniFajlovi.end();ul2++){
          if((*ul2)!=uf){
            for(auto sim2=(*ul2)->dohvatiTabeluSimbola()->dohvTabeluSimbola().begin();sim2!=(*ul2)->dohvatiTabeluSimbola()->dohvTabeluSimbola().end();sim2++){
              Simbol *ss=(*sim2);
              if(ss->globalDef&&strcmp(ss->naziv,s->naziv)==0){
                postoji=true;
              }
            }
          } 
        }
        if(postoji==false){
          printf("LINKER: GRESKA, POSTOJI NERAZRESENI SIMBOL: ");
          printf("%s",s->naziv);
          printf("\n");
          return -1;
        }
      }
    }
  }

  return 0;
}

void Linker::prepraviSimbole(){
  //prvo prepraviti vrednost svih globalnih i lokalnih simbola
  for(auto uf=ulazniFajlovi.begin();uf!=ulazniFajlovi.end();uf++){
    TabelaSimbola *tsim=(*uf)->dohvatiTabeluSimbola();
    for(auto sim=tsim->dohvTabeluSimbola().begin();sim!=tsim->dohvTabeluSimbola().end();sim++){
      if((*sim)->externDef==false){
        //znaci da je simbol ili localni ili globalni
        Sekcija* sekcijaSimb=(*uf)->dohvatiTabeluSekcija()->dohvatiSekciju((*sim)->sekcija);
        (*sim)->vrednost=(*sim)->vrednost+sekcijaSimb->base;
      }
    }
  }
  //kada su svi globalni i localni razreseni, mozemo razresiti externe
  for(auto uf=ulazniFajlovi.begin();uf!=ulazniFajlovi.end();uf++){
    TabelaSimbola *tsim=(*uf)->dohvatiTabeluSimbola();
    for(auto sim=tsim->dohvTabeluSimbola().begin();sim!=tsim->dohvTabeluSimbola().end();sim++){
      if((*sim)->externDef==true){
        //treba pronaci gde se nalazi simbol definisan kao globalni i dohvatiti njegovu vrednost
        for(auto uf2=ulazniFajlovi.begin();uf2!=ulazniFajlovi.end();uf2++){
          if((*uf2)!=(*uf)){
            for(auto sim2=(*uf2)->dohvatiTabeluSimbola()->dohvTabeluSimbola().begin();sim2!=(*uf2)->dohvatiTabeluSimbola()->dohvTabeluSimbola().end();sim2++){
              if((*sim2)->globalDef==true&&strcmp((*sim)->naziv,(*sim2)->naziv)==0){
                (*sim)->vrednost=(*sim2)->vrednost;
                break;
              }
            }
          }
        }
      }
    }
  }
}

void Linker::prepraviMKod(unsigned int vrednost, unsigned int offset,vector<unsigned char>&mKod){
  //podatak se smesta po little endian formatu
  unsigned char c1=(unsigned char)(vrednost&0xFF);
  unsigned char c2=(unsigned char)((vrednost>>8)&0xFF);
  unsigned char c3=(unsigned char)((vrednost>>16)&0xFF);
  unsigned char c4=(unsigned char)((vrednost>>24)&0xFF);
  int i=0;
  mKod[offset]=c1;
  mKod[offset+1]=c2;
  mKod[offset+2]=c3;
  mKod[offset+3]=c4;
}

void Linker::resiRelokacioneZapise(){
  for(auto uf=ulazniFajlovi.begin();uf!=ulazniFajlovi.end();uf++){
    TabelaSimbola *tsim=(*uf)->dohvatiTabeluSimbola();
    for(auto sek=(*uf)->dohvatiTabeluSekcija()->dohvatiTabeluSekcija().begin();sek!=(*uf)->dohvatiTabeluSekcija()->dohvatiTabeluSekcija().end();sek++){
      for(auto rz=(*sek)->tabelaRealokacija->dohvatiTabeluRealokacija().begin();rz!=(*sek)->tabelaRealokacija->dohvatiTabeluRealokacija().end();rz++){
        prepraviMKod(tsim->dohvatiSimbol((*rz)->simbol)->vrednost+(*rz)->addend,(*rz)->offset,(*sek)->masinskiKod);
      }
    }
  }
}


int Linker::linkujFajlove(){
  int i=proveriSimbole();
  if(i==-1){
    return -1;
  }
  i=smestiSekcije();
  if(i==-1){
    return -1;
  }
  prepraviSimbole();
  resiRelokacioneZapise();
  ispisiHexFajlOutput();
  return 0;
}

int Linker::smestiSekcije(){
  //ako ima prvo sekcije za koje je navedena place adresa, prvo se one smestaju
  for(auto ps=placeSekcije.begin();ps!=placeSekcije.end();ps++){
    bool pronadjen=false;
    for(auto ul=ulazniFajlovi.begin();ul!=ulazniFajlovi.end();ul++){
      TabelaSekcija *t=(*ul)->dohvatiTabeluSekcija();
      for(auto sek=t->dohvatiTabeluSekcija().begin();sek!=t->dohvatiTabeluSekcija().end();sek++){
        if((*ps)->imeSekcije.compare((*sek)->naziv)==0){
          pronadjen=true;
          if(counter!=0&&counter>(*ps)->mesto){
            //sledeca sekcija se ne moze postaviti na predodredjeno mesto->greska
            printf("LINKER: GRESKA, PREKLAPANJE SEKCIJA: ");
            cout<<(*ps)->imeSekcije<<" i "<<(*(--ps))->imeSekcije<<endl;
            return -1;
          }
          novaTabelaSekcija->dodajSekciju((*sek));
          (*sek)->base=(*ps)->mesto;
          (*sek)->nazivFajla=(*ul)->dohvatiNazivFajla();
          counter=(*ps)->mesto;
          int pom=counter;
          counter+=(*sek)->length;
          if(pom>counter){
            cout<<"LINKER: GRESKA, NEMOGUCE JE SMESTITI SEKCIJU "<<(*sek)->naziv<<" U ADRESNI PROSTOR"<<endl;
            return -1;
          }
          pom=counter;
          counter+=(*sek)->lengthBZ;
          if(pom>counter){
            cout<<"LINKER: GRESKA, NEMOGUCE JE SMESTITI SEKCIJU "<<(*sek)->naziv<<" U ADRESNI PROSTOR"<<endl;
            return -1;
          }
          //provera da li postoji jos neka sekcija sa istim imenom, ako postoji, nadovezati je
          for(auto ul2=ulazniFajlovi.begin();ul2!=ulazniFajlovi.end();ul2++){
            if((*ul2)!=(*ul)){
              TabelaSekcija * tt=(*ul2)->dohvatiTabeluSekcija();
              for(auto sek2=tt->dohvatiTabeluSekcija().begin();sek2!=tt->dohvatiTabeluSekcija().end();sek2++){
                if((*ps)->imeSekcije.compare((*sek2)->naziv)==0){
                  //postoji sekcija sa istim imenom, nadovezuje se
                  novaTabelaSekcija->dodajSekciju((*sek2));
                  (*sek2)->base=counter;
                  (*sek2)->nazivFajla=(*ul2)->dohvatiNazivFajla();
                  pom=counter;
                  counter+=(*sek2)->length;
                  if(counter<pom){
                    cout<<"LINKER: GRESKA, NEMOGUCE JE SMESTITI SEKCIJU "<<(*sek2)->naziv<<" U ADRESNI PROSTOR"<<endl;
                    return -1;
                  }
                  pom=counter;
                  counter+=(*sek2)->lengthBZ;
                  if(counter<pom){
                    cout<<"LINKER: GRESKA, NEMOGUCE JE SMESTITI SEKCIJU "<<(*sek2)->naziv<<" U ADRESNI PROSTOR"<<endl;
                    return -1;
                  }
                }
              }
            }
          }
        }
      }
    }
    //moramo proveriti slucaj ako je korisnik naveo neku sekciju koja nigde ne postoji->greska
    if(pronadjen==false){
      //sekcija koja je navedena ne postoji nigde
      cout<<"LINKER: GRESKA, NEPOSTOJECA SEKCIJA: "<<(*ps)->imeSekcije<<endl;
      return -1;
    }
  }
  //smestamo ostale sekcije koje nisu navedene u place
  for(auto ul=ulazniFajlovi.begin();ul!=ulazniFajlovi.end();ul++){
    TabelaSekcija* ts=(*ul)->dohvatiTabeluSekcija();
    for(auto sek=ts->dohvatiTabeluSekcija().begin();sek!=ts->dohvatiTabeluSekcija().end();sek++){
      if(!obradjenaSekcija((*sek)->naziv,(*ul)->dohvatiNazivFajla())){
        novaTabelaSekcija->dodajSekciju((*sek));
        (*sek)->base=counter;
        (*sek)->nazivFajla=(*ul)->dohvatiNazivFajla();
        int pom=counter;
        counter+=(*sek)->length;
        if(counter<pom){
          cout<<"LINKER: GRESKA, NEMOGUCE JE SMESTITI SEKCIJU "<<(*sek)->naziv<<" U ADRESNI PROSTOR"<<endl;
          return -1;
        }
        pom=counter;
        counter+=(*sek)->lengthBZ;
        if(counter<pom){
          cout<<"LINKER: GRESKA, NEMOGUCE JE SMESTITI SEKCIJU "<<(*sek)->naziv<<" U ADRESNI PROSTOR"<<endl;
          return -1;
        }
        //proveri da li postoji sekcije sa istim nazivom, i izvrsi spajanje
        for(auto ul2=ulazniFajlovi.begin();ul2!=ulazniFajlovi.end();ul2++){
            if((*ul2)!=(*ul)){
              TabelaSekcija * tt=(*ul2)->dohvatiTabeluSekcija();
              for(auto sek2=tt->dohvatiTabeluSekcija().begin();sek2!=tt->dohvatiTabeluSekcija().end();sek2++){
                if(strcmp((*sek2)->naziv,(*sek)->naziv)==0){
                  //postoji sekcija sa istim imenom, nadovezuje se
                  novaTabelaSekcija->dodajSekciju((*sek2));
                  (*sek2)->base=counter;
                  (*sek2)->nazivFajla=(*ul2)->dohvatiNazivFajla();
                  pom=counter;
                  counter+=(*sek2)->length;
                  if(counter<pom){
                    cout<<"LINKER: GRESKA, NEMOGUCE JE SMESTITI SEKCIJU "<<(*sek2)->naziv<<" U ADRESNI PROSTOR"<<endl;
                    return -1;
                  }
                  pom=counter;
                  counter+=(*sek2)->lengthBZ;
                  if(counter<pom){
                    cout<<"LINKER: GRESKA, NEMOGUCE JE SMESTITI SEKCIJU "<<(*sek2)->naziv<<" U ADRESNI PROSTOR"<<endl;
                    return -1;
                  }
                }
              }
            }
        }
      }
    }
  }
  return 0;
}

bool Linker::obradjenaSekcija(string nazivSekcije, string nazivFajla){
  for(auto sek=novaTabelaSekcija->dohvatiTabeluSekcija().begin();sek!=novaTabelaSekcija->dohvatiTabeluSekcija().end();sek++){
    if((*sek)->nazivFajla.compare(nazivFajla)==0&&nazivSekcije.compare((*sek)->naziv)==0){
      return true;
    }
  }
  return false;
}

void Linker::ispisiHexFajl(){
  unsigned int addr=-1;
  unsigned int i=1;
  for(auto sek=novaTabelaSekcija->dohvatiTabeluSekcija().begin();sek!=novaTabelaSekcija->dohvatiTabeluSekcija().end();sek++){
    if(addr==-1){
      addr=(*sek)->base;
      printf("%08X",addr);
      printf(": ");
      i=1;
    }else if(addr!=(*sek)->base){
      addr=(*sek)->base;
      printf("\n");
      printf("%08X",addr);
      printf(": ");
      i=1;
    }else if(addr==(*sek)->base&&(i-1)%8==0){
      printf("\n");
      printf("%08X",addr);
      printf(": ");
    }
    for(auto bajt=(*sek)->masinskiKod.begin();bajt!=(*sek)->masinskiKod.end();bajt++){
      printf("%02X",(*bajt));
      printf(" ");
      addr++;
      if(i%8==0&&(bajt!=((*sek)->masinskiKod.end()-1))){
        printf("\n");
        printf("%08X",addr);
        printf(": ");
      }
      i++;
    }
    
  }
}

void Linker::ispisiHexFajlOutput(){
  ofstream izlaz;
  izlaz.open(nazivIzlazongFajla);
  unsigned int addr=-1;
  unsigned int i=1;
  for(auto sek=novaTabelaSekcija->dohvatiTabeluSekcija().begin();sek!=novaTabelaSekcija->dohvatiTabeluSekcija().end();sek++){
    if(addr==-1){
      addr=(*sek)->base;
      char c[10];
      sprintf(c,"%08X",addr);
      izlaz<<string(c)<<": ";
      i=1;
    }else if(addr!=(*sek)->base){
      addr=(*sek)->base;
      char c[10];
      sprintf(c,"%08X",addr);
      izlaz<<endl;
      izlaz<<string(c)<<": ";
      i=1;
    }else if(addr==(*sek)->base&&(i-1)%8==0){
      char c[10];
      sprintf(c,"%08X",addr);
      izlaz<<endl;
      izlaz<<string(c)<<": ";
    }
    for(auto bajt=(*sek)->masinskiKod.begin();bajt!=(*sek)->masinskiKod.end();bajt++){
      char c[5];
      sprintf(c,"%02X",(*bajt));
      izlaz<<string(c)<<" ";
      addr++;
      if(i%8==0&&(bajt!=((*sek)->masinskiKod.end()-1))){
        char cc[10];
        sprintf(cc,"%08X",addr);
        izlaz<<endl;
        izlaz<<string(cc)<<": ";
      }
      i++;
    }
  }
  izlaz.close();
}

Linker::~Linker(){
  //ujedno brise i novuTabeluSekcija(jer se sve sekcije koje se nalaze posebno u fajlovima nalaze i u novoj tabeli sekcija)
  for(auto ul=ulazniFajlovi.begin();ul!=ulazniFajlovi.end();ul++){
    delete (*ul);
  }
  for(auto ps=placeSekcije.begin();ps!=placeSekcije.end();ps++){
    delete (*ps);
  }
}
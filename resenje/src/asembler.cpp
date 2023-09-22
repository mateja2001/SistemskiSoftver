#include "../inc/asembler.hpp"
Asembler::Asembler(){
  prvaInstrukcija=headInstrukcija;
  locationCounter=0;
  indexTrenSekcije=-1;
  indexTrenSimbola=0;
  tabelaSimbola=new TabelaSimbola();
  tabelaSekcija= new TabelaSekcija();
}
int Asembler::obradiDirektivuPP(struct instrukcija* ins){
  if(strcmp(ins->naziv,".global")==0){
        struct operand* prviArgument=ins->operandi;
        if(ins->operandi==NULL){
          printf("ASEMBLER: GRESKA, GLOBAL NEMA NI JEDNOG OPERANDA\n");
          return -1;
        }
        while(prviArgument!=NULL){
          Simbol* sim=tabelaSimbola->dohvatiSimbol(prviArgument->simbol);
          if(sim==NULL){
            //simbol se prvi put nasao u global direktivi-->(postavljamo na UND)tj do sad se nije pojavljivao
            if(tabelaSekcija->postojiSekcija(prviArgument->simbol)){
              //ako se postoji sekcija i ako se nadje simbol koji se zove kao sekcija->visestruka definicija
              printf("ASEMBLER: GRESKA, VISESTRUKA DEFINICJA SIMBOLA(SEKCIJE): ");
              printf("%s",prviArgument->simbol);
              printf("\n");
              return -1;
            }
            Simbol *s=new Simbol();
            s->globalDef=true;
            s->externDef=false;
            s->naziv=prviArgument->simbol;
            s->povezivanje==Povezivanje::GLOBAL;
            s->sekcija=-1; //oznacava da je UND
            s->tip=Tip::NOTYP;
            s->index=indexTrenSimbola;
            indexTrenSimbola++;
            tabelaSimbola->dodajSimbol(s);
          }
          else{
            //simbol se vec nalazi u tabeli, samo treba postaviti fleg GLOBALDEF na true
            if(sim->tip==Tip::SCTN){
              printf("ASEMBLER: GRESKA, NAZIV SEKCIJE NE MOZE BITI GLOBALAN: ");
              printf("%s",sim->naziv);
              printf("\n");
              return -1;
            }
            sim->globalDef=true;
          }
          prviArgument=prviArgument->next;
        }
  }else if(strcmp(ins->naziv,".extern")==0){
        struct operand* prviArgument=ins->operandi;
        if(ins->operandi==NULL){
          printf("ASEMBLER: GRESKA, EXTERN NEMA NI JEDNOG OPERANDA\n");
          return -1;
        }
        while(prviArgument!=NULL){
          Simbol* sim=tabelaSimbola->dohvatiSimbol(prviArgument->simbol);
          if(sim==NULL){
            //simbol se prvi put nasao u extern direktivi-->(postavljamo na UND)tj do sad se nije pojavljivao
            if(tabelaSekcija->postojiSekcija(prviArgument->simbol)){
              //ako se postoji sekcija i ako se nadje simbol koji se zove kao sekcija->visestruka definicija
              printf("ASEMBLER: GRESKA, VISESTRUKA DEFINICJA SIMBOLA(SEKCIJE): ");
              printf("%s",prviArgument->simbol);
              printf("\n");
              return -1;
            }
            Simbol *s=new Simbol();
            s->globalDef=false;
            s->externDef=true;
            s->naziv=prviArgument->simbol;
            s->povezivanje==Povezivanje::GLOBAL;
            s->sekcija=-1; //oznacava da je UND
            s->tip=Tip::NOTYP;
            s->index=indexTrenSimbola;
            indexTrenSimbola++;
            tabelaSimbola->dodajSimbol(s);
          }
          else{
            //simbol se vec nalazi u tabeli, samo treba postaviti fleg EXTERN na true
            if(sim->sekcija!=-1){
              //znaci da je simbol definisan u ovom fajlu i pritom je deklarisan kao extern-> visestruka definicija
              printf("ASEMBLER: GRESKA, VISESTRUKA DEFINICJA SIMBOLA(SEKCIJE): ");
              printf("%s",sim->naziv);
              printf("\n");
              return -1;
            }
            sim->externDef=true;
          }
          prviArgument=prviArgument->next;
        }
  }else if(strcmp(ins->naziv,".section")==0){
    struct operand* sek=ins->operandi;
    if(tabelaSekcija->postojiSekcija(sek->simbol)){
      printf("ASEMBLER: GRESKA, VISESTRUKA DEFINICJA SIMBOLA(SEKCIJE): ");
      printf("%s",sek->simbol);
      printf("\n");
      return -1;
    }
    if(tabelaSimbola->dohvatiSimbol(sek->simbol)!=NULL){
      //ne moze se sekcija i simbol isto zvati
      printf("ASEMBLER: GRESKA, VISESTRUKA DEFINICJA SIMBOLA(SEKCIJE): ");
      printf("%s",sek->simbol);
      printf("\n");
      return -1;
    }
    //otvoriti novu sekciju(ukoliko je prva sekcija) i zatvoriti prethodnu sekciju(ako ova nije prva koja se otvara)
    if(indexTrenSekcije==-1){
      //prva sekcija koja se otvara
      Sekcija* sekc=new Sekcija();
      sekc->base=0;
      sekc->index=0;
      sekc->naziv=sek->simbol;
      tabelaSekcija->dodajSekciju(sekc);
      indexTrenSekcije=0;
      locationCounter=0;
    }
    else{
      //postojala je ternutna sekcija, ona se prvo zatvara a zatim se otvara nova sekcija
      tabelaSekcija->zatvoriSekciju(indexTrenSekcije,locationCounter);
      indexTrenSekcije++;
      locationCounter=0;
      Sekcija* sekc=new Sekcija();
      sekc->base=0;
      sekc->index=indexTrenSekcije;
      sekc->naziv=sek->simbol;
      tabelaSekcija->dodajSekciju(sekc);
    }
    //dodavanje naziva sekcije u tabelu simbola
    Simbol *s=new Simbol();
    s->globalDef=false;
    s->externDef=false;
    s->naziv=sek->simbol;
    s->povezivanje==Povezivanje::LOCAL;
    s->sekcija=indexTrenSekcije; 
    s->tip=Tip::SCTN;
    s->index=indexTrenSimbola;
    s->vrednost=0;
    indexTrenSimbola++;
    tabelaSimbola->dodajSimbol(s);
  }else if(strcmp(ins->naziv,".word")==0){
    struct operand* arg=ins->operandi;
    if(arg==NULL){
      printf("ASEMBLER: GRESKA, DIREKTIVA WORD NEMA NI JEDAN ARGUMENT\n");
      return -1;
    }
    if(indexTrenSekcije==-1){
      printf("ASEMBLER: GRESKA, DIREKTIVA WORD SE NE SME NACI VAN SEKCIJE\n");
      return -1;
    }
    while(arg!=NULL){
      locationCounter+=4;
      arg=arg->next;
    }
  }else if(strcmp(ins->naziv,".skip")==0){
    struct operand* arg=ins->operandi;
    if(arg==NULL){
      printf("ASEMBELR: GRESKA, SKIP NEMA NI JEDAN ARGUMENT\n");
      return -1;
    }
    if(indexTrenSekcije==-1){
      printf("ASEMBLER: GRESKA, DIREKTIVA WORD SE NE SME NACI VAN SEKCIJE\n");
      return -1;
    }
    locationCounter+=arg->vrednost;
  }
  return 0;
}

int Asembler::obradiLabeluPP(struct instrukcija* ins){
  Simbol* sim=tabelaSimbola->dohvatiSimbol(ins->naziv);
  if(sim==NULL){
    //prvo pojavljivanje labele(simbola)
    if(tabelaSekcija->postojiSekcija(ins->naziv)){
      //ako se postoji sekcija i ako se nadje simbol koji se zove kao sekcija->visestruka definicija
      printf("ASEMBLER: GRESKA, VISESTRUKA DEFINICJA SIMBOLA(SEKCIJE): ");
      printf("%s",ins->naziv);
      printf("\n");
      return -1;
    }
    Simbol *s=new Simbol();
    s->externDef=false;
    s->globalDef=false;
    s->index=indexTrenSimbola;
    indexTrenSimbola++;
    s->naziv=ins->naziv;
    s->povezivanje=Povezivanje::LOCAL;
    s->tip=Tip::NOTYP;
    if(indexTrenSekcije==-1){
      //simbol je definisan izvan bilo koje sekcije----> greska;
      printf("ASEMBLER: GRESKA, SIMBOL ");
      printf("%s",s->naziv);
      printf(" NIJE DEFINISAN UNUTAR SEKCIJE\n");
      return -1;
    }
    s->sekcija=indexTrenSekcije;
    s->vrednost=locationCounter;
    tabelaSimbola->dodajSimbol(s);
  }else{
    //labela(simbol) se prethodno pojavila u global direktivi ili je rec o visestrukoj definiciji simbola(labele)
    if(sim->sekcija!=-1||(sim->sekcija==-1&&sim->externDef==true)){
      //rec je o visestrukoj definiciji simbola
      printf("ASEMBLER: GRESKA, VISESTRUKA DEFINICIJA SIMBOLA: ");
      printf("%s",ins->naziv);
      printf("\n");
      return -1;
    }
    sim->sekcija=indexTrenSekcije;
    sim->vrednost=locationCounter;
  }

  return 0;
}

int Asembler::obradiInstrukcijuPP(struct instrukcija* ins){
  //ukoliko se radi o instrukcijama brach,jmp,call,ld,st moramo literale dodati u bazen
  //TABELA LITERALA PREDSTAVLJA BAZEN LITERALA
  //stavljanje literala u bazen
  if(indexTrenSekcije==-1){
    printf("ASEMBLER: GRESKA, INSTRUKCIJA SE NE MOZE NACI VAN SEKCIJE\n");
    return -1;
  }
  if(strcmp(ins->naziv,"jmp")==0||strcmp(ins->naziv,"call")==0||strcmp(ins->naziv,"ld")==0||strcmp(ins->naziv,"st")==0
  ||strcmp(ins->naziv,"beq")==0||strcmp(ins->naziv,"bne")==0||strcmp(ins->naziv,"bgt")==0){
    if(strcmp(ins->naziv,"jmp")==0||strcmp(ins->naziv,"call")==0||strcmp(ins->naziv,"beq")==0
    ||strcmp(ins->naziv,"bne")==0||strcmp(ins->naziv,"bgt")==0){
      if(ins->adresiranje!=Adresiranje::MEMDIR){
        printf("ASEMBLER: GRESKA, POGRESNO ADRESIRANJE SKOKOVA I POZIVA POTPROGRAMA: ");
        printf("%s",ins->naziv);
        printf("\n");
        return -1;
      }
    }
    struct operand* prviArgument=ins->operandi;//zanemarena provera da su operandi null, to je obradjeno u lekseru i parseru
    while(prviArgument!=NULL){
      if(prviArgument->tip==TipOperanda::LITERAL){
        if(tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->tabelaLiterala->postojiVrednost(prviArgument->vrednost)==false){
          //dodajemo literal u tabelu literala za trenutnu sekciju
          Literal *lit=new Literal();
          lit->vrednost=prviArgument->vrednost;
          lit->literal=true;
          tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->tabelaLiterala->dodajKonstantu(lit);
        }
      }else if(prviArgument->tip==TipOperanda::SIMBOL){
        //moramo dodati novi ulaz u tabelu literala , linker ce vrednost u bazenu literal promeniti nakon linkovanja,
        //pa se ne moze koristiti ista konstanta za literal i simbol iako ona postoji
        //dodajemo simbol(vrednost simbola) u tabelu literala za trenutnu sekciju
          if(tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->tabelaLiterala->postojiLabela(prviArgument->simbol)==false){
            Literal *lit=new Literal();
            lit->literal=false;
            lit->simbol=prviArgument->simbol;
            tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->tabelaLiterala->dodajKonstantu(lit);
          }
      }
      else if(prviArgument->tip==TipOperanda::REGISTAR&&prviArgument->adr==Adresiranje::REGINDPOM&&prviArgument->simbol==NULL){
        //proverava se da li je pomeraj(literal) veci od 12b, ako jeste->greska
        //provera regind pom sa simbolom se proverava kad se sazna vrednost tog simbola(zbog toga je greska),(greska ako se ne sazna)->drugi prolaz
        //ne treba stavljati u bazen litrela jer pomeraj staje u 12b
        if(prviArgument->pomeraj>pow(2,12)-1){
          printf("ASEMBLER: GRESKA, POMERAJ JE VECI OD 12b ");
          printf("%d",prviArgument->pomeraj);
          printf("\n");
          return -1;
        }
      }else if(prviArgument->tip==TipOperanda::REGISTAR&&prviArgument->adr==Adresiranje::REGINDPOM&&prviArgument->simbol!=NULL){
        //greska jer za A nivo ne moze postojati simbol cija ce vrednost biti poznata u trenutku asembliranja
        //ne postoji .equ direktiva
        printf("ASEMBLER: GRESKA, VREDNOST SIMBOLA ZA POMERAJ NIJE POZNATA U TRENUTKU ASEMBLIRANJA: ");
        printf("%s",prviArgument->simbol);
        printf("\n");
        return -1;
      }
      prviArgument=prviArgument->next;
    }
  }
  //povecavanje locationCountera
  locationCounter+=4;
  if(strcmp(ins->naziv,"iret")==0||(strcmp(ins->naziv,"ld")==0&&ins->adresiranje==Adresiranje::MEMDIR)){
    //iret razlaze na dve instrukcije pa LC moramo uvecati za 2*4
    //takodje i ld za slucajeve ucitavanja mem[lit] i mem[simb] se razlaze na 2 ld instrukcije
    locationCounter+=4;
  }
  //provera ispravnosti pojedinih instrukcija
  if(strcmp(ins->naziv,"st")==0&&ins->adresiranje==Adresiranje::IMMED){
    //greske jer ne mozemo nesto da upisemo u labelu a ni u literal;
    printf("ASEMBLER: GRESKA, ST INSTRUKCIJA NE MOZE STAVITI PODATAK U LITERAL ILI LABELU\n");
    return -1;
  }
  return 0;
}

int Asembler::prviProlaz(){
  struct instrukcija* prva=prvaInstrukcija;
  if(prva==NULL){
    //mozda treba da se baci greska ili ispise;
    printf("ASEMBLER: GRESKA, ASEMBLER NEMA INSTRUKCIJA\n");//parser nije nista vratio
    return -1;
  }
  while(prva!=NULL){
    if(prva->direktiva==true){
      if(strcmp(prva->naziv,".end")==0){
        //zavrsava se asembliranje
        break;
      }
      int i=obradiDirektivuPP(prva);
      if(i==-1){
        return -1;
      }
    }else if(prva->labela==true){
      int i=obradiLabeluPP(prva);
      if(i==-1){
        return -1;
      }
    }else if(prva->labela==false&&prva->direktiva==false){
      //rec je o instrukciji
      int i=obradiInstrukcijuPP(prva);
      if(i==-1){
        return -1;
      }
    }
    prva=prva->next;
  }
  //zatvaranje poslednje sekcije i postavljanje length i resetvoanje locationCounter-a
  tabelaSekcija->zatvoriSekciju(indexTrenSekcije,locationCounter);
  locationCounter=0;
  indexTrenSekcije=-1;
  indexTrenSimbola=0;
  //samo extern simboli smeju ostati nedefinisani na kraju prvog prolaza
  if(tabelaSimbola->proveriNedefinisaneSimbole()){
    printf("ASEMBLER: GRESKA, POSTOJE NEDEFINISANI GLOBALNI ILI LOKALNI SIMBOLI\n");
    return -1;
  }
  //provera da li postoji simbol koji je i extern i global
  if(tabelaSimbola->globalExternSimbol()){
    printf("ASEMBLER: GRESKA, POSTOJI SIMBOL KOJI JE DEKLARISAN I KAO GLOBAL I KAO EXTERN\n");
    return -1;
  }
  // na kraju prvog prolaza kad znamo velicinu sekcije, popunimo lokacije za bazen literala
  tabelaSekcija->popuniLokazijeUBazenuLiterala();
  return 0;
}

int Asembler::obradiDirektivuDP(struct instrukcija* ins){
  //.extern, .global, ->ne radi se nista
  struct operand* arg=ins->operandi;
  if(strcmp(ins->naziv,".section")==0){
    if(indexTrenSekcije!=-1){
      //prethodna sekcija se zatvara, generise se masinski kod za bazen literala i rel. zapis za bazen lit
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->kreirajMKodIBazenaLit();
      int i=napraviRelZapZaBazenLit();
      if(i==-1){
        return -1;
      }
    }
    //moramo resetovati i locationCoutner
    indexTrenSekcije++;
    locationCounter=0;
    //smestiti bazen literal posle svake sekcije
  }
  else if(strcmp(ins->naziv,".word")==0){
    //ukoliko je literal u pitanju->ugradjujemo ga u masinski kod
    //ukoliko je simbol u pitanju ugradjujemo 0 (4B) i relokacioni zapis da se to mesto nakon realokacije prepravi
    //ne pristupa se bazenu literala jer se sam simb/literal ugradjuje u 4B
    while(arg!=NULL){
      if(arg->tip==TipOperanda::LITERAL){
        //svaki od literala se smesta u 4B
        unsigned char c1=arg->vrednost& 0xFF;
        unsigned char c2=(arg->vrednost >> 8)& 0xFF;
        unsigned char c3=(arg->vrednost >> 16)& 0xFF;
        unsigned char c4=(arg->vrednost >> 24)& 0xFF;
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
      }
      else if(arg->tip==TipOperanda::SIMBOL){
        //upisujemo nule i generisemo relokacioni zapis
        unsigned char c1=0x00;
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
        Simbol *s=tabelaSimbola->dohvatiSimbol(arg->simbol);
        if(s==NULL){
          printf("ASEMBLER: GRESKA, .WORD: UPOTREBA SIMBOLA KOJI NIJE NI DEKLARISAN NI DEFINISAN\n");
          return -1;
        }
        RelZapis* rz=new RelZapis();
        if(s->externDef==1||s->globalDef==1){
          rz->addend=0;
          rz->tipRelokacije=TipRel::R_X86_64_32;
          rz->offset=locationCounter;
          rz->simbol=s->index;
          rz->sekcija=s->sekcija;//-1 govori da je simbol externi za tu datoteku
        }
        else{
          //simbol je lokalni, u tabelu relokacije ide sekcija u kojoj je simbol def, i addent do tog simbola u sekciji
          rz->addend=s->vrednost;
          rz->sekcija=s->sekcija;
          rz->tipRelokacije=TipRel::R_X86_64_32;
          rz->offset=locationCounter;
          //umesto simbola stoji index sekcije
          Simbol *ss=tabelaSimbola->dohvatiSimbol(tabelaSekcija->dohvatiSekciju(s->sekcija)->naziv);
          rz->simbol=ss->index;
        }
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->tabelaRealokacija->dodajZapis(rz);
      }
      locationCounter+=4;
      arg=arg->next;
    }
  }else if(strcmp(ins->naziv,".skip")==0){
    for(int i=0; i<arg->vrednost;i++){
      unsigned char c1=0x00;
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
    }
    locationCounter+=arg->vrednost;
  }
  return 0;
}

int Asembler::obradiInstrukcijuDP(struct instrukcija* ins){
  struct operand* prviArgument=ins->operandi;
  if(strcmp(ins->naziv,"halt")==0){
    for(int i=0;i<4;i++){
      unsigned char c=0x00;
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c);
    }
  }else if(strcmp(ins->naziv,"int")==0){
    unsigned char c=0x10;
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c);
    c=0x00;
    for(int i=0;i<3;i++){
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c);
    }
  }else if(strcmp(ins->naziv,"iret")==0){
    //status<=mem[sp+4]
    unsigned char c1=0x96;
    unsigned char c2=0x0E;
    unsigned char c3=0x00;
    unsigned char c4=0x04;
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
    //pc=<mem[sp], sp=<sp+8
    c1=0x93;
    c2=0xFE;
    c3=0x00;
    c4=0x08;
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
  }else if(strcmp(ins->naziv,"call")==0){
    if(prviArgument->tip==TipOperanda::SIMBOL){
      //ako je simbol iz iste sekcije, onda moze da stane u offset, ako nije moramo pristupiti bazenu literala
      Simbol *s=tabelaSimbola->dohvatiSimbol(prviArgument->simbol);
      if(s==NULL){
        printf("ASEMBLER: GRESKA, UPOTREBA SIMBOLA KOJI NIJE NI DEKLARISAN NI DEFINISAN: ");
        printf("%s",prviArgument->simbol);
        printf("\n");
        return -1;
      }
      if(s->sekcija==indexTrenSekcije){
        //simbol u istoj sekciji, moze da stane offset
        unsigned char c1=0x20;
        unsigned char c2=0xF0;
        int offset=s->vrednost-(locationCounter+4);
        unsigned char c3=(unsigned char)((offset>>8)&0x0F);
        unsigned char c4=(unsigned char)(offset&0xFF);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
      }else{
        //simbol nije u istoj sekciji, moramo pristupati bazenu literala
        unsigned char c1=0x21;
        unsigned char c2=0xF0;
        int lokacijaUbazenuLit=-1;
        lokacijaUbazenuLit=tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->tabelaLiterala->dohvatiLokaciju(prviArgument->simbol);
        int offset=lokacijaUbazenuLit-(locationCounter+4);
        unsigned char c3=(unsigned char)((offset>>8)&0x0F);
        unsigned char c4=(unsigned char)(offset&0xFF);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
      }
    }else{
      //literal->svakako pristupamo bazenu literala
      int lokacijaUbazenuLit=-1;
      lokacijaUbazenuLit=tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->tabelaLiterala->dohvatiLokaciju(prviArgument->vrednost);
      int offset=lokacijaUbazenuLit-(locationCounter+4);
      unsigned char c1=0x21;
      unsigned char c2=0xF0;
      unsigned char c3=(unsigned char)((offset>>8)&0x0F);
      unsigned char c4=(unsigned char)(offset&0xFF);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
    }
  }else if(strcmp(ins->naziv,"jmp")==0){
    if(prviArgument->tip==TipOperanda::SIMBOL){
      //ako je simbol iz iste sekcije, onda moze da stane u offset, ako nije moramo pristupiti bazenu literala
      Simbol *s=tabelaSimbola->dohvatiSimbol(prviArgument->simbol);
      if(s==NULL){
        printf("ASEMBLER: GRESKA, UPOTREBA SIMBOLA KOJI NIJE NI DEKLARISAN NI DEFINISAN: ");
        printf("%s",prviArgument->simbol);
        printf("\n");
        return -1;
      }
      if(s->sekcija==indexTrenSekcije){
        //simbol u istoj sekciji, moze da stane offset
        unsigned char c1=0x30;
        unsigned char c2=0xF0;
        int offset=s->vrednost-(locationCounter+4);
        unsigned char c3=(unsigned char)((offset>>8)&0x0F);
        unsigned char c4=(unsigned char)(offset&0xFF);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
      }else{
        //simbol nije u istoj sekciji, moramo pristupati bazenu literala
        unsigned char c1=0x38;
        unsigned char c2=0xF0;
        int lokacijaUbazenuLit=-1;
        lokacijaUbazenuLit=tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->tabelaLiterala->dohvatiLokaciju(prviArgument->simbol);
        int offset=lokacijaUbazenuLit-(locationCounter+4);
        unsigned char c3=(unsigned char)((offset>>8)&0x0F);
        unsigned char c4=(unsigned char)(offset&0xFF);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
      }
    }else{
      //literal->svakako pristupamo bazenu literala
      int lokacijaUbazenuLit=-1;
      lokacijaUbazenuLit=tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->tabelaLiterala->dohvatiLokaciju(prviArgument->vrednost);
      int offset=lokacijaUbazenuLit-(locationCounter+4);
      unsigned char c1=0x38;
      unsigned char c2=0xF0;
      unsigned char c3=(unsigned char)((offset>>8)&0x0F);
      unsigned char c4=(unsigned char)(offset&0xFF);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
    }
  }else if(strcmp(ins->naziv,"beq")==0){
    if(prviArgument->next->next->tip==TipOperanda::SIMBOL){
      //ako je simbol iz iste sekcije, onda moze da stane u offset, ako nije moramo pristupiti bazenu literala
      Simbol *s=tabelaSimbola->dohvatiSimbol(prviArgument->next->next->simbol);
      if(s==NULL){
        printf("ASEMBLER: GRESKA, UPOTREBA SIMBOLA KOJI NIJE NI DEKLARISAN NI DEFINISAN: ");
        printf("%s",prviArgument->next->next->simbol);
        printf("\n");
        return -1;
      }
      if(s->sekcija==indexTrenSekcije){
        //simbol u istoj sekciji, moze da stane offset
        unsigned char c1=0x31;
        unsigned char c2=(unsigned char)((prviArgument->registar&0x0F)|0xF0);
        int offset=s->vrednost-(locationCounter+4);
        unsigned char c3=(unsigned char)(((offset>>8)&0x0F)|((prviArgument->next->registar<<4)&0xF0));
        unsigned char c4=(unsigned char)(offset&0xFF);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
      }else{
        //simbol nije u istoj sekciji, moramo pristupati bazenu literala
        unsigned char c1=0x39;
        unsigned char c2=(unsigned char)((prviArgument->registar&0x0F)|0xF0);
        int lokacijaUbazenuLit=-1;
        lokacijaUbazenuLit=tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->tabelaLiterala->dohvatiLokaciju(prviArgument->next->next->simbol);
        int offset=lokacijaUbazenuLit-(locationCounter+4);
        unsigned char c3=(unsigned char)(((offset>>8)&0x0F)|((prviArgument->next->registar<<4)&0xF0));
        unsigned char c4=(unsigned char)(offset&0xFF);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
      }
    }else{
      //literal->svakako pristupamo bazenu literala
      unsigned char c1=0x39;
      unsigned char c2=(unsigned char)((prviArgument->registar&0x0F)|0xF0);
      int lokacijaUbazenuLit=-1;
      lokacijaUbazenuLit=tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->tabelaLiterala->dohvatiLokaciju(prviArgument->next->next->vrednost);
      int offset=lokacijaUbazenuLit-(locationCounter+4);
      unsigned char c3=(unsigned char)(((offset>>8)&0x0F)|((prviArgument->next->registar<<4)&0xF0));
      unsigned char c4=(unsigned char)(offset&0xFF);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
    }
  }else if(strcmp(ins->naziv,"bne")==0){
    if(prviArgument->next->next->tip==TipOperanda::SIMBOL){
      //ako je simbol iz iste sekcije, onda moze da stane u offset, ako nije moramo pristupiti bazenu literala
      Simbol *s=tabelaSimbola->dohvatiSimbol(prviArgument->next->next->simbol);
      if(s==NULL){
        printf("ASEMBLER: GRESKA, UPOTREBA SIMBOLA KOJI NIJE NI DEKLARISAN NI DEFINISAN: ");
        printf("%s",prviArgument->next->next->simbol);
        printf("\n");
        return -1;
      }
      if(s->sekcija==indexTrenSekcije){
        //simbol u istoj sekciji, moze da stane offset
        unsigned char c1=0x32;
        unsigned char c2=(unsigned char)((prviArgument->registar&0x0F)|0xF0);
        int offset=s->vrednost-(locationCounter+4);
        unsigned char c3=(unsigned char)(((offset>>8)&0x0F)|((prviArgument->next->registar<<4)&0xF0));
        unsigned char c4=(unsigned char)(offset&0xFF);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
      }else{
        //simbol nije u istoj sekciji, moramo pristupati bazenu literala
        unsigned char c1=0x3A;
        unsigned char c2=(unsigned char)((prviArgument->registar&0x0F)|0xF0);
        int lokacijaUbazenuLit=-1;
        lokacijaUbazenuLit=tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->tabelaLiterala->dohvatiLokaciju(prviArgument->next->next->simbol);
        int offset=lokacijaUbazenuLit-(locationCounter+4);
        unsigned char c3=(unsigned char)(((offset>>8)&0x0F)|((prviArgument->next->registar<<4)&0xF0));
        unsigned char c4=(unsigned char)(offset&0xFF);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
      }
    }else{
      //literal->svakako pristupamo bazenu literala
      unsigned char c1=0x3A;
      unsigned char c2=(unsigned char)((prviArgument->registar&0x0F)|0xF0);
      int lokacijaUbazenuLit=-1;
      lokacijaUbazenuLit=tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->tabelaLiterala->dohvatiLokaciju(prviArgument->next->next->vrednost);
      int offset=lokacijaUbazenuLit-(locationCounter+4);
      unsigned char c3=(unsigned char)(((offset>>8)&0x0F)|((prviArgument->next->registar<<4)&0xF0));
      unsigned char c4=(unsigned char)(offset&0xFF);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
    }
  }else if(strcmp(ins->naziv,"bgt")==0){
    if(prviArgument->next->next->tip==TipOperanda::SIMBOL){
      //ako je simbol iz iste sekcije, onda moze da stane u offset, ako nije moramo pristupiti bazenu literala
      Simbol *s=tabelaSimbola->dohvatiSimbol(prviArgument->next->next->simbol);
      if(s==NULL){
        printf("ASEMBLER: GRESKA, UPOTREBA SIMBOLA KOJI NIJE NI DEKLARISAN NI DEFINISAN: ");
        printf("%s",prviArgument->next->next->simbol);
        printf("\n");
        return -1;
      }
      if(s->sekcija==indexTrenSekcije){
        //simbol u istoj sekciji, moze da stane offset
        unsigned char c1=0x33;
        unsigned char c2=(unsigned char)((prviArgument->registar&0x0F)|0xF0);
        int offset=s->vrednost-(locationCounter+4);
        unsigned char c3=(unsigned char)(((offset>>8)&0x0F)|((prviArgument->next->registar<<4)&0xF0));
        unsigned char c4=(unsigned char)(offset&0xFF);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
      }else{
        //simbol nije u istoj sekciji, moramo pristupati bazenu literala
        unsigned char c1=0x3B;
        unsigned char c2=(unsigned char)((prviArgument->registar&0x0F)|0xF0);
        int lokacijaUbazenuLit=-1;
        lokacijaUbazenuLit=tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->tabelaLiterala->dohvatiLokaciju(prviArgument->next->next->simbol);
        int offset=lokacijaUbazenuLit-(locationCounter+4);
        unsigned char c3=(unsigned char)(((offset>>8)&0x0F)|((prviArgument->next->registar<<4)&0xF0));
        unsigned char c4=(unsigned char)(offset&0xFF);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
      }
    }else{
      //literal->svakako pristupamo bazenu literala
      unsigned char c1=0x3B;
      unsigned char c2=(unsigned char)((prviArgument->registar&0x0F)|0xF0);
      int lokacijaUbazenuLit=-1;
      lokacijaUbazenuLit=tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->tabelaLiterala->dohvatiLokaciju(prviArgument->next->next->vrednost);
      int offset=lokacijaUbazenuLit-(locationCounter+4);
      unsigned char c3=(unsigned char)(((offset>>8)&0x0F)|((prviArgument->next->registar<<4)&0xF0));
      unsigned char c4=(unsigned char)(offset&0xFF);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
    }
  }else if(strcmp(ins->naziv,"ret")==0){
    //pop PC
    unsigned char c1=0x93;
    unsigned char c2=0xFE;
    unsigned char c3=0x00;
    unsigned char c4=0x04;
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
  }else if(strcmp(ins->naziv,"push")==0){
    unsigned char c1=0x81;
    unsigned char c2=0xE0;
    unsigned char c3=(unsigned char)((prviArgument->registar<<4)|0x0F);
    unsigned char c4=0xFC;
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
  }else if(strcmp(ins->naziv,"pop")==0){
    unsigned char c1=0x93;
    unsigned char c2=(unsigned char)((prviArgument->registar<<4)|0x0E);
    unsigned char c3=0x00;
    unsigned char c4=0x04;
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
  }else if(strcmp(ins->naziv,"xchg")==0){
    unsigned char c1=0x40;
    unsigned char c2=(unsigned char)(0x0F&(prviArgument->registar));
    unsigned char c3=(unsigned char)((prviArgument->next->registar<<4)&0xF0);
    unsigned char c4=0x00;
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
  }else if(strcmp(ins->naziv,"add")==0){
    unsigned char c1=0x50;
    unsigned char c2=(unsigned char)(((prviArgument->next->registar<<4)&0xF0)|prviArgument->next->registar);
    unsigned char c3=(unsigned char)((prviArgument->registar<<4)&0xF0);
    unsigned char c4=0x00;
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
  }else if(strcmp(ins->naziv,"sub")==0){
    unsigned char c1=0x51;
    unsigned char c2=(unsigned char)(((prviArgument->next->registar<<4)&0xF0)|prviArgument->next->registar);
    unsigned char c3=(unsigned char)((prviArgument->registar<<4)&0xF0);
    unsigned char c4=0x00;
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
  }else if(strcmp(ins->naziv,"mul")==0){
    unsigned char c1=0x52;
    unsigned char c2=(unsigned char)(((prviArgument->next->registar<<4)&0xF0)|prviArgument->next->registar);
    unsigned char c3=(unsigned char)((prviArgument->registar<<4)&0xF0);
    unsigned char c4=0x00;
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
  }else if(strcmp(ins->naziv,"div")==0){
    unsigned char c1=0x53;
    unsigned char c2=(unsigned char)(((prviArgument->next->registar<<4)&0xF0)|prviArgument->next->registar);
    unsigned char c3=(unsigned char)((prviArgument->registar<<4)&0xF0);
    unsigned char c4=0x00;
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
  }else if(strcmp(ins->naziv,"not")==0){
    unsigned char c1=0x60;
    unsigned char c2=(unsigned char)(((prviArgument->registar<<4)&0xF0)|prviArgument->registar);
    unsigned char c3=0x00;
    unsigned char c4=0x00;
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
  }else if(strcmp(ins->naziv,"and")==0){
    unsigned char c1=0x61;
    unsigned char c2=(unsigned char)(((prviArgument->next->registar<<4)&0xF0)|prviArgument->next->registar);
    unsigned char c3=(unsigned char)((prviArgument->registar<<4)&0xF0);
    unsigned char c4=0x00;
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
  }else if(strcmp(ins->naziv,"or")==0){
    unsigned char c1=0x62;
    unsigned char c2=(unsigned char)(((prviArgument->next->registar<<4)&0xF0)|prviArgument->next->registar);
    unsigned char c3=(unsigned char)((prviArgument->registar<<4)&0xF0);
    unsigned char c4=0x00;
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
  }else if(strcmp(ins->naziv,"xor")==0){
    unsigned char c1=0x63;
    unsigned char c2=(unsigned char)(((prviArgument->next->registar<<4)&0xF0)|prviArgument->next->registar);
    unsigned char c3=(unsigned char)((prviArgument->registar<<4)&0xF0);
    unsigned char c4=0x00;
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
  }else if(strcmp(ins->naziv,"shl")==0){
    unsigned char c1=0x70;
    unsigned char c2=(unsigned char)(((prviArgument->next->registar<<4)&0xF0)|prviArgument->next->registar);
    unsigned char c3=(unsigned char)((prviArgument->registar<<4)&0xF0);
    unsigned char c4=0x00;
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
  }else if(strcmp(ins->naziv,"shr")==0){
    unsigned char c1=0x71;
    unsigned char c2=(unsigned char)(((prviArgument->next->registar<<4)&0xF0)|prviArgument->next->registar);
    unsigned char c3=(unsigned char)((prviArgument->registar<<4)&0xF0);
    unsigned char c4=0x00;
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
  }else if(strcmp(ins->naziv,"ld")==0){
    if(ins->adresiranje==Adresiranje::IMMED){
      //$literal i $simbol-> u oba slucaja se pristupa bazenu literala
      //pc izvrsavanja ove instrukcije pokazuje na jednu instrukicju ispred
      int lokacijaUbazenuLit=-1;
      if(prviArgument->tip==TipOperanda::LITERAL){
        //$literal
        lokacijaUbazenuLit=tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->tabelaLiterala->dohvatiLokaciju(prviArgument->vrednost);
      }
      else{
        //$simbol
        lokacijaUbazenuLit=tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->tabelaLiterala->dohvatiLokaciju(prviArgument->simbol);
      }
      //do ovog slucaja verovatno nece doci
      if(lokacijaUbazenuLit==-1){
        printf("ASEMBLER: GRESKA, LD:UPOTREBA SIMBOLA KOJI NIJE DEKLARISAN/DEFINISAN\n");
        return -1;
      } 
      //location coutner trenutno pokazuje na pocetak ove instrukcije
      //PC ce u trenutku izvrsavanja ove instrukcije pokazivati na jednu instrukciju ispred(a to je locationCnt+4)
      int offset=lokacijaUbazenuLit-(locationCounter+4);
      unsigned char c1=0x92;
      unsigned char c2=(unsigned char)((prviArgument->next->registar<<4)|0x0F);
      //pomeraj ne moze biti negativan, tako da je ovo ok
      unsigned char c11=(unsigned char)(offset&0xFF);//nizi bajtovi displacmenta
      unsigned char c22=(unsigned char)((offset>>8)&0x0F);//visi bajt displacmenta
      unsigned char c3=c22;
      unsigned char c4=c11;
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
    }else if(ins->adresiranje==Adresiranje::REGDIR){
      //ld %r1 %r2
      unsigned char c1=0x91;
      unsigned char c2=(unsigned char)((prviArgument->next->registar<<4)&0xF0)|(prviArgument->registar&0x0F);
      unsigned char c3=0x00;
      unsigned char c4=0x00;
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
    }else if(ins->adresiranje==Adresiranje::REGIND){
      //ld [%r1] r2
      unsigned char c1=0x92;
      unsigned char c2=(unsigned char)((prviArgument->next->registar<<4)&0xF0)|(prviArgument->registar&0x0F);
      unsigned char c3=0x00;
      unsigned char c4=0x00;
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
    }else if(ins->adresiranje==Adresiranje::REGINDPOM){
      // imamo samo jedan slucaj [reg+lit], slucaj [reg+simb] ne moze doci za nivo A (nema .equ direktive, pa nakon asembliranja ni jedan simbol nema konacnu vrednost)
      unsigned char c1=0x92;
      unsigned char c2=(unsigned char)((prviArgument->next->registar<<4)&0xF0)|(prviArgument->registar&0x0F);
      unsigned char c11=(unsigned char)(0xFF&prviArgument->pomeraj);
      unsigned char c22=(unsigned char)((prviArgument->pomeraj>>8)&0x0F);
      unsigned char c3=c22;
      unsigned char c4=c11;
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
    }
    else{
      //MEMDIR adresiranje
      //neophodne su 2 instrukcije
      //ne moramo generisati relokacioni zapis jer se pristupa bazenu literala->na kraju drugog prolaza za svaki simbol ce biti generisan relokacioni zapis
      //reg<-mem32[pc+off] pristupamo bazenu lit i upisujemo u reg
      //reg<-mem32[reg] dovlacimo podatak u reg sa lokacije mem32[reg]
      unsigned char c1=0x92;
      unsigned char c2=(unsigned char)(((prviArgument->next->registar<<4)&0xF0)|0x0F);
      int lokacijaUbazenuLit=-1;
      if(prviArgument->tip==TipOperanda::LITERAL){
        //literal
        lokacijaUbazenuLit=tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->tabelaLiterala->dohvatiLokaciju(prviArgument->vrednost);
      }
      else{
        //simbol
        lokacijaUbazenuLit=tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->tabelaLiterala->dohvatiLokaciju(prviArgument->simbol);
      }
      //do ovog slucaja verovatno nece doci
      if(lokacijaUbazenuLit==-1){
        printf("ASEMBLER: GRESKA, LD:UPOTREBA SIMBOLA KOJI NIJE DEKLARISAN/DEFINISAN\n");
        return -1;
      } 
      int offset=lokacijaUbazenuLit-(locationCounter+4);
      unsigned char c4=(unsigned char)(offset&0xFF);//nizi bajtovi displacmenta
      unsigned char c3=(unsigned char)((offset>>8)&0x0F);//visi bajt displacmenta
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
      //sada drugi deo instrukcije reg<-mem32[reg]
      c1=0x92;
      c2=(unsigned char)(((prviArgument->next->registar<<4)&0xF0)|((prviArgument->next->registar)&0x0F));
      c3=0x00;
      c4=0x00;
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
    }
  }else if(strcmp(ins->naziv,"st")==0){
    if(ins->adresiranje==Adresiranje::MEMDIR){
      // mem32[lit/sim]=<gpr,(mem[mem[pc+off]]<=gpr)
      int lokacijaUbazenuLit=-1;
      if(prviArgument->next->tip==TipOperanda::LITERAL){
        //literal
        lokacijaUbazenuLit=tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->tabelaLiterala->dohvatiLokaciju(prviArgument->next->vrednost);
      }
      else{
        //simbol
        lokacijaUbazenuLit=tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->tabelaLiterala->dohvatiLokaciju(prviArgument->next->simbol);
      }
      int offset=lokacijaUbazenuLit-(locationCounter+4);
      //do ovog slucaja verovatno nece doci
      if(lokacijaUbazenuLit==-1){
        printf("ASEMBLER: GRESKA, ST:UPOTREBA SIMBOLA KOJI NIJE DEKLARISAN/DEFINISAN\n");
        return -1;
      }
      unsigned char c1=0x82;
      unsigned char c2=0x0F;
      unsigned char c3=(unsigned char)(((prviArgument->registar<<4)&0xF0)|((offset>>8)&0x0F));
      unsigned char c4=(unsigned char)(offset&0xFF);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
    }else if(ins->adresiranje==Adresiranje::REGDIR){
      //st reg, reg
      //ista instrukcija kao ld reg,reg
      unsigned char c1=0x91;
      unsigned char c2=(unsigned char)((prviArgument->next->registar<<4)&0xF0)|(prviArgument->registar&0x0F);
      unsigned char c3=0x00;
      unsigned char c4=0x00;
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
    }else if(ins->adresiranje==Adresiranje::REGIND){
      unsigned char c1=0x80;
      unsigned char c2=(unsigned char)((prviArgument->next->registar<<4)&0xF0);
      unsigned char c3=(unsigned char)((prviArgument->registar<<4)&0xF0);
      unsigned char c4=0x00;
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
    }else{
      //REGINDPOM [reg+lit] jer ne moze simbol bez .equ direktive
      unsigned char c1=0x80;
      unsigned char c2=(unsigned char)((prviArgument->next->registar<<4)&0xF0);
      unsigned char c3=(unsigned char)(((prviArgument->registar<<4)&0xF0)|((prviArgument->next->pomeraj>>8)&0x0F));
      unsigned char c4=(unsigned char)(prviArgument->next->pomeraj&0xFF);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
      tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
    }
  }else if(strcmp(ins->naziv,"csrrd")==0){
    unsigned char c1=0x90;
    unsigned char c2;
    if(strcmp(prviArgument->cRegistar,"status")==0){
      //status->0
      c2=(unsigned char)((prviArgument->next->registar<<4)&0xF0);
    }else if(strcmp(prviArgument->cRegistar,"handler")==0){
      //handler->1
      c2=(unsigned char)(((prviArgument->next->registar<<4)&0xF0)|0x01);
    }else{
      //cause->2
      c2=(unsigned char)(((prviArgument->next->registar<<4)&0xF0)|0x02);
    }
    unsigned char c3=0x00;
    unsigned char c4=0x00;
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
  }else if(strcmp(ins->naziv,"csrwr")==0){
    unsigned char c1=0x94;
    unsigned char c2;
    if(strcmp(prviArgument->next->cRegistar,"status")==0){
      //status->0
      c2=(unsigned char)(0x0F&(prviArgument->registar));
    }else if(strcmp(prviArgument->next->cRegistar,"handler")==0){
      //handler->1
      c2=(unsigned char)((0x0F&(prviArgument->registar))|0x10);
    }else{
      //cause->2
      c2=(unsigned char)((0x0F&(prviArgument->registar))|0x20);
    }
    unsigned char c3=0x00;
    unsigned char c4=0x00;
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c1);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c2);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c3);
    tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->masinskiKod.push_back(c4);
  }
  locationCounter+=4;
  if(strcmp(ins->naziv,"iret")==0||(strcmp(ins->naziv,"ld")==0&&ins->adresiranje==Adresiranje::MEMDIR)){
    locationCounter+=4;//u tom slucaju su 2 instrukcije pa locationCounter moramo povecati za 2*4
  }
  return 0;
}

int Asembler::napraviRelZapZaBazenLit(){
  TabelaLiterala *tabelaLit=tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->tabelaLiterala;
  int size=tabelaLit->dohvatiBrojLiterala();
  for(int i=0;i<size;i++){
    Literal* lit=tabelaLit->dohvatiLiteral(i);
    if(lit->literal==false){
      Simbol *s=tabelaSimbola->dohvatiSimbol(lit->simbol);
      if(s==NULL){
        printf("ASEMBLER: GRESKA,UPOTREBA SIMBOLA KOJI NIJE DEKLARISAN NITI DEFINISAN: ");
        printf("%s",lit->simbol);
        printf("\n");
        return -1;
      }
      RelZapis *rz=new RelZapis();
        if(s->externDef==1||s->globalDef==1){
          rz->addend=0;
          rz->tipRelokacije=TipRel::R_X86_64_32;
          rz->offset=lit->lokacija;
          rz->simbol=s->index;
          rz->sekcija=s->sekcija;//-1 govori da je simbol externi za tu datoteku
        }
        else{
          //simbol je lokalni, u tabelu relokacije ide sekcija u kojoj je simbol def, i addent do tog simbola u sekciji
          rz->addend=s->vrednost;
          rz->sekcija=s->sekcija;
          rz->tipRelokacije=TipRel::R_X86_64_32;
          rz->offset=lit->lokacija;
          //kao simbol je index sekcije u kojoj on pripada
          Simbol *ss=tabelaSimbola->dohvatiSimbol(tabelaSekcija->dohvatiSekciju(s->sekcija)->naziv);
          rz->simbol=ss->index;
        }
        tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->tabelaRealokacija->dodajZapis(rz);
    }
  }
  return 0;
}

int Asembler::drugiProlaz(){
  struct instrukcija* prva=prvaInstrukcija;
  while(prva!=NULL){
    //nema nikakve posebne obrade labele u drugom prolazu
    if(prva->direktiva==true){
      if(strcmp(prva->naziv,".end")==0){
        //zavrsava se asembliranje
        break;
      }
      int i=obradiDirektivuDP(prva);
      if(i==-1){
        return -1;
      }
    }else if(prva->labela==false&&prva->direktiva==false){
      //rec je o instrukciji
      int i=obradiInstrukcijuDP(prva);
      if(i==-1){
        return -1;
      }
    }
    prva=prva->next;
  }
  //mora se napraviti masinski kod rel zapis i za bazen lit. zadnje sekcije
  tabelaSekcija->dohvatiSekciju(indexTrenSekcije)->kreirajMKodIBazenaLit();
  int i=napraviRelZapZaBazenLit();
  if(i==-1){
    return -1;
  }
  tabelaSekcija->popuniVelicinuBazenaLiterala();
  //zbog offseta od 12b, max velicina jedne sekcije je 4KB, kako bi se pc-rel adresiranjem moglo sve dohvatiti
  i=proveriVelicinuSekcija();
  if(i==-1){
    return -1;
  }
  return 0;
}

int Asembler::proveriVelicinuSekcija(){
  unsigned int maxSize=1UL<<12;
  for(auto sek=tabelaSekcija->dohvatiTabeluSekcija().begin();sek!=tabelaSekcija->dohvatiTabeluSekcija().end();sek++){
    unsigned int velicina=(unsigned int)((*sek)->length+(*sek)->lengthBZ);
    if(velicina>maxSize){
      printf("ASEMBLER: GRESKA, MAKSIMALNA DOZVOLJENA VELICINA SEKCIJE JE 4KB: ");
      printf("%s",(*sek)->naziv);
      printf("\n");
      return -1;
    }
  }
  return 0;
}

void Asembler::ispisiTabeluSimbola(){
  tabelaSimbola->ispisiTabelu();
}

void Asembler::ispisiTabeluSekcija(){
  tabelaSekcija->ispisiTabeluSekcija();
}

void Asembler::ispisiTabeleLiterala(){
  tabelaSekcija->ispisiTabeleLiterala();
}

void Asembler::ispisiMasinskeKodove(){
  tabelaSekcija->ispisiMasinskeKodove();
}

void Asembler::ispisiTabeleRealokacije(){
  tabelaSekcija->ispisiTabeleRealokacija();
}

void Asembler::upisiSadrzaj(string nazivFajla){
  ofstream izlaz;
  izlaz.open(nazivFajla);

  izlaz<<"#simboli"<<" "<<tabelaSimbola->dohvBrojSimbola()<<endl;
  for(int i=0;i<tabelaSimbola->dohvBrojSimbola();i++){
    Simbol *s=tabelaSimbola->dohvatiSimbol(i);
    izlaz<<s->index<<" "<<s->naziv<<" "<<s->vrednost<<" "<<s->tip<<" "<<s->povezivanje<<" "<<s->sekcija<<" "<<s->globalDef<<" "<<s->externDef<<endl;
  }
  izlaz<<"#sekcije"<<" "<<tabelaSekcija->dohvBrojSekcija()<<endl;
  for(int i=0;i<tabelaSekcija->dohvBrojSekcija();i++){
    Sekcija* sek=tabelaSekcija->dohvatiSekciju(i);
    izlaz<<sek->index<<" "<<sek->naziv<<" "<<sek->base<<" "<<sek->lengthBZ<<" "<<sek->length<<endl;
    int vel=(int)ceil(sek->masinskiKod.size()*1.0/4);
    izlaz<<sek->naziv<<" "<<vel<<endl;
    int k=1;
    for(int j=0;j<sek->masinskiKod.size();j++){
      char c1[5];
      sprintf(c1,"%02X",sek->masinskiKod[j]);
      izlaz<<string(c1)<<" ";
      if(k%4==0){
        izlaz<<endl;
      }
      k++;
    }
    if(sek->masinskiKod.size()%4!=0) izlaz<<endl;
    //izlaz<<sek->naziv<<" "<<sek->masinskiKod.size()/4<<endl;
    // for(int j=0;j<sek->masinskiKod.size();j+=4){
    //   char c1[5];
    //   char c2[5];
    //   char c3[5];
    //   char c4[5];
    //   sprintf(c1,"%02X",sek->masinskiKod[j]);
    //   sprintf(c2,"%02X",sek->masinskiKod[j+1]);
    //   sprintf(c3,"%02X",sek->masinskiKod[j+2]);
    //   sprintf(c4,"%02X",sek->masinskiKod[j+3]);
    //   izlaz<<string(c1)<<" ";
    //   izlaz<<string(c2)<<" ";
    //   izlaz<<string(c3)<<" ";
    //   izlaz<<string(c4)<<" ";
    //   izlaz<<endl;
    // }
    izlaz<<"rela."<<sek->naziv<<" "<<sek->tabelaRealokacija->dohvBrojRelZapisa()<<endl;
    for(int j=0;j<sek->tabelaRealokacija->dohvBrojRelZapisa();j++){
      RelZapis * rz=sek->tabelaRealokacija->dohvRelZapis(j);
      izlaz<<rz->simbol<<" "<<rz->sekcija<<" "<<rz->offset<<" "<<rz->addend<<" "<<rz->tipRelokacije<<endl;
    } 
  }
  izlaz.close();
}

Asembler::~Asembler(){
  delete tabelaSimbola;
  delete tabelaSekcija;
}
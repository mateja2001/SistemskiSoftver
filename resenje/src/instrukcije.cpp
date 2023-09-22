#include "../inc/instrukcije.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


struct instrukcija* headInstrukcija=NULL;
struct instrukcija* tailInstrukcija=NULL;
struct instrukcija* currInstrukcija=NULL;

struct instrukcija* napraviInstrukciju(char * naziv, struct operand * args, Adresiranje adr){
  struct instrukcija* i=(instrukcija*)malloc(sizeof(struct instrukcija));
  i->operandi=args;
  i->naziv=naziv;
  i->direktiva=false;
  i->labela=false;
  i->adresiranje=adr;
  if(tailInstrukcija==NULL){
    //nema jos ni jedne instrukcije;
    tailInstrukcija=i;
    headInstrukcija=i;
  }
  else{
    tailInstrukcija->next=i;
  }
  tailInstrukcija=i;
  tailInstrukcija->next=NULL;
  return i;
}

struct instrukcija* napraviDirektivu(char * naziv, struct operand * args){
  //adresiranje ostaje nedefinisano
  struct instrukcija* i=(instrukcija*)malloc(sizeof(struct instrukcija));
  i->operandi=args;
  i->naziv=naziv;
  i->direktiva=true;
  i->labela=false;
  if(tailInstrukcija==NULL){
    //nema jos ni jedne instrukcije;
    tailInstrukcija=i;
    headInstrukcija=i;
  }
  else{
    tailInstrukcija->next=i;
  }
  tailInstrukcija=i;
  tailInstrukcija->next=NULL;
  return i;
}

struct instrukcija* napraviTrenutnuDirektivu(char * naziv){
  currInstrukcija=NULL;
  struct instrukcija* i=(instrukcija*)malloc(sizeof(struct instrukcija));
  i->naziv=naziv;
  i->operandi=NULL;
  i->direktiva=true;
  i->labela=false;
  currInstrukcija=i;
  return i; 
}

void dodajOperandTrenInstrukciji(struct operand * op){
  if(currInstrukcija!=NULL){
    struct operand* first=currInstrukcija->operandi;
    if(first==NULL){
      //nema jos ni jedan operand, dodajemo prvi operand, next je vec postavljen na NULL
      currInstrukcija->operandi=op;
    }
    else{
      //ima vec neki operand, dodajemo operand na kraju liste
      op->next=currInstrukcija->operandi;
      currInstrukcija->operandi=op;
    }
  }
}

void dodajTrenutnuInstrukciju(){
  if(tailInstrukcija==NULL){
    //jos nema ni jedna instrukcija
    tailInstrukcija=currInstrukcija;
    headInstrukcija=currInstrukcija;
  }
  else{
    tailInstrukcija->next=currInstrukcija;
    currInstrukcija->next=NULL;
    tailInstrukcija=currInstrukcija;
  }
  currInstrukcija=NULL;
}

struct instrukcija* napraviLabelu(char * naziv){
  //adresiranje ostaje nedefinisano
  struct instrukcija* i=(instrukcija*)malloc(sizeof(struct instrukcija));
  i->operandi=NULL;
  i->naziv=naziv;
  i->direktiva=false;
  i->labela=true;
  if(tailInstrukcija==NULL){
    //nema jos ni jedne instrukcije;
    tailInstrukcija=i;
    headInstrukcija=i;
  }
  else{
    tailInstrukcija->next=i;
  }
  tailInstrukcija=i;
  tailInstrukcija->next=NULL;
  return i;
}

struct operand * cRegistar(char * registar, Adresiranje adr){
  struct operand* a= (operand *)malloc(sizeof(struct operand));
  a->registar=-1;
  a->tip=TipOperanda::CREGISTAR;
  a->pomeraj=0;
  a->cRegistar=registar;
  a->adr=adr;
  a->next=NULL;
  return a;
}

struct operand * registar(int registar, Adresiranje adr){
  struct operand* a= (operand *)malloc(sizeof(struct operand));
  a->registar=registar;
  a->tip=TipOperanda::REGISTAR;
  a->pomeraj=0;
  a->cRegistar=NULL;
  a->adr=adr;
  a->next=NULL;
  return a;
}
struct operand * registarPomLit(int registar, int pomeraj){
  //tu je jedino pomeraj razlicit od 0
  struct operand* a= (operand *)malloc(sizeof(struct operand));
  a->registar=registar;
  a->pomeraj=pomeraj;
  a->simbol=NULL;
  a->tip=TipOperanda::REGISTAR;
  a->cRegistar=NULL;
  a->next=NULL;
  a->adr=Adresiranje::REGINDPOM;
  return a;
}
struct operand * registarPomSim(int registar, char * simbol){
  struct operand* a= (operand *)malloc(sizeof(struct operand));
  a->registar=registar;
  a->simbol=simbol;
  a->tip=TipOperanda::REGISTAR;
  a->cRegistar=NULL;
  a->next=NULL;
  a->adr=Adresiranje::REGINDPOM;
  return a;
}

struct operand * literal(int vrednost, Adresiranje adr){
  struct operand* a= (operand *)malloc(sizeof(struct operand));
  a->pomeraj=0;
  a->vrednost=vrednost;
  a->tip=TipOperanda::LITERAL;
  a->cRegistar=NULL;
  a->adr=adr;
  a->next=NULL;
  return a;
}

struct operand * simbol(char * naziv, Adresiranje adr){
  struct operand* a= (operand *)malloc(sizeof(struct operand));
  a->pomeraj=0;
  a->simbol=naziv;
  a->tip=TipOperanda::SIMBOL;
  a->adr=adr;
  a->cRegistar=NULL;
  a->next=NULL;
  return a;
}

void oslobodiProstor(){
  struct instrukcija *ins=headInstrukcija;
  while(ins!=NULL){
    if(ins->operandi!=NULL){
      struct operand *a=ins->operandi;
      while(a!=NULL){
        struct operand *pom=a;
        a=a->next;
        free(pom);
      }
    }
    struct instrukcija *poms=ins;
    ins=ins->next;
    free(poms);
  }
}

void ispisiInstrukcije(){
  struct instrukcija *ins=headInstrukcija;
  while(ins!=NULL){
    printf("%s",ins->naziv);
    struct operand * fristOp=ins->operandi;
    while(fristOp!=NULL){
      printf(" ");
      if(fristOp->tip==TipOperanda::REGISTAR){
        printf("%d",fristOp->registar);
        // u slucaju da postoji neki pomeraj [reg + lit/simb]
        if(fristOp->adr==Adresiranje::REGINDPOM){
          printf("+");
          if(fristOp->simbol==NULL){
            printf("%d",fristOp->pomeraj);
          }
          else {
            printf("%s",fristOp->simbol);
          }
        }
      }else if(fristOp->tip==TipOperanda::SIMBOL){
        printf("%s",fristOp->simbol);
      }else if(fristOp->tip==TipOperanda::LITERAL){
        printf("%d", fristOp->vrednost);
      }else{
        printf("%s",fristOp->cRegistar);
      }
      fristOp=fristOp->next;
    }
    printf("\n");
    ins=ins->next;
  }
}

int brojInstrukcija(){
  struct instrukcija *ins=headInstrukcija;
  int cnt=0;
  while(ins!=NULL){
    ins=ins->next;
    cnt++;
  }
  return cnt;
}
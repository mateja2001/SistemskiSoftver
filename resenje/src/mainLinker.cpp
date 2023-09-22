#include "../inc/linker.hpp"


int main(int argc, char * argv[]){

  vector<LinkerFajl*> ulazniFajlovi;
  vector<PlaceSekcije*> placeSekcije;
  string izlfajl;
  int i=1;
  bool imaHex=false;
  while(i<argc){
    //i=0 je naziv programa
    string s(argv[i]);
    if(strcmp(argv[i],"-o")==0){
      izlfajl=argv[i+1];
      i++;
    }else if(s.find("-place=")!=string::npos){
      //place
      string ss;
      string p;
      int j=7;
      while(argv[i][j]!='\0'){
        if(argv[i][j]!='@'){
          ss+=argv[i][j];
        }
        else{
          j++;
          while(argv[i][j]!='\0'){
            p+=argv[i][j];
            j++;
          }
          break;
        }
        j++;
      }
      PlaceSekcije *ps=new PlaceSekcije();
      ps->imeSekcije=ss;
      unsigned long adr=stol(p,nullptr,16);
      if(adr>UINT32_MAX){
        cout<<"LINKER: GRESKA, ADRESA ZA SMESTANJE SEKCIJE "<<ss<<" JE IZVAN ADRESNOG PROSTORA"<<endl;
        return -1;
      }
      ps->mesto=(unsigned int)stol(p,nullptr,16);
      //provera da li je za tu sekciju vec specificiran place
      for(int m=0;m<placeSekcije.size();m++){
        if(placeSekcije[m]->imeSekcije.compare(ps->imeSekcije)==0){
          printf("LINKER: GRESKA, VISE PUTA JE SPECIFICIRAN PLACE ZA ISTU SEKCIJU: ");
          cout<<ps->imeSekcije<<endl;
          return -1;
        }
      }
      placeSekcije.push_back(ps);
      //sortiranje placeSekcija
      for(int m=0;m<placeSekcije.size()-1;m++){
        for(int n=m+1;n<placeSekcije.size();n++){
          if(placeSekcije[m]->mesto>placeSekcije[n]->mesto){
            PlaceSekcije *p=placeSekcije[m];
            placeSekcije[m]=placeSekcije[n];
            placeSekcije[n]=p;
          }
        }
      }
    }else if(strcmp(argv[i],"-relocatable")==0){
      printf("LINKER: GRESKA, POGRESNO NAVODJENJE ARGUMENATA LINKOVANJA, LINKER NE PODRZAVA -relocatable\n");
      return -1;
    }else if(strcmp(argv[i],"-hex")==0){
      imaHex=true;
    }else{
      //ime fajla
      LinkerFajl* lf=new LinkerFajl(argv[i]);
      int j=lf->napraviStrukture();
      if(j==-1){
        delete lf;
        return -1;
      }
      ulazniFajlovi.push_back(lf);
    }
    i++;
  }
  if(imaHex==false){
    printf("LINKER: GRESKA, OPCIJA -HEX NIJE NAVEDENA\n");
    return -1;
  }
  if(izlfajl.empty()){
    printf("LINKER: GRESKA, NE POSTOJI NAZIV IZLAZNOG FAJLA\n");
    return -1;
  }
  Linker* linker=new Linker(ulazniFajlovi,placeSekcije,izlfajl);
  int j=linker->linkujFajlove();
  if(j==-1){
    return -1;
  }
  delete linker;
  return 0;
}
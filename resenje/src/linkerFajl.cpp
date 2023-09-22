#include "../inc/linkerFajl.hpp"

LinkerFajl::LinkerFajl(string nazivFajla){
  this->nazivFajla=nazivFajla;
  tabelaSimbola=new TabelaSimbola();
  tabelaSekcija=new TabelaSekcija();
}

int::LinkerFajl::napraviStrukture(){
  ifstream ulaz;
  ulaz.open(nazivFajla);
  if(ulaz.fail()){
    printf("LINKER: GRESKA, FAJL NE POSTOJI!");
    return -1;
  }
  string linija;
  vector<string>reci;
  //simboli
  getline(ulaz,linija);
  parsirajLiniju(linija,reci);
  int brojSimbola=stoi(reci[1]);
  for(int i=0;i<brojSimbola;i++){
    getline(ulaz,linija);
    parsirajLiniju(linija,reci);
    Simbol *s=new Simbol();
    s->index=stoi(reci[0]);
    s->naziv=strdup(reci[1].c_str());
    s->vrednost=stoi(reci[2]);
    s->tip=Tip(stoi(reci[3]));
    s->povezivanje=Povezivanje(stoi(reci[4]));
    s->sekcija=stoi(reci[5]);
    s->globalDef=stoi(reci[6]);
    s->externDef=stoi(reci[7]);
    tabelaSimbola->dodajSimbol(s);
  }
  getline(ulaz,linija);
  parsirajLiniju(linija,reci);
  int brojSekcija=stoi(reci[1]);
  for(int i=0;i<brojSekcija;i++){
    getline(ulaz,linija);
    parsirajLiniju(linija,reci);
    Sekcija* sek=new Sekcija();
    sek->index=stoi(reci[0]);
    sek->naziv=strdup(reci[1].c_str());
    sek->base=stoi(reci[2]);
    sek->lengthBZ=stoi(reci[3]);
    sek->length=stoi(reci[4]);
    tabelaSekcija->dodajSekciju(sek);
    getline(ulaz,linija);
    parsirajLiniju(linija,reci);
    int velMasinskogKoda=stoi(reci[1]);//ali podaljena sa 4, jer se u svakom redu nalaze 4B
    for(int j=0;j<velMasinskogKoda;j++){
      getline(ulaz,linija);
      parsirajLiniju(linija,reci);
      for(auto bajt=reci.begin();bajt!=reci.end();bajt++){
        unsigned char c1=(unsigned char)stoi((*bajt),nullptr,16);
        sek->masinskiKod.push_back(c1);
      }
    }
    getline(ulaz,linija);
    parsirajLiniju(linija,reci);
    int brRelZapisa=stoi(reci[1]);
    for(int j=0;j<brRelZapisa;j++){
      getline(ulaz,linija);
      parsirajLiniju(linija,reci);
      RelZapis *rz=new RelZapis();
      rz->simbol=stoi(reci[0]);
      rz->sekcija=stoi(reci[1]);
      rz->offset=stoi(reci[2]);
      rz->addend=stoi(reci[3]);
      rz->tipRelokacije=TipRel(stoi(reci[4]));
      sek->tabelaRealokacija->dodajZapis(rz);
    }
  }
  ulaz.close();
  return 0;
}

void LinkerFajl::parsirajLiniju(string linija,vector<string>& parsirano){
    parsirano.clear();
    stringstream neprasirana(linija);
    string rec;
    while (neprasirana >> rec) {
      parsirano.push_back(rec);
    }
}

TabelaSimbola* LinkerFajl::dohvatiTabeluSimbola(){
  return tabelaSimbola;
}

TabelaSekcija * LinkerFajl::dohvatiTabeluSekcija(){
  return tabelaSekcija;
}

string LinkerFajl::dohvatiNazivFajla(){
  return nazivFajla;
}

LinkerFajl::~LinkerFajl(){
  for(auto sek=tabelaSekcija->dohvatiTabeluSekcija().begin();sek!=tabelaSekcija->dohvatiTabeluSekcija().end();sek++){
    free((*sek)->naziv);
  }
  for(auto sim=tabelaSimbola->dohvTabeluSimbola().begin();sim!=tabelaSimbola->dohvTabeluSimbola().end();sim++){
    free((*sim)->naziv);
  }
  delete tabelaSimbola;
  delete tabelaSekcija;
}
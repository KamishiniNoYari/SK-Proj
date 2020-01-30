//
// Created by krystian on 22.12.2019.
//

#ifndef SK_PROJ_SERVER_H
#define SK_PROJ_SERVER_H
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <iterator>
#include <list>
#include <pthread.h>
using namespace std;

//Global definitions
#define PORT 8000
#define BUFFER_SIZE 500

//Commands
const string START = "START";
const string ANSWER = "AN";
const string END = "END";
const string ROUND = "RO";
const string cities[] = {"Augustów", "Ateny", "Antoninek", "Andrychów", "Anieliny", "Amsterdam","Będzin", "Barlinek", "Białystok",
"Bielsko-Biała", "Cedynia", "Chełm", "Chorzów", "Chojnice", "Chrzanów", "Czeladź,Darłówko", "Dąbrowa Górnicza",
"Dobrodzień", "Elbląg", "Ełk", "Edynburg", "Frombork", "Frampol", "Facja", "Fidżi","Gdynia", "Gdańsk", "Gniezno",
"Gorzów Wielkopolski", "Halinów", "Hajnówka", "Hel", "Iława", "Imielin", "Inowrocław", "Jastrzębia Góra",
"Jastrzębia Zdrój", "Kalisz", "Kalety", "Kazimierz Dolny","Legnica", "Legionowo", "Leśnica", "Leszno","Malbork",
"Małogoszcz", "Marki", "Miastko", "Niepołomice", "Niemodlin", "Nidzica", "Olecko", "Olsztyn", "Opoczno", "Piaseczno",
"Piekary Śląskie", "Pajęczno", "Pabianice", "Racibórz", "Rabka", "Raciąż", "Radom", "Sandomierz", "Siedlce", "Sanok",
"Sosnowiec", "Tarchomin", "Tarczyn", "Tarnów", "Terespol", "Ujazd", "Ujście", "Ustroń", "Ustrzyki Dolne", "Wadowice",
"Wałbrzych", "Warszawa", "Wieliczka,Zalewo", "Ząbki", "Zamość"};
const string countries[] = {"Afganistan","Albania","Algieria","Andora","Angola","Antigua i Barbuda","Arabia Saudyjska","Argentyna",
"Argentyna","Armenia","Australia","Austria","Azerbejdżan","Bahamy","Bahrajn","Bangladesz","Barbados","Belgia","Belize",
"Benin","Bhutan","Białoruś","Boliwia","Bośnia i Hercegowina","Botswana","Brazylia","Brunei","Bułgaria","Burkina Faso",
"Burundi","Chile","Chiny","Chorwacja","Cypr","Czad","Czarnogóra","Czechy","Dania","Demokratyczna Republika Konga",
"Dominika","Dominikana","Dżibuti","Egipt","Ekwador","Erytrea","Estonia","Eswatini","Etiopia","Fidżi","Filipiny",
"Finlandia","Francja","Gabon","Gambia","Ghana","Grecja","Grenada","Gruzja","Gujana","Gwatemala","Gwinea","Haiti",
"Hiszpania","Holandia","Honduras","Indie","Indonezja","Irak","Iran","Irlandia","Islandia","Izrael","Jamajka","Japonia",
"Jemen","Jordania","Kambodża","Kamerun","Kanada","Katar","Kazachstan","Kenia","Kirgistan","Kiribati","Kolumbia","Komory",
"Kongo","Korea Południowa","Korea Północna","Kostaryka","Kuba","Kuwejt","Laos","Lesotho","Liban","Liberia","Libia",
"Liechtenstein","Litwa","Luksemburg","Łotwa","Macedonia Północna","Madagaskar","Malawi","Malediwy","Malezja","Mali",
"Malta","Maroko","Mauretania","Mauritius","Meksyk","Mikronezja","Mjanma","Mołdawia","Monako","Mongolia","Mozambik",
"Namibia","Nauru","Nepal","Niemcy","Niger","Nigeria","Nikaragua","Norwegia","Nowa Zelandia","Oman","Pakistan","Palau",
"Panama","Papua-Nowa Gwinea","Paragwaj","Peru","Polska","Południowa Afryka","Portugalia","Rosja","Rumunia","Rwanda",
"Salwador","Samoa","San Marino","Senegal","Serbia","Seszele","Sierra Leone","Singapur","Słowacja","Słowenia","Somalia",
"Sri Lanka","Stany Zjednoczone","Sudan","Surinam","Syria","Szwajcaria","Szwecja","Tadżykistan","Tajlandia","Tanzania",
"Timor Wschodni","Togo","Tonga","Trynidad i Tobago","Tunezja","Turcja","Turkmenistan","Tuvalu","Uganda","Ukraina",
"Urugwaj","Uzbekistan","Vanuatu","Watykan","Wenezuela","Węgry","Wielka Brytania","Wietnam","Włochy","Wybrzeże Kości Słoniowej",
"Wyspy Marshalla","Wyspy Salomona","Zambia","Zimbabwe"};
const string animals[] = {"ara", "antylopa", "albatros", "aligator", "anakonda", "alpaka", "arui", "biedronka", "bażant", "borsuk",
"bóbr", "chomik", "czapla", "chrabąszcz", "cielak", "cap", "czajka", "delfin", "dinozaur", "daniel", "dzięcioł",
"drozd", "dorsz", "dzik", "dudek", "emu", "empala", "eland", "elanus", "emberiza", "eremopila", "fenek", "foksterier",
"fretka", "flądra", "flaming", "goryl", "gołąb", "gęś", "gawron", "halibut", "hiena", "hipopotam", "indyk", "iguana",
"jaguar", "jastrząb", "jak", "jaskółka", "jamnik", "kot", "kogut", "koń", "kura", "lama", "lis", "lampart", "modliszka",
"muł", "mamut", "mrówka", "norka", "niedźwiedź", "nietoperz", "orka", "osa", "okoń", "padalec", "paź", "pelikan",
"rekin", "rak", "rybitwa", "słoń", "struś", "słowik", "tygrys", "tarantula", "tęcznik", "ukleja", "uszatek", "wilk",
"wiewiórka", "wydra", "zając", "zebra", "zaskroniec"};
const string plants[] = {"agawa", "aglonema", "akacja", "aloes", "ananas", "awokado", "arbuz", "agrest", "banan", "bób", "bakłażan",
"burak", "bez", "borówki", "cytryna", "cebula", "chryzantema", "czereśnia", "czerwony grejfrut", "dąb", "dynia",
"dracena", "daktyl", "dalia", "dąbrówka", "daskia", "eukaliptus", "echmea", "estragon", "fasola", "farbownik", "fołek",
"floks", "fikus", "gazeta", "guma", "garnitur", "grzech", "genitalia", "hiacynt", "haworsja", "hiacynta", "iksja",
"irga", "jarząb", "jałowiec", "jałówka", "kaczeniec", "kaktus", "kliwia", "klon", "klimek", "lilia", "lak", "lipa",
"maciejka", "marzecianka", "mak", "magnolia", "niezapominajka", "nagietek", "nemezja", "narcyz", "ogórek", "orzech",
"orlik", "papirus", "pelargonia", "pierwiosnek", "rannik", "rebucja", "rącznik", "rogownica", "róża", "sasanka",
"surmia", "szałwia", "szczawik", "trawa", "tulipan", "tuja", "ubiorek", "ukwap", "werbana", "wiązówka", "widliczka",
"wilec", "zawilec", "zimowit", "zielistka"};
const string names[] = {"Ala", "Alicja", "Anna", "Adam", "Adolf", "Aleksander", "Adelajda", "Ambroży", "Barbara", "Beata", "Bartek",
"Brandon" , "Błażej", "Borys", "Cyprian", "Cezary", "Daniel", "Dominik", "Dagmara", "Dobrusia", "Dalia", "Dawid",
"Dominika", "Eliza", "Edmund", "Edyta", "Edward", "Ewa", "Filip", "Frania", "Funda", "Florencja", "Fido", "Fabian",
"Feliks", "Gabriel", "Gabriela", "Hania", "Halina", "Henryk", "Iga", "Iza", "Justyna", "Karol", "Kamila", "Kamil",
"Karolina", "Kinga", "Liza", "Laura", "Lena", "Lucyfer", "Leszek", "Małgosia", "Mateusz", "Marek", "Natalia",
"Natan", "Nikola", "Ola", "Olek", "Paulina", "Paweł", "Patryk", "Piotrek", "Radek", "Róża", "Sebastian", "Staszek",
"Tomek", "Tadeusz", "Ula", "Wiktoria", "Wiktor", "Zuzia", "Zdzisław"};
const int roundsNumber = 2;


void * client_handler(void * arg);
void * timer_handler(void * arg);
void sendtoall(string message);
int iscountry(string word, char letter);
int iscity(string word, char letter);
int isplant(string word, char letter);
int isanimal(string word, char letter);
int isname(string word, char letter);
string placechecker(int fd);


#endif //SK_PROJ_SERVER_H

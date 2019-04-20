#include "Compression.h"

#include <iostream>
using namespace std;
#include "Matrix.h"
#include "BitStream.h"
#include "Math.h"

//initialiseren
Compression::Compression(string file, int width, int height):ma(height,width)
{
	this->file = file;
	this->width = width;
	this->height = height;
	xblock=yblock=4;
	qStart = 10;
	qRaise = 10;
	rij=new int[width*height];
	count=(int)max(xblock,yblock)*2-1;
	bits=new int[count];
}
Compression::Compression(int block, int qraise, int qstart, int width, int height)
{
	this->width = width;
	this->height = height;
	xblock=yblock=block;
	this->qRaise=qraise;
	this->qStart=qstart;
	rij=new int[width*height];
	count=(int)max(xblock,yblock)*2-1;
	bits=new int[count];
}

// raw file inlezen en de bytes ervan in de bitstreamreader plaatsen
int Compression::Read()
{
	cout << "Opening \'" << file.c_str() << "\'." << endl;
	FILE *fptr;
	if ( (fptr=fopen(file.c_str(), "rb")) == NULL){
		cerr << "Compression(Read): Can't open file " << file.c_str() << "." << endl;
		return 0;
	}

	// berekenen lengte file
	if( fseek(fptr, 0, SEEK_END) != 0){
		cerr << "Error: Can't move filepointer to the end of the file." << endl;
		system("pause");
		exit(0);
	}
	int length = ftell(fptr); // lengte van de file

	// filepointer vooraan te zetten om vanaf daar te lezen
	if( fseek(fptr, 0, SEEK_SET) != 0){
		cerr << "Error: Can't move filepointer to the end of the file." << endl;
		system("pause");
		exit(0);
	}
	ma.zeros();
	//matrix inlezen
	for(int i=0;i<height;i++)
		for(int j=0;j<width;j++)
			ma[i][j]=getc(fptr);
	fclose(fptr);
	//cout << "\"" << file.c_str() << "\" succesvol ingelezen." << endl;
	return 1;
}



//matrix vernieuwen voor hergebruik
void Compression::renewMatrix()
{
	Matrix<int> m(this->height,this->width);
	ma = m;
	ma.zeros();
	delete [] rij;
	rij=new int[width*height];
}

//dctmatrixvermenigvuldigingen
void Compression::dct(){
	Matrix<double> m(yblock,xblock);
	m.toDctMatrix();
	ma.dctmultiply1(m);
}

//quantiseren adhv de parameters
void Compression::quantisation(){
	Matrix<int> m(yblock,xblock);
	m.qInitialise(qStart,qRaise);
	ma.quanti(m);
}

//matrix in array omzetten
void Compression::toArray(){
	ma.zigzag(rij,yblock,xblock);
}

//run length encoding toepassen op de nullen
void Compression::rle(){
	
	int block=xblock*yblock,blbits=0,current=0,teller,move=-1,ph=0,rp=0,maxim=max(xblock,yblock)*2-1,record=1;
	int* mx=maxi();
	struct send s;
	int *row=new int[xblock*yblock];//hulprij
	bool busy=false;
	//maximum aantal bits nodig om nullen nea elkaar op te slaan
	for(int h=0;h<width*height;h++)
		if(rij[h]==0){//per 0 current omhoog
			current++;busy=true;
		}else if(busy=true && rij[h]!=0){
			record=max(record,current);busy=false;current=0;
		}
	if(busy=true)
		record=max(record,current);

	//bits nodig om maximum aantal nullen uit te drukken 
	while(record>=pow(2.0,blbits))
		blbits++;
	//aantal bits nodig om de maximumwaardes op te slaan
	for(int i=0;i<count;i++)
		while(pow(2.0,(double)(bits[i]))<=mx[i])
			bits[i]++;
	write=new BitStreamWriter(new unsigned __int8[((width*height*2*bits[0]+7*16)+7)/8],width*height*2*bits[0]+7*16);
	//de header opslaan
	write->put(16,height);
	write->put(16,width);
	write->put(16,yblock);
	write->put(16,xblock);
	write->put(10,qStart);
	write->put(10,qRaise);
	write->put(30,blbits);
	for(int i=0;i<count;i++)
		write->put(6,bits[i]);
	//zie zigzag in matrix.h voor uitleg
	//hulprij om het aantal benodigde bits te bepalen
	for(int i=0 ;i<maxim;i++){//telkens zijn de coordinaten samen i
		for(;ph>=0 && ph<=i &&ph<yblock && (i-ph)<xblock;ph+=move){
			row[rp++]=bits[i];
		}
		ph-=move;
		if( (ph+1)<yblock&&((move==-1 && (i-ph+1)>=xblock)||(move==1 && (i-ph)<=0))) 
			ph++;
		move=-move;//ph wisselt af tsn naar boven en naar beneden
	}
	//startwaarde om te overlopen
	s.current=rij[0];
	s.bits=row[0];
	s.nr=1;	
	for(int i=1;i<width*height;i++)
	{//de huidige is geen nul, tenzij de vorige waarde nog moet worden opgeslaan
		if(s.current!=0 ||( s.current==0 && 0!=rij[i])){
			if(s.current>=0)//tekenbit
				write->put_bit(0);
			else
				write->put_bit(1);
			//getal zelf
			write->put(s.bits,abs(s.current));
			//indien 0, aantal meegeven
			if(s.current==0)
				write->put(blbits,s.nr);
			//opnieuw tellen
			s.nr=1;
			s.bits=row[i%block];
			s.current=rij[i];
		}else{//anders 0 bijtellen
			s.current=0;
			s.nr++;
		}
	}//laatste element bijtellen
	if(s.current>=0)
			write->put_bit(0);
		else
			write->put_bit(1);
	write->put(s.bits,abs(s.current));
	if(s.current==0)
		write->put(blbits,s.nr);
	delete [] mx;
	delete [] row;
}


void Compression::rleb(){
	int block=xblock*yblock,blbits=0,current=0,teller,move=-1,ph=0,rp=0,maxim=max(xblock,yblock)*2-1,record=1;
	int* mx=maxi();
	struct send s;
	int *row=new int[xblock*yblock];//hulprij
	bool busy=false;
	//maximum aantal bits nodig om nullen nea elkaar op te slaan, max 
	for(int h=0;h<width*height;h++)
		if(rij[h]==0){//per 0 current omhoog
			current++;busy=true;
		}else if(busy=true && rij[h]!=0){
			record=max(record,current);busy=false;current=0;
		}
	if(busy=true)
		record=max(record,current);

	//bits nodig om maximum aantal nullen uit te drukken 
	while(record>=pow(2.0,blbits))
		blbits++;
	//aantal bits nodig om de maximumwaardes op te slaan
	for(int i=0;i<count;i++)
		while(pow(2.0,(double)(bits[i]))<=mx[i])
			bits[i]++;
	//de header opslaan
	write->put(9,blbits);
	for(int i=0;i<count;i++)
		write->put(6,bits[i]);
	//zie zigzag in matrix.h voor uitleg
	//hulprij om het aantal benodigde bits te bepalen
	for(int i=0 ;i<maxim;i++){//telkens zijn de coordinaten samen i
		for(;ph>=0 && ph<=i &&ph<yblock && (i-ph)<xblock;ph+=move){
			row[rp++]=bits[i];
		}
		ph-=move;
		if( (ph+1)<yblock&&((move==-1 && (i-ph+1)>=xblock)||(move==1 && (i-ph)<=0))) 
			ph++;
		move=-move;//ph wisselt af tsn naar boven en naar beneden
	}
	//startwaarde om te overlopen
	s.current=rij[0];
	s.bits=row[0];
	s.nr=1;	
	for(int i=1;i<width*height;i++)
	{//de huidige is geen nul, tenzij de vorige waarde nog moet worden opgeslaan
		if(s.current!=0 ||( s.current==0 && 0!=rij[i])){
			if(s.current>=0)//tekenbit
				write->put_bit(0);
			else
				write->put_bit(1);
			//getal zelf
			write->put(s.bits,abs(s.current));
			//cout << s.bits << " ";
			//indien 0, aantal meegeven
			if(s.current==0)
				write->put(blbits,s.nr);
			//opnieuw tellen
			s.nr=1;
			s.bits=row[i%block];
			s.current=rij[i];
		}else{//anders 0 bijtellen
			s.current=0;
			s.nr++;
		}
	}//laatste element bijtellen


	if(s.current>=0)
			write->put_bit(0);
		else
			write->put_bit(1);
	write->put(s.bits,abs(s.current));
	//cout << s.bits << "\n";
	if(s.current==0)
		write->put(blbits,s.nr);
	delete [] mx;
	delete [] row;
}

//maximum per diagonaal bepalen
int* Compression::maxi(){
	count=(int)max(xblock,yblock)*2-1;
	//alvast ook goede array voor bits/diagonaal aanmaken
	delete [] bits;
	bits =new int[count];
	int* maxi=new int[count];
	for(int i=0;i<count;i++)
	{	maxi[i]=0;
		bits[i]=0;	
	}
	//zie zigzagalgoritme in matrix.h
	int move=-1,ph=0,rp=0,mx=max(yblock,xblock)*2-1;
	for( int a =0; a<height; a+=yblock)
		for( int b=0; b<width; b+=xblock,move=-1,ph=0)
			for(int i=0 ;i<count;i++){//telkens zijn de coordinaten samen i
				for(;ph>=0 && ph<=i &&ph<yblock && (i-ph)<xblock;ph+=move){
					if(abs(rij[rp])>maxi[i])
						maxi[i]=abs(rij[rp]);
					rp++;
				}
				ph-=move;
				if( (ph+1)<yblock&&((move==-1 && (i-ph+1)>=xblock)||(move==1 && (i-ph)<=0))) ph++;
				move=-move;//ph wisselt af tsn naar boven en naar beneden
			}
	return maxi;
}

int Compression::getLengthFile()
{
	return write->get_position()/8;
}


int Compression::toFile(string fileName){
	cout << "Opening \'" << fileName.c_str() << "\'." << endl;
	FILE *fptr;
	if ( (fptr=fopen(fileName.c_str(), "wb")) == NULL){
		cerr << "Compression(Read): Can't open file " << file.c_str() << "." << endl;
		return 0;
	}

	// berekenen lengte file
	if( fseek(fptr, 0, SEEK_END) != 0){
		cerr << "Error: Can't move filepointer to the end of the file." << endl;
		system("pause");
		exit(0);
	}
	int length = ftell(fptr); // lengte van de file

	// filepointer vooraan te zetten om vanaf daar te lezen
	if( fseek(fptr, 0, SEEK_SET) != 0){
		cerr << "Error: Can't move filepointer to the end of the file." << endl;
		system("pause");
		exit(0);
	}
	util::write(fptr,*write);

	fclose(fptr);

	//cout << "\"" << fileName.c_str() << "\" succesvol weggeschreven." << endl;
	return 1;
}

Compression::~Compression(){
	delete [] rij;
	delete [] bits;
	//delete write;
}
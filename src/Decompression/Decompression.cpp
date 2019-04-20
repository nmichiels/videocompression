#include "Decompression.h"

#include <iostream>
using namespace std;
#include "Matrix.h"
#include "BitStream.h"

Decompression::Decompression(string file)
{
	this->file = file;
	ma = NULL;
}




Decompression::Decompression(BitStreamWriter *writer)
{
	this->write = writer;
}

int Decompression::Open(){
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
	write=new BitStreamWriter(new unsigned char[length],length*8);

	for(int j=0;j<length;j++)
		write->put(8,(unsigned __int32)getc(fptr));
	fclose(fptr);
	return 1;
	
}

void Decompression::Read()
{
	//header lezen
	read=new BitStreamReader(*write);
	height=read->get(16);
	width=read->get(16);
	ma=new Matrix<int>(height,width);
	yblock=read->get(16);
	xblock=read->get(16);
	rij=new int[height*width];
	qStart=read->get(10);
	qRaise=read->get(10);
	count=max(xblock,yblock)*2-1;
	blbits=read->get(30);
	bits=new int[count];
	for(int j=0;j<count;j++)
		bits[j]=read->get(6);
	//de rest inlezen
	antiRle();
	//cout << "\"" << file.c_str() << "\" succesvol ingelezen." << endl;

}

void Decompression::Pinitiate(int block=8, int qstart=2, int qraise=2, int macro=16){
	height=macro;
	width=macro;
	ma=new Matrix<int>(height,width);
	rij=new int[height*width];
	yblock=block;
	xblock=block;
	qStart=qstart;
	qRaise=qraise;
	count=max(xblock,yblock)*2-1;
	bits=new int[count];
}

void Decompression::Read2()
{
	//header lezen
	//read=new BitStreamReader(*write);
	blbits=read->get(9);
	for(int j=0;j<count;j++)
		bits[j]=read->get(6);
	//de rest inlezen
	antiRle();
	//cout << "\"" << file.c_str() << "\" succesvol ingelezen." << endl;

}

void Decompression::deletes()
{
	delete [] rij;
	delete ma;
	delete [] bits;
}

//dct omdraaien
void Decompression::antiDct(){
	Matrix<double> m(yblock,xblock);
	m.toDctMatrix();
	ma->dctmultiply1(m.transpose());
}
//quantisatie omdraaien
void Decompression::antiQuantisation(){
	Matrix<int> qua(xblock,yblock);
	qua.qInitialise(qStart,qRaise);
	ma->antiquanti(qua);
}
//array->matrix
void Decompression::toMatrix(){
	ma->antizigzag(rij,yblock,xblock);
}
void Decompression::antiRle(){
	int nr,current,sign,block=xblock*yblock,b=0,move=-1,ph=0,rp=0,maxi=max(xblock,yblock)*2-1;
	//eerst hulpmatrix opstellen om de bitwaarden te kennen
	int *row=new int[block];
	
	//hulprij om te helpen bepalen hoeveel bits moeten ingelezen worden
	for(int i=0 ;i<maxi;i++){//telkens zijn de coordinaten samen i
		for(;ph>=0 && ph<=i &&ph<yblock && (i-ph)<xblock;ph+=move)
			row[rp++]=bits[i];
		ph-=move;
		if( (ph+1)<yblock&&((move==-1 && (i-ph+1)>=xblock)||(move==1 && (i-ph)<=0))) 
			ph++;
		move=-move;//ph wisselt af tsn naar boven en naar beneden
	}

	int numel=width*height;
	for(int i=0;i<numel;)
	{
		b=row[i%block];
		//cout << b << " ";
		sign=read->get_bit();
		current=read->get(b);
		if(sign==1)//tekenbit toepassen
			current=-current;
		nr=0;
		if(current==0){//indien 0, aantal 
			nr=read->get(blbits);
			//cout << " rke:"<< nr << "\n ";
			for(int k=0;k<nr ;k++){
				rij[i++]=current;	
			}
		}else{//anders gewoon toekennen
			rij[i]=current;
			i+=1;
		}
	}
	delete [] row;
}


int Decompression::toFile(string str){
	FILE* fptr;
	if ( (fptr=fopen(str.c_str(),"wb")) == NULL)
	{
		cerr << "Decompression (toFile): Can't open file " << str.c_str() << "." << endl;
		return 0;
	}
	for(int a=0;a<height;a++)
		for(int b=0;b<width;b++)
			if ( (*ma)[a][b] > 255 )
				putc(255,fptr);
			else if ( (*ma)[a][b] < 0 )
				putc(0,fptr);
			else
				putc((*ma)[a][b],fptr);
	fclose(fptr);
	return 1;
}

void Decompression::deleteWriter()
{
	delete write;
}


Decompression::~Decompression(){
}
#ifndef MATRIX
#define MATRIX 1
#include <math.h>

template <typename E>
class Matrix{
public:
	int Height(){return height;}
	int Width() {return width;}
	void setHeight(int height){this->height =  height;}
	void setWidth(int width) {this->width =  width;}

	Matrix(int height=512, int width=512):width(width),height(height){
		matrix =new E*[height];
		for(int i=0;i<height;i++)
			matrix[i]=new E[width];
	}

 Matrix( Matrix& m){

		matrix =new E*[height=m.Height()];
		width=m.Width();
		for(int i=0;i<height;i++)
			matrix[i]=new E[width];

		for(int i=0;i<height;i++)
			for(int j=0; j<width;j++)
				matrix[i][j]=m[i][j];
	}
 void zeros(){
		for(int i=0;i<height;i++)
			for(int j=0;j<width;j++)
				matrix[i][j]=0;
	}

  void negatives(){
		for(int i=0;i<height;i++)
			for(int j=0;j<width;j++)
				matrix[i][j]=-1;
	}
 void Matrix<E>::deletes(){
	for(int i=0;i<height;i++)
			delete [] matrix[i];
		delete [] matrix;
 }

 void Matrix<E>::renew(){
	 matrix =new E*[height];
		for(int i=0;i<height;i++)
			matrix[i]=new E[width];
}
   Matrix<int> fromBlocks(int macro){
		Matrix<int> ma(height*macro,width*macro);
		for(int i=0;i<height;i++)
			for(int j=0;j<width;j++)
			{
				for(int a=0;a<macro;a++)
					for(int b=0;b<macro;b++)
						ma[i*macro+a][j*macro+b] = (*(this->matrix[i][j]))[a][b];
			}
		return ma;

   }

   void removeRoundingError(int macro){
	   macro = 8;
	   if (macro > 2)
	   {
		   int value = 0;
		   for (int i=1; (i<(height/macro)) && (i*macro < height); i++){
			   for (int j=1; (j<(width/macro)) && (j*macro < width); j++){
				   value += this->matrix[i*macro-1][j*macro-1];
				   value += this->matrix[i*macro-1][j*macro];
				   value += this->matrix[i*macro-1][j*macro+1];
				   value += this->matrix[i*macro][j*macro-1];	
				   //value += this->matrix[i*macro][j*macro];
				   value += this->matrix[i*macro][j*macro+1];
				   value += this->matrix[i*macro+1][j*macro-1];
				   value += this->matrix[i*macro+1][j*macro];
				   value += this->matrix[i*macro+1][j*macro+1];
				   this->matrix[i*macro][j*macro] = value/8;
				   value=0;
			   }
		   }
	   }
   }
		   

  E** getMatrix(){return matrix;}

		    Matrix<Matrix<int>*> toBlocks(int macro){
		Matrix<Matrix<int>*> ma(height/macro,width/macro);
		Matrix<int> * cur;
		for(int i=0;i<height/macro;i++)
			for(int j=0;j<width/macro;j++)
			{
				ma[i][j]=new Matrix<int>(macro,macro);
				ma[i][j]->zeros();
				cur=ma[i][j];
				for(int a=0;a<macro;a++)
					for(int b=0;b<macro;b++)
						(*cur)[a][b]=matrix[a+i*macro][b+j*macro];
			}
		return ma;

  }


double calcMad(Matrix<int> &R, int a, int b){
	double N2=height*height;
	double MAD=0;
	for(int i=0;i<height;i++)
		for(int j=0;j<width;j++)
			MAD+=abs((double)(matrix[i][j]-R[i+a][j+b]));
	return MAD/N2;
  }

 void Matrix<E>::ones(){
		for(int i=0;i<height;i++)
			for(int j=0;j<width;j++)
				matrix[i][j]=0;
	}
/* void Matrix<E>::renew(){
	matrix =new E*[height];
		for(int i=0;i<height;i++)
			matrix[i]=new E[width];
	for(int i=0;i<height;i++)
			for(int j=0;j<width;j++)
				matrix[i][j]=0;
 }*/

E* operator[](int i){
		return matrix[i];
	}

const Matrix operator= (Matrix<E> &m){
		if(matrix != NULL)
		{
			for(int i=0;i<height;i++)
				delete [] matrix[i];
			delete [] matrix;
		}
		matrix =new E*[height=m.Height()];
		width=m.Width();
		for(int i=0;i<height;i++)
			matrix[i]=new E[width];
		for(int i=0;i<height;i++)
			for(int j=0; j<width;j++)
				matrix[i][j]=m[i][j];
		return *this;
	}


Matrix transpose(){
		Matrix m(width,height);
		for(int i=0; i<width ; i++)
			for(int j=0;j<height;j++)
				m[j][i]=matrix[i][j];
		return m;
	}

void dctmultiply1(Matrix<double> &m){
	Matrix<double> m2(height,width);
	m2.zeros();
	for (int i=0;i<height;i+=m.Height())
		for(int j=0;j<width;j+=m.Width())
			for(int a=0;a<m.Height();a++)
				for(int b=0;b<m.Width();b++)
					for(int c=0;c<m.Height();c++)
						m2[i+a][j+b]+=(m[a][c]*(double)matrix[c+i][j+b]);

	Matrix<double> m3(height,width);
	m3.zeros();
	m=m.transpose();
	for (int i=0;i<height;i+=m.Height())
		for(int j=0;j<width;j+=m.Width())
			for(int a=0;a<m.Height();a++)
				for(int b=0;b<m.Width();b++)
					for(int c=0;c<m.Height();c++)
						m3[i+a][j+b]+=(m2[i+a][c+j]*m[c][b]);

	for(int i=0;i<height;i++)
		for(int j=0;j<width;j++)
			matrix[i][j]=(int)(m3[i][j]+0.5);
}
 

int maximum(){
	int m=0;
	for(int i=0;i<height;i++)
		for(int j=0;j<width;j++)
			if(m<matrix[i][j])
				m=matrix[i][j];
return m;
}

int minimum(){
	int m=0;
	for(int i=0;i<height;i++)
		for(int j=0;j<width;j++)
			if(m>matrix[i][j])
				m=matrix[i][j];
return m;
}

void antiquanti(Matrix &m){
	for (int i=0;i<height;i+=m.Height())
		for(int j=0;j<width;j+=m.Width())
			for(int a=0;a<m.Height();a++)
				for(int b=0;b<m.Width();b++)
					matrix[i+a][j+b]=matrix[i+a][j+b]*m[a][b];

}

~Matrix(){
	for(int i=0;i<height;i++)
			delete [] matrix[i];
		delete [] matrix;
}

void antizigzag(E* rij, int height, int width){
	int move=-1,ph=0,rp=0,mx=max(width,height)*2-1;
	for( int a =0; a<this->height; a+=height)
		for( int b=0; b<this->width; b+=width,move=-1,ph=0)
			for(int i=0 ;i<mx;i++){//telkens zijn de coordinaten samen i
				for(;ph>=0 && ph<=i &&ph<height && (i-ph)<width;ph+=move)
					matrix[a+ph][b+i-ph]=rij[rp++];
				ph-=move;
				if( (ph+1)<height&&((move==-1 && (i-ph+1)>=width)||(move==1 && (i-ph)<=0))) ph++;
				move=-move;//ph wisselt af tsn naar boven en naar beneden
			}
}

void qInitialise( int start, int raise){
	int move=1,ph=0,rp=0,mx=max(width,height)*2-1,current=start;
	matrix[0][0]=start;
	for(int i=1 ;i<mx;i++){//telkens zijn de coordinaten samen i
		for(;ph>=0 && ph<=i &&ph<height && (i-ph)<width;ph+=move)
			matrix[ph][i-ph]=current;
		ph-=move;
		current+=raise;
		if( (ph+1)<height&&((move==-1 && (i-ph+1)>=width)||(move==1 && (i-ph)<=0))) ph++;
			move=-move;//ph wisselt af tsn naar boven en naar beneden
	}
}

void toDctMatrix(){
	for(int i=0;i<height;i++)
		for(int j=0;j<width;j++)
		{
			if(i==0)
				matrix[i][j]=sqrt(1.0/(double)height)*cos((2.0*(double)j+1.0)*(double)i*3.14/(2.0*(double)height));
			else
				matrix[i][j]=sqrt(2.0/(double)height)*cos((2.0*(double)j+1.0)*(double)i*3.14/(2.0*(double)height));
		}
}

E** matrix;

private:
	int width;
	int height;
	



};
#endif
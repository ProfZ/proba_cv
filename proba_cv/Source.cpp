//#include <stdio.h>
//#include <stdlib.h>
//#include <cmath>
//#include <opencv\cv.h>
//#include <opencv\highgui.h>
//#include <opencv2\core\core.hpp>
////#include <opencv2\ocl\ocl.hpp>		//dodatne bibilioteke za dalje ubrzanje koda
////#include <opencv2\gpu\gpu.hpp>		//koriste graficki procesor
//
//using namespace cv;
////using namespace cv::ocl;
//using namespace std;
//
//enum Sablon {
//	_TOP_BOTTOM_S,
//	_TOP_BOTTOM_D,
//	_HEAD_TOP_BOTTOM_D,
//	_HEAD_TOP_BOTTOM_D_BODY
//};
//
//class CvMarker {
//public:
//	int posX;				//X koordinata tacke
//	int posY;				//Y koordinata tacke
//	int deltaX[2];			//nizovi koji govore o 2 zadnje brzine tacke po X
//	int deltaY[2];			//nizovi koji govore o 2 zadnje brzine tacke po Y
//	double opseg;			//
//	double greska;			//indikacija koliko pomeraja (pixela) oprastamo radi jako malih promena i pazenja na oscilacije slike
//
//	CvMarker *prethodni;	//za kretanje po odredjenoj listi tacaka
//	CvMarker *naredni;
//
//	CvMarker() : posX(-1), posY(-1), prethodni(NULL), naredni(NULL), opseg(600), greska(0.001) {
//		deltaX[0] = 0; deltaX[1] = 0;
//		deltaY[0] = 0; deltaY[1] = 0;
//	}
//	CvMarker(const CvMarker& x){
//		this->posX = x.posX; this->posY = x.posY;
//		this->deltaX[0] = x.deltaX[0]; this->deltaX[1] = x.deltaX[1];
//		this->deltaY[0] = x.deltaY[0]; this->deltaY[1] = x.deltaY[1];
//		this->opseg = x.opseg; this->greska = x.greska;
//		this->prethodni = x.prethodni; this->naredni = x.naredni;
//	}
//};
//
//const int _NUMBER_OF_MARKERS = 2;
//const double greska = 0.0;
//
//int * pocetniX, *pocetniY;
//unsigned DAJ = 0;
//Sablon sablon = _TOP_BOTTOM_S;
//
//double pede_fast_atan(double x){
//	return (x*((64*x*x+735)*x*x+945))/((225*x*x+1050)*x*x + 945);
//}
//double pede_fast_asin(double x){
//	return (x*((69049 * x*x - 717780)*x*x + 922320)) / ((145125 * x*x - 871500)*x*x + 922320);
//}
//double pede_fast_tan(double x){
//	return (x*((x*x - 105)*x*x + 945)) / ((15 * x*x - 420)*x*x + 945);
//}
//double pede_fast_sin(double x){
//	return (x*((551 * x*x - 22260)*x*x + 166320)) / ((75 * x*x + 5460)*x*x + 166320);
//}
//double pede_fast_cos(double x){
//	return ((313*x*x - 6900)*x*x+15120)/((13*x*x+660)*x*x+15120);
//}
//
//bool inicialize_start(CvMarker *CVM) {
//	if (sablon == _HEAD_TOP_BOTTOM_D_BODY || sablon == _TOP_BOTTOM_D) {
//		if (_NUMBER_OF_MARKERS % 2 != 0)
//			return false;
//	}
//	else if (sablon == _HEAD_TOP_BOTTOM_D) {
//		if (_NUMBER_OF_MARKERS % 2 != 1)
//			return false;
//	}
//	pocetniX = (int*)calloc(_NUMBER_OF_MARKERS, sizeof(int));
//	pocetniY = (int*)calloc(_NUMBER_OF_MARKERS, sizeof(int));
//	return true;
//}
//
///*returns index of clossest Marker
//*/
//int justCompareClossest(int x, int y, CvMarker *CVM){
//	double *niz = (double *)calloc(_NUMBER_OF_MARKERS, sizeof(double));
//	for (int i = 0; i < _NUMBER_OF_MARKERS; ++i){
//		niz[i] = (double)sqrt(pow((double)CVM[i].posX - (double)x, 2) + pow((double)CVM[i].posY - (double)y, 2));
//	}
//	double min = niz[0];
//	for (int i = 1; i<_NUMBER_OF_MARKERS; ++i){
//		if (min > niz[i])
//			min = niz[i];
//	}
//	for (int i = 0; i<_NUMBER_OF_MARKERS; ++i) {
//		if ( min == niz[i]) {
//			return i;
//		}
//	}
//	return -1;
//}
//
///*compares Point with Markers and clossest is moved to new location
//*/
//void compareClossest(int x, int y, CvMarker *CVM){
//	for (int i = 0; i < _NUMBER_OF_MARKERS; ++i) {
//		if (pocetniX[i] == 0 || pocetniY[i] == 0) return ;
//	}
//	double *niz = (double *)calloc(_NUMBER_OF_MARKERS, sizeof(double));
//	for (int i = 0; i < _NUMBER_OF_MARKERS; ++i){
//		niz[i] = (double)sqrt(pow((double)CVM[i].posX - (double)x, 2) + pow((double)CVM[i].posY - (double)y, 2));
//	}
//	double min = niz[0];
//	for (int i = 1; i<_NUMBER_OF_MARKERS; ++i){
//		if (min > niz[i])
//			min = niz[i];
//	}
//	for (int i = 0; i<_NUMBER_OF_MARKERS; ++i) {
//		if (min == niz[i]) {
//			CVM[i].deltaX[0] = CVM[i].deltaX[1];
//			CVM[i].deltaX[1] = CVM[i].posX - x;
//			CVM[i].deltaY[0] = CVM[i].deltaY[1];
//			CVM[i].deltaY[1] = CVM[i].posY - y;
//			CVM[i].posX = x; CVM[i].posY = y;
//			return;
//		}
//	}
//}
//
///* void function for initial setting up Markers by built in paterns
//*/
//void setUpStartingPoints(CvMarker *CVM, Sablon sab) {
//	int i;
//	int a = _NUMBER_OF_MARKERS / 2 - 1;
//	int b = _NUMBER_OF_MARKERS - 1;
//	int c = a + 1;
//
//	if (sab == _TOP_BOTTOM_S){
//		CVM[0].prethodni = NULL;
//		CVM[0].naredni = &CVM[1];
//		CVM[0].posX = pocetniX[0];
//		CVM[0].posY = pocetniY[0];
//		CVM[b].prethodni = &CVM[b-1];
//		CVM[b].naredni = NULL;
//		CVM[b].posX = pocetniX[b];
//		CVM[b].posY = pocetniY[b];
//		for (i = 1; i < b;  ++i){
//			CVM[i].prethodni = &CVM[i - 1];
//			CVM[i].naredni = &CVM[i + 1];
//			CVM[i].posX = pocetniX[i];
//			CVM[i].posY = pocetniY[i];
//		}
//	}
//	else if (sab == _TOP_BOTTOM_D){
//		CVM[0].prethodni = NULL;
//		CVM[0].naredni = &CVM[1];
//		CVM[0].posX = pocetniX[0];
//		CVM[0].posY = pocetniY[0];
//		CVM[a].prethodni = &CVM[a - 1];
//		CVM[a].naredni = NULL;
//		CVM[a].posX = pocetniX[a];
//		CVM[a].posY = pocetniY[a];
//		for (i = 1; i < a; ++i){
//				CVM[i].prethodni = &CVM[i - 1];
//				CVM[i].naredni = &CVM[i + 1];
//				CVM[i].posX = pocetniX[i];
//				CVM[i].posY = pocetniY[i];
//		}
//		CVM[c].prethodni = NULL;
//		CVM[c].naredni = &CVM[c + 1];
//		CVM[c].posX = pocetniX[c];
//		CVM[c].posY = pocetniY[c];
//		CVM[b].prethodni = &CVM[b - 1];
//		CVM[b].naredni = NULL;
//		CVM[b].posX = pocetniX[b];
//		CVM[b].posY = pocetniY[b];
//		for (i = a + 1; i < _NUMBER_OF_MARKERS; ++i){
//				CVM[i].prethodni = &CVM[i - 1];
//				CVM[i].naredni = &CVM[i + 1];
//				CVM[i].posX = pocetniX[i];
//				CVM[i].posY = pocetniY[i];
//		}
//	}
//	else if (sab == _HEAD_TOP_BOTTOM_D){
//		int tp = (_NUMBER_OF_MARKERS - 1) / 2;
//		CVM[0].prethodni = NULL;
//		CVM[0].naredni = &CVM[1];
//		CVM[0].posX = pocetniX[0];
//		CVM[0].posY = pocetniY[0];
//		CVM[1].prethodni = &CVM[0];
//		CVM[1].naredni = &CVM[2];
//		CVM[1].posX = pocetniX[1];
//		CVM[1].posY = pocetniY[1];
//		CVM[tp].prethodni = &CVM[0];
//		CVM[tp].naredni = &CVM[tp + 1];
//		CVM[tp].posX = pocetniX[tp];
//		CVM[tp].posY = pocetniY[tp];
//		for (i = 2; i < _NUMBER_OF_MARKERS; ++i){
//			if (i != tp){
//				CVM[i].prethodni = &CVM[i - 1];
//				CVM[i].naredni = &CVM[i + 1];
//				CVM[i].posX = pocetniX[i];
//				CVM[i].posY = pocetniY[i];
//			}
//
//		}
//	}
//	else if (sab == _HEAD_TOP_BOTTOM_D_BODY){
//		//i ovo treba realizovati kada budem mogao u skladu 
//		//sa onim dole sto isto nije realizovano
//	}else {
//		printf("\npogresan unos za format markera");
//		return;
//	}
//}
//
///*void function for initial sort of markers by built in paterns
//*/
//void sortMarkers(CvMarker *CVM, Sablon sab){
//	int i, j, tempX, tempY;
//	if (sab == _TOP_BOTTOM_S){										    //redosledno.gore.dole - red u nizu
//		for (i = 0; i < _NUMBER_OF_MARKERS - 1; ++i){
//			for (j = i + 1; j < _NUMBER_OF_MARKERS; ++j){
//				if (pocetniY[i] > pocetniY[j]){
//					tempX = pocetniX[i];
//					tempY = pocetniY[i];
//					pocetniX[i] = pocetniX[j]; 
//					pocetniX[j] = tempX;
//					pocetniY[i] = pocetniY[j];
//					pocetniY[j] = tempY;
//				}
//			}
//		}
//	}
//	else if (sab == _TOP_BOTTOM_D) {								    //leva-ruka.desna-ruka - red u nizu
//		for (i = 0; i < _NUMBER_OF_MARKERS -1; ++i){
//			for (j = i + 1; j < _NUMBER_OF_MARKERS; ++j)
//				if (pocetniX[i] > pocetniX[j]){
//					tempX = pocetniX[i];
//					tempY = pocetniY[i];
//					pocetniX[i] = pocetniX[j]; 
//					pocetniX[j] = tempX;
//					pocetniY[i] = pocetniY[j]; 
//					pocetniY[j] = tempY;
//			}
//		}
//		for (i = 0; i < _NUMBER_OF_MARKERS / 2 - 1; ++i)
//			for (j = i + 1; j < _NUMBER_OF_MARKERS / 2; ++j)
//				if (pocetniY[i] > pocetniY[j]){
//					tempX = pocetniX[i];
//					tempY = pocetniY[i];
//					pocetniX[i] = pocetniX[j]; 
//					pocetniX[j] = tempX;
//					pocetniY[i] = pocetniY[j]; 
//					pocetniY[j] = tempY;
//		}
//		for (i = _NUMBER_OF_MARKERS / 2; i < _NUMBER_OF_MARKERS - 1; ++i)
//			for (j = i + 1; j < _NUMBER_OF_MARKERS; ++j)
//				if (pocetniY[i] > pocetniY[j]){
//					tempX = pocetniX[i];
//					tempY = pocetniY[i];
//					pocetniX[i] = pocetniX[j]; 
//					pocetniX[j] = tempX;
//					pocetniY[i] = pocetniY[j]; 
//					pocetniY[j] = tempY;
//		}
//	}
//	else if (sab == _HEAD_TOP_BOTTOM_D){								//glava.leva-ruka.desna-ruka - red u nizu
//		for (i = 0; i < _NUMBER_OF_MARKERS -1; ++i){
//			for (j = i + 1; j < _NUMBER_OF_MARKERS; ++j){
//				if (pocetniY[i] > pocetniY[j]){
//					tempX = pocetniX[i];
//					tempY = pocetniY[i];								//trazimo "glavu"
//					pocetniX[i] = pocetniX[j]; 
//					pocetniX[j] = tempX;
//					pocetniY[i] = pocetniY[j]; 
//					pocetniY[j] = tempY;
//				}
//			}
//		}
//		for (i = 1; i < _NUMBER_OF_MARKERS - 1; ++i){
//			for (j = i + 1; j < _NUMBER_OF_MARKERS; ++j){
//				if (pocetniX[i] > pocetniX[j]){
//					tempX = pocetniX[i];
//					tempY = pocetniY[i];								//sortitramo po X, da bi otkrili ruke
//					pocetniX[i] = pocetniX[j]; 
//					pocetniX[j] = tempX;
//					pocetniY[i] = pocetniY[j];
//					pocetniY[j] = tempY;
//				}
//			}
//		}
//		for (i = 1; i < (_NUMBER_OF_MARKERS - 1) / 2 -1; ++i){
//			for (j = i + 1; j < (_NUMBER_OF_MARKERS - 1) / 2; ++j){
//				if (pocetniY[i] > pocetniY[j]){
//					tempX = pocetniX[i];								//"levu" ruku sortiramo po Y
//					tempY = pocetniY[i];
//					pocetniX[i] = pocetniX[j]; 
//					pocetniX[j] = tempX;
//					pocetniY[i] = pocetniY[j]; 
//					pocetniY[j] = tempY;
//				}
//			}
//		}
//		for (i = (_NUMBER_OF_MARKERS - 1) / 2; i < _NUMBER_OF_MARKERS -1; ++i){
//			for (j = i + 1; j < _NUMBER_OF_MARKERS; ++j){
//				if (pocetniY[i] > pocetniY[j]){
//					tempX = pocetniX[i];
//					tempY = pocetniY[i];								//"desnu" ruku sortiramo po Y
//					pocetniX[i] = pocetniX[j];
//					pocetniX[j] = tempX;
//					pocetniY[i] = pocetniY[j]; 
//					pocetniY[j] = tempY;
//				}
//			}
//		}
//
//	}
//	else if (sab == _HEAD_TOP_BOTTOM_D_BODY ) {                         //glava.leva-ruka.telo.desna-ruka - red u nizu
//		for (i = 0; i < _NUMBER_OF_MARKERS -1; ++i){
//			for (j = i + 1; j < _NUMBER_OF_MARKERS; ++j){
//				if (pocetniY[i] > pocetniY[j]){
//					tempX = pocetniX[i];
//					tempY = pocetniY[i];								//trazimo "glavu"
//					pocetniX[i] = pocetniX[j]; 
//					pocetniX[j] = tempX;
//					pocetniY[i] = pocetniY[j]; 
//					pocetniY[j] = tempY;
//				}
//			}
//		}
//		for (i = 1; i < _NUMBER_OF_MARKERS - 1; ++i){
//			for (j = i + 1; j < _NUMBER_OF_MARKERS; ++j){
//				if (pocetniX[i] > pocetniX[j]){
//					tempX = pocetniX[i];
//					tempY = pocetniY[i];								//sortitramo po X, da bi otkrili ruke
//					pocetniX[i] = pocetniX[j]; 
//					pocetniX[j] = tempX;
//					pocetniY[i] = pocetniY[j];
//					pocetniY[j] = tempY;
//				}
//			}
//		}
//		for (i = 1; i < (_NUMBER_OF_MARKERS - 2) / 2 - 1; ++i){
//			for (j = i + 1; j < (_NUMBER_OF_MARKERS - 2) / 2; ++j){
//				if (pocetniY[i] > pocetniY[j]){
//					tempX = pocetniX[i];								//"levu" ruku sortiramo po Y
//					tempY = pocetniY[i];
//					pocetniX[i] = pocetniX[j]; 
//					pocetniX[j] = tempX;
//					pocetniY[i] = pocetniY[j]; 
//					pocetniY[j] = tempY;
//				}
//			}
//		}
//		for (i = _NUMBER_OF_MARKERS / 2 + 1; i < _NUMBER_OF_MARKERS -1; ++i){
//			for (j = i + 1; j < _NUMBER_OF_MARKERS; ++j){
//				if (pocetniY[i] > pocetniY[j]){
//					tempX = pocetniX[i];
//					tempY = pocetniY[i];								//"desnu" ruku sortiramo po Y
//					pocetniX[i] = pocetniX[j];
//					pocetniX[j] = tempX;
//					pocetniY[i] = pocetniY[j]; 
//					pocetniY[j] = tempY;
//				}
//			}
//		}
//	}
//	else {
//		printf("\npogresan unos za format markera");
//		return;
//	}
//}
//
//void findStartingPoints(IplImage *img, CvMarker *CVM){
//	CvSeq * konture;
//	CvSeq * rez;
//	CvSeq * vazne = new CvSeq[_NUMBER_OF_MARKERS];
//	int x = 0, i  = 0, j =0;
//	int tempx = 0, tempy = 0, tempx1 = 0, tempy1 = 0, tempx2 = 0, tempy2 =0;
//	CvMemStorage * skladiste = cvCreateMemStorage(0);
//	cvFindContours(img, skladiste, &konture, sizeof(CvContour), CV_RETR_LIST, CV_LINK_RUNS, cvPoint(0, 0));
//
//		if (konture != NULL){
//			try{
//				if (konture->total >= _NUMBER_OF_MARKERS){
//					while (konture) {
//						rez = cvApproxPoly(konture, sizeof(CvContour), skladiste, CV_POLY_APPROX_DP, cvContourPerimeter(konture)*0.04, 0);
//						if (rez->total == 4 && fabs(cvContourArea(rez, CV_WHOLE_SEQ)) > 300){
//							if (x < _NUMBER_OF_MARKERS) vazne[x++] = *rez;
//							else ++x;
//						}
//						konture = konture->h_next;
//					}
//				}
//			}
//			catch (...){
//				printf("nis\n bate\n");
//			}
//		}
//		if (x == _NUMBER_OF_MARKERS){
//			CvPoint *pt;
//			for (i = 0; i < _NUMBER_OF_MARKERS; ++i) {
//				pocetniX[i] = 0;
//				pocetniY[i] = 0;
//				for (j = 0; j < 4; ++j){
//					pt = (CvPoint*)cvGetSeqElem(&vazne[i], j);
//					pocetniX[i] += pt->x;
//					pocetniY[i] += pt->y;
//				}
//				pocetniX[i] /= 4;
//				pocetniY[i] /= 4;
//			}
//			if (DAJ == 0){
//				sortMarkers(CVM, sablon);
//				setUpStartingPoints(CVM, sablon);
//				printf("\n------------------------------\npronadjeni markeri\n------------------------------\n");
//				++DAJ;
//			}
//			else {
//				for (i = 0; i < _NUMBER_OF_MARKERS; ++i){
//					compareClossest(pocetniX[i], pocetniY[i], CVM);
//				}
//			}
//		}
//		else if (x == _NUMBER_OF_MARKERS - 1 && DAJ > 0){
//			CvPoint *pt;
//			int ttemp = 0;
//			int s = 0;
//			int suma = 0;
//			for (i = 0; i < _NUMBER_OF_MARKERS - 1; ++i ){
//				if ((ttemp = justCompareClossest(pocetniX[i], pocetniY[i], CVM)) == -1)
//					return;
//				s += ttemp;
//				suma += i+1;
//			}
//						// e  sada da probamo algoritam neki da smislimo da racuna gde je nasa nesreca od markera
//			suma -= s;  //taj marker je uteko
//
//			if (suma <0 || suma >_NUMBER_OF_MARKERS -1 )
//				return;
//			if (suma != 0) {
//				for (i = 0; i < suma; ++i){
//					pocetniX[i] = 0; 
//					pocetniY[i] = 0;
//					for (j = 0; j < 4; ++j){
//						pt = (CvPoint*)cvGetSeqElem(&vazne[i], j);
//						pocetniX[i] += pt->x;
//						pocetniY[i] += pt->y;
//					}	
//					pocetniX[i] /= 4;
//					pocetniY[i] /= 4;
//					compareClossest(pocetniX[i], pocetniY[i], CVM);
//				}
//				for (i = suma + 1; i < _NUMBER_OF_MARKERS; ++i){
//					pocetniX[i] = 0;
//					pocetniY[i] = 0;
//					for (j = 0; j < 4; ++j){
//						pt = (CvPoint*)cvGetSeqElem(&vazne[i], j);
//						pocetniX[i] += pt->x;
//						pocetniY[i] += pt->y;
//					}
//					pocetniX[i] /= 4;
//					pocetniY[i] /= 4;
//					compareClossest(pocetniX[i], pocetniY[i], CVM);
//				}
//			}else{
//				for (i = 1; i < _NUMBER_OF_MARKERS; ++i){
//					pocetniX[i] = 0; 
//					pocetniY[i] = 0;
//					for (j = 0; j < 4; ++j){
//						pt = (CvPoint*)cvGetSeqElem(&vazne[i-1], j);
//						pocetniX[i] += pt->x;
//						pocetniY[i] += pt->y;
//					}
//					pocetniX[i] /= 4;
//					pocetniY[i] /= 4;
//					compareClossest(pocetniX[i], pocetniY[i], CVM);
//				}
//			}
//
//			i = 0; j = 0;
//			int deltaXplus = 0, deltaYplus = 0, deltaXminus = 0, deltaYminus = 0;
//			tempx = CVM[suma].deltaX[1];
//			tempy = CVM[suma].deltaY[1];
//			CVM[suma].deltaX[1] = 2 * CVM[suma].deltaX[1] - CVM[suma].deltaX[0];				   //nova brzina po X koordinati
//			CVM[suma].deltaY[1] = 2 * CVM[suma].deltaY[1] - CVM[suma].deltaY[0];				   //nova brzina po Y koordinati
//			CVM[suma].deltaX[0] = tempx;														   //najstariju brzinu menjamo sa starom
//			CVM[suma].deltaY[0] = tempy;
//			if (CVM[suma].naredni != NULL){
//				++i; 
//				deltaXplus = CVM[suma + 1].deltaX[1]; 
//				deltaYplus = CVM[suma + 1].deltaY[1];
//			}
//			if (CVM[suma].prethodni != NULL){
//				++j; 
//				deltaXminus = CVM[suma - 1].deltaX[1]; 
//				deltaYminus = CVM[suma - 1].deltaY[1];
//			}
//			CVM[suma].deltaX[1] = ( CVM[suma].deltaX[1] + deltaXplus + deltaXminus) / (i + 1 + j); // sada je nova aproksimirana brzina po x i po y u funkciji od starih brzina 
//			CVM[suma].deltaY[1] = ( CVM[suma].deltaY[1] + deltaYplus + deltaYminus) / (i + 1 + j); // i od brzina prethodne i sledece tacke
//
//			//if (CVM[suma].deltaX[1] <= CVM[suma].greska)
//				//CVM[suma].deltaX[1] = 0;															// moram ograniciti, da se ne bi uvek smanjivao/povecavao razmak
//			//if (CVM[suma].deltaY[1] <= CVM[suma].greska)
//				//CVM[suma].deltaY[1] = 0;
//			
//			CVM[suma].posX = CVM[suma].posX - CVM[suma].deltaX[1];									// nove koordinate za marker
//			CVM[suma].posY = CVM[suma].posY - CVM[suma].deltaY[1];									//
//			pocetniX[suma] = CVM[suma].posX;														// i moramo u niz da stavimo te vrednosti
//			pocetniY[suma] = CVM[suma].posY;														// zarad sledece iteracije
//		}
//	cvReleaseMemStorage(&skladiste);																//oslobadjanje memorije
//}
//
//
///* Draws lines that are connecting Markers by built in paterns
//*/
//void drawImage(IplImage *f, CvMarker *CVM, Sablon sab){
//	if (sab == _TOP_BOTTOM_S || sab == _TOP_BOTTOM_D){
//		for (int i = 0; i < _NUMBER_OF_MARKERS; ++i){
//			if (CVM[i].naredni != NULL)
//				cvLine(f, cvPoint(CVM[i].posX, CVM[i].posY), cvPoint(CVM[i].naredni->posX, CVM[i].naredni->posY), cvScalar(50, 70, 60, 4));
//		}
//	}
//	else if (sab == _HEAD_TOP_BOTTOM_D){
//		
//			cvLine(f, cvPoint(CVM[0].posX, CVM[0].posY), cvPoint(CVM[1].posX, CVM[1].posY), cvScalar(100, 100, 100, 4));
//			cvLine(f, cvPoint(CVM[0].posX, CVM[0].posY), cvPoint(CVM[(_NUMBER_OF_MARKERS - 1) / 2].posX, CVM[(_NUMBER_OF_MARKERS - 1) / 2].posY), cvScalar(100, 100, 100, 4));
//			for (int i = 1; i < _NUMBER_OF_MARKERS; ++i){
//				if (CVM[i].naredni != NULL)
//					cvLine(f, cvPoint(CVM[i].posX, CVM[i].posY), cvPoint(CVM[i].naredni->posX, CVM[i].naredni->posY), cvScalar(50, 70, 60, 4));
//			}
//	}
//	else if (sab == _HEAD_TOP_BOTTOM_D_BODY){
//		// realizovati
//	}
//}
//
//
//IplImage *getThresholdedImage(IplImage *imgHsv){
//	IplImage *imgThresh = cvCreateImage(cvGetSize(imgHsv), IPL_DEPTH_8U, 1);
//	cvInRangeS(imgHsv, cvScalar(22, 137, 95), cvScalar(33, 256, 256), imgThresh); 			 //HSV transformacija slike u crno belu (moja soba)
//	/*IplImage *imgThresh2 = cvCreateImage(cvGetSize(imgThresh), IPL_DEPTH_8U, 1);
//	cvSmooth(imgThresh, imgThresh2, CV_BILATERAL, 0,0,21,3);
//	cvShowImage("Bilateral", imgThresh2);*/
//	//cvInRangeS(imgHsv, cvScalar(26, 49, 184), cvScalar(36, 256, 256), imgThresh);          //ovo je u racunarskom centru osvetljenje
//	return imgThresh; //zuto trazi po slici bate
//}
//
//int main() {
//	
//	CvMarker* CMarkeri = static_cast<CvMarker*>(::operator new (sizeof CvMarker * _NUMBER_OF_MARKERS));
//	for (size_t i = 0; i < _NUMBER_OF_MARKERS; ++i) //staticno alociranje niza markera
//		new(&CMarkeri[i]) CvMarker();
//
//	if (!inicialize_start(CMarkeri)) {
//		printf("\nNevazeca kombinacija:");
//		return -1;
//	}
//	printf("JEBEM TI MAJKU");
//	CvCapture *capture = 0;
//	capture = cvCaptureFromCAM(0);
//	if (!capture){
//		printf("Neuspelo\n");
//		return -1;
//	}
//	IplImage *frame = 0;
//	frame = cvQueryFrame(capture);
//	if (!frame) return -1;
//	IplImage *imgTracking = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 3);
//	cvNamedWindow("Video");
//	cvNamedWindow("HSV->Black_white");
//	IplImage *imgGrayScale;
//	while (true){
//		frame = cvQueryFrame(capture);
//		if (!frame)break;
//		frame = cvCloneImage(frame);
//		cvCvtColor(frame, frame, CV_BGR2HSV);
//		imgGrayScale = getThresholdedImage(frame);
//		findStartingPoints(imgGrayScale, CMarkeri);
//		drawImage(frame, CMarkeri, sablon);
//		cvShowImage("Video", frame);
//		cvShowImage("HSV->Black_white", imgGrayScale);
//		cvReleaseImage(&imgGrayScale);
//		cvReleaseImage(&frame);
//		int c = cvWaitKey(1);
//		if ((char)c == 27) break;
//	}
//	cvDestroyAllWindows();
//	cvReleaseCapture(&capture);
//
//	for (size_t i = 0; i < _NUMBER_OF_MARKERS; ++i)
//		CMarkeri[i].~CvMarker(); //staticno dealociranje niza markera
//
//	return 0;
//}
//

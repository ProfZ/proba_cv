#include "SearchAlgorithm.hpp"

using namespace std;

bool initializeStart(){

	switch(tracking::sablon){
		case tracking::Sablon::_HEAD_TOP_BOTTOM_D_BODY:
			assert(MARKER_NUM % 2 != 0);
			cout << "Nije impelemntirano!" << endl;
			exit(-1);
		case tracking::Sablon::_TOP_BOTTOM_D:
			assert(MARKER_NUM % 2 != 0);
			cout << "Nije impelemntirano!" << endl;
			exit(-1);
		case tracking::Sablon::_HEAD_TOP_BOTTOM_D:
			assert(MARKER_NUM % 2 == 0);
			cout << "Nije impelemntirano!" << endl;
			exit(-1);
		case tracking::Sablon::_TOP_BOTTOM_S:
			assert(MARKER_NUM - 3 == 0);
			if(mark::markerArrayBuff.num == MARKER_NUM){
				sortMarkers();
				return true;
			}
	}
	return false;
}

/*Najblizi marker u nizu za (x,y,z) se pomera na to mesto
*/
void compareClossest(){
	using namespace mark;
	int i;

	unsigned ocupyTrack = 0;
	double min,tmp;
	int minIdx;
	for(i=0; i < MARKER_NUM; ++i){
		minIdx = 0;
		min = markerArray[i].cart_distance(markerArrayBuff[0]);
		for(int j=1; j < markerArrayBuff.num; ++j){
			if((tmp=mark::markerArray[i].cart_distance(mark::markerArrayBuff[j])) < min && !(ocupyTrack & (1 << j))){
				min = tmp;
				minIdx = j;
			}
		}
		markerArray[i] = markerArrayBuff[minIdx];
		ocupyTrack += (1 << minIdx);
	}
}

/*Sortira nizove prilikom 1. pronalazenja da bi ih lepo koristio dalje
*/
void sortMarkers(){
	int i, j;
	mark::cvMarker tmp;
	unsigned Num1 = MARKER_NUM - 1,Num2 = MARKER_NUM/2, Num3 = (MARKER_NUM-1)/2, Num4 = (MARKER_NUM-2)/2;


	if(tracking::sablon == tracking::Sablon::_TOP_BOTTOM_S){
		for(i = 0; i < Num1; ++i){
			for(j = i + 1; j < MARKER_NUM; ++j){
				if(mark::markerArrayBuff[i].y > mark::markerArrayBuff[j].y){
					tmp = mark::markerArrayBuff[i];
					mark::markerArrayBuff[i] =  mark::markerArrayBuff[j];
					mark::markerArrayBuff[j] = tmp;
				}
			}
			mark::markerArray[i] = mark::markerArrayBuff[i];
			mark::markerArray[i].deltaD[1] = 0;
			mark::markerArray[i].deltaX[1] = 0;
			mark::markerArray[i].deltaY[1] = 0;
		}
	}else if(tracking::sablon == tracking::Sablon::_TOP_BOTTOM_D){
		for(i = 0; i <Num1; ++i){
			for(j = i + 1; j <MARKER_NUM; ++j){
				if(mark::markerArrayBuff[i].x > mark::markerArrayBuff[j].x){
					tmp = mark::markerArrayBuff[i];
					mark::markerArrayBuff[i] = mark::markerArrayBuff[j];
					mark::markerArrayBuff[j] = tmp;
				}
			}
		}
		for(i = 0; i <Num2 -1; ++i){
			for(j = i +1; j <Num2; ++j){
				if(mark::markerArrayBuff[i].y > mark::markerArrayBuff[j].y){
					tmp = mark::markerArrayBuff[i];
					mark::markerArrayBuff[i] = mark::markerArrayBuff[j];
					mark::markerArrayBuff[j] = tmp;
				}
			}
			mark::markerArray[i] = mark::markerArrayBuff[i];
			mark::markerArray[i].deltaD[1] = 0;
			mark::markerArray[i].deltaX[1] = 0;
			mark::markerArray[i].deltaY[1] = 0;
		}
		for(i = Num2; i < Num1; ++i){
			for(j = i +1; j <MARKER_NUM; ++j){
				if(mark::markerArrayBuff[i].y > mark::markerArrayBuff[j].y){
					tmp = mark::markerArrayBuff[i];
					mark::markerArrayBuff[i] = mark::markerArrayBuff[j];
					mark::markerArrayBuff[j] = tmp;
				}
			}
			mark::markerArray[i] = mark::markerArrayBuff[i];
			mark::markerArray[i].deltaD[1] = 0;
			mark::markerArray[i].deltaX[1] = 0;
			mark::markerArray[i].deltaY[1] = 0;
		}
	}else if(tracking::sablon == tracking::Sablon::_HEAD_TOP_BOTTOM_D){
		for( i =0; i < Num1; ++i){
			for( j = i +1; j <MARKER_NUM; ++j){
				if(mark::markerArrayBuff[i].y > mark::markerArrayBuff[j].y){
					tmp = mark::markerArrayBuff[i];
					mark::markerArrayBuff[i] = mark::markerArrayBuff[j];
					mark::markerArrayBuff[j] = tmp;
				}
			}
		}
		for(i = 1; i < Num1; ++i){
			for(j = i + 1; j <MARKER_NUM; ++j){
				if(mark::markerArrayBuff[i].x > mark::markerArrayBuff[j].x){
					tmp = mark::markerArrayBuff[i];
					mark::markerArrayBuff[i] = mark::markerArrayBuff[j];
					mark::markerArrayBuff[j] = tmp;
				}
			}
		}
		for(i = 1; i <Num3 -1; ++i){
			for(j = i +1; j <Num3; ++j){
				if(mark::markerArrayBuff[i].y > mark::markerArrayBuff[j].y){
					tmp = mark::markerArrayBuff[i];
					mark::markerArrayBuff[i] = mark::markerArrayBuff[j];
					mark::markerArrayBuff[j] = tmp;
				}
			}
			mark::markerArray[i] = mark::markerArrayBuff[i];
			mark::markerArray[i].deltaD[1] = 0;
			mark::markerArray[i].deltaX[1] = 0;
			mark::markerArray[i].deltaY[1] = 0;
		}
		for(i = Num3; i < Num1; ++i){
			for(j = i +1; j < MARKER_NUM; ++j){
				if(mark::markerArrayBuff[i].y > mark::markerArrayBuff[j].y){
					tmp = mark::markerArrayBuff[i];
					mark::markerArrayBuff[i] = mark::markerArrayBuff[j];
					mark::markerArrayBuff[j] = tmp;
				}
			}
			mark::markerArray[i] = mark::markerArrayBuff[i];
			mark::markerArray[i].deltaD[1] = 0;
			mark::markerArray[i].deltaX[1] = 0;
			mark::markerArray[i].deltaY[1] = 0;
		}
	}else if(tracking::sablon == tracking::Sablon::_HEAD_TOP_BOTTOM_D_BODY){
		//realizovati
	}else{
		printf("\npogresan unos za format markera");
		return;
	}
}

/*Iscrtava linije izmedju markera - za debug koristimo
*/
void drawLines(){
	int i;
	if (tracking::sablon == tracking::Sablon::_TOP_BOTTOM_S ){
		for (i = 0; i < MARKER_NUM-1; ++i){
			cv::line(tracking::frame,cv::Point(cvRound(mark::markerArray[i].x),cvRound(mark::markerArray[i].y)),cv::Point(cvRound(mark::markerArray[i+1].x),cvRound(mark::markerArray[i+1].y)), cvScalar(255,255,255),2);
		}
	}else if(tracking::sablon == tracking::Sablon::_TOP_BOTTOM_D){
		for (i = 0; i < MARKER_NUM/2-1; ++i){
			cv::line(tracking::frame,cv::Point(cvRound(mark::markerArray[i].x),cvRound(mark::markerArray[i].y)),cv::Point(cvRound(mark::markerArray[i+1].x),cvRound(mark::markerArray[i+1].y)), cvScalar(255,255,255),2);
		}
		for (i = MARKER_NUM/2; i < MARKER_NUM-1; ++i){
			cv::line(tracking::frame,cv::Point(cvRound(mark::markerArray[i].x),cvRound(mark::markerArray[i].y)),cv::Point(cvRound(mark::markerArray[i+1].x),cvRound(mark::markerArray[i+1].y)), cvScalar(255,255,255),2);
		}
	}
	else if (tracking::sablon == tracking::Sablon::_HEAD_TOP_BOTTOM_D){
		for (i = 0; i < MARKER_NUM/2; ++i){
			cv::line(tracking::frame,cv::Point(cvRound(mark::markerArray[i].x),cvRound(mark::markerArray[i].y)),cv::Point(cvRound(mark::markerArray[i+1].x),cvRound(mark::markerArray[i+1].y)), cvScalar(255,255,255),2);
		}
		i = MARKER_NUM/2;
		cv::line(tracking::frame,cv::Point(cvRound(mark::markerArray[0].x),cvRound(mark::markerArray[0].y)),cv::Point(cvRound(mark::markerArray[i].x),cvRound(mark::markerArray[i].y)), cvScalar(255,255,255),2);
		for (; i < MARKER_NUM-1; ++i){
			cv::line(tracking::frame,cv::Point(cvRound(mark::markerArray[i].x),cvRound(mark::markerArray[i].y)),cv::Point(cvRound(mark::markerArray[i+1].x),cvRound(mark::markerArray[i+1].y)), cvScalar(255,255,255),2);
		}
	}
	else if (tracking::sablon == tracking::Sablon::_HEAD_TOP_BOTTOM_D_BODY){
		// realizovati
	}
}

/*ovo se non stop poziva posle searchSphere i calculateRectangle
*/
void setUpMarkers(){
	int i;
	if(mark::markerArrayBuff.num >= MARKER_NUM){				//sve smo pronasli a moguce i vise(treba paziti na greske)
			compareClossest();
	}else{
		unsigned foundCheck = 0; //bitwise evidencija pronadjenih markera
		float min,tmp;
		int minIdx;
		for(int j=0; j < mark::markerArrayBuff.num; ++j){
			minIdx = 0;
			min = mark::markerArray[0].cart_distance(mark::markerArrayBuff[j]);
			for(i=1; i < MARKER_NUM; ++i){
				if(tmp=mark::markerArray[i].cart_distance(mark::markerArrayBuff[j]) < min && !(foundCheck & (1 << i))){
					min = tmp;
					minIdx = i;
				}
			}
			mark::markerArray[minIdx] = mark::markerArrayBuff[j];
			foundCheck += (1 << minIdx);
		}

		/*unsigned ocupyCheck = 0;
		for(i=0; i < MARKER_NUM; ++i){
			if(foundCheck & (1 << i))
				continue;
			minIdx =0;
			min = mark::markerArray[i].sphere_distance(mark::testArray[0]);
			for(int j = 1; j < mark::testArray.num; ++j){
				if(tmp=mark::markerArray[i].sphere_distance(mark::testArray[j]) < min && !(ocupyCheck & (1 << j))){
					min = tmp;
					minIdx = j;
				}
			}
			mark::markerArray[i].aprox_equal(mark::testArray[minIdx]);
			ocupyCheck += 1 << minIdx;
		}*/

		for(i=0; i < MARKER_NUM; ++i){
			if(foundCheck & (1 << i))
				continue;
			mark::markerArray[i].aprox_move(); //koristi dvokoracni metod u sluzbi prediktora sledece pozicije markera
											   //dvokoracni sa ogranicenjem:: aprox_move(int indeks);
		}
	}
}
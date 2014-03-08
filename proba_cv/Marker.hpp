#ifndef MARKER_HPP
#define MARKER_HPP

#include "defs.hpp"

namespace mark{
	//referentni poluprecnik, odgovarace rastojanju za 1 m od kamere,
	//za ovu vrednost se postavlja poluprecnik prvog najblizeg markera prilikom inicijalizacije
	extern float refR; 

	class cvMarker:public cv::Point2f{
	private:
		cv::Point3f CartPoint;
	public:
		float deltaX[2], deltaY[2], deltaD[2];
		float distance;

		cvMarker(): cv::Point2f(0,0),CartPoint(0,0,0){
			distance = 0;
			deltaX[0] = 0; deltaX[1] = 0;
			deltaY[0] = 0; deltaY[1] = 0;
			deltaD[0] = 0; deltaD[1] = 0;
		}
		cvMarker(cv::Point2f &pt,float r): cv::Point2f(pt){
			distance = REF_R/r;

			float focDiagonalX = sqrt(FOCUS*FOCUS + pow(x - tracking::cart_x0,2));
			CartPoint.x = (x - tracking::cart_x0)*distance/focDiagonalX;
			CartPoint.z = FOCUS*distance/focDiagonalX;
			CartPoint.y = (y - tracking::cart_x0)*CartPoint.z/FOCUS;

			deltaX[0] = 0; deltaX[1] = 0;
			deltaY[0] = 0; deltaY[1] = 0;
			deltaD[0] = 0; deltaD[1] = 0;
		}

		cvMarker(float xx,float yy,float rr): cv::Point2f(xx,yy){
			distance=REF_R/rr;

			float focDiagonalX = sqrt(FOCUS*FOCUS + pow(x - tracking::cart_x0,2));
			CartPoint.x = (x - tracking::cart_x0)*distance/focDiagonalX;
			CartPoint.z = FOCUS*distance/focDiagonalX;
			CartPoint.y = (y - tracking::cart_x0)*CartPoint.z/FOCUS;

			deltaX[0] = 0; deltaX[1] = 0;
			deltaY[0] = 0; deltaY[1] = 0;
			deltaD[0] = 0; deltaD[1] = 0;
		};
		cvMarker(cv::Vec3f circle){
			x = circle[0];
			y = circle[1];
			distance = REF_R/circle[2];

			float focDiagonalX = sqrt(FOCUS*FOCUS + pow(x - tracking::cart_x0,2));
			CartPoint.x = (x - tracking::cart_x0)*distance/focDiagonalX;
			CartPoint.z = FOCUS*distance/focDiagonalX;
			CartPoint.y = (y - tracking::cart_x0)*CartPoint.z/FOCUS;

			deltaX[0] = 0; deltaX[1] = 0;
			deltaY[0] = 0; deltaY[1] = 0;
			deltaD[0] = 0; deltaD[1] = 0;
		}
		inline float cart_distance(const cvMarker &marker) const{
			return pow(CartPoint.x - marker.CartPoint.x,2) + pow(CartPoint.y - marker.CartPoint.y,2) + pow(CartPoint.z - marker.CartPoint.z,2);
		}
		inline float sphere_distance(const cvMarker &marker) const{
			return pow(x - marker.x,2) + pow(y - marker.y,2);
		}
		inline void aprox_equal(const cvMarker &marker){
			deltaX[0] = deltaX[1];
			deltaX[1] = (marker.x - x);
			deltaY[0] = deltaY[1];
			deltaY[1] = (marker.y - y);

			x = marker.x;
			y = marker.y;

			float focDiagonalX = sqrt(FOCUS*FOCUS + pow(x - tracking::cart_x0,2));
			CartPoint.x = (x - tracking::cart_x0)*distance/focDiagonalX;
			CartPoint.z = FOCUS*distance/focDiagonalX;
			CartPoint.y = (y - tracking::cart_x0)*CartPoint.z/FOCUS;
		}
		//Moze i mnogo bolje ali ne u real time varijanti
		//Optimalno bi bilo da aproksmiramo poziciju markera interpolacijom (pored implementirane metode)
		inline void aprox_move(unsigned poz);
		inline void aprox_move(){
			float tmpD=distance,
				   tmpX=x,
				   tmpY=y;

			x += (3*deltaX[1] - deltaX[0])/2;
			y += (3*deltaY[1] - deltaY[0])/2;
			distance += (3*deltaD[1] - deltaD[0])/2;

			deltaX[0] = deltaX[1];
			deltaX[1] = (x - tmpX);
			deltaY[0] = deltaY[1];
			deltaY[1] = (y - tmpY);
			deltaD[0] = deltaD[1];
			deltaD[1] = (distance - tmpD);

			float focDiagonalX = sqrt(FOCUS*FOCUS + pow(x - tracking::cart_x0,2));
			CartPoint.x = (x - tracking::cart_x0)*distance/focDiagonalX;
			CartPoint.z = FOCUS*distance/focDiagonalX;
			CartPoint.y = (y - tracking::cart_x0)*CartPoint.z/FOCUS;
		}
		
		inline cvMarker& operator=(const cvMarker &marker){
			deltaX[0] = deltaX[1];
			deltaX[1] = (marker.x - x);
			deltaY[0] = deltaY[1];
			deltaY[1] = (marker.y - y);
			deltaD[0] = deltaD[1];
			deltaD[1] = (marker.distance - distance);
			CartPoint = marker.CartPoint;
			distance = marker.distance;
			x = marker.x;
			y = marker.y;
			return *this;
		}
	};

	class cvMarkerArray{
	private:
		std::mutex mt;
		bool notReady;
		std::condition_variable cv;
	public:
		cvMarker *array;
		int num;

		cvMarkerArray(int alloc = RECT_ALLOC_UNIT){
			array = new cvMarker[alloc];
			notReady = false;
			num = 0;
		}
		cvMarkerArray(const cvMarkerArray &marks){
			array = new cvMarker[RECT_ALLOC_UNIT];
			notReady = false;
			num = marks.num;
			memcpy(array,marks.array,num*sizeof(cvMarker));
		}
		//Thread-safe add
		inline void ts_add(const cvMarker& marker){
			std::unique_lock<std::mutex> l(mt);
			while(notReady)
				cv.wait(l);

			notReady = true;
			array[num++]=marker;
			notReady = false;
			cv.notify_one();
		}
		//Nije thread safe
		inline cvMarker& operator[](int idx){
			return array[idx];
		}
		//Nije thread safe
		inline cvMarker operator[](int idx) const{
			return array[idx];
		}
		~cvMarkerArray(){
			delete[] array;
		}
	};

	extern cvMarkerArray markerArray,markerArrayBuff,testArray;

	inline void cvMarker::aprox_move(unsigned poz){
			float tmD = distance, tmpX, tmpY;
			float deltaXplus=0, deltaYplus=0, deltaXminus=0, deltaYminus=0, i = 0, j = 0;
			tmpX = deltaX[1];
			tmpY = deltaY[1];
			deltaX[1] = 3*deltaX[1] - deltaX[0];
			deltaY[1] = 3*deltaY[1] - deltaY[0];
			deltaX[1] = tmpX;
			deltaY[1] = tmpY;
			tmpX = x; tmpY = y;
			if(tracking::sablon == tracking::Sablon::_TOP_BOTTOM_S){
				if(poz != 0){
					++j;
					deltaXminus = mark::markerArray[poz-1].deltaX[1];
					deltaYminus = mark::markerArray[poz-1].deltaY[1];
				}
				if(poz != MARKER_NUM-1){
					++i;
					deltaXplus = mark::markerArray[poz+1].deltaX[1];
					deltaYplus = mark::markerArray[poz+1].deltaY[1];
				}
			}else if(tracking::sablon == tracking::Sablon::_TOP_BOTTOM_D){
				//implementiracu kasnije 
			}else if(tracking::sablon == tracking::Sablon::_HEAD_TOP_BOTTOM_D){
				//implementiracu kasnije 
			}else if(tracking::sablon == tracking::Sablon::_HEAD_TOP_BOTTOM_D_BODY){
				//implementiracu kasnije 
			}
			deltaX[1] = (deltaX[1] + deltaXplus + deltaXminus)/(i+j+1);
			deltaY[1] = (deltaY[1] + deltaYplus + deltaYminus)/(i+j+1);
			x += deltaX[1];
			y += deltaY[1];

			distance += (3*deltaD[1] - deltaD[0])/2;
			deltaD[0] = deltaD[1];
			deltaD[1] = (distance - tmD);
			float focDiagonalX = sqrt(FOCUS*FOCUS + pow(x - tracking::cart_x0,2));
			CartPoint.x = (x - tracking::cart_x0)*distance/focDiagonalX;
			CartPoint.z = FOCUS*distance/focDiagonalX;
			CartPoint.y = (y - tracking::cart_x0)*CartPoint.z/FOCUS;
		}
}
#endif

#ifndef COMMON_H_
#define COMMON_H_

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <utility>
#include <climits>
#include <vector>
#include <ctime>
#include <map>
#include <algorithm>
#include <deque>
#include <cmath>

namespace common
{
	extern long long START_OF_DAY;
	extern long long END_OF_DAY;
	extern std::string filepath_common;

	long long epoch_time(std::string date, std::string time = "0915");

	class Orderbook
	{
		public:
		//Data:
			std::pair<int,int> BestBid;
			std::pair<int,int> BestAsk;
		
		//Constructor:
			Orderbook();

		//Other functions:
			bool isEmpty();

			std::vector<int> FillCurrentOrder(std::vector<int> order);//ID, price, quantity, Side | ONLY CALLED WHEN QUANTITY =/= 0
			friend std::ostream& operator<<(std::ostream& os, Orderbook Book);
	};
}

#endif
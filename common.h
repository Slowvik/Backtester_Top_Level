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

using namespace std;

long long START_OF_DAY;
long long END_OF_DAY;
string filepath_common = ".\\Input Files\\";

long long epoch_time(string date, string time = "0915")
{
	int year = 0, day = 0, month = 0, hour = 0, min = 0;
	basic_string<char>::size_type pos_count = 0;

	long long nanoseconds = 1000000000;

	while(pos_count<4)
	{
        year = year * 10 + static_cast<int>(date.at(pos_count)-'0');
		pos_count++;
	}

	while(pos_count<6)
	{
		month = month * 10 + static_cast<int>(date.at(pos_count)-'0');
        pos_count++;
	}

	while(pos_count<8)
	{
		day = day * 10 + static_cast<int>(date.at(pos_count)-'0');
        pos_count++;
	}

	pos_count = 0;

	while(pos_count<2)
	{
		hour = hour* 10 + static_cast<int>(time.at(pos_count)-'0');
        pos_count++;
	}

	while(pos_count<4)
	{
		min = min * 10 + static_cast<int>(time.at(pos_count)-'0');
        pos_count++;
	}

    struct tm t;
	const int year_start = 1900;
    const int month_start = 1;

	t.tm_year = year - year_start;  // This is year-1900
    t.tm_mon = month - month_start;
    t.tm_mday = day;
    t.tm_min = min;
    t.tm_hour = hour;
	t.tm_sec = 0;

	time_t time_since_epoch = mktime(&t);
    long long final_time = (time_since_epoch) * nanoseconds;

	return (final_time);
}


class Orderbook
{
	public:
	//Data:
		pair<int,int> BestBid;
		pair<int,int> BestAsk;
	
	//Constructor:
		Orderbook()
		{
			this->BestAsk = make_pair(0, 0);
			this->BestBid = make_pair(0, 0);
		}

	//Other functions:
	
		bool isEmpty()
		{
			if (BestBid.first == 0 && BestAsk.first == 0)
			{
				return true;
			}

			return false;
		}

		vector<int> FillCurrentOrder(vector<int> order)//ID, price, quantity, Side | ONLY CALLED WHEN QUANTITY =/= 0
		{
			
			vector<int> fill_status; //ID, Price, Quantity, Side
			int quantity_filled = 0; //initialise quantity_remaining with the entire number of contracts to be traded

			if (order[3] == -1)//bid
			{
				if (this->BestAsk.first!=0)
				{
					quantity_filled = BestAsk.second > order[2] ? order[2] : BestAsk.second;
					BestAsk.second -= order[2];
					if (BestAsk.second <= 0)
					{
						BestAsk = make_pair(0, 0);
					}
				}		
			}

			else if (order[3] == 1)//ask
			{
				quantity_filled = BestBid.second > order[2] ? order[2] : BestBid.second;
				BestBid.second -= order[2];
				if (BestBid.second <= 0)
				{
					BestBid= make_pair(0, 0);
				}

			}

			fill_status.push_back(order[0]);
			fill_status.push_back(order[1]);
			fill_status.push_back(quantity_filled);
			fill_status.push_back(order[3]);
				
			return fill_status;
		}

		friend ostream& operator<<(ostream& os, Orderbook Book)
		{
			if (Book.isEmpty())
			{
				os << "Orderbook is empty\n";
				return os;
			}

			os << "Best ask: " << Book.BestAsk.first << "," << Book.BestAsk.second << "\n";
			os << "Best bid: " << Book.BestBid.first << "," << Book.BestBid.second << "\n";

			return os;
		}
};

#endif
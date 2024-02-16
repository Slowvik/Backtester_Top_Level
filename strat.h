#ifndef STRAT_H_
#define STRAT_H_

#include "common.h"

//Wrapper class:

class Strategy
{
	private:
		//Data:
		//ofstream log;
		std::map<int, int> portfolio;
		long long Total_PnL = 0;
		
		std::map<int,long long> Initial_Capital;
		std::map<int,bool> IsCapitalExhausted;
		std::map<int, long long> PnL;
		//bool PnL_Flag = false;
		
		long long current_time = common::START_OF_DAY;
		
		std::map<int,bool> order_flags;
		std::map<int,std::deque <bool>> Timer_Trend;
		int Num_Timers = 3;
		double TAX = 0.005;


		//Functions:
		
	public:
		//Data:
		std::vector <int> instruments;
		std::map<long long, std::string> timers;
		std::map<long long, int> timer_counter;
		std::map<int, std::vector<int>> trade_list; //first entry in map is instrumentID, second entry is the prices at which trades occurred for that instrument

		//Constructor
		Strategy(std::string date);

		//Actions:
		std::pair<std::vector<int>, std::map<long long, std::string>> init();
		std::vector<int> OnMarketDataUpdate(std::vector<long long> m);
		void OnTradePrint(std::vector<long long> t);
		void OnTimer(long long curr_time);

		void OnFill(std::vector<int> filled_order);

		bool Stop();
};

#endif
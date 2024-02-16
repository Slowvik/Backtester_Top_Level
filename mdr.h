#ifndef MDR_H_
#define MDR_H_

#include "common.h"

//Wrapper class:
class MDR 
{
	private:
		//Data:
		bool eof_mkt_data = false;
		bool eof_trade_print = false;

		std::vector<long long> eod_vec = { common::END_OF_DAY,0,0,0,0 };

		//File streams
		std::string filename_mkt_data;
		std::string filename_trade_print;
		std::ifstream stream_mkt_data;
		std::ifstream stream_trade_print;

		//Events:
		std::pair<std::vector<long long>, int> NextEvent;
		std::vector<long long> mkt_data_update;
		std::vector<long long> trade_print;

		//File Reader Functions:
		void ReadNextMarketDataUpdate();
		void ReadNextTradePrint();

	public:

		//constructor:
		MDR(std::string date);

		//Next Event Chooser:
		std::pair<std::vector<long long>, int> GetNextEvent();
		void Stop();
};

#endif
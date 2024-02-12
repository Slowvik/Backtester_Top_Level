//Includes:
#pragma once

#include "common.h"

using namespace std;

//Wrapper class:
class MDR 
{
	private:
		//Data:
		bool eof_mkt_data = false;
		bool eof_trade_print = false;

		vector<long long> eod_vec = { END_OF_DAY,0,0,0,0 };

		//File streams
		string filename_mkt_data;
		string filename_trade_print;
		ifstream stream_mkt_data;
		ifstream stream_trade_print;

		//Events:
		pair<vector<long long>, int> NextEvent;
		vector<long long> mkt_data_update;
		vector<long long> trade_print;

		//File Reader Functions:
		void ReadNextMarketDataUpdate();
		void ReadNextTradePrint();

	public:

		//constructor:
		MDR(string date);

		//Next Event Chooser:
		pair<vector<long long>, int> GetNextEvent();
		void Stop();
};


//FUNCTION DEFINITIONS:

void MDR::ReadNextMarketDataUpdate()
{
	string line;
	string val;

	if (stream_mkt_data.eof() || eof_mkt_data)
	{
		mkt_data_update.clear();
		mkt_data_update.push_back(END_OF_DAY);
		eof_mkt_data = true;
		return;
	}

	else
	{
		mkt_data_update.clear();

		stream_mkt_data >> line;

		istringstream ss(line);
		while (getline(ss, val, ','))
		{
			mkt_data_update.push_back(stoll(val, nullptr, 10));
		}
		if (mkt_data_update[0] >= END_OF_DAY)
		{
			eof_mkt_data = true;
		}
	}
}

void MDR::ReadNextTradePrint()
{
	string line;
	string val;

	if (stream_trade_print.eof() || eof_trade_print)
	{
		trade_print.clear();
		trade_print.push_back(END_OF_DAY);
		eof_trade_print = true;
		return;
	}
	else 
	{
		trade_print.clear();
	
		stream_trade_print >> line;
		
		if (line=="")
		{
			eof_trade_print = true;
			return;
		}

		istringstream ss(line);
		while (getline(ss, val, ','))
		{
			trade_print.push_back(stoll(val, nullptr, 10));
		}
	}
}

pair<vector<long long>, int> MDR::GetNextEvent() //central hub in MDR
{
	if (eof_mkt_data && eof_trade_print)
	{
		cout<<"END OF DAY\n";
		return make_pair(eod_vec, 0);
	}
	else if (eof_mkt_data)
	{
		cout << " NO MORE MARKET DATA UPDATES\n";
		mkt_data_update.clear();
		mkt_data_update = eod_vec;
		stream_mkt_data.close();
	}
	else if (eof_trade_print)
	{
		cout << " NO MORE TRADES\n";
		trade_print.clear();
		trade_print = eod_vec;
		stream_trade_print.close();
	}


	if (mkt_data_update[0] <= trade_print[0])
	{
		NextEvent = make_pair(mkt_data_update, 1); //1 = market data update
		ReadNextMarketDataUpdate();
	}
	else
	{
		NextEvent = make_pair(trade_print, 2); //2 = trade event
		ReadNextTradePrint();
	}

	return NextEvent;
}


void MDR::Stop()
{
	if(stream_mkt_data.is_open())
	{
		stream_mkt_data.close();
	}
	if(stream_trade_print.is_open())
	{
		stream_trade_print.close();
	}
	
}


MDR::MDR(string date)
{
	cout << "Initialising Market Data Reader\n";

	filename_mkt_data = date + ".csv";
	filename_trade_print = date + "_prints.csv";

	//Opening the two files with the two filestreams:
	stream_mkt_data.open(filename_mkt_data.c_str());
	stream_trade_print.open(filename_trade_print.c_str());

	//Reading the first mkt data and trade entries:
	ReadNextMarketDataUpdate();
	ReadNextTradePrint();
}



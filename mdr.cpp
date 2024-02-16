#include "common.h"
#include "mdr.h"

//FUNCTION DEFINITIONS:

void MDR::ReadNextMarketDataUpdate()
{
	std::string line;
	std::string val;

	if (stream_mkt_data.eof() || eof_mkt_data)
	{
		mkt_data_update.clear();
		mkt_data_update.push_back(common::END_OF_DAY);
		eof_mkt_data = true;
		return;
	}

	else
	{
		mkt_data_update.clear();

		stream_mkt_data >> line;
		
		std::istringstream ss(line);
		while (std::getline(ss, val, ','))
		{
			mkt_data_update.push_back(stoll(val, nullptr, 10));
		}
		if (mkt_data_update[0] >= common::END_OF_DAY)
		{
			eof_mkt_data = true;
		}
	}
}

void MDR::ReadNextTradePrint()
{
	std::string line;
	std::string val;

	if (stream_trade_print.eof() || eof_trade_print)
	{
		trade_print.clear();
		trade_print.push_back(common::END_OF_DAY);
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

		std::istringstream ss(line);
		while (std::getline(ss, val, ','))
		{
			trade_print.push_back(stoll(val, nullptr, 10));
		}
	}
}

std::pair<std::vector<long long>, int> MDR::GetNextEvent() //central hub in MDR
{
	if (eof_mkt_data && eof_trade_print)
	{
		std::cout<<"END OF DAY\n";
		return std::make_pair(eod_vec, 0);
	}
	else if (eof_mkt_data)
	{
		std::cout << "NO MORE MARKET DATA UPDATES\n";
		mkt_data_update.clear();
		mkt_data_update = eod_vec;
		stream_mkt_data.close();
	}
	else if (eof_trade_print)
	{
		std::cout << "NO MORE TRADES\n";
		trade_print.clear();
		trade_print = eod_vec;
		stream_trade_print.close();
	}


	if (mkt_data_update[0] <= trade_print[0])
	{
		NextEvent = std::make_pair(mkt_data_update, 1); //1 = market data update
		ReadNextMarketDataUpdate();
	}
	else
	{
		NextEvent = std::make_pair(trade_print, 2); //2 = trade event
		ReadNextTradePrint();
	}

	return NextEvent;
}


MDR::MDR(std::string date)
{
	std::cout << "Initialising Market Data Reader\n";

	filename_mkt_data = common::filepath_common + date + ".csv";
	filename_trade_print = common::filepath_common + date + "_prints.csv";

	//Opening the two files with the two filestreams:
	stream_mkt_data.open(filename_mkt_data.c_str());
	stream_trade_print.open(filename_trade_print.c_str());

	//Reading the first mkt data and trade entries:
	ReadNextMarketDataUpdate();
	ReadNextTradePrint();
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
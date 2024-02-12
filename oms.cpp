#pragma once
#include "common.h"
#include "strat.h"
#include "mdr.h"

using namespace std; 


//Main:

int main()
{
	
	//Variable initialisations:
	string date;
	ifstream date_reader("date.txt", ios::in);
	date_reader>>date;
	date_reader.close();

	START_OF_DAY = epoch_time(date);
	END_OF_DAY = epoch_time(date, "0330");

	long long current_time = START_OF_DAY;

	map<int, Orderbook> Library;
	pair <vector<long long>, int> market_event;

	MDR MarketDataReader(date.c_str());
	Strategy Strat(date.c_str());
	pair<vector<int>, map<long long, string>> instruments_timers;
	vector<int> timer_counter;
	instruments_timers = Strat.init();
	vector<long long> timers;

	//ofstream log_oms;
	//log_oms.open("log_oms.txt", ios::out);

	vector<int> Received_Order;

	//Populating Library of Orderbooks (ID vs Orderbook)
	for (auto i : instruments_timers.first)
	{
		//cout << i << endl;
		Orderbook Book;
		
		Library.insert(make_pair(i,Book));
		//User_Library.insert(make_pair(i, Book));
	}

	//Inserting all timers requested by strategy into a vector for easier manipulation
	for (auto i : instruments_timers.second)
	{
		timers.push_back(i.first);
		timer_counter.push_back(1);
	}

	cout << "Starting..." << endl;
	long long count = 0;

	//Start:
	do
	{
		count++;
		//cout << "Getting Next Event" << endl;
		market_event = MarketDataReader.GetNextEvent(); //pair of vector(ll) and int, in vector we have timestamp, ID, etc.
		//

		current_time = market_event.first[0];

		//Checking if it's END_OF_DAY already
		if (current_time >= END_OF_DAY)
		{
			cout << "END OF DAY\n";
			break;
		}

		//checking if a timer is hit before current event
		for (int i=0;i<timer_counter.size();i++)
		{
			if (START_OF_DAY + timer_counter[i]*timers[i] < current_time)
			{
				timer_counter[i]++;
				Strat.OnTimer(timers[i]);
			}
		}


		//Market data update
		if (market_event.second == 1) 
		{

			if(find(instruments_timers.first.begin(), instruments_timers.first.end(), int(market_event.first[1]))!= instruments_timers.first.end())
			{
				//log_oms << market_event.first[0] << " Market Data Update of " << market_event.first[1] << "\n";
				//cout << market_event.first[0] << " Market Data Update of " << market_event.first[1] << "\n";
				
				Library[market_event.first[1]].BestBid = make_pair(market_event.first[2], market_event.first[3]);
				Library[market_event.first[1]].BestAsk = make_pair(market_event.first[4], market_event.first[5]);

				//Telling strat that market data update has been received and expecting an order from strat
				Received_Order = Strat.OnMarketDataUpdate(market_event.first);//ID, price, quantity, side

				vector<int> status = { 0,0,0,0 };
				
				if (Received_Order[2] != 0)//Order sent by user, call FillCurrentOrder
				{
					//log_oms << current_time << " Order ID " << Received_Order[0] << "\n";
					status = Library[market_event.first[1]].FillCurrentOrder(Received_Order);//Orderbook gets updated inside FillCurrentOrder
				}
				
				Strat.OnFill(status);
			}		
		}

		//Trade event in market
		else if (market_event.second == 2)
		{/*
		 * When trade comes in, aggressor side refers to whether the bid was aggressive (0) or the ask was aggressive (1)
		 * An aggressive bid will be matched with the best ask. If best ask is exhausted, it will continue to the second best ask and so on.
		 * An aggressive ask will be matched with the best bid. If best bid is exhausted, it will continue to the next level.
		 */
			//log_oms << market_event.first[0] << " Tradeprint of "<<market_event.first[1]<<"\n";
			if (find(instruments_timers.first.begin(), instruments_timers.first.end(), market_event.first[1]) != instruments_timers.first.end())
			{
				//Update Orderbook to reflect the trade
				if (market_event.first[4] == 0)//Aggrresor side 0 (ask was already present in orderbook)
				{
					Library[market_event.first[1]].BestAsk.second -= market_event.first[3];
					if (Library[market_event.first[1]].BestAsk.second <= 0)
					{
						Library[market_event.first[1]].BestAsk = make_pair(0, 0);
					}
				}

				else if (market_event.first[4] == 1)//Ask aggressed to meet bid (bid was already present in orderbook)
				{

					Library[market_event.first[1]].BestBid.second -= market_event.first[3];
					if (Library[market_event.first[1]].BestBid.second <= 0)
					{
						Library[market_event.first[1]].BestBid = make_pair(0, 0);
					}					
				}

				Strat.OnTradePrint(market_event.first);
			}
		}

	}while (current_time < END_OF_DAY);

	return 0;
}
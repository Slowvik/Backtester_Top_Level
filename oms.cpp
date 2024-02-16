#include "common.h"
#include "strat.h"
#include "mdr.h"

//Main:
int main()
{
	
	//Variable initialisations:
	std::string date;
	std::ifstream date_reader((common::filepath_common + "date.txt").c_str(), std::ios::in);
	date_reader>>date;
	date_reader.close();

	common::START_OF_DAY = common::epoch_time(date);
	common::END_OF_DAY = common::epoch_time(date, "1530");
	
	long long current_time = common::START_OF_DAY;

	std::map<int, common::Orderbook> Library;
	std::pair <std::vector<long long>, int> market_event;

	MDR MarketDataReader(date.c_str());
	Strategy Strat(date.c_str());
	std::pair<std::vector<int>, std::map<long long, std::string>> instruments_timers;
	std::vector<int> timer_counter;
	instruments_timers = Strat.init();
	std::vector<long long> timers;

	//ofstream log_oms;
	//log_oms.open("log_oms.txt", ios::out);

	std::vector<int> Received_Order;

	//Populating Library of Orderbooks (ID vs Orderbook)
	for (auto i : instruments_timers.first)
	{
		//cout << i << endl;
		common::Orderbook Book;
		
		Library.insert(std::make_pair(i,Book));
		//User_Library.insert(make_pair(i, Book));
	}

	//Inserting all timers requested by strategy into a vector for easier manipulation
	for (auto i : instruments_timers.second)
	{
		timers.push_back(i.first);
		timer_counter.push_back(1);
	}

	std::cout << "Starting..." << std::endl;
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
		if (current_time >= common::END_OF_DAY)
		{
			std::cout << "END OF DAY\n";
			break;
		}

		//checking if a timer is hit before current event
		for (auto i=0;i<timer_counter.size();i++)
		{
			if (common::START_OF_DAY + timer_counter[i]*timers[i] < current_time)
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
				
				Library[market_event.first[1]].BestBid = std::make_pair(market_event.first[2], market_event.first[3]);
				Library[market_event.first[1]].BestAsk = std::make_pair(market_event.first[4], market_event.first[5]);

				//Telling strat that market data update has been received and expecting an order from strat
				Received_Order = Strat.OnMarketDataUpdate(market_event.first);//ID, price, quantity, side

				std::vector<int> status = { 0,0,0,0 };
				
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
						Library[market_event.first[1]].BestAsk = std::make_pair(0, 0);
					}
				}

				else if (market_event.first[4] == 1)//Ask aggressed to meet bid (bid was already present in orderbook)
				{

					Library[market_event.first[1]].BestBid.second -= market_event.first[3];
					if (Library[market_event.first[1]].BestBid.second <= 0)
					{
						Library[market_event.first[1]].BestBid = std::make_pair(0, 0);
					}					
				}

				Strat.OnTradePrint(market_event.first);
			}
		}

	}while (current_time < common::END_OF_DAY);

	return 0;
}
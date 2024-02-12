//Includes:
#pragma once

#include "common.h"

using namespace std;

//Wrapper class:

class Strategy
{
	private:
		//Data:
		//ofstream log;
		map<int, int> portfolio;
		long long Total_PnL = 0;
		
		map<int,long long> Initial_Capital;
		map<int,bool> IsCapitalExhausted;
		map<int, long long> PnL;
		//bool PnL_Flag = false;
		
		long long current_time = START_OF_DAY;
		
		map<int,bool> order_flags;
		map<int,deque <bool>> Timer_Trend;
		int Num_Timers = 3;
		double TAX = 0.005;


		//Functions:
		
	public:
		//Data:
		vector <int> instruments;
		map<long long, string> timers;
		map<long long, int> timer_counter;
		map<int, vector<int>> trade_list; //first entry in map is instrumentID, second entry is the prices at which trades occurred for that instrument

		//Constructor
		Strategy(string date);

		//Actions:
		pair<vector<int>, map<long long, string>> init();
		vector<int> OnMarketDataUpdate(vector<long long> m);
		void OnTradePrint(vector<long long> t);
		void OnTimer(long long curr_time);

		void OnFill(vector<int> filled_order);

		bool Stop();
};

//FUNCTION DEFINITIONS:


Strategy::Strategy(string date)
{
	string file_name_instruments = date + "_instruments.txt";
	string file_name_timers = date + "_timers.txt";
	string file_name_capital = date + "_capital.txt";
	current_time = START_OF_DAY;

	ifstream fin_instruments, fin_timers, fin_capital;
	pair<long long, string> timer;
	int ID;

	fin_instruments.open(file_name_instruments.c_str());

	while (!fin_instruments.eof())
	{
		fin_instruments >> ID;
		instruments.push_back(ID);
	}
	fin_instruments.close();

	fin_timers.open(file_name_timers.c_str());

	while (!fin_timers.eof())
	{
		fin_timers >> timer.first >> timer.second;
		timers.insert(timer);
		timer_counter[timer.first] = 1;
	}
	fin_timers.close();

	fin_capital.open(file_name_capital.c_str());
	long long capital;

	for (auto i : instruments)
	{
		order_flags.insert(make_pair(i,false));
		IsCapitalExhausted.insert(make_pair(i, false));
		fin_capital >> capital;
		Initial_Capital.insert(make_pair(i, capital));
		PnL.insert(make_pair(i, 0));
		for (auto it = 0;it < Num_Timers;it++)
			Timer_Trend[i].push_back(0);
	}
	fin_capital.close();

	
	//log.open("log_strat.txt", ios::out);
}

pair<vector<int>,map<long long, string>> Strategy::init()
{
	return make_pair(instruments,timers);
}

vector<int> Strategy::OnMarketDataUpdate(vector<long long> m)//Timestamp(0), ID(1), Bid Price(2), Bid Size(3), Ask Price(4), Ask Size(5)
{
	//log << m[0]<< " MarketDataUpdate\n";
	current_time = m[0];

	vector<int> order{ 0,0,0,0 };//ID, Price, Quantity, Side

	if (order_flags[m[1]] && (Total_PnL > 0 || !IsCapitalExhausted[m[1]]))
	{
		int order_size = m[5];
		order[0] = m[1];
		order[1] = m[4];
		if (IsCapitalExhausted[m[1]])
		{
			order_size = Total_PnL / ((1+TAX) * double(m[4])); //tax on BestAsk
			//log << "OnMarketDataUpdate: Sending buy order:\n"<< "Price: " << BestAsk << " Quantity: " << order_size << endl;
		}
		order[2] = order_size;
		order[3] = -1;
	}

	else if (portfolio[m[1]] > 0)
	{
		if (abs(Total_PnL / double(portfolio[m[1]])) < (1-TAX) * double(m[2]))//10% tax on best bid
		{
			order[0] = m[1];
			order[1] = m[2];
			order[2] = portfolio[m[1]];
			order[3] = 1;
			//log << "OnMarketDataUpdate: Sending sell order: " << order[0] << "," << order[1] << "," << order[2] << "," << order[3] << endl;
		}
	}
	
	return order;
}

void Strategy::OnTradePrint(vector<long long> t)
{
	//log << t[0] <<" OnTradePrint: Tradeprint\n";
	current_time = t[0];
	trade_list[t[1]].push_back(t[2]);
}

void Strategy::OnTimer(long long timer_increment)
{
	//sections with different possible timers. Not all of them will necessarily be subscribed to at every run

	//At 1 sec timer:
	if (timers[timer_increment] == "SEC")
	{

	}


	//At 10 sec timer:
	if (timers[timer_increment] == "10SEC")
	{
		cout << "Total PnL at " << timers[timer_increment] << " timer: " << Total_PnL << endl;
	}


	//At 1 min timer:
	if (timers[timer_increment] == "MIN")
	{
		//perform fixed interval calculations
		for (auto i:instruments)
		{
			if (!trade_list[i].empty())
			{
				if (*trade_list[i].rbegin() > *trade_list[i].begin()) //Prices rising in the last timer
				{
					Timer_Trend[i].pop_front();
					Timer_Trend[i].push_back(1);
				}
				else //Prices not rising in the last timer
				{
					Timer_Trend[i].pop_front();
					Timer_Trend[i].push_back(0);
				}
				order_flags[i] = true;//Time to buy
				for (auto it = Timer_Trend[i].begin();it != Timer_Trend[i].end();it++)
				{
					if (!(*it))
					{
						order_flags[i] = false;
					}
				}
				trade_list[i].clear();
			}
			//log << "OnTimer: Portfolio of instrument ID in MIN timer" << i <<" = "<< portfolio[i] << endl;
			//log << "OnTimer: PnL of instrument ID in MIN timer" << i << " = " << PnL[i] << endl;
			cout << "OnTimer: Position of instrument ID " << i <<" in MIN timer "  << " = " << portfolio[i] << endl;
			cout << "OnTimer: PnL of instrument ID " << i <<" in MIN timer "  << " = " << PnL[i] << endl;
		}
		
		//log << "OnTimer: Total PnL at "<< timers[timer_increment]<<" timer: " << Total_PnL << endl;
		cout << "OnTimer: Total PnL at " << timers[timer_increment] << " timer: " << Total_PnL << endl;
	}
	//At 5 min timer:
	if (timers[timer_increment] == "5MIN")
	{

	}

	//At 10 min timer:
	if (timers[timer_increment] == "10MIN")
	{
		cout << "Total PnL at " << timers[timer_increment] << " timer: " << Total_PnL << endl;
	}

	//At 30 min timer:
	if (timers[timer_increment] == "30MIN")
	{

	}

	//At 1 hr timer:
	if (timers[timer_increment] == "HR")
	{
		
	}


}


void Strategy::OnFill(vector<int> filled_order)
{
	if (filled_order[2] != 0)
	{
		portfolio[filled_order[0]] += filled_order[2]*(-filled_order[3]);
		
		//Price x Size x Side (=0 if size = 0) - tax
		PnL[filled_order[0]] += double(filled_order[1] * filled_order[2] * filled_order[3]) - (TAX * double(filled_order[1]) * double(filled_order[2]));
		Total_PnL += double(filled_order[1] * filled_order[2] * filled_order[3]) - (TAX * double(filled_order[1]) * double(filled_order[2]));

		//log <<"OnFill:\n" << "Total PnL: " << Total_PnL << "\nPortfolio: " << portfolio[filled_order[0]] << "\n";
	}

	
	
	if (PnL[filled_order[0]] < Initial_Capital[filled_order[0]])
	{
		IsCapitalExhausted[filled_order[0]] = true;
	}
	
}

bool Strategy::Stop()
{
	if (current_time >= END_OF_DAY)
		return true;
	return false;
}
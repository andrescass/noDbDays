#include <cstdio>
#include <iostream>
#include <vector>
#include <sstream>
#include <time.h>
#include <thread>
#include <map>
#include <fstream>
#include <thread>
#include <mutex>
#include <chrono>
#include <iomanip>

#include "csv.h"

using namespace csv;

static const std::string APP_VERSION = "2.0.7";

std::vector<std::vector<std::string>> ldatedata;
std::vector<std::vector<float>> lavgdata;
std::unordered_map<std::string, int> locsMap;
std::unordered_map<std::string, int> lfiltMap;
std::unordered_map<std::string, float> lavgMap;
std::unordered_map<std::string, int> locsMap1;
std::unordered_map<std::string, int> lfiltMap1;
std::unordered_map<std::string, float> lavgMap1;
std::unordered_map<std::string, int> locsMap2;
std::unordered_map<std::string, int> lfiltMap2;
std::unordered_map<std::string, float> lavgMap2;
std::unordered_map<std::string, int> locsMap3;
std::unordered_map<std::string, int> lfiltMap3;
std::unordered_map<std::string, float> lavgMap3;
std::vector<std::vector<std::string>> hdatedata;
std::vector < std::vector<float>> havgdata;
std::unordered_map<std::string, int> hocsMap;
std::unordered_map<std::string, int> hfiltMap;
std::unordered_map<std::string, float> havgMap;
std::unordered_map<std::string, int> hocsMap1;
std::unordered_map<std::string, int> hfiltMap1;
std::unordered_map<std::string, float> havgMap1;
std::unordered_map<std::string, int> hocsMap2;
std::unordered_map<std::string, int> hfiltMap2;
std::unordered_map<std::string, float> havgMap2;
std::unordered_map<std::string, int> hocsMap3;
std::unordered_map<std::string, int> hfiltMap3;
std::unordered_map<std::string, float> havgMap3;
std::mutex mu;
std::mutex lmu;
std::mutex hmu;


class dayClass
{
public:
	std::string day;
	std::vector<std::string> hour;
	std::vector<float> open;
	std::vector<float> high;
	std::vector<float> low;
	std::vector<float> close;

	dayClass(std::string d)
		:day(d)
	{}

	void dayPushback(std::string a, float o, float h, float l, float c)
	{
		hour.push_back(a);
		open.push_back(o);
		high.push_back(h);
		low.push_back(l);
		close.push_back(c);
		return;
	};

};

std::map<std::string, dayClass> dayMap;

void showHelpMsg()
{
	std::cout << "version " << APP_VERSION << std::endl << std::endl;
	std::cout << "Usage: daycomp.exe -i file1.csv file2.csv -l lowTh -h highTh" << std::endl;
	std::cout << "Or: daycomp.exe -f folder -l lowTh -h highTh -p printTh -q " << std::endl << std::endl;
	std::cout << "\t -i input csv files' names. Files' names must not contain white spaces" << std::endl;
	std::cout << "\t -f input foler's names. Folder's names must not contain white spaces" << std::endl;
	std::cout << "\t -l low filter threshold" << std::endl;
	std::cout << "\t -h high filter threshold" << std::endl;
	std::cout << "\t --h help. Show this message " << std::endl << std::endl;


	return;
}

std::vector<std::string> createDateVector()
{
	std::vector<std::string> result;
	std::stringstream ss;

	for (int i = 0; i < 24; i++)
	{
		for (int j = 0; j < 60; j++)
		{
			for (int k = 0; k < 60; k++)
			{
				if (i < 10)
					ss << "0";
				ss << i << ":";

				if (j < 10)
					ss << "0";
				ss << j << ":";

				if (k < 10)
					ss << "0";
				ss << k;

				result.push_back(ss.str());
				std::stringstream().swap(ss);
			}
		}
	}

	return result;
}

void getDataMap(dayClass day, unsigned long int idx, int low_0_high_1, int thres)
{
	std::vector<std::string> result;
	std::vector<float> avgR;
	std::stringstream ss;

	//for (unsigned long int i = idx; i < day.hour.size() - 1; i++)

	if (low_0_high_1 == 0)
	{
		for (unsigned long int j = idx + 1; j < day.hour.size(); j++)
		{
			ss << day.hour[idx] << "-" << day.hour[j];
			lmu.lock();
			locsMap[ss.str()]++;
			lmu.unlock();
			float diff = day.low[idx] - day.low[j];
			if (diff < thres)
			{

				result.push_back(ss.str());
				lmu.lock();
				//lavgMap[ss.str()] += diff;
				avgR.push_back(diff);
				lmu.unlock();

			}

			std::stringstream().swap(ss);
		}


		lmu.lock();
		ldatedata.push_back(result);
		lavgdata.push_back(avgR);
		lmu.unlock();
	}
	else
	{
		for (unsigned long int j = idx + 1; j < day.hour.size(); j++)
		{
			ss << day.hour[idx] << "-" << day.hour[j];
			hmu.lock();
			hocsMap[ss.str()]++;
			hmu.unlock();

			float diff = day.high[idx] - day.high[j];
			if (diff > thres)
			{
				//ss << day.hour[idx] << "-" << day.hour[j];
				result.push_back(ss.str());
				hmu.lock();
				//havgMap[ss.str()] += diff;
				avgR.push_back(diff);
				hmu.unlock();
				//std::stringstream().swap(ss);
			}

			std::stringstream().swap(ss);
		}


		hmu.lock();
		hdatedata.push_back(result);
		havgdata.push_back(avgR);
		hmu.unlock();
	}
}

void getDataMapMap(dayClass day, unsigned long int idx, int low_0_high_1)
{
	//std::vector<std::string> result;
	std::stringstream ss;

	//for (unsigned long int i = idx; i < day.hour.size() - 1; i++)

	for (unsigned long int j = idx + 1; j < day.hour.size(); j++)
	{
		if (low_0_high_1 == 0)
		{
			ss << day.hour[idx] << "-" << day.hour[j];
			float diff = day.low[idx] - day.low[j];
			//lmu.lock();
			locsMap[ss.str()]++;

			//lmu.unlock();

			if (diff < 4)
			{

				//lmu.lock();
				lfiltMap[ss.str()]++;
				lavgMap[ss.str()] += diff;
				//lmu.unlock();

			}
		}
		else
		{
			ss << day.hour[idx] << "-" << day.hour[j];
			float diff = day.high[idx] - day.high[j];
			//hmu.lock();
			hocsMap[ss.str()]++;

			//hmu.unlock();	

			if (diff > -4)
			{
				//hmu.lock();
				hfiltMap[ss.str()]++;
				havgMap[ss.str()] += diff;
				//hmu.unlock();
			}
		}
		std::stringstream().swap(ss);
	}
}

void getDataMapMap0(std::vector<dayClass> days, unsigned long int idx, int low_0_high_1)
{
	//std::vector<std::string> result;
	std::stringstream ss;

	//for (unsigned long int i = idx; i < day.hour.size() - 1; i++)

	for (auto& day : days)
	{
		for (unsigned long int j = idx + 1; j < day.hour.size(); j++)
		{
			if (low_0_high_1 == 0)
			{
				ss << day.hour[idx] << "-" << day.hour[j];
				float diff = day.low[idx] - day.low[j];
				//lmu.lock();
				locsMap[ss.str()]++;

				//lmu.unlock();

				if (diff < 4)
				{

					//lmu.lock();
					lfiltMap[ss.str()]++;
					lavgMap[ss.str()] += diff;
					//lmu.unlock();

				}
			}
			else
			{
				ss << day.hour[idx] << "-" << day.hour[j];
				float diff = day.high[idx] - day.high[j];
				//hmu.lock();
				hocsMap[ss.str()]++;

				//hmu.unlock();	

				if (diff > -4)
				{
					//hmu.lock();
					hfiltMap[ss.str()]++;
					havgMap[ss.str()] += diff;
					//hmu.unlock();
				}
			}
			std::stringstream().swap(ss);
		}
	}
}

void getDataMapMap1(std::vector<dayClass> days, unsigned long int idx, int low_0_high_1)
{
	//std::vector<std::string> result;
	std::stringstream ss;

	//for (unsigned long int i = idx; i < day.hour.size() - 1; i++)

	for (auto& day : days)
	{
		for (unsigned long int j = idx + 1; j < day.hour.size(); j++)
		{
			if (low_0_high_1 == 0)
			{
				ss << day.hour[idx] << "-" << day.hour[j];
				float diff = day.low[idx] - day.low[j];
				//lmu.lock();
				locsMap1[ss.str()]++;

				//lmu.unlock();

				if (diff < 4)
				{

					//lmu.lock();
					lfiltMap1[ss.str()]++;
					lavgMap1[ss.str()] += diff;
					//lmu.unlock();

				}
			}
			else
			{
				ss << day.hour[idx] << "-" << day.hour[j];
				float diff = day.high[idx] - day.high[j];
				//hmu.lock();
				hocsMap1[ss.str()]++;

				//hmu.unlock();	

				if (diff > -4)
				{
					//hmu.lock();
					hfiltMap1[ss.str()]++;
					havgMap1[ss.str()] += diff;
					//hmu.unlock();
				}
			}
			std::stringstream().swap(ss);
		}
	}
}

void getDataMapMap2(std::vector<dayClass> days, unsigned long int idx, int low_0_high_1)
{
	//std::vector<std::string> result;
	std::stringstream ss;

	//for (unsigned long int i = idx; i < day.hour.size() - 1; i++)

	for (auto& day : days)
	{
		for (unsigned long int j = idx + 1; j < day.hour.size(); j++)
		{
			if (low_0_high_1 == 0)
			{
				ss << day.hour[idx] << "-" << day.hour[j];
				float diff = day.low[idx] - day.low[j];
				//lmu.lock();
				locsMap2[ss.str()]++;

				//lmu.unlock();

				if (diff < 4)
				{

					//lmu.lock();
					lfiltMap2[ss.str()]++;
					lavgMap2[ss.str()] += diff;
					//lmu.unlock();

				}
			}
			else
			{
				ss << day.hour[idx] << "-" << day.hour[j];
				float diff = day.high[idx] - day.high[j];
				//hmu.lock();
				hocsMap2[ss.str()]++;

				//hmu.unlock();	

				if (diff > -4)
				{
					//hmu.lock();
					hfiltMap2[ss.str()]++;
					havgMap2[ss.str()] += diff;
					//hmu.unlock();
				}
			}
			std::stringstream().swap(ss);
		}
	}
}

void getDataMapMap3(std::vector<dayClass> days, unsigned long int idx, int low_0_high_1)
{
	//std::vector<std::string> result;
	std::stringstream ss;

	//for (unsigned long int i = idx; i < day.hour.size() - 1; i++)

	for (auto& day : days)
	{
		for (unsigned long int j = idx + 1; j < day.hour.size(); j++)
		{
			if (low_0_high_1 == 0)
			{
				ss << day.hour[idx] << "-" << day.hour[j];
				float diff = day.low[idx] - day.low[j];
				//lmu.lock();
				locsMap3[ss.str()]++;

				//lmu.unlock();

				if (diff < 4)
				{

					//lmu.lock();
					lfiltMap3[ss.str()]++;
					lavgMap3[ss.str()] += diff;
					//lmu.unlock();

				}
			}
			else
			{
				ss << day.hour[idx] << "-" << day.hour[j];
				float diff = day.high[idx] - day.high[j];
				//hmu.lock();
				hocsMap3[ss.str()]++;

				//hmu.unlock();	

				if (diff > -4)
				{
					//hmu.lock();
					hfiltMap3[ss.str()]++;
					havgMap3[ss.str()] += diff;
					//hmu.unlock();
				}
			}
			std::stringstream().swap(ss);
		}
	}
}

void printDays(std::vector<dayClass> days, int low_0_high_1, std::string baseName, int thres)
{
	std::vector<std::string> dates = createDateVector();
	std::vector<unsigned long int> idxs;
	std::vector<unsigned long int> int_idxs;
	std::ofstream of;
	int n_files = days.size();
	std::string name;
	int qty;
	float avgt;
	std::vector<std::thread> th;

	if (low_0_high_1 == 0)
		baseName.append("_low");
	else
		baseName.append("_high");
	baseName.append(".csv");
	of.open(baseName);
	of << std::fixed;

	std::cout << "File " << baseName << " created" << std::endl;

	of << "Timeslot,Qty,Perc,Avg\n";

	for (int i = 0; i < n_files; i++)
	{
		idxs.push_back(0);
	}

	// initialize data vector
	/*for (long int i = 0; i < days.size(); i++)
	{
		data.push_back(getDataMap(days[i], idxs[i], low_0_high_1));
	}*/

	try
	{
		// go through the date vector
		{
			for (long int i = 0; i < dates.size() - 1; i++)
			{
				// Prepare file vectors for this hour;
				if (low_0_high_1 == 0)
				{
					ldatedata.clear();
					locsMap.clear();
					lavgdata.clear();
				}
				else
				{
					hdatedata.clear();
					hocsMap.clear();
					havgdata.clear();
				}
				th.clear();
				//auto map_start = std::chrono::high_resolution_clock::now();
				for (int n = 0; n < n_files; n++)
				{
					if (days[n].hour.size() > 0)
					{
						if ((idxs[n] < days[n].hour.size() - 1) && (dates[i].compare(days[n].hour[idxs[n]]) == 0))
						{
							th.push_back(std::thread(getDataMap, days[n], idxs[n], low_0_high_1, thres));
							//getDataMap(days[n], idxs[n], low_0_high_1);

							/*try {
								data.push_back(v);
							}
							catch (std::exception const& e)
							{
								std::cout << e.what();
							}*/
							idxs[n] = idxs[n] + 1;
						}
					}
				}
				for (auto& t : th)
				{
					t.join();
				}
				//auto map_end = std::chrono::high_resolution_clock::now();

				int_idxs.clear();
				for (int i = 0; i < n_files; i++)
				{
					int_idxs.push_back(0);
				}
				//auto w_start = std::chrono::high_resolution_clock::now();
				for (long int j = i + 1; j < dates.size(); j++)
				{
					name = dates[i];
					name.append("-").append(dates[j]);

					qty = 0;
					avgt = 0;

					if (low_0_high_1 == 0)
					{
						for (int f = 0; f < ldatedata.size(); f++)
						{
							if (ldatedata[f].size() > 0)
							{
								if (name.compare(ldatedata[f][int_idxs[f]]) == 0) // current mane matchs the first date in the day
								{
									qty++;
									avgt += lavgdata[f][int_idxs[f]];
									if (int_idxs[f] < (ldatedata[f].size() - 1))
										int_idxs[f] = int_idxs[f] + 1;
								}
							}
						}
						if (qty > 0)
						{
							try {
								//of << name << "," << qty << "," << qty * 100 / locsMap[name] << "," <<
									//std::setprecision(4) << lavgMap[name] / qty << "\n";
								of << name << "," << qty << "," << qty * 100 / locsMap[name] << "," <<
									std::setprecision(4) << avgt / qty << "\n";
							}
							catch (std::exception const& e)
							{
								std::cout << e.what();
							}
						}
					}
					else
					{
						for (int f = 0; f < hdatedata.size(); f++)
						{
							if (hdatedata[f].size() > 0)
							{
								if (name.compare(hdatedata[f][int_idxs[f]]) == 0) // current mane matchs the first date in the day
								{
									qty++;
									avgt += havgdata[f][int_idxs[f]];
									if (int_idxs[f] < (hdatedata[f].size() - 1))
										int_idxs[f] = int_idxs[f] + 1;
								}
							}
						}
						if (qty > 0)
						{
							try {
								of << name << "," << qty << "," << qty * 100 / hocsMap[name] << "," <<
									std::setprecision(4) << avgt / qty << "\n";
							}
							catch (std::exception const& e)
							{
								std::cout << e.what();
							}
						}
					}
				}
				//auto w_end = std::chrono::high_resolution_clock::now();
				if (i % 1800 == 0)
				{
					std::cout << baseName << " " << dates[i] << std::endl;
					//of.close();
					//of.open(baseName, std::ofstream::out | std::ofstream::app);
				}
				//std::chrono::duration<double> mapelapsed = map_end - map_start;
				//std::chrono::duration<double> welapsed = w_end - w_start;
				//std::chrono::duration<double> telapsed = w_end - map_start;
				//std::cout << low_0_high_1 << " i: " << i << " map: " << mapelapsed.count() << ", write: " << welapsed.count() << std::endl;
				//if (low_0_high_1 == 0)
					//std::cout << "VEC " << " i: " << i << " total: " << telapsed.count() << std::endl;
			}
		}
	}
	catch (std::exception const& e)
	{
		std::cout << baseName << " " << e.what();
	}
}

void printDaysMap(std::vector<dayClass> days, int low_0_high_1, std::string baseName)
{
	std::vector<std::string> dates = createDateVector();
	std::vector<unsigned long int> idxs;
	std::vector<unsigned long int> int_idxs;
	std::ofstream of;
	int n_files = days.size();
	std::string name;
	int qty = 0;
	std::vector<std::thread> th;
	std::vector<dayClass> vec0;
	std::vector<dayClass> vec1;
	std::vector<dayClass> vec2;
	std::vector<dayClass> vec3;
	int daysj = 0;
	int t_number = std::thread::hardware_concurrency();

	if (low_0_high_1 == 0)
		baseName.append("_low");
	else
		baseName.append("_high");
	baseName.append(".csv");
	of.open(baseName);
	of << std::fixed;

	std::cout << "File " << baseName << " created" << std::endl;

	of << "Timeslot,Qty,Perc,Avg\n";

	for (int i = 0; i < n_files; i++)
	{
		idxs.push_back(0);
	}

	// initialize data vector
	/*for (long int i = 0; i < days.size(); i++)
	{
		data.push_back(getDataMap(days[i], idxs[i], low_0_high_1));
	}*/

	try
	{
		// go through the date vector
		{

			for (long int i = 0; i < dates.size() - 1; i++)
			{
				//auto map_start = std::chrono::high_resolution_clock::now();
				// Prepare file vectors for this hour;
				if (low_0_high_1 == 0)
				{
					//ldatedata.clear();
					locsMap.clear();
					lavgMap.clear();
					locsMap1.clear();
					lavgMap1.clear();
					locsMap2.clear();
					lavgMap2.clear();
					locsMap3.clear();
					lavgMap3.clear();
				}
				else
				{
					//hdatedata.clear();
					hocsMap.clear();
					havgMap.clear();
					hocsMap1.clear();
					havgMap1.clear();
					hocsMap2.clear();
					havgMap2.clear();
					hocsMap3.clear();
					havgMap3.clear();
				}
				th.clear();
				for (int n = 0; n < n_files; n++)
				{
					if (days[n].hour.size() > 0)
					{
						if ((idxs[n] < days[n].hour.size() - 1) && (dates[i].compare(days[n].hour[idxs[n]]) == 0))
						{
							//th.push_back(std::thread(getDataMapMap, days[n], idxs[n], low_0_high_1));
							//getDataMapMap(days[n], idxs[n], low_0_high_1);

							if (daysj == 0)
							{
								vec0.push_back(days[n]);
								th.push_back(std::thread(getDataMapMap0, vec0, idxs[n], low_0_high_1));
							}
							else if (daysj == 1)
							{
								vec1.push_back(days[n]);
								th.push_back(std::thread(getDataMapMap1, vec1, idxs[n], low_0_high_1));
							}
							else if (daysj == 2)
							{
								vec2.push_back(days[n]);
								th.push_back(std::thread(getDataMapMap2, vec2, idxs[n], low_0_high_1));
							}
							else
							{
								vec3.push_back(days[n]);
								th.push_back(std::thread(getDataMapMap3, vec3, idxs[n], low_0_high_1));
							}

							/*try {
								data.push_back(v);
							}
							catch (std::exception const& e)
							{
								std::cout << e.what();
							}*/
							idxs[n] = idxs[n] + 1;
							daysj++;
							if (daysj > 3)
							{
								daysj == 0;
								for (auto& t : th)
								{
									t.join();
								}
								th.clear();
								vec0.clear();
								vec1.clear();
								vec2.clear();
								vec3.clear();
							}
						}
					}
				}
				for (auto& t : th)
				{
					t.join();
				}
				//auto map_end = std::chrono::high_resolution_clock::now();
				int_idxs.clear();
				for (int h = 0; h < n_files; h++)
				{
					int_idxs.push_back(0);
				}
				//auto w_start = std::chrono::high_resolution_clock::now();
				for (long int j = i + 1; j < dates.size(); j++)
				{
					name = dates[i];
					name.append("-").append(dates[j]);
					int qty = 0;
					int ocs = 0;
					float avgs = 0;
					auto it = lfiltMap.find(name);
					if (it != lfiltMap.end())
					{
						qty += it->second;
						ocs += locsMap[name];
						avgs += lavgMap[name];
					}
					auto it1 = lfiltMap1.find(name);
					if (it1 != lfiltMap1.end())
					{
						qty += it1->second;
						ocs += locsMap1[name];
						avgs += lavgMap1[name];
					}
					auto it2 = lfiltMap2.find(name);
					if (it2 != lfiltMap2.end())
					{
						qty += it1->second;
						ocs += locsMap2[name];
						avgs += lavgMap2[name];
					}
					auto it3 = lfiltMap3.find(name);
					if (it3 != lfiltMap3.end())
					{
						qty += it3->second;
						ocs += locsMap3[name];
						avgs += lavgMap3[name];
					}

					if (qty > 0)
					{
						of << name << "," << qty << "," << qty * 100 / ocs << "," <<
							std::setprecision(4) << avgs / qty << "\n";
					}
				}
				lfiltMap.clear();
				lfiltMap1.clear();
				lfiltMap2.clear();
				lfiltMap3.clear();
				/*if (low_0_high_1 == 0)
				{
					for (auto& d : lfiltMap)
					{
						try {
							of << d.first << "," << d.second << "," << d.second * 100 / locsMap[d.first] << "," <<
								std::setprecision(4) << lavgMap[d.first] / locsMap[d.first] << "\n";
						}
						catch (std::exception const& e)
						{
							std::cout << e.what();
						}
					}
					lfiltMap.clear();
				}
				else
				{
					for (auto& d : hfiltMap)
					{
						try {
							of << d.first << "," << d.second << "," << d.second * 100 / hocsMap[d.first] << "," <<
								std::setprecision(4) << havgMap[d.first] / hocsMap[d.first] <<"\n";
						}
						catch (std::exception const& e)
						{
							std::cout << e.what();
						}
					}
					hfiltMap.clear();
				}*/

				//auto w_end = std::chrono::high_resolution_clock::now();
				if (i % 1800 == 0)
				{
					std::cout << baseName << " " << dates[i] << std::endl;
					//of.close();
					//of.open(baseName, std::ofstream::out | std::ofstream::app);
				}

				//std::chrono::duration<double> mapelapsed = map_end - map_start;
				//std::chrono::duration<double> welapsed = w_end - w_start;
				//std::chrono::duration<double> telapsed = w_end - map_start;
				//std::cout << low_0_high_1 <<" i: " << i <<  " map: " << mapelapsed.count() << ", write: " << welapsed.count() << std::endl;
				//if(low_0_high_1 == 0)
					//std::cout << "MAP " <<" i: " << i << " total: " << telapsed.count() << std::endl;
			}

		}
	}
	catch (std::exception const& e)
	{
		std::cout << baseName << " " << e.what();
	}
}

std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> result;
	std::stringstream ss(s);
	std::string item;

	while (getline(ss, item, delim)) {
		result.push_back(item);
	}

	return result;
}

int findInDays(std::vector<dayClass> days, std::string date, std::string hour)
{
	for (auto& d : days)
	{
		if (d.day.compare(date) == 0)
		{
			if (d.hour.size() > 0)
			{
				auto bit = d.hour.begin();
				auto eit = d.hour.back();
				if ((hour >= *bit) && (hour <= eit))
				{
					return 1;
				}
			}
		}
	}

	return 0;
}

void fillDays(std::string filen, CSVFormat format)
{
	try
	{
		CSVReader reader(filen, format);
		CSVRow row;
		std::string currentDay = "";
		std::vector< std::string> parsStamp;

		for (CSVRow& row : reader)
		{
			if (row["Open Price bid"].is_num())
			{
				parsStamp = split(row["Time"].get<std::string>(), 'T');
				dayClass auxDay = dayClass(parsStamp[0]);
				auxDay.dayPushback(parsStamp[1], atof(row["Open Price bid"].get().c_str())
					, atof(row["High Price bid"].get().c_str())
					, atof(row["Low Price ask"].get().c_str())
					, atof(row["Close Price ask"].get().c_str()));
				mu.lock();
				//dayMap[row[" Time"].get<std::string>()] = auxDay;
				dayMap.insert(std::pair<std::string, dayClass>(row["Time"].get<std::string>(), auxDay));
				mu.unlock();
			}
		}
		std::cout << filen << " file readed \n";
	}
	catch (std::exception e)
	{
		std::cout << "File error " << filen << e.what();
		exit(1);
	}
}

int main(int argc, char **argv)
{
	std::cout << "Hello World!\n";
	time_t t = time(NULL);
	struct tm *startT = gmtime(&t);
	int stH = startT->tm_hour;
	int stM = startT->tm_min;
	int stS = startT->tm_sec;
	struct tm *endT;
	std::vector<std::string> in_v;
	std::vector<std::string> inputFileNames;
	std::stringstream ss;
	bool cant = false;
	int highTh = -4;
	int lowTh = 4;
	int printTh = 1;
	std::string fold;
	bool byFiles = true;
	std::vector<dayClass> oneDay;

	// Argument parsing
	if (argc > 1)
	{
		for (int argi = 1; argi < argc; argi++)
		{
			if (strcmp(argv[argi], "-i") == 0)
			{
				// input files
				argi++;
				if ((argi < argc) && (argv[argi][0] != '-'))
				{
					while ((argi < argc) && (argv[argi][0] != '-'))
					{
						inputFileNames.push_back(argv[argi]);
						argi++;
					}

					if (argi < argc)
					{
						argi--;
					}
				}
				else
				{
					std::cout << "Error in input file name. Use --help for help" << std::endl;
					return 0;
				}
			}
			else if (strcmp(argv[argi], "-f") == 0)
			{
				// input files
				argi++;
				if ((argi < argc) && (argv[argi][0] != '-'))
				{
					std::stringstream sst;
					sst << "python jasonice.py ";
					fold = argv[argi];
					byFiles = false;
					sst << fold;
				}
				else
				{
					std::cout << "Error in input file name. Use --help for help" << std::endl;
					return 0;
				}
			}
			else if (strcmp(argv[argi], "-l") == 0)
			{
				// input files
				argi++;
				if ((argi < argc))
				{
					lowTh = atoi(argv[argi]);
				}
				else
				{
					std::cout << "Error in input file name. Use --help for help" << std::endl;
					return 0;
				}
			}
			else if (strcmp(argv[argi], "-h") == 0)
			{
				// input files
				argi++;
				if ((argi < argc) && (argv[argi][0] != '-'))
				{
					highTh = atoi(argv[argi]);
				}
				else
				{
					std::cout << "Error in input file name. Use --help for help" << std::endl;
					return 0;
				}
			}
			else if (strcmp(argv[argi], "-p") == 0)
			{
				// input files
				argi++;
				if ((argi < argc) && (argv[argi][0] != '-'))
				{
					printTh = atoi(argv[argi]);
				}
				else
				{
					std::cout << "Error in input file name. Use --help for help" << std::endl;
					return 0;
				}
			}
			else if (strcmp(argv[argi], "-q") == 0)
			{
				cant = true;
			}
			else if (strcmp(argv[argi], "--help") == 0)
			{
				showHelpMsg();
				return 0;
			}
		}
	}
	else
	{
		//inputFileName = "./the super boring stuf1.csv";
		//inputFileNames.push_back("CC-D-CL-UMP-IP_2020_07_07.csv");
		//inputFileNames.push_back("CC-D-CL-UMP-IP_2020_07_08.csv");
		//inputFileNames.push_back("CC-D-CL-UMP-IP_2020_07_09.csv");
		inputFileNames.push_back("CC-D-CL-UMP-IP_2020_07_10_.csv");
		//inputFileNames.push_back("CC-D-CL-UMP-IP_2020_07_12-13.csv");
		//showHelpMsg();
		//return 0;
	}

	std::cout << "Start processing at " << startT->tm_hour << ":" << startT->tm_min << ":" << startT->tm_sec << std::endl;

	if (!byFiles)
	{
		std::string l;
		std::cout << "Processing folder " << fold << std::endl;
		std::cout << "Generating json files" << std::endl;
		std::stringstream sst;
		sst << "python3 jasonice.py " << fold;
		system(sst.str().c_str());

		std::cout << "Generating csv files" << std::endl;
		system("python3 blanket1.py");

		std::ifstream csvf("csvfiles.txt");

		if (csvf.is_open())
		{
			inputFileNames.clear();
			while (std::getline(csvf, l))
			{
				inputFileNames.push_back(l);
			}
		}
		else {
			std::cout << "Unable to open csvfile.txt. Run again the scripts" << std::endl;
			return 0;
		}
	}

	//std::vector<dayClass> oneDay;

	CSVFormat format;
	format.delimiter(',');
	format.variable_columns(false); // Short-hand
	int dayIdx = 0;
	//format.trim({ ' ', '\t' });
	//format.variable_columns(VariableColumnPolicy::IGNORE);
	std::vector<std::thread> f_th;
	for (auto& filen : inputFileNames)
	{
		f_th.push_back(std::thread(fillDays, filen, format)); // one thread per file
		//try
		//{
		//	CSVReader reader(filen, format);


		//	CSVRow row;
		//	std::string currentDay = "";
		//	std::vector< std::string> parsStamp;

		//	for (CSVRow& row : reader)
		//	{
		//		if (row[" Open Price"].is_num())
		//		{
		//			parsStamp = split(row[" Time"].get<std::string>(), 'T');
		//			if (parsStamp[0].compare(currentDay) != 0) // nuevo día
		//			{
		//				currentDay = parsStamp[0];
		//				oneDay.push_back(dayClass(parsStamp[0]));
		//				dayIdx++;
		//			}
		//			if (findInDays(oneDay, parsStamp[0], parsStamp[1]) == 0)
		//			{
		//				oneDay[dayIdx - 1].dayPushback(parsStamp[1], atof(row[" Open Price"].get().c_str())
		//					, atof(row[" High Price"].get().c_str())
		//					, atof(row[" Low Price"].get().c_str())
		//					, atof(row[" Close Price"].get().c_str()));
		//			}
		//		}
		//	}
		//	std::cout << filen << " file readed \n";
		//}
		//catch (std::exception e)
		//{
		//	std::cout << "File error " << filen << e.what();
		//	exit(1);
		//}
	}

	// join threads
	for (auto& t : f_th)
	{
		t.join();
	}

	// Fill oneDay
	std::string currD = "";
	std::vector<std::string> ts;
	for (auto& d : dayMap)
	{
		ts = split(d.first, 'T');
		if (ts[0].compare(currD) == 0) // if is the same day
		{
			if (oneDay.back().hour.back().compare(ts[1]) < 0) // new timestamp
			{
				oneDay.back().dayPushback(d.second.hour[0], d.second.open[0], d.second.high[0],
					d.second.low[0], d.second.close[0]);
			}
			else // timestamp before actual, create new day
			{
				oneDay.push_back(dayClass(ts[0]));
				oneDay.back().dayPushback(d.second.hour[0], d.second.open[0], d.second.high[0],
					d.second.low[0], d.second.close[0]);
			}
		}
		else // different day
		{
			oneDay.push_back(dayClass(ts[0]));
			oneDay.back().dayPushback(d.second.hour[0], d.second.open[0], d.second.high[0],
				d.second.low[0], d.second.close[0]);
			currD = ts[0];
		}
	}

	// free map
	dayMap.clear();
	// clean oneDay
	/*int getOut = 0;
	int dayi = 0;
	while (getOut == 0)
	{
		if (oneDay[dayi].hour.size() == 0)
		{
			oneDay.erase(oneDay.begin() + dayi);
			std::cout << (oneDay.begin() + dayi)->day << " erased" << std::endl;
			dayi = 0;
		}
		else
		{
			dayi++;
		}

		if (dayi >= oneDay.size())
		{
			getOut = 1;
		}

	}
	*/

	for (dayClass& d : oneDay)
	{
		std::cout << d.day << " with " << d.hour.size() << " entries " << std::endl;
	}

	// one thread for low, one for high
	std::thread lowT(printDays, oneDay, 0, "base", lowTh);
	std::thread highT(printDays, oneDay, 1, "base", highTh);

	//std::thread lowT(printDaysMap, oneDay, 0, "base", lowTh);
	//std::thread highT(printDaysMap, oneDay, 1, "base", highTh);

	lowT.join();
	highT.join();

	//printDays(oneDay, 0, "base");

	time_t te = time(NULL);
	endT = gmtime(&te);
	std::cout << "Start processing at " << stH << ":" << stM << ":" << stS << std::endl;
	std::cout << "End processing at " << endT->tm_hour << ":" << endT->tm_min << ":" << endT->tm_sec << std::endl;
}

// Ejecutar programa: Ctrl + F5 o menú Depurar > Iniciar sin depurar
// Depurar programa: F5 o menú Depurar > Iniciar depuración

// Sugerencias para primeros pasos: 1. Use la ventana del Explorador de soluciones para agregar y administrar archivos
//   2. Use la ventana de Team Explorer para conectar con el control de código fuente
//   3. Use la ventana de salida para ver la salida de compilación y otros mensajes
//   4. Use la ventana Lista de errores para ver los errores
//   5. Vaya a Proyecto > Agregar nuevo elemento para crear nuevos archivos de código, o a Proyecto > Agregar elemento existente para agregar archivos de código existentes al proyecto
//   6. En el futuro, para volver a abrir este proyecto, vaya a Archivo > Abrir > Proyecto y seleccione el archivo .sln

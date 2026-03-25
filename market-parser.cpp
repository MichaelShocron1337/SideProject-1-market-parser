#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <utility>
#include <set>
#include <queue>
#include <deque>
#include <stack>
#include <algorithm>
#include <numeric>
#include <limits>
#include <cctype>
#include <fstream>
#include <sstream>
#include <queue>
#include <cmath>





using namespace std;
/*Date,Open,High,Low,Close,Adj Close,Volume
*/

class MarketData
{
    public:
    string date;
    double open;
    double high;
    double low;
    double close;
    double adjClose;
    double volume;

    public:

    MarketData(const string& date, double open, double high, double low, double close, double adjClose, double volume)
    :date(date), open(open), high(high), low(low), close(close), adjClose(adjClose), volume(volume)
    {
    }
};

ostream& operator<<(ostream& os, MarketData data)
{
    os<< data.date<<" "
      << data.open<<" "
      << data.high<<" "
      << data.low<<" "
      << data.close<<" "
      << data.adjClose<<" "
      <<data.volume;
    return os;
}


double MovingAverge (const vector<MarketData>&Data ,int Period)
{
    //daily closing price * days / days
    double AccumlativeDailyprice = 0;
    for(int i = 0; i < Period; i++)
    {
        AccumlativeDailyprice += Data[i].close;   
    }
    double resultat = AccumlativeDailyprice/Period;
    return resultat;
}

double Total_VWAP (const vector<MarketData>& data)
{
    //formeln: tp*vol / total vol
    // räkna först varje dag = sumoftäljare / totvol
    double sum_tp_vol = 0;
    double sum_vol = 0;
    for(size_t i = 0; i < data.size(); i++)
    {
        if(data[i].volume == 0||
            data[i].close == 0 ||
            data[i].high == 0  ||
            data[i].low == 0)
        {
            throw runtime_error("Invalid data row");
        }
        double typical_price = 0;
        typical_price = (data[i].close + data[i].high + data[i].low) / 3; // tp
        sum_tp_vol += typical_price * data[i].volume;
        sum_vol += data[i].volume;
    }
    double resultat = sum_tp_vol/sum_vol;
    return resultat;

}

double Std1_VWAP (const vector<MarketData>& data)
{
    /*Täljare formel: 
    contribution = vol[i] * (price[i]- wvwap)^2
    */

    double contribution = 0;
    double vwap = Total_VWAP(data);
    double sum_vol = 0;
    for(size_t i = 0; i < data.size(); i++)
    {
        double vol = 0;
        double price = 0; // TP
        double diff = 0;
        price = (data[i].close + data[i].low + data[i].high) / 3;
        vol = data[i].volume;
        diff = (price-vwap) * (price-vwap);
        contribution += vol*diff;
        sum_vol += vol;
    }
    double resultat = sqrt(contribution/sum_vol);
    return resultat;
}

double Std1_Above_VWAP (const vector<MarketData>& data)
{
    double vwap = Total_VWAP(data);
    double variance = Std1_VWAP(data);
    return vwap + variance;
}

double Std1_Below_VWAP(const vector<MarketData>& data)
{
    double vwap = Total_VWAP(data);
    double variance = Std1_VWAP(data);
    return vwap - variance;
}

MarketData DailyStats (const vector<MarketData>& data, string day)
{

    
    for(int i = 0; i < data.size(); i++)
    {
        if(data[i].date == day)
        {
            return data[i];
        }
    }
    throw runtime_error("Date not found \n");
    
}
MarketData HighestPrice(const vector<MarketData>& data)
{
    double HighestPrice = data[0].high;
    int counter = 0;
    for(int i = 0; i < data.size(); i++)
    {
        if(data[i].high > HighestPrice)
        {
            HighestPrice = data[i].high;
            counter = i;
            
        }
    }
    return data[counter];
}

MarketData LowestPrice(const vector<MarketData>& data)
{
    double lowestPrice = data[0].low;
    int counter = 0;
    for(int i = 0; i < data.size(); i++)
    {
        if(data[i].low < lowestPrice)
        {
            lowestPrice = data[i].low;
            counter = i;
        }
    }
    return data[counter];
    
}

bool readFromFile(vector<MarketData>& DataList, const string& filename)
{
    std::ifstream file(filename);
    if(!file.is_open())
    {
        cout<<"Couldn't open file \n";
        return false;
    }
    string line;
    getline(file, line); // kasta header
    
    while(getline(file, line))
    {
        stringstream ss (line);
        string date;
        string openStr;
        string highStr;
        string lowStr;
        string closeStr;
        string adjCloseStr;
        string volumeStr;
        getline(ss, date, ',');
        getline(ss, openStr, ',');
        getline(ss,highStr, ',');
        getline(ss,lowStr, ',');
        getline(ss,closeStr, ',');
        getline(ss,adjCloseStr, ',');
        getline(ss,volumeStr, ',');
        double open = stod(openStr);
        double high = stod(highStr);
        double low = stod(lowStr);
        double close = stod(closeStr);
        double adjClose = stod(adjCloseStr);
        double volume = stod(volumeStr);
        DataList.emplace_back(date, open, high, low, close, adjClose, volume);
        
    }
    return true;

}



int main(int argc, char* argv[])
{
     if(argc < 3)
    {
        cout<<"Program sequence: {function} {file name} {parameter/s}";
        cout<<"Date sequence: Year-Month-Day, e.g 1994-02-05";
        return 1;
    }
    vector<MarketData> Marketinfo;
    string command = argv[1];
    string file = argv[2];
    if(!readFromFile(Marketinfo,file))
    {
        return 1;
    }
    if(Marketinfo.empty())
    {
        cout<<"Data not loaded";
        return 1;
    }
    
    if(command == "LowestPrice")
    {
        auto text = LowestPrice(Marketinfo);
        cout<<text;
    }
    else if(command == "HighestPrice")
    {
        auto text = HighestPrice(Marketinfo);
        cout<<text;
    }
    else if(command == "DailyStats")
    {
        if(argc < 4)
        {
            cout<<"Too few arguments \n";
            return 1;
        }
        auto text = DailyStats(Marketinfo,argv[3]);
        cout<<text;
    }
    else if(command == "Total_VWAP")
    {
        auto text = Total_VWAP(Marketinfo);
        cout<<text;
        // Doesn't overload since it's not MarketData, returning the VWAP Value only
    }
    else if(command == "MovingAverage")
    {
        if(argc < 4)
        {
            cout<<"Too few arguments \n";
            return 1;
        }
        int period = stoi(argv[3]);
        auto text = MovingAverge(Marketinfo,period);
        cout<<text;
    }
    else if(command == "Std1_VWAP")
    {
        auto text = Std1_VWAP(Marketinfo);
        cout<<text;
    }
    else if(command == "Std1_Above_VWAP")
    {
        auto text = Std1_Above_VWAP(Marketinfo);
        cout<<text;
    }
    else if(command == "Std1_Below_VWAP")
    {
        auto text = Std1_Below_VWAP(Marketinfo);
        cout<<text;
    }
    
}

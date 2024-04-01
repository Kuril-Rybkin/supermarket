#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <queue>
#include <stack>
#include <deque>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <memory>
using namespace std;
#endif /* __PROGTEST__ */

class CDate
{
public:
    tm storedDate = {0};

    static bool validDate(int y, int m, int d) {
        struct tm start = {0};

        start.tm_mday = d;
        start.tm_mon = m;
        start.tm_year = y;

        time_t rawtime = mktime(&start) - timezone;

        if (rawtime == -1 || y < 100 || y > 130 || d != start.tm_mday || m != start.tm_mon || y != start.tm_year) {
            return false;
        }
        else {
            return true;
        }
    }

public:
    CDate(int y, int m, int d) {
        m--;
        y -= 1900;

        storedDate.tm_mday = d;
        storedDate.tm_mon = m;
        storedDate.tm_year = y;
    }

    bool operator== (CDate & rhs) {
        time_t rawtime0 = mktime(&storedDate) - timezone;
        time_t rawtime1 = mktime(&(rhs.storedDate)) - timezone;

        return rawtime0 == rawtime1;
    }

    bool operator!= (CDate & rhs) {
        return !(operator==(rhs));
    }

    bool operator< (const CDate & rhs) const {
        if (storedDate.tm_year < rhs.storedDate.tm_year) {
            return true;
        }
        else if ((storedDate.tm_year == rhs.storedDate.tm_year) && (storedDate.tm_mon < rhs.storedDate.tm_mon)) {
            return true;
        }
        else if (((storedDate.tm_year == rhs.storedDate.tm_year) && (storedDate.tm_mon == rhs.storedDate.tm_mon)) && (storedDate.tm_mday < rhs.storedDate.tm_mday)) {
            return true;
        }
        return false;
    }

    bool operator> (CDate & rhs) {
        time_t rawtime0 = mktime(&storedDate) - timezone;
        time_t rawtime1 = mktime(&rhs.storedDate) - timezone;

        return rawtime0 > rawtime1;
    }
};

class CSupermarket
{
public:
    CSupermarket & store( string name, CDate date, int count ) {
        auto pos = db.find(name); //try to find item if its already stored

        if (pos == db.end()) { //new item name, add it
            vector<pair<CDate, int>> temp; //initialize vector of pairs of ExpDates and amount for new product
            temp.emplace_back(pair<CDate, int>(date, count));//push first pair

            db.insert(pair<string,vector<pair<CDate,int>>>(name, temp));//store first product
        }
        else { //item exists, just add amount
            auto vecPos = lower_bound(db[name].begin(), db[name].end(), pair<CDate, int>(date, count), [](pair<CDate, int> lhs, pair<CDate, int> rhs) {
                return rhs.first > lhs.first; //find where it is stored
            });

            pos->second.insert(vecPos, pair<CDate, int>(date, count)); //add new instance of amount and expDate
        }
        return *this;
    }

    void sell(list<pair<string, int>> & ls) {
        string product;
        auto listIt = ls.begin();
        long unsigned int idx = 0;

        while (listIt != ls.end()) {
            int counter = 0;
            bool match = true;
            auto pos = db.find(listIt->first); //find product by exact name
//=====================for  product in list find appropriate key in storage===================

            if (pos == db.end()) { //if item not found by exact name
                for (auto j : db) { //iterate through storage again to possibly find with one typo
                    if (errCmp(listIt->first, j.first) == 1) {//found a possibly appropriate name with typo
                        product = j.first; //store key for finding it
                        counter++; //add number of possible keys
                    }
                    if (counter > 1) { //ambigious name in list, has more than two matching keys, do not sell
                        match = false;
                        break;
                    }
                }
                if (counter == 0 || counter > 1) { //no match or ambigious match
                    ++listIt; //do nothing to list, advance to sell further
                    match = false;
                    continue;
                }
            }
            else { //item found by exact so simple store key how to find it
                product = pos->first;
            }
//========= now sell appropriate amounts, delete sold stuff from map and list if necessary========
            auto found = db.find(product);

            while (idx < found->second.size() && match) {  //if match exists iterate through storage vector of this item
                if (listIt->second >= found->second[idx].second) {//you have to sell more or same amount than you have
                    listIt->second -= found->second[idx].second; //subtract sold amount from list
                    found->second[idx].second = 0; // //you sold all amount from storage

                    found->second.erase(found->second.begin() + idx); //erase empty instances of product
                    if (found->second.empty()) {//if all amount of product was sold
                        db.erase(found); //delete item from map
                        listIt++;
                        break;
                    }
                }
                else {//you have to sell less amount than you have
                    found->second[idx].second -= listIt->second; //subtract sold amount
                    listIt->second = 0; //show you satisfied requirement of client fully
                    idx++;
                }

                if (listIt->second <= 0) { //you satisfied requirement of client fully
                    auto temp = listIt; //safe delete from list
                    --listIt;
                    ls.erase(temp);
                    break;
                }
            }
            idx = 0;
            ++listIt;
        }
    }

    list<pair<string, int>> expired(CDate date) const{
        list<pair<string, int>> result;

        for (auto & i : db) { //iterate through storage map
            int expiredCount = 0;
            for (auto & j : i.second) { //it through vector of each product
                if (j.first < date) { //if this amount of product is expired
                    expiredCount += j.second; //add it to amount of expired stuff
                }
            }
            if (expiredCount > 0) { //if some portion of product is expired add to list
                result.emplace_back(pair<string, int>(i.first, expiredCount));
            }
        }
        result.sort([](pair<string, int> & lhs, pair<string, int> & rhs){
            return lhs.second > rhs.second;//sort list
        });

        return result;
    }

private:
    map<string, vector<pair<CDate, int>>> db;

    //comparison of strings for exact match or just one type

    static bool errCmp(const string & x, const string & y) {
        int discrepancies = 0;

        if (x.size() != y.size()) {
            return false;
        }

        auto iterX = x.begin();
        auto iterY = y.begin();

        while (iterX < x.end() && iterY < y.end()) {
            if (*iterX != *iterY) {
                discrepancies++; // strings differ on a position
            }
            if (discrepancies > 1) //more than one typo
                return false;
            ++iterX;
            ++iterY;
        }

        return true;
    }
};
#ifndef __PROGTEST__
int main ( void )
{
    CSupermarket s;
    s . store ( "bread", CDate ( 2016, 4, 30 ), 100 )
            . store ( "butter", CDate ( 2016, 5, 10 ), 10 )
            . store ( "beer", CDate ( 2016, 8, 10 ), 50 )
            . store ( "bread", CDate ( 2016, 4, 25 ), 100 )
            . store ( "okey", CDate ( 2016, 7, 18 ), 5 );

    list<pair<string,int> > l0 = s . expired ( CDate ( 2018, 4, 30 ) );
    assert ( l0 . size () == 4 );
    assert ( ( l0 == list<pair<string,int> > { { "bread", 200 }, { "beer", 50 }, { "butter", 10 }, { "okey", 5 } } ) );

    list<pair<string,int> > l1 { { "bread", 2 }, { "Coke", 5 }, { "butter", 20 } };
    s . sell ( l1 );
    assert ( l1 . size () == 2 );
    assert ( ( l1 == list<pair<string,int> > { { "Coke", 5 }, { "butter", 10 } } ) );

    list<pair<string,int> > l2 = s . expired ( CDate ( 2016, 4, 30 ) );
    assert ( l2 . size () == 1 );
    assert ( ( l2 == list<pair<string,int> > { { "bread", 98 } } ) );

    list<pair<string,int> > l3 = s . expired ( CDate ( 2016, 5, 20 ) );
    assert ( l3 . size () == 1 );
    assert ( ( l3 == list<pair<string,int> > { { "bread", 198 } } ) );

    list<pair<string,int> > l4 { { "bread", 105 } };
    s . sell ( l4 );
    assert ( l4 . size () == 0 );
    assert ( ( l4 == list<pair<string,int> > {  } ) );

    list<pair<string,int> > l5 = s . expired ( CDate ( 2017, 1, 1 ) );
    assert ( l5 . size () == 3 );
    assert ( ( l5 == list<pair<string,int> > { { "bread", 93 }, { "beer", 50 }, { "okey", 5 } } ) );

    s . store ( "Coke", CDate ( 2016, 12, 31 ), 10 );

    list<pair<string,int> > l6 { { "Cake", 1 }, { "Coke", 1 }, { "cake", 1 }, { "coke", 1 }, { "cuke", 1 }, { "Cokes", 1 } };
    s . sell ( l6 );
    assert ( l6 . size () == 3 );
    assert ( ( l6 == list<pair<string,int> > { { "cake", 1 }, { "cuke", 1 }, { "Cokes", 1 } } ) );

    list<pair<string,int> > l7 = s . expired ( CDate ( 2017, 1, 1 ) );
    assert ( l7 . size () == 4 );
    assert ( ( l7 == list<pair<string,int> > { { "bread", 93 }, { "beer", 50 }, { "Coke", 7 }, { "okey", 5 } } ) );

    s . store ( "cake", CDate ( 2016, 11, 1 ), 5 );

    list<pair<string,int> > l8 { { "Cake", 1 }, { "Coke", 1 }, { "cake", 1 }, { "coke", 1 }, { "cuke", 1 } };
    s . sell ( l8 );
    assert ( l8 . size () == 2 );
    assert ( ( l8 == list<pair<string,int> > { { "Cake", 1 }, { "coke", 1 } } ) );

    list<pair<string,int> > l9 = s . expired ( CDate ( 2017, 1, 1 ) );
    assert ( l9 . size () == 5 );
    assert ( ( l9 == list<pair<string,int> > { { "bread", 93 }, { "beer", 50 }, { "Coke", 6 }, { "okey", 5 }, { "cake", 3 } } ) );

    list<pair<string,int> > l10 { { "cake", 15 }, { "Cake", 2 } };
    s . sell ( l10 );
    assert ( l10 . size () == 2 );
    assert ( ( l10 == list<pair<string,int> > { { "cake", 12 }, { "Cake", 2 } } ) );

    list<pair<string,int> > l11 = s . expired ( CDate ( 2017, 1, 1 ) );
    assert ( l11 . size () == 4 );
    assert ( ( l11 == list<pair<string,int> > { { "bread", 93 }, { "beer", 50 }, { "Coke", 6 }, { "okey", 5 } } ) );

    list<pair<string,int> > l12 { { "Cake", 4 } };
    s . sell ( l12 );
    assert ( l12 . size () == 0 );
    assert ( ( l12 == list<pair<string,int> > {  } ) );

    list<pair<string,int> > l13 = s . expired ( CDate ( 2017, 1, 1 ) );
    assert ( l13 . size () == 4 );
    assert ( ( l13 == list<pair<string,int> > { { "bread", 93 }, { "beer", 50 }, { "okey", 5 }, { "Coke", 2 } } ) );

    list<pair<string,int> > l14 { { "Beer", 20 }, { "Coke", 1 }, { "bear", 25 }, { "beer", 10 } };
    s . sell ( l14 );
    assert ( l14 . size () == 1 );
    assert ( ( l14 == list<pair<string,int> > { { "beer", 5 } } ) );

    s . store ( "ccccb", CDate ( 2019, 3, 11 ), 100 )
            . store ( "ccccd", CDate ( 2019, 6, 9 ), 100 )
            . store ( "dcccc", CDate ( 2019, 2, 14 ), 100 );

    list<pair<string,int> > l15 { { "ccccc", 10 } };
    s . sell ( l15 );
    assert ( l15 . size () == 1 );
    assert ( ( l15 == list<pair<string,int> > { { "ccccc", 10 } } ) );

    cout << "SUCCESS" << endl;
    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */

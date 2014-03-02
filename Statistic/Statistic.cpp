/*
 * Statistics.cpp
 *
 *  Created on: 2 mrt. 2014
 *      Author: svenvandermeij
 */


#include "Statistic.h"

Statistic::Statistic()
{
	clear();
}

// resets all counters
void Statistic::clear()
{
	_cnt = 0;     // count at N stored, becoming N+1 at a new iteration
	_sum = 0.0;
	_min = 0.0;
	_max = 0.0;
#ifdef STAT_USE_STDEV
	_ssqdif = 0.0;  // not _ssq but sum of square differences
	                // which is SUM(from i = 1 to N) of
                        // (f(i)-_ave_N)**2
#endif
}

// adds a new value to the data-set
void Statistic::add(float f)
{
	if (_cnt < 1)
	{
		_min = f;
		_max = f;
	} else {
	  if (f < _min) _min = f;
	  if (f > _max) _max = f;
        } // end of if (_cnt == 0) else
        _sum += f;
	_cnt++;
#ifdef STAT_USE_STDEV
        if (_cnt >1) {
           _store = (_sum / _cnt - f);
           _ssqdif = _ssqdif + _cnt * _store * _store / (_cnt-1);
        } // end if > 1

#endif
}

// returns the number of values added
long Statistic::count()
{
	return _cnt;
}

// returns the average of the data-set added sofar
float Statistic::average()
{
	if (_cnt < 1) return NAN; // original code returned 0
	return _sum / _cnt;
}

// returns the sum of the data-set (0 if no values added)
float Statistic::sum()
{
	return _sum;
}

// returns the sum of the data-set (0 if no values added)
float Statistic::minimum()
{
	return _min;
}

// returns the sum of the data-set (0 if no values added)
float Statistic::maximum()
{
	return _max;
}

// Population standard deviation = s = sqrt [ S ( Xi - u )2 / N ]
// http://www.suite101.com/content/how-is-standard-deviation-used-a99084
#ifdef STAT_USE_STDEV
float Statistic::pop_stdev()
{
	if (_cnt < 1) return NAN; // otherwise DIV0 error
	return sqrt( _ssqdif / _cnt);
}

float Statistic::unbiased_stdev()
{
	if (_cnt < 2) return NAN; // otherwise DIV0 error
	return sqrt( _ssqdif / (_cnt - 1));
}
#endif
// END OF FILE


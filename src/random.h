#pragma once

#ifndef Random_H
#define Random_H

#include <chrono>
#include <random>

class Random
{
	private:
		static std::default_random_engine _engine;
		static std::uniform_real_distribution<double> _distribution;
		static int _seed;
	public:
		static double next();
		static double next(double, double);
		static int nextInt(int, int);
		static void setSeed();
		static void setSeed(int);
};

#endif

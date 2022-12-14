#pragma once

namespace std
{
	// Generic signum function from https://stackoverflow.com/a/4609795/1721329
	template <typename T> inline int sgn(T v) { return (T(0) < v) - (v < T(0)); }
}
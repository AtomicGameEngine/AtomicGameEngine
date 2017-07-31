//
// FPEnvironment_WIN32.h
//
// $Id: //poco/1.4/Foundation/include/Poco/FPEnvironment_WIN32.h#1 $
//
// Library: Foundation
// Package: Core
// Module:  FPEnvironment
//
// Definitions of class FPEnvironmentImpl for WIN32.
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#ifndef Foundation_FPEnvironment_WIN32_INCLUDED
#define Foundation_FPEnvironment_WIN32_INCLUDED


#include "Poco/Foundation.h"
#include <float.h>
#include <math.h>


namespace Poco {

// ATOMIC BEGIN
#if __MINGW32__ || __MINW64__
#	define RC_DOWN _RC_DOWN
#	define RC_UP _RC_UP
#	define RC_NEAR _RC_NEAR
#	define RC_CHOP _RC_CHOP

#	define SW_ZERODIVIDE _EM_ZERODIVIDE
#	define SW_INEXACT _EM_INEXACT
#	define SW_OVERFLOW _EM_OVERFLOW
#	define SW_UNDERFLOW _EM_UNDERFLOW
#	define SW_INVALID _EM_INVALID

#	define MCW_DN _MCW_DN
#	define MCW_EM _MCW_EM
#	define MCW_IC _MCW_IC
#	define MCW_RC _MCW_RC
#	define MCW_PC _MCW_PC
#endif
// ATOMIC END

class Foundation_API FPEnvironmentImpl
{
protected:
	enum RoundingModeImpl
	{
		FP_ROUND_DOWNWARD_IMPL   = RC_DOWN,
		FP_ROUND_UPWARD_IMPL     = RC_UP,
		FP_ROUND_TONEAREST_IMPL  = RC_NEAR,
		FP_ROUND_TOWARDZERO_IMPL = RC_CHOP
	};
	enum FlagImpl
	{
		FP_DIVIDE_BY_ZERO_IMPL = SW_ZERODIVIDE,
		FP_INEXACT_IMPL        = SW_INEXACT,
		FP_OVERFLOW_IMPL       = SW_OVERFLOW,
		FP_UNDERFLOW_IMPL      = SW_UNDERFLOW,
		FP_INVALID_IMPL        = SW_INVALID
	};
	FPEnvironmentImpl();
	FPEnvironmentImpl(const FPEnvironmentImpl& env);
	~FPEnvironmentImpl();
	FPEnvironmentImpl& operator = (const FPEnvironmentImpl& env);
	void keepCurrentImpl();		
	static void clearFlagsImpl();
	static bool isFlagImpl(FlagImpl flag);	
	static void setRoundingModeImpl(RoundingModeImpl mode);
	static RoundingModeImpl getRoundingModeImpl();
	static bool isInfiniteImpl(float value);		
	static bool isInfiniteImpl(double value);
	static bool isInfiniteImpl(long double value);
	static bool isNaNImpl(float value);		
	static bool isNaNImpl(double value);
	static bool isNaNImpl(long double value);
	static float copySignImpl(float target, float source);		
	static double copySignImpl(double target, double source);
	static long double copySignImpl(long double target, long double source);

private:
	unsigned _env;
};


//
// inlines
//
inline bool FPEnvironmentImpl::isInfiniteImpl(float value)
{
	return _finite(value) == 0;
}


inline bool FPEnvironmentImpl::isInfiniteImpl(double value)
{
	return _finite(value) == 0;
}


inline bool FPEnvironmentImpl::isInfiniteImpl(long double value)
{
	return _finite(value) == 0;
}


inline bool FPEnvironmentImpl::isNaNImpl(float value)
{
	return _isnan(value) != 0;
}


inline bool FPEnvironmentImpl::isNaNImpl(double value)
{
	return _isnan(value) != 0;
}


inline bool FPEnvironmentImpl::isNaNImpl(long double value)
{
	return _isnan(value) != 0;
}


inline float FPEnvironmentImpl::copySignImpl(float target, float source)
{
	return float(_copysign(target, source));
}


inline double FPEnvironmentImpl::copySignImpl(double target, double source)
{
	return _copysign(target, source);
}


inline long double FPEnvironmentImpl::copySignImpl(long double target, long double source)
{
	return (source > 0 && target > 0) || (source < 0 && target < 0) ? target : -target;
}


} // namespace Poco


#endif // Foundation_FPEnvironment_WIN32_INCLUDED

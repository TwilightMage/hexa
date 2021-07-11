#pragma once

#define FAST_OPERATOR(type, op, param) FORCEINLINE bool operator##op(const type& rhs) const { return param op rhs.param; }
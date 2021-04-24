#pragma once

#include <streambuf>

#include "List.h"

class TeeBuf : public std::streambuf
{
public:
	TeeBuf(List<std::streambuf*> buffs = List<std::streambuf*>());

	void AddBuff(std::streambuf* buff);

	void RemoveBuff(std::streambuf* buff);

private:
    virtual int overflow(int c) override;

    virtual int sync() override;

private:
	List<std::streambuf*> buffs;
};

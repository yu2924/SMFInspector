//
//  SMFParser.h
//  SMFInspector
//
//  Created by yu2924 on 2016-12-17
//

#pragma once

#include <vector>
#include <string>

struct SMFDivision
{
	enum { metrical, actual } type;
	union
	{
		uint16_t qnotediv; // metrical time
		struct { int8_t fps, framediv; } timeres; // actual time
	};
};

struct SMFEvent
{
	uint32_t deltatime;
	uint8_t status;
	union
	{
		uint8_t chdata[3];
		struct { uint8_t type; uint16_t index; } ckref;
	};
};

using SMFTrack = std::vector<SMFEvent>;
using SMFChunk = std::vector<uint8_t>;

class SMFParser
{
protected:
	SMFParser() {}
public:
	virtual ~SMFParser() {}
	virtual int format() const = 0;
	virtual SMFDivision division() const = 0;
	virtual const std::vector<SMFTrack>& tracks() const = 0;
	virtual const std::vector<SMFChunk>& chunks() const = 0;
	virtual void clear() = 0;
	virtual bool load(const wchar_t* path, std::string* perr = nullptr) = 0;
	virtual bool load(std::basic_istream<uint8_t>& str, std::string* perr = nullptr) = 0;
	static SMFParser* createInstance();
};

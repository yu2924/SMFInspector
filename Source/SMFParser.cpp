//
//  SMFParser.cpp
//  SMFInspector
//
//  Created by yu2924 on 2016-12-17
//

#include "SMFParser.h"
#include <stdexcept>
#include <fstream>
#include <sstream>

struct RMIDBraker
{
	struct CKHDR
	{
		uint8_t ckid[4];
		uint32_t cksize;
	};
	struct CKRANGE
	{
		std::streamoff hdroffset;
		CKHDR hdr;
	};
	CKRANGE ckrriff = {};
	CKRANGE ckrdata = {};
	void clear()
	{
		ckrriff = {};
		ckrdata = {};
	}
	bool parse(std::basic_istream<uint8_t>& str)
	{
		clear();
		ckrriff.hdroffset = str.tellg();
		str.read((uint8_t*)&ckrriff.hdr, sizeof(ckrriff.hdr));
		uint8_t fmtid[4] = {}; str.read(fmtid, sizeof(fmtid));
		if(memcmp(fmtid, "RMID", 4) != 0) return false;
		while(str.tellg() < (std::streampos)(ckrriff.hdroffset + sizeof(CKHDR) + ckrriff.hdr.cksize))
		{
			CKRANGE ckr = {};
			ckr.hdroffset = str.tellg();
			str.read((uint8_t*)&ckr.hdr, sizeof(ckr.hdr));
			if(memcmp(ckr.hdr.ckid, "data", 4) == 0)
			{
				ckrdata = ckr;
				return true;
			}
			str.seekg((ckr.hdroffset + sizeof(CKHDR) + ckr.hdr.cksize + 3) & ~(std::streampos)0x03); // DWORD align
		}
		return false;
	}
};

class SMFParserImpl : public SMFParser
{
public:
	static uint8_t read1(std::basic_istream<uint8_t>& s) { uint8_t b[1]; s.read(b, sizeof(b)); return b[0]; }
	static uint16_t read2(std::basic_istream<uint8_t>& s) { uint8_t b[2]; s.read(b, sizeof(b)); return ((uint16_t)b[0] << 8) | (uint16_t)b[1]; }
	static uint32_t read4(std::basic_istream<uint8_t>& s) { uint8_t b[4]; s.read(b, sizeof(b)); return ((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) | ((uint32_t)b[2] << 8) | (uint32_t)b[3]; }
	static uint32_t readvlq(std::basic_istream<uint8_t>& s) { uint32_t v = 0, u = 0x80; while(u & 0x80) { u = read1(s); v = (v << 7) + (u & 0x7f); } return v; }
	int mFormat = 0;
	SMFDivision mDivision = {};
	std::vector<SMFTrack> mTracks;
	std::vector<SMFChunk> mChunks;
	SMFParserImpl() {}
	virtual int format() const override { return mFormat; }
	virtual SMFDivision division() const override { return mDivision; }
	virtual const std::vector<SMFTrack>& tracks() const override { return mTracks; }
	virtual const std::vector<SMFChunk>& chunks() const override { return mChunks; }
	virtual void clear() override
	{
		mFormat = 0;
		mDivision = {};
		mTracks.clear();
		mChunks.clear();
	}
	virtual bool load(const wchar_t* path, std::string* perr) override
	{
		std::basic_ifstream<uint8_t> str(path, std::ios_base::in | std::ios_base::binary);
		if(!str) { if(perr) *perr = "open error"; return false; }
		return load(str, perr);
	}
	virtual bool load(std::basic_istream<uint8_t>& str, std::string* perr = nullptr) override
	{
		std::streampos off = str.tellg();
		uint8_t fcc[4] = {}; str.read(fcc, sizeof(fcc));
		str.seekg(off);
		if(memcmp(fcc, "RIFF", 4) == 0)
		{
			RMIDBraker brk;
			if(!brk.parse(str)) { if(perr) *perr = "bad format"; return false; }
			str.seekg(brk.ckrdata.hdroffset + sizeof(RMIDBraker::CKHDR));
			std::basic_string<uint8_t> s(brk.ckrdata.hdr.cksize, 0);
			str.read(&s[0], s.size());
			std::basic_stringstream<uint8_t> sstr(s);
			return loadnakedsmf(sstr, perr);
		}
		else
		{
			return loadnakedsmf(str, perr);
		}
	}
	bool loadnakedsmf(std::basic_istream<uint8_t>& str, std::string* perr = nullptr)
	{
		clear();
		if(perr) perr->clear();
		bool r = false;
		try
		{
			// header
			uint8_t hdrsig[4] = {}; str.read(hdrsig, sizeof(hdrsig)); if(memcmp(hdrsig, "MThd", sizeof(hdrsig)) != 0) throw std::runtime_error("MThd not found");
			uint32_t hdrlen = read4(str); if(hdrlen != 6) throw std::runtime_error("invalid header size");
			mFormat = read2(str);
			size_t ntrk = read2(str);
			int16_t div = read2(str);
			if(0 <= div)
			{
				mDivision.type = SMFDivision::metrical;
				mDivision.qnotediv = div;
			}
			else
			{
				const int8_t* p = (const int8_t*)&div;
				mDivision.type = SMFDivision::actual;
				mDivision.timeres.fps = p[1];
				mDivision.timeres.framediv = p[0];
			}
			// tracks
			mTracks.resize(ntrk);
			for(size_t itrk = 0; itrk < ntrk; itrk++)
			{
				SMFTrack& track = mTracks[itrk];
				uint8_t trksig[4]; str.read(trksig, sizeof(trksig)); if(memcmp(trksig, "MTrk", sizeof(trksig)) != 0) throw std::runtime_error("MTrk not found");
				uint32_t length = read4(str);
				std::streampos endpos = str.tellg() + (std::streamoff)length;
				uint8_t runningstatus = 0;
				while(str.tellg() < endpos)
				{
					uint32_t dt = readvlq(str);
					uint8_t bnext = read1(str);
					str.seekg(str.tellg() - (std::streamoff)1);
					uint8_t status = (bnext & 0x80) ? read1(str) : runningstatus;
					SMFEvent ev = { dt, status, {} };
					if(status < 0xf0)
					{
						runningstatus = status;
						switch(status & 0xf0)
						{
							case 0x80: // noteoff
							case 0x90: // noteon
							case 0xa0: // poly.aftertouch
							case 0xb0: // controlchange
							case 0xe0: // pitchwheel
								ev.chdata[0] = read1(str);
								ev.chdata[1] = read1(str);
								track.push_back(ev);
								break;
							case 0xc0: // programchange
							case 0xd0: // ch.aftertouch
								ev.chdata[0] = read1(str);
								track.push_back(ev);
								break;
							default: // unexpected
								track.push_back(ev);
								break;
						}
					}
					else
					{
						switch(status)
						{
							case 0xf0: // sysex
							{
								uint32_t cklen = readvlq(str);
								SMFChunk ckdata(cklen + 1, 0);
								ckdata[0] = 0xf0; if(0 < cklen) str.read(ckdata.data() + 1, cklen);
								mChunks.push_back(std::move(ckdata));
								ev.ckref.index = (uint16_t)mChunks.size() - 1;
								track.push_back(ev);
								break;
							}
							case 0xf7: // partial sysex
							{
								uint32_t cklen = readvlq(str);
								SMFChunk ckdata(cklen, 0);
								if(0 < cklen) str.read(ckdata.data(), cklen);
								mChunks.push_back(std::move(ckdata));
								ev.ckref.index = (uint16_t)mChunks.size() - 1;
								track.push_back(ev);
								break;
							}
							case 0xff: // metaevents
							{
								ev.ckref.type = read1(str);
								uint32_t cklen = readvlq(str);
								SMFChunk ckdata(cklen, 0);
								if(0 < cklen) str.read(ckdata.data(), cklen);
								mChunks.push_back(std::move(ckdata));
								ev.ckref.index = (uint16_t)mChunks.size() - 1;
								track.push_back(ev);
								break;
							}
							default: // unknown
								track.push_back(ev);
								break;
						}
					}
				}
				str.seekg(endpos);
			}
			r = true;
		}
		catch(std::exception& e)
		{
			if(perr) *perr = e.what();
		}
		if(!r) clear();
		return r;
	}
};

SMFParser* SMFParser::createInstance()
{
	return new SMFParserImpl;
}

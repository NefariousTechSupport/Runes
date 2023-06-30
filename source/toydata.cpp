#include "toydata.hpp"
#include "PortalTag.hpp"
#include <iostream>
#include <string.h>
#include <sstream>
#include <cstring>

#define parseBool(str) (strcmp(str, "True") == 0)

Runes::ToyDataManager* Runes::ToyDataManager::_Instance = NULL;

std::vector<std::string> readLineAndSplit(FILE* f)
{
	std::vector<std::string> tokens;
	std::string line;
	char workBuf;
	int prevComma = 0;
	int place = 0;
	while(true)
	{
		int res = fread(&workBuf, 1, 1, f);
		if(res != 1 || workBuf == '\n' || workBuf == ',')
		{
			tokens.push_back(line.substr(prevComma, place-prevComma));
			prevComma = place + 1;
			//printf("%s, ", tokens[tokens.size()-1].c_str());
			if(workBuf != ',') break;
		}
		line += workBuf;
		place++;
	}
	//printf("\n");
	return tokens;
}

//Required because std::string stuff is stored on the stack and will get freed after the function terminates
const char* allocAndCopyString(const char* src)
{
	const char* dst = (const char*)malloc(strlen(src)+1);
	strcpy((char*)dst, src);
	return dst;
}

Runes::ToyDataManager* Runes::ToyDataManager::getInstance()
{
	if(_Instance == NULL)
	{
		_Instance = new Runes::ToyDataManager("Resources/runes.csv");
	}
	return _Instance;
}
Runes::ToyDataManager::ToyDataManager(const char* csvPath)
{
	FILE* f = fopen(csvPath, "rb");
	fseek(f, 0, SEEK_SET);
	while(true)
	{
		std::vector<std::string> tokens = readLineAndSplit(f);
		if(tokens.size() != 3) break;

		Runes::FigureToyData* toyData = new Runes::FigureToyData();

		//Read toy type
		const char* workStr = tokens[0].c_str();
		toyData->_toyType = (kTfbSpyroTag_ToyType)atoi(workStr);

		//Read variant count
		workStr = tokens[1].c_str();
		int variantCount = atoi(workStr);

		//Read toy name
		toyData->_toyName = allocAndCopyString(tokens[2].c_str());

		for(int i = 0; i < variantCount; i++)
		{
			Runes::VariantIdentifier* varId = new Runes::VariantIdentifier();

			tokens = readLineAndSplit(f);

			workStr = tokens[0].c_str();
			varId->_decoId = (kTfbSpyroTag_DecoID)atoi(workStr);

			workStr = tokens[1].c_str();
			varId->_yearCode = (ESkylandersGame)atoi(workStr);

			workStr = tokens[2].c_str();
			varId->_lightCore = parseBool(workStr);
			workStr = tokens[3].c_str();
			varId->_fullAltDeco = parseBool(workStr);
			workStr = tokens[4].c_str();
			varId->_wowPow = parseBool(workStr);
			varId->_variantText = allocAndCopyString(tokens[5].c_str());
			varId->_toyName = allocAndCopyString(tokens[6].c_str());

			toyData->_variants.push_back(varId);
		}

		this->_toyTypeLookup[toyData->_toyType] = toyData;
	}
}
Runes::FigureToyData* Runes::ToyDataManager::LookupCharacter(kTfbSpyroTag_ToyType toyType)
{
	if(this->_toyTypeLookup.find(toyType) != this->_toyTypeLookup.end())
	{
		return this->_toyTypeLookup[toyType];
	}
	return NULL;
}
Runes::VariantIdentifier* Runes::FigureToyData::LookupVariant(uint16_t varId)
{
	ESkylandersGame yearCode;
	bool fullAltDeco, wowPow, lightCore;
	kTfbSpyroTag_DecoID decoId;
	Runes::PortalTag::DecodeSubtype(varId, &yearCode, &fullAltDeco, &wowPow, &lightCore, &decoId);
	Runes::VariantIdentifier* bestMatch = NULL;
	for(int i = 0; i < (int)_variants.size(); i++)
	{
		if(_variants[i]->_wowPow == wowPow && wowPow == true)
		{
			if(!bestMatch || (yearCode >= _variants[i]->_yearCode && _variants[i]->_yearCode >= bestMatch->_yearCode && decoId != bestMatch->_decoId))
			{
				bestMatch = _variants[i];
			}
		}
		if(_variants[i]->_decoId != decoId) continue;
		if(_variants[i]->_yearCode != yearCode) continue;
		if(_variants[i]->_fullAltDeco != fullAltDeco) continue;
		bestMatch = _variants[i];
	}
	return bestMatch;
}
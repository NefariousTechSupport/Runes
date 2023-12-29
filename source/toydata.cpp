#include "toydata.hpp"
#include "PortalTag.hpp"
#include <iostream>
#include <string.h>
#include <sstream>
#include <cstring>
#include <assert.h>
#include <yaml-cpp/yaml.h>

#define parseBool(str) (strcmp(str, "True") == 0)

std::string Runes::LocalizedString::get()
{
	return this->_en;
}

Runes::ToyDataManager* Runes::ToyDataManager::_Instance = NULL;

Runes::ToyDataManager* Runes::ToyDataManager::getInstance()
{
	if(_Instance == NULL)
	{
		_Instance = new Runes::ToyDataManager("Resources/runes.yaml");
	}
	return _Instance;
}
Runes::ToyDataManager::ToyDataManager(const char* yamlPath)
{
	YAML::Node root = YAML::LoadFile(yamlPath);
	assert(root.Type() == YAML::NodeType::Map);
	YAML::Node list = root["list"];
	assert(list);
	assert(list.Type() == YAML::NodeType::Sequence);
	YAML::const_iterator it = list.begin();
	while(it != list.end())
	{
		readCharacter(it->as<YAML::Node>());
		it++;
	}
}
void Runes::ToyDataManager::readCharacter(YAML::Node charNode)
{
	Runes::FigureToyData* figure = new Runes::FigureToyData();
	figure->_toyType = (kTfbSpyroTag_ToyType)charNode["toyId"].as<uint32_t>();
	figure->_element = (EElementType)charNode["element"].as<uint32_t>();
	readLocalizedString(&figure->_toyName, charNode["toyName"]);

	YAML::Node variants = charNode["variants"];
	YAML::const_iterator it = variants.begin();
	while(it != variants.end())
	{
		figure->_variants.push_back(readVariant(it->as<YAML::Node>()));
		it++;
	}

	this->_toyTypeLookup[figure->_toyType] = figure;
}
Runes::VariantIdentifier* Runes::ToyDataManager::readVariant(YAML::Node varNode)
{
	Runes::VariantIdentifier* variant = new Runes::VariantIdentifier();
	variant->_decoId = (kTfbSpyroTag_DecoID)varNode["decoId"].as<int8_t>();
	variant->_yearCode = (ESkylandersGame)varNode["yearCode"].as<int8_t>();
	variant->_lightCore = varNode["lightcore"].as<bool>();
	variant->_fullAltDeco = varNode["fullAltDeco"].as<bool>();
	variant->_wowPow = varNode["wowPow"].as<bool>();
	readLocalizedString(&variant->_variantText, varNode["variantText"]);
	readLocalizedString(&variant->_toyName, varNode["toyName"]);
	return variant;
}
void Runes::ToyDataManager::readLocalizedString(LocalizedString* lstring, YAML::Node textNode)
{
	lstring->_en = textNode["en"].as<std::string>();
	lstring->_fr = textNode["fr"].as<std::string>();
	lstring->_it = textNode["it"].as<std::string>();
	lstring->_de = textNode["de"].as<std::string>();
	lstring->_es = textNode["es"].as<std::string>();
	lstring->_mx = textNode["mx"].as<std::string>();
	lstring->_nl = textNode["nl"].as<std::string>();
	lstring->_da = textNode["da"].as<std::string>();
	lstring->_sv = textNode["sv"].as<std::string>();
	lstring->_fi = textNode["fi"].as<std::string>();
	lstring->_no = textNode["no"].as<std::string>();
	lstring->_pt = textNode["pt"].as<std::string>();
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
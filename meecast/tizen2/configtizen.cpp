/* vim: set sw=4 ts=4 et: */
/*
 * This file is part of Meecast for Tizen
 *
 * Copyright (C) 2012 - 2013 Vlad Vasilyeu
 * 	for the code
 *
 * This software is free software; you can redistribute it and/or
 * modify it under the terms of the GNU  General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  General Public License for more details.
 *
 * You should have received a copy of the GNU  General Public
 * License along with this software; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
*/
/*******************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
/*******************************************************************************/
#include "configtizen.h"

ConfigTizen* ConfigTizen::_self;
int ConfigTizen::_refcount;

ConfigTizen::ConfigTizen(const std::string& filename, const std::string& schema_filename):Core::Config(filename, schema_filename)
{
    dp = null;
}

ConfigTizen::ConfigTizen():Core::Config()
{
    dp = null;
}

ConfigTizen::~ConfigTizen()
{
}


ConfigTizen* 
ConfigTizen::Instance()
{
    if (!_self)
        _self = new ConfigTizen();
    _refcount++;
    /* AppLogDebug("Instance Tizen0 %i", _refcount); */
    return _self;
}

ConfigTizen* 
ConfigTizen::Instance(const std::string& filename, const std::string& schema_filename)
{
    if (!_self)
        _self = new ConfigTizen(filename, schema_filename);
    _refcount++;
    /* AppLogDebug("Instance Tizen1 %i", _refcount); */
    return _self;
}

bool 
ConfigTizen::DeleteInstance(){

    AppLogDebug("DeleteInstance Tizen %i", _refcount); 
    if (_refcount > 1){
        _refcount --;
        return false;
    }
    if (_refcount == 1  && _self){
        AppLogDebug("Destroy Config Tizen");
        delete _self;
        _self = 0;
        AppLogDebug("Destroy Config Tizen is done");
        return true;
    }
    return false;
}

void 
ConfigTizen::set_screen_width(int width){
    _screen_width = width;
}

int  
ConfigTizen::get_screen_width(){
    return _screen_width;
}

void 
ConfigTizen::set_screen_height(int height){
    _screen_height = height;
}

int  
ConfigTizen::get_screen_height(){
    return _screen_height;
}

void
ConfigTizen::convertstations(){
    for (short i=0; i < stationsList().size();i++){
        stationsList().at(i)->run_converter();
    }
}

void
ConfigTizen::updatestations(){
    for (short i=0; i < stationsList().size();i++){
        stationsList().at(i)->updateData(true);
    }
}

bool
ConfigTizen::isupdatingstations(){
    AppLog ("ConfigTizen::isupdatingstations()");
    for (short i=0; i < stationsList().size();i++){
        if (stationsList().at(i)->isupdating())
            return true;
    }
    AppLog (" false ConfigTizen::isupdatingstations()");
    return false;
}

void
ConfigTizen::removeStation(int index){
    stationsList().erase(stationsList().begin() + index);
    if (this->stationsList().size() > 0){
        this->current_station_id(0);
    }
    saveConfig();
}

void 
ConfigTizen::saveStation1(String source, String city_id, String city_name, String country,
                      String region, bool gps, double latitude, double longitude){
    AppLog ("ConfigTizen::saveStation1");
    std::string _source = "";
    ByteBuffer* pBuf = Tizen::Base::Utility::StringUtil::StringToUtf8N(source);
    _source = (const char*)(pBuf->GetPointer());
    delete pBuf;
    std::string _code = "";
    pBuf = Tizen::Base::Utility::StringUtil::StringToUtf8N(city_id);
    _code = (const char*)(pBuf->GetPointer());
    delete pBuf;
    std::string _name = "";
    pBuf = Tizen::Base::Utility::StringUtil::StringToUtf8N(city_name);
    _name =  (const char*)(pBuf->GetPointer());
    delete pBuf;
    std::string _country = "";
    pBuf = Tizen::Base::Utility::StringUtil::StringToUtf8N(country);
    _country =  (const char*)(pBuf->GetPointer());
    delete pBuf;
    std::string _region = "";
    pBuf = Tizen::Base::Utility::StringUtil::StringToUtf8N(region);
    _region =  (const char*)(pBuf->GetPointer());
    delete pBuf;

    Core::Station *station;
    station = new Core::Station(
                _source,
                _code,
                _name,
                _country,
                _region,
                gps, latitude, longitude);

    AppLog ("ConfigTizen::saveStation preend");
    stationsList().push_back(station);
    ////ConfigQml::Config::stationsList(*stationlist);
    saveConfig();
    //refreshconfig();

    AppLog ("ConfigTizen::saveStation end");
 
}

int
ConfigTizen::getGpsStation(){
    int index = -1;
    for (unsigned int i=0; i<stationsList().size(); i++){
        if (stationsList().at(i)->gps() == true)
            index = i;
    }
    return index;
}

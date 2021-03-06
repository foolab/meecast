/* vim: set sw=4 ts=4 et: */
/*
 * This file is part of Other Maemo Weather(omweather)
 *
 * Copyright (C) 2006-2011 Vlad Vasiliev
 * Copyright (C) 2010-2011 Tanya Makova
 *     for the code
 *
 * Copyright (C) 2008 Andrew Zhilin
 *		      az@pocketpcrussia.com 
 *	for default icon set (Glance)
 *
 * This software is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU weather-config.h General Public
 * License along with this software; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
*/
/*******************************************************************************/


#include "dataqml.h"
DataQml::DataQml():QObject(),Core::Data(){

}

DataQml::DataQml(const Core::Data* data):QObject(),Core::Data(data){
   // DataQml::Data(data);
}
QString
DataQml::temperature_high(){
    QString c;
    if (DataQml::Data::temperature_hi().value() == INT_MAX){
        c = "N/A";
        return c;
    }
    return c.number((DataQml::Data::temperature_hi().value()),'f',0);
}

QString
DataQml::temperature_low(){
    QString c;
    if (DataQml::Data::temperature_low().value() == INT_MAX){
        c = "N/A";
        return c;
    }
    return c.number((DataQml::Data::temperature_low().value()),'f',0);
}


QString
DataQml::text(){
    QString c;
    c = DataQml::Data::Text().c_str();
    return c;
}

int
DataQml::icon(){
    return DataQml::Data::Icon();
}

void
DataQml::refreshview(){
    emit DataQml::temperature_highChanged();
    emit DataQml::temperature_lowChanged();
    emit DataQml::iconChanged();
    emit DataQml::textChanged();
}


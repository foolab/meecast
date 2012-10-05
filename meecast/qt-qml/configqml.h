/* vim: set sw=4 ts=4 et: */
/*
 * This file is part of Other Maemo Weather(omweather)
 *
 * Copyright (C) 2006-2012 Vlad Vasilyeu
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


#ifndef CONFIGQML_H
#define CONFIGQML_H

#include "core.h"
#include <QObject>
#include <QColor>
#include <QDesktopServices>
#include <QUrl>
#include "updatethread.h"
#include "gpsposition.h"
#include <MGConfItem>
#include <QSettings>
#include <QDir>

#include <libintl.h>
#include <locale.h>


#define _(String) gettext(String)

class ConfigQml : public QObject, public Core::Config

{
    Q_OBJECT
    Q_PROPERTY(QString iconset READ iconset NOTIFY iconsetChanged)
    Q_PROPERTY(QString iconspath READ iconspath NOTIFY iconspathChanged)
    Q_PROPERTY(QString imagespath READ imagespath NOTIFY imagespathChanged)
    Q_PROPERTY(QString iconsbutton READ iconsbutton NOTIFY iconsbuttonChanged)
    Q_PROPERTY(QString temperatureunit READ temperatureunit NOTIFY temperatureunitChanged)
    Q_PROPERTY(QString windspeedunit READ windspeedunit NOTIFY windspeedunitChanged)
    Q_PROPERTY(QString pressureunit READ pressureunit NOTIFY pressureunitChanged)
    Q_PROPERTY(bool fullscreen READ fullscreen NOTIFY fullscreenChanged)
    Q_PROPERTY(bool lockscreen READ lockscreen NOTIFY lockscreenChanged)
    Q_PROPERTY(bool standbyscreen READ standbyscreen NOTIFY standbyscreenChanged)
    Q_PROPERTY(bool eventwidget READ eventwidget NOTIFY eventwidgetChanged)
    Q_PROPERTY(bool splash READ splash NOTIFY splashChanged)
    Q_PROPERTY(bool gps READ gps NOTIFY gpsChanged)
    Q_PROPERTY(QColor fontcolor READ fontcolor NOTIFY fontcolorChanged)
    Q_PROPERTY(QColor standby_color_font_stationname READ standby_color_font_stationname NOTIFY standby_color_font_stationnameChanged)
    Q_PROPERTY(QColor standby_color_font_temperature READ standby_color_font_temperature NOTIFY standby_color_font_temperatureChanged)
    Q_PROPERTY(QColor standby_color_font_current_temperature READ standby_color_font_current_temperature NOTIFY standby_color_font_current_temperatureChanged)
    Q_PROPERTY(QString stationname READ stationname NOTIFY stationnameChanged)
    Q_PROPERTY(QString prevstationname READ prevstationname NOTIFY prevstationnameChanged)
    Q_PROPERTY(QString nextstationname READ nextstationname NOTIFY nextstationnameChanged)
    Q_PROPERTY(QString filename READ filename NOTIFY filenameChanged)
    Q_PROPERTY(QString source READ source NOTIFY sourceChanged)
    Q_PROPERTY(QString version READ version NOTIFY versionChanged)
    Q_PROPERTY(int updateinterval READ updateinterval NOTIFY updateintervalChanged)
private:
    Core::DatabaseSqlite *db;
    UpdateThread *thread;
    GpsPosition *_gps;
    int getCountryId(int index);
    int getRegionId(int country, int index);
    int getGpsStation();
    QString getCityId(int region_id, int index);
    void init();
    QStringList wind_list;
    QStringList press_list;
    QSettings *standby_settings;
    QColor _standby_color_font_stationname;
    QColor _standby_color_font_temperature;
    QColor _standby_color_font_current_temperature;
protected:
    static ConfigQml* _self;
    static int _refcount;
    virtual ~ConfigQml();
    ConfigQml();
    ConfigQml(const std::string& filename, const std::string& schema_filename = "/usr/" + schemaPath + "config.xsd");



public:
    static ConfigQml* Instance();
    static ConfigQml* Instance(const std::string& filename, const std::string& schema_filename = "/usr/" + schemaPath + "config.xsd");
    QString iconset();
    QString iconspath();
    QString imagespath();
    QString iconsbutton();
    QString temperatureunit();
    QString windspeedunit();
    QString pressureunit();
    bool fullscreen();
    bool lockscreen();
    bool standbyscreen();
    bool eventwidget();
    bool gps();
    bool splash();
    QColor fontcolor();
    QColor standby_color_font_stationname();
    QColor standby_color_font_temperature();
    QColor standby_color_font_current_temperature();
    QString stationname();
    QString prevstationname();
    QString nextstationname();
    QString filename();
    QString source();
    QString version();
    void saveConfig();
    int updateinterval();
    QString viewURL();
    Q_INVOKABLE QStringList stations();
    Q_INVOKABLE void removeStation(int index);
    Q_INVOKABLE QStringList Sources();
    Q_INVOKABLE QStringList Countries(QString source);
    Q_INVOKABLE QStringList Regions(int index);
    Q_INVOKABLE QStringList Cities(int country_index, int index);
    Q_INVOKABLE void saveStation(int city_id, QString city,
                                 int region_id, QString region,
                                 int country_id, QString country,
                                 int source_id, QString source);
    Q_INVOKABLE void saveStation1(QString city_id, QString city_name, QString region,
                                  QString country, QString source, int source_id, bool gps=false, double latitude = 0.0, double longitude = 0.0);
    Q_INVOKABLE void changestation();
    Q_INVOKABLE void nextstation();
    Q_INVOKABLE void prevstation();
    Q_INVOKABLE void updatestations();
    Q_INVOKABLE void showweb();
    Q_INVOKABLE void showwebdonation();
    Q_INVOKABLE void showwebsupport();
    Q_INVOKABLE void closeapplication();
    Q_INVOKABLE QStringList temperature_list();
    Q_INVOKABLE void temperature_unit(QString c);
    Q_INVOKABLE QStringList windspeed_list();
    Q_INVOKABLE void windspeed_unit(int index);
    Q_INVOKABLE void update_interval(int interval);
    Q_INVOKABLE QStringList pressure_list();
    Q_INVOKABLE void pressure_unit(int index);
    Q_INVOKABLE void setfullscreen(bool c);
    Q_INVOKABLE void setlockscreen(bool c);
    Q_INVOKABLE void setstandbyscreen(bool c);
    Q_INVOKABLE void seteventwidget(bool c);
    Q_INVOKABLE void setgps(bool c);
    Q_INVOKABLE void setsplash(bool c);
    Q_INVOKABLE QStringList icon_list();
    Q_INVOKABLE void set_iconset(QString c);
    Q_INVOKABLE void set_standby_color_font_stationname(QColor c);
    Q_INVOKABLE void set_standby_color_font_temperature(QColor c);
    Q_INVOKABLE void set_standby_color_font_current_temperature(QColor c);
    Q_INVOKABLE QString tr(QString str);
    Q_INVOKABLE void enableGps();
    void refreshconfig();
signals:
    void iconsetChanged();
    void iconspathChanged();
    void imagespathChanged();
    void iconsbuttonChanged();
    void temperatureunitChanged();
    void windspeedunitChanged();
    void pressureunitChanged();
    void fullscreenChanged();
    void lockscreenChanged();
    void standbyscreenChanged();
    void eventwidgetChanged();
    void gpsChanged();
    void fontcolorChanged();
    void standby_color_font_stationnameChanged();
    void standby_color_font_temperatureChanged();
    void standby_color_font_current_temperatureChanged();
    void stationnameChanged();
    void prevstationnameChanged();
    void nextstationnameChanged();
    void filenameChanged();
    void sourceChanged();
    void versionChanged();
    void updateintervalChanged();
    void configChanged();
    void splashChanged();
public Q_SLOTS:
    void reload_config();
    void addGpsStation(double latitude, double longitude);
private slots:
    void downloadFinishedSlot();

       };

#endif // CONFIGQML_H

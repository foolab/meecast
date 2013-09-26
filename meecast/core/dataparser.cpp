/* vim: set sw=4 ts=4 et: */
/*
 * This file is part of Other Maemo Weather(omweather) - MeeCast
 *
 * Copyright (C) 2006-2012 Vlad Vasilyeu
 * Copyright (C) 2006-2011 Pavel Fialko
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


#include "dataparser.h"
#include <iostream>
#include <stdlib.h>
#ifdef QT
    #include <QFileInfo>
#endif
////////////////////////////////////////////////////////////////////////////////
namespace Core {
     DataParser* DataParser::_self;
     int DataParser::_refcount = 0;
     DataList *DataParser::_list;
////////////////////////////////////////////////////////////////////////////////
    DataParser::DataParser()
    {
        AppLogDebug("DataParser");
        _timezone = 0;
        _list = new DataList;
        Data *forecast_data;
        forecast_data = new Data();
        _list->push_back(forecast_data);
        _last_update = 0;
    }
////////////////////////////////////////////////////////////////////////////////
    DataParser::DataParser(const std::string& filename, const std::string& schema_filename) : Parser(filename, schema_filename) {
        this->load_data(filename, schema_filename);
           }
////////////////////////////////////////////////////////////////////////////////
    void
    DataParser::load_data(const std::string& filename, const std::string& schema_filename){
        _timezone = 0;
        _last_update = 0;
        struct stat statv;
        _list = new DataList;
        Data* forecast_data;
        if(stat(filename.c_str(), &statv))
            _last_update = 0;
        else{
            _last_update = statv.st_mtime;
        }

        /* std::cerr<<"Filename   "<<filename<<std::endl; */
        
        AppLogDebug("DataParser load data");
        this->_filename = filename;
        this->Reloadfile();
#ifdef LIBXML
#ifdef LIBXMLCPP_EXCEPTIONS_ENABLED
        try{
#endif //LIBXMLCPP_EXCEPTIONS_ENABLED
            if(parser){
                //Walk the tree:
                const xmlpp::Node* pNode = parser->get_document()->get_root_node(); //deleted by DomParser.
                processNode(pNode);
            }
#ifdef LIBXMLCPP_EXCEPTIONS_ENABLED
        }
        catch(const std::exception& ex){
            throw(ex.what());
        }
#endif //LIBXMLCPP_EXCEPTIONS_ENABLED
#else
#ifdef QT
        
        QDomElement root = _doc.documentElement();

        QDomElement e = root.firstChildElement("timezone");
        if (!e.isNull()){
            _timezone = e.text().toInt();
            std::cerr<<"TIMEZONE in entering   "<<e.text().toStdString()<<std::endl;
        }
        QDomNodeList nodelist = root.elementsByTagName("period");
        for (int i=0; i<nodelist.count(); i++){
            forecast_data = new Data();
            e = nodelist.at(i).toElement();
            forecast_data->StartTime(e.attribute("start").toInt());
            forecast_data->EndTime(e.attribute("end").toInt());
            if (e.hasAttribute("hour") && e.attribute("hour") == "true")
                forecast_data->Hour(1);
            if (e.hasAttribute("current") && e.attribute("current") == "true")
                forecast_data->Current(1);
            QDomNode n = e.firstChild();
            while (!n.isNull()){
                QDomElement el = n.toElement();
                QString tag = el.tagName();

                if (tag == "temperature" ){
                    forecast_data->temperature().value(el.text().toFloat());
                }else if (tag == "temperature_hi"){
                    forecast_data->temperature_hi().value(el.text().toFloat());
                }else if (tag == "temperature_low"){
                    forecast_data->temperature_low().value(el.text().toFloat());
                }else if (tag == "icon"){
                    forecast_data->Icon(el.text().toInt());
                }else if (tag == "description"){
                    forecast_data->Text(el.text().toStdString());
                }else if (tag == "uv_index"){
                    forecast_data->UVindex(el.text().toInt());
                }else if (tag == "humidity"){
                    forecast_data->Humidity(el.text().toInt());
                }else if (tag == "wind_speed"){
                    if (el.text() == "calm")
                        forecast_data->WindSpeed().value((float)0);
                    else
                        forecast_data->WindSpeed().value(el.text().toFloat());
                }else if (tag == "visible"){
                    forecast_data->ViSible().value(el.text().toFloat());
                }else if (tag == "wind_direction"){
                    forecast_data->WindDirection(el.text().toStdString());
                }else if (tag == "flike"){
                    forecast_data->Flike().value(el.text().toFloat());
                }else if (tag == "pressure"){
                    //forecast_data->Pressure(el.text().toFloat());
                    forecast_data->pressure().value(el.text().toFloat());
                }else if (tag == "ppcp"){
                    forecast_data->Ppcp(el.text().toFloat());
                }else if (tag == "sunrise"){
                    forecast_data->SunRiseTime(el.text().toInt());
                }else if (tag == "sunset")
                    forecast_data->SunSetTime(el.text().toInt());

                n = n.nextSibling();
            }
            _list->push_back(forecast_data);
        }
#else
      xmlChar *temp_xml_string = NULL;
      if (!_doc)
           return;
       xmlNodePtr root = xmlDocGetRootElement(_doc);
       if (!root)
           return;
       for(xmlNodePtr p = root->children; p; p = p->next) {
            if (p->type != XML_ELEMENT_NODE)
                continue;
            if (!xmlStrcmp(p->name, (const xmlChar*)"timezone")){
                temp_xml_string = xmlNodeGetContent(p);
                _timezone = atoi((char *)temp_xml_string);
                xmlFree(temp_xml_string);
            }
            if (!xmlStrcmp(p->name, (const xmlChar*)"period")){
                forecast_data = new Data();
                if ((temp_xml_string = xmlGetProp(p, (const xmlChar*)"start")) != NULL){
                    forecast_data->StartTime(atoi((const char *)temp_xml_string));
                    xmlFree(temp_xml_string);
                }
                if ((temp_xml_string = xmlGetProp(p, (const xmlChar*)"end")) != NULL){
                    forecast_data->EndTime(atoi((const char *)temp_xml_string));
                    xmlFree(temp_xml_string);
                }
                if ((temp_xml_string = xmlGetProp(p, (const xmlChar*)"hour")) != NULL){
                    forecast_data->Hour(1);
                    xmlFree(temp_xml_string);
                }
                if ((temp_xml_string = xmlGetProp(p, (const xmlChar*)"current")) != NULL){
                    forecast_data->Current(1);
                    xmlFree(temp_xml_string);
                }
                for(xmlNodePtr p1 = p->children; p1; p1 = p1->next) {
                    if (!xmlStrcmp(p1->name, (const xmlChar*)"temperature")){
                        temp_xml_string = xmlNodeGetContent(p1);
                        forecast_data->temperature().value((float)atof((char *)temp_xml_string));
                        xmlFree(temp_xml_string);
                    }
                    if (!xmlStrcmp(p1->name, (const xmlChar*)"temperature_hi")){
                        temp_xml_string = xmlNodeGetContent(p1);
                        forecast_data->temperature_hi().value((float)atof((char *)temp_xml_string));
                        xmlFree(temp_xml_string);
                    }
                    if (!xmlStrcmp(p1->name, (const xmlChar*)"temperature_low")){
                        temp_xml_string = xmlNodeGetContent(p1);
                        forecast_data->temperature_low().value((float)atof((char *)temp_xml_string));
                        xmlFree(temp_xml_string);
                    }
                    if (!xmlStrcmp(p1->name, (const xmlChar*)"uv_index")){
                        temp_xml_string = xmlNodeGetContent(p1);
                        forecast_data->UVindex((int)atoi((char *)temp_xml_string));
                        xmlFree(temp_xml_string);
                    }
                    if (!xmlStrcmp(p1->name, (const xmlChar*)"flike")){
                        temp_xml_string = xmlNodeGetContent(p1);
                        forecast_data->Flike().value((float)atof((char *)temp_xml_string));
                        xmlFree(temp_xml_string);
                    }
                    if (!xmlStrcmp(p1->name, (const xmlChar*)"icon")){
                        temp_xml_string = xmlNodeGetContent(p1);
                        forecast_data->Icon(atoi((char *)temp_xml_string));
                        xmlFree(temp_xml_string);
                    }
                    if (!xmlStrcmp(p1->name, (const xmlChar*)"description")){
                        temp_xml_string = xmlNodeGetContent(p1);
                        forecast_data->Text((char *)(temp_xml_string));
                        xmlFree(temp_xml_string);
                    }
                    if (!xmlStrcmp(p1->name, (const xmlChar*)"humidity")){
                        temp_xml_string = xmlNodeGetContent(p1);
                        forecast_data->Humidity(atoi((char *)temp_xml_string));
                        xmlFree(temp_xml_string);
                    }
                    if (!xmlStrcmp(p1->name, (const xmlChar*)"visible")){
                        temp_xml_string = xmlNodeGetContent(p1);
                        if (temp_xml_string){
                            forecast_data->ViSible().value((float)atof((char*)temp_xml_string));
                            xmlFree(temp_xml_string);
                        }
                    }
                    if (!xmlStrcmp(p1->name, (const xmlChar*)"pressure")){
                        temp_xml_string = xmlNodeGetContent(p1);
                        forecast_data->pressure().value((float)atof((char *)temp_xml_string));
                        xmlFree(temp_xml_string);
                    }
                    if (!xmlStrcmp(p1->name, (const xmlChar*)"ppcp")){
                        temp_xml_string = xmlNodeGetContent(p1);
                        if (temp_xml_string){
                            forecast_data->Ppcp((float)atof((char *)temp_xml_string));
                            xmlFree(temp_xml_string);
                        }
                    }
                    if (!xmlStrcmp(p1->name, (const xmlChar*)"wind_speed")){
                        temp_xml_string = xmlNodeGetContent(p1);
                        if (temp_xml_string){
                            if (!xmlStrcmp(temp_xml_string, (const xmlChar*)"clam"))
                                forecast_data->WindSpeed().value((float)0);
                            else{
                                forecast_data->WindSpeed().value((float)atof((char *)temp_xml_string));
                            }
                            xmlFree(temp_xml_string);
                        }
                    }
                    if (!xmlStrcmp(p1->name, (const xmlChar*)"wind_direction")){
                        temp_xml_string = xmlNodeGetContent(p1);
                        forecast_data->WindDirection((char *)temp_xml_string);
                        xmlFree(temp_xml_string);
                    }
                    if (!xmlStrcmp(p1->name, (const xmlChar*)"sunrise")){
                        temp_xml_string = xmlNodeGetContent(p1);
                        forecast_data->SunRiseTime((int)atoi((char *)temp_xml_string));
                        xmlFree(temp_xml_string);
                    }
                    if (!xmlStrcmp(p1->name, (const xmlChar*)"sunset")){
                        temp_xml_string = xmlNodeGetContent(p1);
                        forecast_data->SunSetTime((int)atoi((char *)temp_xml_string));
                        xmlFree(temp_xml_string);
                    }

                }

                  //  if  (source_name=="yr.no")
                  //      forecastURL.replace("#","/");
                  //  if  (source_name=="yr.no")
                  //      viewURL.replace("#","/");
                _list->push_back(forecast_data);

            }
       }

#endif
#endif
    }

////////////////////////////////////////////////////////////////////////////////
    int
    DataParser::timezone(){
        return _timezone;
    }
////////////////////////////////////////////////////////////////////////////////
    time_t
    DataParser::LastUpdate(){
        return _last_update;
    } 
////////////////////////////////////////////////////////////////////////////////
    DataParser* 
    DataParser::Instance(){
        if (!_self)
            _self = new DataParser();
        _refcount++;
        /*  std::cerr<<"Refcount for DataParser: "<<_refcount<<std::endl; */
        return _self;
    }

////////////////////////////////////////////////////////////////////////////////
    DataParser* 
    DataParser::Instance(const std::string& filename, const std::string& schema_filename){
        Data* forecast_data;
        /* std::cerr<<"Refcount for DataParser1: "<<_refcount<<std::endl; */
        if (!_self)
            _self = new DataParser(filename, schema_filename);
        else{
            while (!_list->empty()){
                forecast_data = _list->back();
                delete forecast_data;
                _list->pop_back();
            }
            delete _list;
            _self->load_data(filename, schema_filename);
        }
        if (_refcount == 0)
            _refcount++;
        /* std::cerr<<"Refcount for DataParser2: "<<_refcount<<std::endl; */
        return _self;
    }
////////////////////////////////////////////////////////////////////////////////
    void
    DataParser::DeleteInstance(){
       AppLog("Try to delete Dataparser Instance");
       if (_refcount == 0)
            return;
       _refcount--;
       /*  std::cerr<<"Delete Refcount for DataParser: "<<_refcount<<std::endl; */ 
       if (_refcount == 0){
            delete _self;
            _self = NULL;
       }
    }
////////////////////////////////////////////////////////////////////////////////
    DataParser::~DataParser(){
       AppLog("Delete DataParser");
       Data* forecast_data;
       while (!_list->empty()){
         forecast_data = _list->back();
         delete forecast_data;
         _list->pop_back();
       }
       delete _list;
    }
////////////////////////////////////////////////////////////////////////////////
#ifdef LIBXML
    void DataParser::processNode(const xmlpp::Node* node){
        static Data* forecast_data;
        if(!node)
            return;
        std::string nodeName = node->get_name();

        // source tag
        if(nodeName == "station"){
            xmlpp::Node::NodeList list = node->get_children();
            for(xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
                processNode(*iter);
        }
        // timezone tag
        if(nodeName == "timezone"){
            xmlpp::Node::NodeList list = node->get_children();
            xmlpp::Node::NodeList::iterator iter = list.begin();
            const xmlpp::TextNode* nodeText = dynamic_cast<const xmlpp::TextNode*>(*iter);
            _timezone = atoi(nodeText->get_content().c_str());
            /* std::cout<<"timezone "<<_timezone<<std::endl; */
            return;
        }
        // period tag
        if(nodeName == "period"){
            forecast_data = new Data();
            const xmlpp::Element* nodeElement = dynamic_cast<const xmlpp::Element*>(node);
            const xmlpp::Attribute* attribute = nodeElement->get_attribute("start");
            forecast_data->StartTime(atoi(attribute->get_value().c_str()));
            attribute = nodeElement->get_attribute("end");
            forecast_data->EndTime(atoi(attribute->get_value().c_str()));
            attribute = nodeElement->get_attribute("current");

            if (attribute && attribute->get_value() == "true"){
                forecast_data->Current(1);
            }
            attribute = nodeElement->get_attribute("hour");

            if (attribute && attribute->get_value() == "true"){
                forecast_data->Hour(1);
            }

            xmlpp::Node::NodeList list = node->get_children();
            for(xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
                processNode(*iter);
            _list->push_back(forecast_data);
        }
        // temperature tag
        if(nodeName == "temperature"){
            xmlpp::Node::NodeList list = node->get_children();
            xmlpp::Node::NodeList::iterator iter = list.begin();
            const xmlpp::TextNode* nodeText = dynamic_cast<const xmlpp::TextNode*>(*iter);
            /* std::cout<<"temperature "<< nodeText->get_content() <<std::endl; */
            forecast_data->temperature().value(atof(nodeText->get_content().c_str()));
            return;
        }
        // temperature tag
        if(nodeName == "temperature_low"){
            xmlpp::Node::NodeList list = node->get_children();
            xmlpp::Node::NodeList::iterator iter = list.begin();
            const xmlpp::TextNode* nodeText = dynamic_cast<const xmlpp::TextNode*>(*iter);
            /* std::cout<<"temperature "<< nodeText->get_content() <<std::endl; */
            forecast_data->temperature_low().value(atof(nodeText->get_content().c_str()));
            return;
        }
        // temperature tag
        if(nodeName == "temperature_hi"){
            xmlpp::Node::NodeList list = node->get_children();
            xmlpp::Node::NodeList::iterator iter = list.begin();
            const xmlpp::TextNode* nodeText = dynamic_cast<const xmlpp::TextNode*>(*iter);
            /* std::cout<<"temperature "<< nodeText->get_content() <<std::endl; */
            forecast_data->temperature_hi().value(atof(nodeText->get_content().c_str()));
            return;
        }
        // icon tag
        if(nodeName == "icon"){
            xmlpp::Node::NodeList list = node->get_children();
            xmlpp::Node::NodeList::iterator iter = list.begin();
            const xmlpp::TextNode* nodeText = dynamic_cast<const xmlpp::TextNode*>(*iter);
            /* std::cout<<"icon "<< nodeText->get_content() <<std::endl; */
            forecast_data->Icon(atoi(nodeText->get_content().c_str()));
            return;
        }
        // Text tag
        if(nodeName == "description"){
            xmlpp::Node::NodeList list = node->get_children();
            xmlpp::Node::NodeList::iterator iter = list.begin();
            const xmlpp::TextNode* nodeText = dynamic_cast<const xmlpp::TextNode*>(*iter);
            /* std::cout<<"Description "<< nodeText->get_content() <<std::endl; */
            forecast_data->Text(nodeText->get_content().c_str());
            return;
        }
        // Humidity tag
        if(nodeName == "humidity"){
            xmlpp::Node::NodeList list = node->get_children();
            xmlpp::Node::NodeList::iterator iter = list.begin();
            const xmlpp::TextNode* nodeText = dynamic_cast<const xmlpp::TextNode*>(*iter);
            /* std::cout<<"Humidity "<< nodeText->get_content() <<std::endl; */
            forecast_data->Humidity(atoi(nodeText->get_content().c_str()));
            return;
        }
        // Wind speed tag
        if(nodeName == "wind_speed"){
            xmlpp::Node::NodeList list = node->get_children();
            xmlpp::Node::NodeList::iterator iter = list.begin();
            const xmlpp::TextNode* nodeText = dynamic_cast<const xmlpp::TextNode*>(*iter);
            /* std::cout<<"Wind speed "<< nodeText->get_content() <<std::endl; */
            //forecast_data->WindSpeed    (atoi(nodeText->get_content().c_str()));
            forecast_data->WindSpeed().value(atoi(nodeText->get_content().c_str()));
            return;
        }
        // Wind direction tag
        if(nodeName == "wind_direction"){
            xmlpp::Node::NodeList list = node->get_children();
            xmlpp::Node::NodeList::iterator iter = list.begin();
            const xmlpp::TextNode* nodeText = dynamic_cast<const xmlpp::TextNode*>(*iter);
            /* std::cout<<"Wind direction "<< nodeText->get_content() <<std::endl; */
            forecast_data->WindDirection(nodeText->get_content().c_str());
            return;
        }
        // Flike tag
        if(nodeName == "flike"){
            xmlpp::Node::NodeList list = node->get_children();
            xmlpp::Node::NodeList::iterator iter = list.begin();
            const xmlpp::TextNode* nodeText = dynamic_cast<const xmlpp::TextNode*>(*iter);
            /* std::cout<<"flike "<< nodeText->get_content() <<std::endl;*/
            forecast_data->Flike().value(atof(nodeText->get_content().c_str()));
            return;
        }
        // Pressure tag
        if(nodeName == "pressure"){
            xmlpp::Node::NodeList list = node->get_children();
            xmlpp::Node::NodeList::iterator iter = list.begin();
            const xmlpp::TextNode* nodeText = dynamic_cast<const xmlpp::TextNode*>(*iter);
            /* std::cout<<"pressure "<< nodeText->get_content() <<std::endl;*/
            //forecast_data->Pressure(atoi(nodeText->get_content().c_str()));
            forecast_data->pressure().value(atoi(nodeText->get_content().c_str()));
            return;
        }
        // Ppcp tag
        if(nodeName == "ppcp"){
            xmlpp::Node::NodeList list = node->get_children();
            xmlpp::Node::NodeList::iterator iter = list.begin();
            const xmlpp::TextNode* nodeText = dynamic_cast<const xmlpp::TextNode*>(*iter);
            /* std::cout<<"ppcp "<< nodeText->get_content() <<std::endl;*/
            forecast_data->Ppcp(atof(nodeText->get_content().c_str()));
            return;
        }

    }
#endif
////////////////////////////////////////////////////////////////////////////////
    DataList& DataParser::data(){
        return *_list;
    }
////////////////////////////////////////////////////////////////////////////////
} // namespace Core

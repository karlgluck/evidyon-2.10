//------------------------------------------------------------------------------------------------
// File:    dcxinifilereader.h
//
// Desc:    Provides a simple interface for reading a .ini settings file
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCXINIFILEREADER_H__
#define __DCXINIFILEREADER_H__


#include <set>
#include <string>
#include <map>


namespace dcx {

    /**
     * Provides a simple interface for reading a standard INI file
     */
    class dcxIniFileReader
    {
        friend class dcxIniFileWriter;
        typedef std::set<std::string> SectionList;
        typedef SectionList::iterator SectionIterator;
        typedef std::pair<const char*,std::string> SettingMapKeyType;
        typedef std::map<SettingMapKeyType,std::string> SettingMap;
        typedef SettingMap::iterator SettingIterator;
        typedef SettingMap::value_type SettingMapElementType;

        public:

            /**
             * Scans a source file that contains an INI that provides settings information.  This
             * method clears any information that has been previously obtained using read().  This
             * method returns 'false' only if the file doesn't exist.
             * @param file The path to the source file
             */
            bool read( const char* file );

            /**
             * Gets an integer setting
             * @param section The name of the section to access
             * @param setting The name of the section to access
             * @param defaultValue The value to return if the setting couldn't be found
             * @return The value of the specified setting, or defaultValue if the setting wasn't found
             */
            int getSetting( const char* section, const char* setting, int defaultValue );

            /**
             * Gets a string setting
             * @param section The name of the section to access
             * @param setting The name of the section to access
             * @param defaultValue The value to return if the setting couldn't be found
             * @return The value of the specified setting, or defaultValue if the setting wasn't found
             */
            const char* getSetting( const char* section, const char* setting, const char* defaultValue );

            /**
             * Gets a floating-point setting
             * @param section The name of the section to access
             * @param setting The name of the section to access
             * @param defaultValue The value to return if the setting couldn't be found
             * @return The value of the specified setting, or defaultValue if the setting wasn't found
             */
            float getSetting( const char* section, const char* setting, float defaultValue );


        protected:

            /**
             * Searches the internal list/map for the specified setting.  This method first searches
             * the section names list to find the pointer for the section name, then calls findInMap.
             * @return 'true' if the setting was found (the object referenced by iterator will be set)
             */
            bool find( const std::string& section, const std::string& setting, SettingIterator* iterator );

            /**
             * Searches the internal setting map for the specified setting.  This method looks directly into
             * the map itself, using the section's actual pointer value as part of the search.  This saves
             * memory by not duplicating section names.
             * @return 'true' if the setting was found (the object referenced by iterator will be set)
             */
            bool findInMap( const char* sectionNamePointer, const std::string& setting, SettingIterator* iterator );


        protected:

            /// The different section names found in the INI file
            SectionList mySections;

            /// All of the different settings in the INI file
            SettingMap mySettings;

    };

}



#endif

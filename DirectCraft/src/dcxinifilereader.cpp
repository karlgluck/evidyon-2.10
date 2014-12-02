//------------------------------------------------------------------------------------------------
// File:    dcxinifilereader.cpp
//
// Desc:    Provides a simple interface for reading a .ini settings file
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "../dcx/inifilereader"
#include <stdio.h>
#include "../dc/debug"


using namespace dcx;

//------------------------------------------------------------------------------------------------
// Name:  read
// Desc:  Scans a source stream that contains an INI file for settings information
//------------------------------------------------------------------------------------------------
bool dcxIniFileReader::read( const char* file )
{
    // The file-access pointer
    FILE* pf;

    // Open the source file
    if( 0 != fopen_s( &pf, file, "rt" ) )
    {
        // Warn about this problem
        DEBUG_WARNING( "Couldn't open settings file %s", file );

        // Exit
        return false;
    }

    // Clear all the internal information
    mySections.clear();
    mySettings.clear();

    // The current section
    SectionIterator currentSection = mySections.end();

    // Read lines from the source file until there's nothing left
    while( !feof( pf ) )
    {
        // Read a line into this string
        std::string line;
        {
            char lineBuffer[1024];
            fscanf_s( pf, "%[^\r\n]\n", lineBuffer, sizeof(lineBuffer) );
            line = lineBuffer;
        }

        // Make sure the line contains something
        if( line.empty() ) continue;

        // Read from the line
        if( line.at(0) == '[' )
        {
            // This is a section
            std::string::size_type begin = line.find_first_not_of("[");
            std::string sectionName = line.substr( begin, line.find_first_of("]") - begin );

            // Make sure we found a valid section name
            if( sectionName.empty() ) continue;

            // Add this section name to the internal list, and start using it for inserting settings
            currentSection = mySections.insert( sectionName ).first;
        }
        else if( line.at(0) == '%' )
        {
            // This is a comment line; just move on
            continue;
        }
        else
        {
            // Get the dividing point for the label and its value
            std::string::size_type equalsPoint = line.find_first_of("=");
            if( equalsPoint == line.npos ) continue;

            // Get the label (setting name) and the seting value
            std::string name = line.substr( 0, equalsPoint );
            std::string value = line.substr( equalsPoint + 1 );

            // Make sure the name and value are valid
            if( name.empty() || value.empty() ) continue;

            // Add this to the internal settings map
            const char* keySection = currentSection == mySections.end() ? 0 : currentSection->c_str();
            mySettings.insert( SettingMapElementType( SettingMapKeyType( keySection, name ), value ) );
        }
    }

    // Close the ini file
    fclose( pf );

    // Successfully read information
    return true;
}




//------------------------------------------------------------------------------------------------
// Name:  getSetting
// Desc:  Gets an integer setting
//------------------------------------------------------------------------------------------------
int dcxIniFileReader::getSetting( const char* section, const char* setting, int defaultValue )
{
    // The setting value
    SettingIterator iterator;

    // Look internally for the setting
    if( find( section, setting, &iterator ) )
        return atol( iterator->second.c_str() );
    else
        return defaultValue;
}




//------------------------------------------------------------------------------------------------
// Name:  getSetting
// Desc:  Gets a string setting
//------------------------------------------------------------------------------------------------
const char* dcxIniFileReader::getSetting( const char* section, const char* setting, const char* defaultValue )
{
    // The setting
    SettingIterator iterator;

    // Look internally for the setting
    if( find( section, setting, &iterator ) )
        return iterator->second.c_str();
    else
        return defaultValue;
}




//------------------------------------------------------------------------------------------------
// Name:  getSetting
// Desc:  Gets a floating-point setting
//------------------------------------------------------------------------------------------------
float dcxIniFileReader::getSetting( const char* section, const char* setting, float defaultValue )
{
    // The setting
    SettingIterator iterator;

    // Look internally for the setting
    if( find( section, setting, &iterator ) )
        return (float)atof( iterator->second.c_str() );
    else
        return defaultValue;
}




//------------------------------------------------------------------------------------------------
// Name:  find
// Desc:  Searches the internal list/map for the specified setting
//------------------------------------------------------------------------------------------------
bool dcxIniFileReader::find( const std::string& section, const std::string& setting, SettingIterator* iterator )
{
    // Search the section map for this section
    SectionIterator sectionIter = mySections.find( section );
    return findInMap( sectionIter == mySections.end() ? 0 : sectionIter->c_str(), setting, iterator );
}




//------------------------------------------------------------------------------------------------
// Name:  findInMap
// Desc:  Searches the internal setting map for the specified setting
//------------------------------------------------------------------------------------------------
bool dcxIniFileReader::findInMap( const char* sectionNamePointer, const std::string& setting, SettingIterator* iterator )
{
    // Search the settings map
    *iterator = mySettings.find( SettingMapKeyType( sectionNamePointer, setting ) );
    return (*iterator) != mySettings.end();
}

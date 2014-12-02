//------------------------------------------------------------------------------------------------
// File:    resourcepack.cpp
//
// Desc:    A resource pack puts together a bunch of resources in a flat file so that they can
//          be distributed as a unit
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "allgameresources.h"
#include "resourcepack.h"
#include <dc/filestream>
#include <dc/dcresourcetrader.h>



#include "stdafx.h"











// Resolve self-implementation linker
dcClass<ResourcePack>::Implements<ResourcePack> ResourcePack::implementsSelf;


//------------------------------------------------------------------------------------------------
// Name:  ResourcePack
// Desc:  Initializes the class
//------------------------------------------------------------------------------------------------
ResourcePack::ResourcePack()
{
    member("Source File", &mySourceFile);
}


//------------------------------------------------------------------------------------------------
// Name:  openResource
// Desc:  Opens a stream at the location of the resource with the given index
//------------------------------------------------------------------------------------------------
bool ResourcePack::openResource(int index, dcBuffer* buffer)
{
    // Fail if the parameters are invalid
    if (APP_ERROR(index < 0 || !buffer)("Invalid parameter to openResource"))
        return false;

    // Open the file
    dc::dcFileStream file;
    if (!file.open(mySourceFile.getValue().c_str(), 0))
        return false;

    // Load the header
    FileHeader header;
    if (!file.read(&header, sizeof(header)))
    {
        file.close();
        return false;
    }

    // Make sure the magic number matches and this resource is valid
    if (APP_WARNING(header.MagicNumber != ResourcePack::MAGIC_NUMBER)("Resource pack invalid") ||
        APP_WARNING(index >= header.ResourceCount)("Invalid resource index requested"))
    {
        file.close();
        return false;
    }

    // Look forward in the file to find this resource's location and size
    size_t resourceLocation, resourceSize;
    if (!file.move(sizeof(size_t) * 2 * index) ||
        !file.read(&resourceLocation, sizeof(resourceLocation)) ||
        !file.read(&resourceSize, sizeof(resourceSize)))
    {
        file.close();
        return false;
    }

    // Scan to wherever this resource is
    file.seek(resourceLocation);

    // Create a temporary buffer
    //unsigned char* buffer = new unsigned char[resourceSize];
    //if (APP_FATAL(!buffer)("Out of memory")) { file.close(); return false; }

    // Read into this buffer
    //if (APP_ERROR(!file.read(buffer, resourceSize)))
    if (APP_ERROR(!buffer->readFrom(&file, resourceSize))("Couldn't read resource from pack file"))
    {
        // Get rid of the input stream
        file.close();

        // Failure
        return false;
    }

    // Close the file
    file.close();

    // Success
    return true;
}


//------------------------------------------------------------------------------------------------
// Name:  loadPackResources
// Desc:  Loads the resources from this pack into the specified project
//------------------------------------------------------------------------------------------------
bool ResourcePack::loadPackResources(std::string* packName, ProjectVolucris* emptyProject)
{
    // Make sure the parameter is valid
    if (APP_ERROR(!emptyProject)("Invalid parameter to loadPackResources"))
        return false;

    // Open the file
    dc::dcFileStream file;
    if (!file.open(mySourceFile.getValue().c_str(), 0))
        return false;

    // Load the header
    FileHeader header;
    if (!file.read(&header, sizeof(header)))
    {
        file.close();
        return false;
    }

    // Make sure the magic number matches
    if (APP_WARNING(header.MagicNumber != ResourcePack::MAGIC_NUMBER)("Resource pack invalid"))
    {
        file.close();
        return false;
    }

    // Save the pack name
    packName->assign(header.PackName);

    // Move to the project location in the file
    file.seek(header.ProjectLocation);

    // Read the project using the importer
    dc::dcResourceTrader importer;
    bool returnValue = importer.importFrom(emptyProject, &file);
    //bool returnValue = emptyProject->load(&file);

    // Close the file
    file.close();

    // Return the result of the loading process
    return returnValue;
}



//------------------------------------------------------------------------------------------------
// Name:  staticTypeName
// Desc:  Returns the type name of this class
//------------------------------------------------------------------------------------------------
std::string ResourcePack::staticTypeName()
{
    return "Resource Pack";
}


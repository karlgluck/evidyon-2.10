//------------------------------------------------------------------------------------------------
// File:    resourcepack.h
//
// Desc:    A resource pack puts together a bunch of resources in a flat file so that they can
//          be distributed as a unit
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __RESOURCEPACK_H__
#define __RESOURCEPACK_H__

#include <dc/buffer>


/**
 *
 */
class ResourcePack : public dc::dcResource<ResourcePack>
{
    public:

        /// The number that is always the same in the resource pack header
        static const int MAGIC_NUMBER = 0x7FEED01B;

        /**
         * Structure always at the start of the resource pack.  The pack
         * file is set up like the following:
         *      FileHeader header;
         *      FileResourceIndexElement index[FileHeader.ResourceCount];
         *      (possible junk data here)
         *      << array of resources >>
         *      ProjectVolucris project;
         */
        struct FileHeader
        {
            /// Always the same number (unless the pack changes)
            unsigned long MagicNumber;

            /// The name of this resource pack
            char PackName[32];

            /// Where the project to be imported is located in the file
            size_t ProjectLocation;

            /// How many resources there are in this pack file
            size_t ResourceCount;
        };

        /**
         * Specifies where to find a resource and that resource's byte size.  An
         * array of these comes directly after the header in the resource pack,
         * with one entry for each saved resource.
         */
        struct FileResourceIndexElement
        {
            /// The absolute byte location of the resource in the file
            size_t ResourceLocation;

            /// How large the resource is, in byte
            size_t ResourceSize;
        };


    public:

        /**
         * Initializes the class
         */
        ResourcePack();

        /**
         * Opens a stream at the location of the resource with the given index
         * @param index The index of the item to access
         * @param buffer The destination buffer to be filled with data
         * @return Whether or not the stream will return valid data
         */
        bool openResource(int index, dcBuffer* buffer);

        /**
         * Loads the resources from this pack into the specified project
         * @param packName Destination for the name that this this resource pack
         *                 needs to have in order for its references to work correctly.
         * @param emptyProject A blank project into which to load the resources
         *                     in the pack file.
         */
        bool loadPackResources(std::string* packName, ProjectVolucris* emptyProject);

        /**
         * Gets the source filename for this pack
         */
        dc::dcFileName* getSourceFile() { return &mySourceFile; }


    public:

        /**
         * Returns the type name of this class
         */
        static std::string staticTypeName();


    protected:

        /// The resource pack file
        dc::dcFileName mySourceFile;

        /// Implements its own type
        static dc::dcClass<ResourcePack>::Implements<ResourcePack> implementsSelf;
};



#endif
//------------------------------------------------------------------------------------------------
// File:   dcresourcetrader.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCRESOURCTRADER_H__
#define __DCRESOURCTRADER_H__



// TODO: add the ability to translate import types


namespace dc {

    /**
     * This class can be used to export a resource to a file so that it can be imported by another
     * program using a different version of the DirectCraft library.
     */
            // dcResourceOperator
    class dcResourceTrader
    {
        enum Operation
        {
            OPERATION_EXPORT,
            OPERATION_IMPORT,
            OPERATION_MERGE,
        };

        public:

            /// A group of resource types
            typedef std::map<unsigned long,std::string> ResourceTypes;

            /// Resource managers for all of the types
            typedef std::map<unsigned long,dcResourceManager*> ResourceManagers;

            /// List of errors
            typedef std::list<std::string> Errors;

            /// List of warnings
            typedef std::list<std::string> Warnings;

            /// Determines whether or not one resource should overwrite another
            typedef bool (*OverwriteCallback)( dcGenericResource* into, dcGenericResource* from, void* context );

        public:

            /**
             * Initializes the class
             */
            dcResourceTrader();

            /**
             * Exports the provided resource and all of its members into the output stream.  Only
             * one resource should be exported into a single stream.
             */
            bool exportTo( dcGenericResource* rootResource, dcStreamOut* stream );

            /**
             * Imports a resource from the stream by trying to fill all of its members with data
             * matched up from the file.
             */
            bool importFrom( dcGenericResource* rootResource, dcStreamIn* stream );

            /**
             * Merges two resources together.  The callback is used to check to see which of two resources
             * should be preserved during the merge; if it returns 'true' then the source resource will
             * overwrite the destination one; otherwise, the current resource will be preserved.  The
             * 'context' variable can be any value, and is passed to overwriteCallback.
             * @param into Destination resource
             * @param from Source resource
             * @param overwriteCallback Callback to check to see whether to overwrite one resource with
             *                          another.  This is optional, and if 'null' is specified then no
             *                          resources will be overwritten.
             * @param context The context parameter for overwriteCallback.  No significance to this class.
             */
            void merge( dcGenericResource* into, dcGenericResource* from, OverwriteCallback overwriteCallback, void* context );

            /**
             * Gets the error output that was generated during an import/export operation
             */
            const Errors& getErrors();

            /**
             * Gets the warning output that was generated during an import/export operation
             */
            const Warnings& getWarnings();

            /**
             * Obtains the resource types that couldn't be imported to a resource class
             */
            const ResourceTypes& getUnresolvedImportTypes();

            /**
             * Writes a bunch of information about the last import/export operation to a file and
             * returns the path of the log file that was generated (it will be in the current
             * directory).
             */
            void generateLogFile( std::string* logFile ) const;


        public:

            /**
             * Writes all of the resource types in the program as text to the output string.  This
             * prints a line of hexadecimal code followed by a type name for each type of resource
             * in the program.
             */
            static void printAllResourceTypes( std::string* output );


        private:

            /**
             * Helper method used internally to export a resource
             */
            bool exportRecursive( dcGenericResource* resource, dcStreamOut* stream );

            /**
             * Helper method used internally to import a resource
             */
            bool importRecursive( dcGenericResource* resource, dcStreamIn* stream );

            /**
             * Merges (via copy) members from one resource into another
             */
            void mergeRecursive( dcGenericResource* into, dcGenericResource* from );

            /**
             * Gets the most unique member of the parent resource using the provided name/type
             */
            dcGenericResource* obtainMemberLike( dcGenericResource* parent, const char* name, unsigned long type ) const;


        private:

            /// The last operation that this class was used to perform
            Operation myLastOperation;

            /// Textual errors generated during import/export
            Errors myErrors;

            /// Textual warnings generated during import/export
            Warnings myWarnings;

            /// All of the resource types that couldn't be resolved
            ResourceTypes myUnresolvedImportTypes;

            /**
             * Builds a string form of the operation context list
             */
            std::string getOperationContext() const;

        private:

            /// All of the resource types in the source import stream
            ResourceTypes myImportSourceTypes;

            /// Used during imports to build a text form of where we are in the file
            std::list<std::string> myOperationContext;

            /// All of the possible resource types that this program can create
            ResourceManagers myProgramTypeManagers;

            /// The callback used during merging to determine whether or not to overwrite resources
            OverwriteCallback myOverwriteCallback;

            /// The user context variable passed to the callback
            void* myContext;
    };

}




#endif
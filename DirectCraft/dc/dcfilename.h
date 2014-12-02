//------------------------------------------------------------------------------------------------
// File:    dcfilename.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCFILENAME_H__
#define __DCFILENAME_H__

namespace dc {

    /**
     * Stores a file name
     */
    class dcFileName : public dc::dcObject<dcFileName,std::string>
    {
        public:

            /**
             * Initializes this resource
             */
            dcFileName();

            /**
             * Method implemented by the derived class to save this object
             * to the destination stream
             */
            bool save( dcStreamOut* stream );

            /**
             * Method implemented by the derived class to load this object
             * from the stream
             */
            bool load( dcStreamIn* stream );

            /**
             * Gets a string representation of this class type
             * @return String value indicating what's in this class
             */
            std::string toString() const;

            virtual void serialize(std::string* string);
            virtual void interpret(const char* string);


        public:

            /**
             * Obtains the name of this class type
             * @return dcClass name string
             */
            static std::string staticTypeName();

            /**
             * Converts the absolute path provided into a path relative to the current path
             * using standard directory notation such that appending currentPath with relativePath
             * will yield the same file as absolutePath.  This method produces undefined behavior
             * if the currentPath parameter contains a file name at the end; if a directory that
             * begins with the same characters as that file exists, relative paths to children of
             * that directory will not be correctly generated.
             * @param currentPath The path from which to "locate" the relative path
             * @param absolutePath The path that should be created
             * @param relativePath The output relative path
             * @param maxPathLen The maximum length that can be written into relativePath
             */
            static void convertToRelativePath( const char* currentPath, const char* absolutePath,
                                               char* relativePath, size_t maxPathLen );

            /**
             * Converts a relative path and source directory pair to an absolute path.  This method
             * doesn't care of currentPath contains a file name at the end of the directory.
             * @param currentPath The path from which to "locate" the relative path
             * @param relativePath The source relative path that should be resolved
             * @param absolutePath The absolute path conversion of the current/relative paths
             * @param maxPathLen The maximum length that can be written into relativePath
             */
            static void convertToAbsolutePath( const char* currentPath, const char* relativePath,
                                               char* absolutePath, size_t maxPathLen );

        public:

            /**
             * Whenever a file name is saved or loaded, it converts between the absolute path stored
             * in memory and the relative file saved in the stream using the global current path.
             * This method sets that path.
             */
            static void setCurrentPath( const char* currentPath );

            /**
             * Calls setCurrentPath after removing the file name from the end of the path.
             * @param fileNameInPath Fully-qualified path to a file from which to extract the directory
             */
            static void setCurrentPathFromFileDirectory( const char* fileNameInPath );


        protected:

            /**
             * Method implemented by the derived class to compare the class
             * to another resource that is guaranteed to be valid and of
             * the same type.
             * @see compareTo
             */
            int compareValue( const std::string& otherValue ) const;


        protected:

            /// The path to convert from/to when loading/saving file names
            static std::string theCurrentPath;
    };
};


#endif
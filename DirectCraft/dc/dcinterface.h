//------------------------------------------------------------------------------------------------
// File:    dcinterface.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCINTERFACE_H__
#define __DCINTERFACE_H__


namespace dc {

    /**
     * Implements methods that it is not necesary to duplicate for every kind of dcInterface.
     */
    class dcGenericInterface : public dcGenericResource
    {
        public:

            /**
             * Initializes this resource
                 */
            dcGenericInterface();

            /**
             * Cleans up when the class is being deconstructed
             */
            virtual ~dcGenericInterface();

            /**
             * Erases the implementation inside of the interface, if it exists
             */
            void release();

            /**
             * Changes the implementation of this resource to be the type corresponding
             * to the provided type ID.
             */
            dcGenericResource* implementByType( unsigned long id );

            /**
             * Determines whether or not this interface has an implementation
             */
            bool isImplemented() const;

            /**
             * Gets the type of this resource's implementation.  The returned value is only
             * valid if isImplemented() returns 'true'.
             */
            unsigned long getImplementationType() const;

            /**
             * Gets the generic implementation of this interface.  If this interface is
             * not implemented, the return value will be null.
             */
            dcGenericResource* getGenericImplementation() const;


        public:

            /**
             * Returns 'false' because this is not a subclass of dcGenericCollection.  This
             * delcaration is not strictly necessary, but it prevents subclasses from
             * overriding this method erroneously.
             */
            bool isCollection() const;

            /**
             * Returns 'true' because all subclasses will implement dcGenericInterface
             */
            bool isInterface() const;

            /**
             * Saves whatever kind of resource is referenced by this interface.
             */
            bool save( dcStreamOut* stream );

            /**
             * Loads an object implementing the interface that this class references from
             * the stream.
             */
            bool load( dcStreamIn* stream );

            /**
             * Compares this resource with another resource
             */
            int compareTo( const dcGenericResource* other ) const;

            virtual dcGenericResource* acquireMember(const char* member_name,
                                                     const char* type,
                                                     size_t index_hint);

        protected:

            /**
             * Gets an instance of the resource manager for the given type ID
             */
            virtual dcResourceManager* getSubclassManager( unsigned long id ) = 0;

            /**
             * Typecasts a resource variable into a generic interface
             */
            const dcGenericInterface* typecastGeneric( const dcGenericResource* variable ) const;
    };


    /**
     * Stores a reference to an abstract type that implements the interface T
     */
    template <typename T> class dcInterface : public dcResource<dcInterface<T>,dcGenericInterface>
    {
        public:

            // Allow the input parameters to be referenced
            typedef T ResourceInterface;

        public:

            /**
             * Initializes this resource
             */
            dcInterface()
            {
            }

            /**
             * Gets a string representation of this class type
             * @return String value indicating what's in this class
             */
            std::string toString() const
            {
                return staticTypeName() + "{ " + (isImplemented() ? getGenericImplementation()->toString() : "null") + " }";
            }

            /**
             * Obtains a list of all of the managers
             * TODO: clean this up.  should this be a std::list<>? should we typedef this type?  should this
             * only pass back type names? (that's why it's used...by projectvolucriseditor.cpp)
             * also used by dcresourceexporter now...
             */
            static void getSubclassManagerList( std::list<dcResourceManager*>* managers )
            {
                InterfaceLinker<T>* linker = InterfaceLinker<T>::firstLinker;
                while( linker != 0 )
                {
                    managers->push_back( linker->myManager );
                    linker = linker->myNextLinker;
                }
            }


        public:

            /**
             * Obtains the sub-class resource manager for a particular type ID number
             */
            static dcResourceManager* generateSubclass( unsigned long id )
            {
                // Find the administrator for this type ID
                InterfaceLinker<T>* linker = InterfaceLinker<T>::firstLinker;
                while( linker != 0 && linker->myManager->uniqueID() != id )
                    linker = linker->myNextLinker;
                if( linker && linker->myManager)
                    return linker->myManager->generate();
                else
                    return 0;
            }


        public:

            /**
             * Obtains the value stored in the object
             * @return The current value saved in this resource
             */
            ResourceInterface* getImplementation() const { return (ResourceInterface*)getGenericImplementation(); }

            /**
             * Creates a new instance of an interface-implementing class, sets it as the value for
             * this class's interface, and returns a reference to the newly generated object.
             */
            template <typename C> C* implement()
            {
                return (C*)implementByType( dcClass<C>::staticResourceManager()->uniqueID() );
            }


        public:

            /**
             * Obtains the name of this class type
             * @return dcClass name string
             */
            static std::string staticTypeName()
            {
                return std::string( "<" ) + T::staticTypeName() + ">";
            }


        protected:

            /**
             * Allows dcGenericInterface to access the static type-creating method for its
             * derived interface type.
             */
            dcResourceManager* getSubclassManager( unsigned long id )
            {
                // Find the administrator for this type ID
                InterfaceLinker<T>* linker = InterfaceLinker<T>::firstLinker;
                while( linker != 0 && linker->myManager->uniqueID() != id )
                    linker = linker->myNextLinker;
                if( linker )
                    return linker->myManager;
                else
                    return 0;
            }
    };
};


#endif
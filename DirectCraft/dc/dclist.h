//------------------------------------------------------------------------------------------------
// File:    dclist.h
//
// Desc:    Stores a list of resources
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCLIST_H__
#define __DCLIST_H__



namespace dc {

    /**
     * Stores a list of elements of the given type
     */
    template <typename T> class dcList : public dc::dcCollection<dcList<T>>
    {
        public:

            class Element : public dcResource<Element,T>
            {
                public:

                    /**
                     * Initializes this element
                     */
                    Element()
                    {
                    }


                public:

                    /**
                     * Gets the name of this type of resource
                     */
                    static std::string staticTypeName()
                    {
                        return "Element of " + dcList<T>::staticTypeName();
                    }
            };

            /// Define the type that references an element in this list
            typedef dcReference<typename dcList<T>::Element> Reference;


        public:

            /**
             * Initializes this resource
             */
            dcList()
            {
            }

            /**
             * Determines whether or not this collection allows duplicate entries to
             * be held internally.
             * @return Flag value
             */
            bool allowDuplicates() const
            {
                return true;
            }

            /**
             * Determines whether or not the derived collection type requires that its
             * entries be kept sorted.
             */
            bool keepSorted() const
            {
                return false;
            }

        public:

            /**
             * The name of this class type
             * @return dcClass name string
             */
            static std::string staticTypeName() { return std::string("List of ") + T::staticTypeName(); }
    };

}



#endif
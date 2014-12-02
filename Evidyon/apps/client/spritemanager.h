//------------------------------------------------------------------------------------------------
// File:  spritemanager.h
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __SPRITEMANAGER_H__
#define __SPRITEMANAGER_H__




template <typename Element> class GrowingArray
{
  public:

    /**
     * Resets the class
     */
    GrowingArray()
    {
      myArray = 0;
      myEntries = 0;
      myCapacity = 0;
    }

    /**
     * Makes sure that the array's memory is cleared
     */
    ~GrowingArray()
    {
      freeMemory();
    }

    /**
     * Gets rid of everything in this array
     */
    void clear()
    {
      myEntries = 0;
    }

    /**
     * Changes the size of this array.  Any elements past the new size will now be
     * invalid.  Truncating to a larger size than there are elements in the array
     * is invalid and nothing will happen.
     */
    void truncate(size_t newSize)
    {
      if (newSize < myEntries)
        myEntries = newSize;
    }

    /**
     * Gets rid of the element at the specified index by copying down the element at
     * the end of the list (if it isn't the one being erased).  If the index is
     * beyond the size of the array, nothing happens.  If an element is erased, the
     * array's size decreases by 1.
     */
    void remove(size_t index)
    {
      // Copy down the end element, if necessary
      if (index < myEntries - 1)
        memcpy_s(&myArray[index], sizeof(Element), &myArray[myEntries - 1], sizeof(Element));
      if (index < myEntries)
        myEntries--;
    }

    /**
     * 
     */
    bool add(const Element* element)
    {
      // Make room for this element
      if (!reserve(myEntries + 1)) return false;
      
      // Copy the element
      memcpy(&myArray[myEntries], element, sizeof(Element));
      
      // Increment the number of entries in the array
      myEntries++;
      
      // Success
      return true;
    }

    /**
     * Creates a new element in the array and returns a reference to it
     */
    Element* add()
    {
      // Make sure there is enough space in the list
      if (!reserve(myEntries + 1)) return false;
      
      // Return a reference to this empty element
      return &myArray[myEntries++];
    }

    /**
     * Erases the memory used by this class.  If a call to add() is made
     * after invoking this method, memory will be re-allocated as necessary.
     */
    void freeMemory()
    {
      myEntries = 0;
      myCapacity = 0;
      if (myArray) delete[] myArray;
      myArray = 0;
    }

    /**
     * Makes sure that this class has at least the given capacity
     */
    bool reserve(size_t capacity)
    {
      // If we can hold this many members, we don't have to do anything
      if (myArray && (myCapacity > capacity))
        return true;

      // Calculate the new capacity of the array
      size_t newCapacity = capacity * 2;
      
      // Allocate the new array
      Element* newArray = new Element[newCapacity];
      
      // Make sure allocation succeeded
      if (!newArray) return false;
      
      // Copy old elements and get rid of the old array, if necessary
      if (myArray)
      {
        memcpy(newArray, myArray, sizeof(Element) * myCapacity);

        // Free the old array
        delete[] myArray;
      }
      
      // Save the new array pointer and capacity
      myArray = newArray;
      myCapacity = newCapacity;
      
      // Success
      return true;
    }

    /**
     * Returns the number of elements in this array
     */
    inline size_t size()
    {
      return myEntries;
    }

    /**
     * Obtains the element at the specified index.  This function
     * returns 0 (null) if the element index is invalid
     */
    inline Element* get(size_t index)
    {
      return index >= myEntries ? 0 : &myArray[index];
    }
    
  protected:

    /// Contents held by this class
    Element* myArray;

    /// How many elements are in this array
    size_t myEntries;
    
    /// How large the array is
    size_t myCapacity;
};








class SpriteManager
{
  public:

    /**
     * Controls an element that is to be rendered to the screen
     * @note This class is OPTIMALLY SIZED AT 16 BYTE ALIGNMENT!!!  Currently, it
     *     its size is a multiple of 16 bytes.
     */
    struct Sprite
    {
      /// How to transform the sprite (scale, rotate, translate)
      D3DXMATRIX transform;

      /// The source region of the texture to render
      RECT srcRegion;

      /// The texture with which to render this sprite
      LPDIRECT3DTEXTURE9 texture;

      /// Which group this sprite is a part of.  This is provided for convenience
      /// and has no specific meaning within the SpriteManager class.  It can be
      /// used, for example, to give all of the sprites that represent characters
      /// in a text string the same group ID.
      int group;
      
      /// Color parameter for the sprite
      D3DCOLOR color;

      /// This is a pointer to a position-offset for this sprite.  If this pointer
      /// is valid, then when the sprite is rendered, its screen location will be
      /// adjusted by the value in the referenced memory.
      const D3DXVECTOR3* positionOffset;
    };


  public:

    /**
     * Specifies different layers in which sprites can reside.  As many of these
     * as necessary can be created by adding more labels before LAYER_COUNT.
     * Layers are rendered in increasing numerical order.
     */
    enum Layer
    {
      LAYER_BACKGROUND,
      LAYER_CONTENT,
      LAYER_GUI,
      
      LAYER_COUNT,
    };

  public:

    /**
     * Resets the class
     */
    SpriteManager();

    /**
     * Sets up the sprite manager for use.  This must be called before
     * rendering any sprites.
     */
    bool initialize(LPDIRECT3DDEVICE9 d3dDevice);

    /**
     * Clears D3D objects used by this sprite manager.
     */
    void shutdown();

    /**
     * Removes all of the sprites from this manager
     */
    void clear();

    /**
     * Removes all of the sprites from one of the layers of this manager
     */
    void clearLayer(Layer layer);

    /**
     * Creates a new sprite in the given layer, and returns a pointer to that sprite's memory
     * block.  If creation fails (this only occurs due to lack of memory) then the return
     * value is null.  If a valid sprite is returned, the values it contains are undefined.
     */
    Sprite* addSprite(Layer layer);

    /**
     * Draws all of the sprites in this manager to the screen.  This must be
     * called between d3dDevice->Begin() and d3dDevice->End() blocks on the
     * device that was used to initialize this class.
     */
    bool render();

    /**
     * Erases all of the sprites with the designated group in the specified layer
     */
    void deleteGroup(Layer layer, int group);

    /**
     * Sets the color of all of the sprites that are in the designated group
     */
    void setGroupColor(Layer layer, int group, D3DCOLOR color);

    /**
     * Returns the first sprite in the given group, or null if it doesn't exist
     * TODO: this is used for things like the HP/MP bars which need dynamic updates of their transform...
     *     but there has to be a better way to do this.
     */
    Sprite* getFirstSpriteInGroup(Layer layer, int group);


  /// UNIMPLEMENTED BUT POTENTIALLY USEFUL METHODS  
  public:

    /**
     * Offsets all of the sprites in the given layer by the transformation
     */
    void transformLayer(Layer layer, const D3DXMATRIX* matrix);

    /**
     *
     */
    void fadeSprites(double secondsUntilStart, double entranceDuration, double secondsUntilEnd, double exitDuration);



  protected:

    /// Sprite-rendering object
    LPD3DXSPRITE mySpriteRenderer;

    /// The internal batches of sprites
    GrowingArray<Sprite> myLayers[LAYER_COUNT];
    
    /// Layer transformations
    D3DXMATRIXA16 myLayerOffsets[LAYER_COUNT];
};





#endif
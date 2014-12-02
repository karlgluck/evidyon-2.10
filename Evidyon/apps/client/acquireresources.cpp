//------------------------------------------------------------------------------------------------
// File:  acquiregraphics.cpp
//
// Desc:  Initializes the graphical (window, direct3d) components of PV
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "volucrisguidefs.h"
#include "../shared/client-editor/clientprojectiledescription.h"

#define RESOURCEFILE_MAX_IMAGES         1000
#define RESOURCEFILE_MAX_TEXTURES         2000
#define RESOURCEFILE_MAX_SKINS          500
#define RESOURCEFILE_MAX_MESHES         1000
#define RESOURCEFILE_MAX_SCENERY        4000
#define RESOURCEFILE_MAX_TERRAINMESHES      400
#define RESOURCEFILE_MAX_ATLAS_MAPIMAGE_BYTES   4096 * 4096 * COMPILEDATLAS_MAP_BYTES_PER_PIXEL
#define RESOURCEFILE_MAX_RACES          10
#define RESOURCEFILE_MAX_CLASSES        10
#define RESOURCEFILE_MAX_ABILITIES        256 /* for now */
#define RESOURCEFILE_MAX_SFX          1000
#define RESOURCEFILE_MAX_EFFECTVISUALIZATIONS   3000
#define RESOURCEFILE_MAX_PROJECTILES      32*4
#define RESOURCEFILE_MAX_SPELLS         32*4
#define RESOURCEFILE_MAX_ITEMS          3000

#define RESOURCEFILE_MAX_SKINNEDMESHES      500
#define RESOURCEFILE_MAX_SKINNEDMESHANIMATIONS  1000
#define RESOURCEFILE_MAX_ACTORTYPES       500

#define RESOURCEFILE_MAX_SOUNDS         800


#include "../common/lzf/lzf.h"
#include <dc/buffer>
#include <dc/filestream>

class LZFFileReader {
public:
  bool open(const char* filename);
  void close();
  bool read(void* dest, size_t element_size, size_t elements);
  bool move(int amount);

  dc::dcStreamIn* stream() { return &reader_; }

private:
  dc::dcBuffer buffer_stream_;
  dc::dcBuffer::Reader reader_;
};


bool LZFFileReader::open(const char* filename) {
  dc::dcFileStream file_stream;
  if (APP_ERROR(!file_stream.open(filename, 0))("Couldn't open resource file %s", filename)) return false;
  CONFIRM(buffer_stream_.readFrom(&file_stream, file_stream.size())) else {
    file_stream.close();
    return false;
  }
  file_stream.close();

  // decrypt the buffer--no more than 50% inflation
  size_t temp_buffer_size = buffer_stream_.getDataSize() * 2;
  char* temporary = new char[temp_buffer_size];
  ASSERT(temporary != NULL) else return false;
  size_t decompressed_size =
    lzf_decompress(buffer_stream_.getDataPointer(),
                   buffer_stream_.getDataSize(),
                   temporary, temp_buffer_size);

  CONFIRM(buffer_stream_.readFrom(temporary, decompressed_size)) else {
    delete[] temporary;
    return false;
  }

  delete[] temporary;

  reader_.reset(&buffer_stream_);

  return true;
}


void LZFFileReader::close() {
  reader_.reset(NULL);
}


bool LZFFileReader::read(void* dest, size_t element_size, size_t elements) {
  return reader_.read(dest, element_size * elements);
}

bool LZFFileReader::move(int amount) {
  return reader_.move(amount);
}









template <typename T> T* readArraySizeAndAllocate(LZFFileReader& file_reader, size_t* arrayEntries, size_t maxEntries)
{
  // Read the number of entries in this array from the file
  size_t numEntries;
  file_reader.read(&numEntries, sizeof(numEntries), 1);

  // Make sure there are the right number of entries
  if (APP_ERROR(numEntries > maxEntries)("Invalid number of array entries in media array"))
  {
    // There are no entries
    *arrayEntries = 0;

    // Return a null pointer
    return 0;
  }

  // If the size is zero, don't allocate anything
  if (numEntries == 0)
  {
    *arrayEntries = 0;
    return 0;
  }

  // Allocate the array
  T* arrayElements = new T[numEntries];

  // Make sure allocation succeeded
  if (APP_FATAL(!arrayElements)("Unable to allocate media array (%lu bytes)", numEntries * sizeof(T)))
  {
    // There are no entries
    *arrayEntries = 0;

    // Return a null pointer
    return 0;
  }

  // Success; save the number of entries and return the pointer to the allocated array
  *arrayEntries = numEntries;
  return arrayElements;
}


//------------------------------------------------------------------------------------------------
// Name: acquireInput
// Desc: Loads the game file into the resources structure
//------------------------------------------------------------------------------------------------
bool VolucrisClient::acquireResources()
{
  // This class provides a nice, easy interface for getting rid of things that were loaded
  // during this method when an error occurs
  Cleanup cleanup;

  // Open the file
  LZFFileReader file_reader;
  const char* resourceFile = myConfig.getSetting("Media", "MediaFile", STRING_DEFAULT_GAME_FILE);
  if (APP_ERROR(!file_reader.open(resourceFile))("Couldn't open media file %s", resourceFile))
    return false;

  // Make sure this file is the correct type
  DEBUG_INFO("TODO: Make sure the media file is the correct type");
  myResources.fileVersion = 0; // TODO

  //=======================================================================================
  // Images
  //=======================================================================================

  // Read the images
  size_t numImages;
  myResources.images.d3dTextureArray = readArraySizeAndAllocate<LPDIRECT3DTEXTURE9>(file_reader, &numImages, RESOURCEFILE_MAX_IMAGES);

  // Read in the images
  if (numImages)
  {
    // Allocate the image bank
    if (APP_FATAL(0 == myResources.images.d3dTextureArray)("%s - failed to load image array", resourceFile))
      return false;

    // Add this array to the cleanup class
    cleanup.addArray((void**)&myResources.images.d3dTextureArray);

    // Load each of the images in the resource file
    for (size_t img = 0; img < numImages; ++img)
    {
      // Local variables used during the loading process
      size_t imageSize;
      BYTE* image = 0;
      bool readFailed;
      LPDIRECT3DTEXTURE9 d3dTexture = 0;

      // Load this image's byte size
      readFailed = !file_reader.read(&imageSize, sizeof(imageSize), 1);

      // Allocate temporary memory for the image
      readFailed = readFailed | (0 == (image = new BYTE[imageSize]));

      // Load the image data
      readFailed = readFailed | !file_reader.read(image, imageSize, 1);

      // Load the image
      readFailed = readFailed | FAILED(D3DXCreateTextureFromFileInMemory(myD3DDevice, image, (UINT)imageSize, &d3dTexture));

      // Free the source image memory
      SAFE_DELETE_ARRAY(image);

      // Make sure that everything succeeded
      if (APP_WARNING(readFailed)("%s - failed to load image %lu", resourceFile, img))
        myResources.images.d3dTextureArray[img] = 0;
      else
      {
        // Save the texture
        myResources.images.d3dTextureArray[img] = d3dTexture;

        // Add to the cleanup class
        cleanup.addInterface((IUnknown**)&myResources.images.d3dTextureArray[img]);
      }
    }
  }

  // Load the assigned images
  {
    size_t fontIndex, titleLeftIndex, titleRightIndex, guiIndex, symbolIndex, lightParticleIndex;
    if (APP_ERROR(!file_reader.read(&fontIndex,     sizeof(size_t), 1) ||
             !file_reader.read(&titleLeftIndex,  sizeof(size_t), 1) ||
             !file_reader.read(&titleRightIndex, sizeof(size_t), 1) ||
             !file_reader.read(&guiIndex,    sizeof(size_t), 1) ||
             !file_reader.read(&symbolIndex,   sizeof(size_t), 1) ||
             !file_reader.read(&lightParticleIndex,   sizeof(size_t), 1))
        ("%s - couldn't load assigned image indices", resourceFile))
      return false;

    // Sanity check these indices
    APP_WARNING(fontIndex >= numImages || titleLeftIndex >= numImages ||
             titleRightIndex >= numImages || guiIndex >= numImages)
      ("%s - at least one of the assigned image indices were invalid", resourceFile);

    // Assign the images
    myResources.images.d3dTextureFont     = fontIndex     >= numImages ? 0 : myResources.images.d3dTextureArray[fontIndex];
    myResources.images.d3dTextureTitle  = titleLeftIndex  >= numImages ? 0 : myResources.images.d3dTextureArray[titleLeftIndex];
    myResources.images.d3dTextureTitleBackground = titleRightIndex >= numImages ? 0 : myResources.images.d3dTextureArray[titleRightIndex];
    myResources.images.d3dTextureGUI    = guiIndex    >= numImages ? 0 : myResources.images.d3dTextureArray[guiIndex];
    myResources.images.d3dTextureEvidyonSymbol  = symbolIndex >= numImages ? 0 : myResources.images.d3dTextureArray[symbolIndex];
    myResources.images.d3dTextureDayNightLightParticle = lightParticleIndex >= numImages ? 0 : myResources.images.d3dTextureArray[lightParticleIndex];

    // Add to the reference count of each of the images, if they exist
    if (myResources.images.d3dTextureFont)     myResources.images.d3dTextureFont->AddRef();
    if (myResources.images.d3dTextureTitle)  myResources.images.d3dTextureTitle->AddRef();
    if (myResources.images.d3dTextureTitleBackground) myResources.images.d3dTextureTitleBackground->AddRef();
    if (myResources.images.d3dTextureGUI)    myResources.images.d3dTextureGUI->AddRef();
    if (myResources.images.d3dTextureEvidyonSymbol)    myResources.images.d3dTextureEvidyonSymbol->AddRef();
    if (myResources.images.d3dTextureDayNightLightParticle)    myResources.images.d3dTextureDayNightLightParticle->AddRef();

    // Add images to the cleanup
    cleanup.addInterface((IUnknown**)&myResources.images.d3dTextureFont);
    cleanup.addInterface((IUnknown**)&myResources.images.d3dTextureTitle);
    cleanup.addInterface((IUnknown**)&myResources.images.d3dTextureTitleBackground);
    cleanup.addInterface((IUnknown**)&myResources.images.d3dTextureGUI);
    cleanup.addInterface((IUnknown**)&myResources.images.d3dTextureEvidyonSymbol);
    cleanup.addInterface((IUnknown**)&myResources.images.d3dTextureDayNightLightParticle);
  }

  // Save the array size
  myResources.images.d3dTextureArraySize = numImages;


  //=======================================================================================
  // Textures
  //=======================================================================================
  {
    size_t types;
    file_reader.read(&types, sizeof(types), 1);

    global_rendering_manager_.create(myD3DDevice);

    for (size_t type=0; type < types; ++type) {

      // This data is referenced by the textures
      size_t image_index;
      DWORD flags;
      float params[2];

      // Read texture data
      CONFIRM(file_reader.read(&image_index, sizeof(image_index), 1) &&
          file_reader.read(&flags, sizeof(flags), 1) &&
          file_reader.read(params, sizeof(float), 2)) else return false;

      // Build a description in the game
      TextureDescription description;
      description.alpha_blend = ((flags & 1) != 0) ? true : false;
      description.alpha_test = ((flags & 2) != 0) ? true : false;
      if (flags & 64) {
        description.type = TextureDescription::ANIMATED;
        description.animated_flags.frames_high = (flags&0xFF000000)>>24;
        description.animated_flags.frames_wide = (flags&0x00FF0000)>>16;
        description.animated_flags.speed = params[1];
      } else if (flags & 32) {
        description.type = TextureDescription::SLIDING;
        description.sliding_flags.dx = params[1] * cosf(params[2]);
        description.sliding_flags.dy = params[1] * sinf(params[2]);
      } else if (flags & 16) {
        description.type = TextureDescription::CIRCLING;
        description.circling_flags.radius = params[1];
        description.circling_flags.speed = params[0] * D3DX_PI * 2;
      } else {
        description.type = TextureDescription::STATIC;
      }

      if (image_index < myResources.images.d3dTextureArraySize) {
        description.texture = myResources.images.d3dTextureArray[image_index];
        description.texture->AddRef();
      } else {
        description.texture = NULL;
      }

      description.flip_horizontal = ((flags & 8) != 0) ? true : false;
      description.flip_vertical = ((flags & 4) != 0) ? true : false;


      global_rendering_manager_.addTexture(type, description);
    }
  }


  //=======================================================================================
  // Mesh Skins
  //=======================================================================================

  // Create the array for skins
  size_t numSkins;
  myResources.skins.skinArray = readArraySizeAndAllocate<GameFileData_Skin>(file_reader, &numSkins, RESOURCEFILE_MAX_SKINS);

  // Read in the textures array
  if (numSkins)
  {
    // Add this array to the cleanup structure
    cleanup.addArray((void**)&myResources.skins.skinArray);

    // Read each of the skins
    for (size_t i = 0; i < numSkins; ++i)
    {
      // Get the number of entries in this skin
      size_t skinEntries;
      if (APP_ERROR(!file_reader.read(&skinEntries, sizeof(size_t), 1))("Failed reading skin entry %lu", i)) return false;

      // The skin into which we are reading elements
      GameFileData_Skin* destSkin = &myResources.skins.skinArray[i];

      // Read in each element
      for (size_t e = 0; e < skinEntries; ++e)
      {
        // Read the data
        // TODO: optimize this into 1 read
        size_t subset, texture;
        if (APP_ERROR(!file_reader.read(&subset, sizeof(subset), 1) ||
                 !file_reader.read(&texture, sizeof(texture), 1))("Failed reading skin %lu subset %lu", i, e)) return false;

        // Assign this subset texture
        destSkin->insert(GameFileData_Skin::value_type(subset, texture));
        /*
        destSkin->insert(
          GameFileData_Skin::value_type(
            subset,
            (texture < myResources.textures.textureArraySize)
              ? &myResources.textures.textureArray[texture] : 0));*/
      }
    }
  }

  // Save the number of skins
  myResources.skins.skinArraySize = numSkins;


  //=======================================================================================
  // Meshes
  //=======================================================================================

  size_t meshArraySize;
  myResources.meshes.meshArray = readArraySizeAndAllocate<GameFileData_Mesh*>(file_reader, &meshArraySize, RESOURCEFILE_MAX_MESHES);

  // Read in the meshes array
  // TODO: unify this loading with the loading for the terrain meshes...it's exactly the same
  if (meshArraySize)
  {
    // Make sure the array was allocated
    if (APP_ERROR(!myResources.meshes.meshArray)("Couldn't allocate mesh geometry array"))
      return false;

    // Add to the cleanup variable
    cleanup.addArray((void**)&myResources.meshes.meshArray);

    // Read each entry in the set of meshes
    for (size_t i = 0; i < meshArraySize; ++i)
    {
      // Read the number of subsets
      SubsetIndex subsets;
      if (APP_ERROR(!file_reader.read(&subsets, sizeof(subsets), 1))("Failed reading mesh %lu subset count", i))
        return false;

      // Allocate this entry
      GameFileData_Mesh* mesh = new GameFileData_Mesh();
      if (APP_FATAL(!mesh)("Out of memory")) return false;

      // Set the corresponding array entry, and add memory to the cleanup structure
      myResources.meshes.meshArray[i] = mesh;
      cleanup.addObject((void**)(&myResources.meshes.meshArray[i]));

      // Scan in each subset
      for (size_t subset = 0; subset < subsets; ++subset)
      {
        // Read the subset index and vertex/index counts
        SubsetIndex subsetIndex;
        DWORD numVertices, numIndices;
        if (APP_ERROR(!file_reader.read(&subsetIndex, sizeof(subsetIndex), 1) ||
                 !file_reader.read(&numVertices, sizeof(numVertices), 1) ||
                 !file_reader.read(&numIndices, sizeof(numIndices), 1))("Failed reading mesh %lu subset %lu data", i, subset))
          return false;

        // Allocate geometry for this subset
        Geometry* geometry;
        if (APP_ERROR(FAILED(AllocateGeometry(numVertices, numIndices, &geometry)))
          ("Unable to allocate %lu vertices/%lu indices for mesh %lu subset %lu", numVertices, numIndices, i, subset))
          return false;

        // Add this geometry to the cleanup buffer
        // TODO: make this not assume anything about the index/vertex array
        cleanup.addArray((void**)&geometry->pVertices);
        cleanup.addArray((void**)&geometry->pIndices);

        // Read the vertices
        if (numVertices > 0)
        {
          // Read the array of vertices
           if (APP_ERROR(!file_reader.read(geometry->pVertices, sizeof(GeometryVertex), numVertices))("Couldn't read vertices for mesh %lu subset %lu", i, subset))
             return false;
        }
        else
          DEBUG_WARNING("Mesh %lu subset %lu has no vertices", i, subset);

        // Read the indices
        if (numIndices > 0)
        {
          // Read the array of indices
           if (APP_ERROR(!file_reader.read(geometry->pIndices, sizeof(GeometryIndex), numIndices))("Couldn't read indices for mesh %lu subset %lu", i, subset))
             return false;
        }
        else
          DEBUG_WARNING("Mesh %lu subset %lu has no indices", i, subset);

        // Add this subset to the mesh's geometry
        mesh->insert(GameFileData_Mesh::value_type(subset, geometry));
      }
    }
  }

  // Save the array size
  myResources.meshes.meshArraySize = meshArraySize;


  //=======================================================================================
  // Scenery
  //=======================================================================================

  size_t numScenery;
  myResources.scenery.sceneryArray = readArraySizeAndAllocate<GameFileData_Scenery>(file_reader, &numScenery, RESOURCEFILE_MAX_SCENERY);

  // Read in the textures array
  if (numScenery)
  {
    // Add this array to the cleanup structure
    cleanup.addArray((void**)&myResources.scenery.sceneryArray);

    // Read each of the scenery elements
    for (size_t i = 0; i < numScenery; ++i)
    {
      size_t mesh, skin;
      if (APP_ERROR(!file_reader.read(&mesh, sizeof(mesh), 1) ||
               !file_reader.read(&skin, sizeof(skin), 1) ||
               !file_reader.read(&myResources.scenery.sceneryArray[i].transform, sizeof(D3DXMATRIX), 1))
           ("Couldn't read scenery element %lu", i))
         return false;

      // Assign the mesh/skin
      myResources.scenery.sceneryArray[i].mesh = mesh < myResources.meshes.meshArraySize ? myResources.meshes.meshArray[mesh] : 0;
      myResources.scenery.sceneryArray[i].skin = skin < myResources.skins.skinArraySize  ? &myResources.skins.skinArray[skin] : 0;
    }
  }

  // Save the number of skins
  myResources.scenery.sceneryArraySize = numScenery;
  mySceneryRenderer.setSceneryArray(myResources.scenery.sceneryArray, numScenery);


  //=======================================================================================
  // Races
  //=======================================================================================
  {
    size_t numRaces;
    myResources.avatars.raceArray = readArraySizeAndAllocate<GameFileData::AvatarRace>(file_reader, &numRaces, RESOURCEFILE_MAX_RACES);

    // Make sure the array was allocated
    if (APP_ERROR(!myResources.avatars.raceArray)("Couldn't allocate race data array"))
      return false;

    // Add to the cleanup variable
    cleanup.addArray((void**)&myResources.avatars.raceArray);

    // Read each element
    for (size_t i = 0; i < numRaces; ++i)
    {
      // Read the name
      char name[CLIENTGAMEFILE_RACE_STRING_LENGTH];
      char desc[CLIENTGAMEFILE_RACE_DESCRIPTION_STRING_LENGTH];
      if (APP_ERROR(!file_reader.read(name, CLIENTGAMEFILE_RACE_STRING_LENGTH * sizeof(char), 1) ||
                    !file_reader.read(desc, CLIENTGAMEFILE_RACE_DESCRIPTION_STRING_LENGTH*sizeof(char), 1))("Failed reading race"))
        return false;

      // Assure string safety
      name[CLIENTGAMEFILE_RACE_STRING_LENGTH-1] = '\0';
      desc[CLIENTGAMEFILE_RACE_DESCRIPTION_STRING_LENGTH-1] = '\0';

      // Assign the name
      myResources.avatars.raceArray[i].name = name;
      myResources.avatars.raceArray[i].description = desc;

      // Read maxima
      for (int m = 0; m < 5; ++m)
      {
        // Read this value
        unsigned char b;
        if (!file_reader.read(&b, sizeof(unsigned char), 1)) return false;

        // Store the value
        myResources.avatars.raceArray[i].max_stat[m] = (int)b;
      }
    }

    // Save the array size
    myResources.avatars.raceArraySize = numRaces;
  }

  //=======================================================================================
  // Classes
  //=======================================================================================
  {
    size_t numClasses;
    myResources.avatars.classArray = readArraySizeAndAllocate<GameFileData::AvatarClass>(file_reader, &numClasses, RESOURCEFILE_MAX_CLASSES);

    // Make sure the array was allocated
    if (APP_ERROR(!myResources.avatars.classArray)("Couldn't allocate class data array"))
      return false;

    // Add to the cleanup variable
    cleanup.addArray((void**)&myResources.avatars.classArray);

    // Read each element
    for (size_t i = 0; i < numClasses; ++i)
    {
      // Read the name
      char name[CLIENTGAMEFILE_CLASS_STRING_LENGTH];
      char desc[CLIENTGAMEFILE_CLASS_DESCRIPTION_STRING_LENGTH];
      if (APP_ERROR(!file_reader.read(name, CLIENTGAMEFILE_CLASS_STRING_LENGTH * sizeof(char), 1) ||
                    !file_reader.read(desc, CLIENTGAMEFILE_CLASS_DESCRIPTION_STRING_LENGTH * sizeof(char), 1))("Failed reading class"))
        return false;

      // Assure string safety
      name[CLIENTGAMEFILE_CLASS_STRING_LENGTH-1] = '\0';
      desc[CLIENTGAMEFILE_CLASS_DESCRIPTION_STRING_LENGTH-1] = '\0';

      // Assign the name
      myResources.avatars.classArray[i].name = name;
      myResources.avatars.classArray[i].description = desc;
    }

    // Save the array size
    myResources.avatars.classArraySize = numClasses;
  }

  //=======================================================================================
  // Sounds
  //=======================================================================================
  if (myConfig.getSetting("Audio", "Enable", 1))
  {/*
    size_t numSounds;
    myResources.sounds.soundArray = readArraySizeAndAllocate<ClientSound>(file_reader, &numSounds, RESOURCEFILE_MAX_SOUNDS);
    myResources.sounds.soundArraySize = numSounds;

    for (size_t i = 0; i < numSounds; ++i)
    {
      // Load the sound header
      file_reader.read(&myResources.sounds.soundArray[i].header, sizeof(ClientSound::Header), 1);

      // Allocate the header and read sound data into it
      myResources.sounds.soundArray[i].waveformData = new BYTE[myResources.sounds.soundArray[i].header.waveformSize];
      file_reader.read(myResources.sounds.soundArray[i].waveformData, sizeof(BYTE), myResources.sounds.soundArray[i].header.waveformSize);
    }*/

    size_t num_sounds;
    CONFIRM(file_reader.read(&num_sounds, sizeof(num_sounds), 1));
    for (size_t i = 0; i < num_sounds; ++i) {
      ClientSound client_sound;

      file_reader.read(&client_sound.header, sizeof(ClientSound::Header), 1);
      client_sound.waveformData = new BYTE[client_sound.header.waveformSize];
      file_reader.read(client_sound.waveformData, sizeof(BYTE), client_sound.header.waveformSize);

      CONFIRM(global_sound_manager_.addSound(i, client_sound));
    }

    // Read the defined sounds:
    // 0 - footstep
    // 1 - button
    size_t defined_sounds[2];
    file_reader.read(defined_sounds, sizeof(size_t), 2);
    global_actor_manager_.assignSoundIndices(defined_sounds[0]);
  }
  else
  {
    // Seek past all of the sounds
    size_t numSounds;
    file_reader.read(&numSounds, sizeof(numSounds), 1);
    for (size_t i = 0; i < numSounds; ++i)
    {
      ClientSound::Header soundHeader;
      file_reader.read(&soundHeader, sizeof(soundHeader), 1);
      file_reader.move(soundHeader.waveformSize);
    }
    file_reader.move(sizeof(size_t) * 2);
  }

  //=======================================================================================
  // Projectiles
  //=======================================================================================
  {
    size_t number = 0;
    file_reader.read(&number, sizeof(number), 1);
    for (size_t i = 0; i < number; ++i) {
      ClientProjectileDescription description;
      file_reader.read(&description, sizeof(description), 1);
      special_fx_manager_.addProjectileFXType(i, &description);
    }
  }

  //=======================================================================================
  // Enchantments
  //=======================================================================================
  {
    { // classes
      size_t number = 0;
      file_reader.read(&number, sizeof(number), 1);
      for (size_t i = 0; i < number; ++i) {
        Evidyon::ClientEnchantmentClassDescription description;
        file_reader.read(&description, sizeof(description), 1);
        special_fx_manager_.addEnchantmentClass(i, &description);
      }
    }
    { // enchantment types
      size_t number = 0;
      file_reader.read(&number, sizeof(number), 1);
      for (size_t i = 0; i < number; ++i) {
        Evidyon::ClientEnchantmentDescription description;
        file_reader.read(&description, sizeof(description), 1);
        special_fx_manager_.addEnchantmentFXType(i, &description);
      }
    }
  }


  //=======================================================================================
  // Spells
  //=======================================================================================
  {
    size_t number_of_spells;
    file_reader.read(&number_of_spells, sizeof(number_of_spells), 1);

    if (number_of_spells > RESOURCEFILE_MAX_SPELLS) return false;

    myResources.spells.spell_descriptions_ =
    new Evidyon::ClientSpellDescription[number_of_spells];
    myResources.spells.number_of_spell_descriptions = number_of_spells;

    file_reader.read(myResources.spells.spell_descriptions_,
      sizeof(Evidyon::ClientSpellDescription),
      number_of_spells);
  }


  //=======================================================================================
  // Special FX
  //=======================================================================================
  CONFIRM(special_fx_manager_.create(myD3DDevice, &global_sound_manager_)) else return false;
  {
    { // images
      size_t images = 0;
      file_reader.read(&images, sizeof(images), 1);
      for (size_t i = 0; i < images; ++i) {
        CompiledVisualFXImage image;
        file_reader.read(&image, sizeof(image), 1);
        LPDIRECT3DTEXTURE9 texture = image.image_index > 0 && image.image_index < myResources.images.d3dTextureArraySize ?
        myResources.images.d3dTextureArray[image.image_index] : NULL;
        special_fx_manager_.addVisualFXImage(i, texture);
      }
    }
    {
      size_t magic = 0;
      file_reader.read(&magic, sizeof(magic), 1);
      for (size_t i = 0; i < magic; ++i) {
        CompiledMagicFXParams magic;
        file_reader.read(&magic, sizeof(magic), 1);
        special_fx_manager_.addMagicFXType(i, &magic);
      }
    }
    {
      size_t visual = 0;
      file_reader.read(&visual, sizeof(visual), 1);
      for (size_t i = 0; i < visual; ++i) {
        CompiledVisualFXParams params;
        file_reader.read(&params, sizeof(params), 1);
        special_fx_manager_.addVisualFXType(i, &params);
      }
    }

    size_t physical_hit_magic_fx;
    file_reader.read(&physical_hit_magic_fx, sizeof(physical_hit_magic_fx), 1);
    global_actor_manager_.assignFXIndices(physical_hit_magic_fx);
  }


  //=======================================================================================
  // Skinned Meshes
  //=======================================================================================
  {
    size_t count;
    myResources.animatedMeshes.skinnedMeshes =
      readArraySizeAndAllocate<GameFileDataSkinnedMesh>(file_reader, &count, RESOURCEFILE_MAX_SKINNEDMESHES);

    if (count > 0)
    {
      // Make sure the array was allocated
      if (APP_ERROR(!myResources.animatedMeshes.skinnedMeshes)("Couldn't allocate skinned meshes array"))
        return false;

      // Add to the cleanup variable
      cleanup.addArray((void**)&myResources.animatedMeshes.skinnedMeshes);

      // Make ure we get the the hardware-mode flag
      if (myUsingMixedDeviceFlag) myD3DDevice->SetSoftwareVertexProcessing(FALSE);
      D3DCAPS9 caps;
      myD3DDevice->GetDeviceCaps(&caps);
      DWORD maxHardwareVertexInfluences = caps.MaxVertexBlendMatrices;
      DWORD maxHardwareBlendMatrixIndex = caps.MaxVertexBlendMatrixIndex;
      DEBUG_INFO("Max vetex blend matrices:  %lu", maxHardwareVertexInfluences );
      DEBUG_INFO("Max vertex blend matrix index:  %lu", maxHardwareBlendMatrixIndex);

      // Read the elements
      for (size_t index = 0; index < count; ++index) {
        // Get the target mesh
        GameFileDataSkinnedMesh* skinnedMesh = &myResources.animatedMeshes.skinnedMeshes[index];

        // Read info from source file
        UnseenSkinnedMeshInfo info;
        file_reader.read(&info, sizeof(info), 1);
        skinnedMesh->numberOfBones = info.numberOfBones;
        skinnedMesh->numberOfTextureGroups = info.numberOfTextureGroups;
        skinnedMesh->numberOfFrames = info.numberOfFrames;
        skinnedMesh->skeleton = new GameFileDataSkinnedMesh::Bone[info.numberOfBones];
        skinnedMesh->frameHierarchy = new GameFileDataSkinnedMesh::Frame[info.numberOfFrames];
        skinnedMesh->textureGroups = new GameFileDataSkinnedMesh::TextureGroup[info.numberOfTextureGroups];

        // Add arrays to the cleanup structure
        cleanup.addArray((void**)&skinnedMesh->skeleton);
        cleanup.addArray((void**)&skinnedMesh->frameHierarchy);
        cleanup.addArray((void**)&skinnedMesh->textureGroups);


        // Read bones from the file
        for (DWORD i = 0; i < info.numberOfBones; ++i)
        {
          UnseenSkinnedMeshBoneInfo bone;
          file_reader.read(&bone, sizeof(bone), 1);
          skinnedMesh->skeleton[i].inverseOffset = bone.inverseOffset;
          skinnedMesh->skeleton[i].frameIndex = bone.frameIndex;
        }

        // Read frames from the file
        for (DWORD i = 0; i < info.numberOfFrames; ++i)
        {
          UnseenSkinnedMeshFrameInfo frame;
          file_reader.read(&frame, sizeof(frame), 1);
          skinnedMesh->frameHierarchy[i].offset = frame.offset;
          skinnedMesh->frameHierarchy[i].parentFrameIndex = frame.parentFrameIndex;
        }


        // Read texture groups
        for (DWORD i = 0; i < info.numberOfTextureGroups; ++i)
        {
          GameFileDataSkinnedMesh::TextureGroup* textureGroup = &skinnedMesh->textureGroups[i];

          // Read this group information from the file
          UnseenSkinnedMeshTextureGroupInfo textureGroupInfo;
          CONFIRM(file_reader.read(&textureGroupInfo, sizeof(textureGroupInfo), 1)) else return false;

          // Allocate the influence groups
          textureGroup->blendingGroups = new GameFileDataSkinnedMesh::TextureGroup::BlendingGroup[textureGroupInfo.numberOfVertexInfluenceGroups];
          cleanup.addArray((void**)&textureGroup->blendingGroups);

          for (DWORD influenceGroup = 0; influenceGroup < textureGroupInfo.numberOfVertexInfluenceGroups; ++influenceGroup)
          {
            GameFileDataSkinnedMesh::TextureGroup::BlendingGroup* blendingGroup = &textureGroup->blendingGroups[influenceGroup];

            // Read the influence group info
            UnseenSkinnedMeshVertexInfluenceGroupInfo influenceGroupInfo;
            CONFIRM(file_reader.read(&influenceGroupInfo, sizeof(influenceGroupInfo), 1)) else return false;

            // If we can't blend all of these matrices in hardware, set the software flag
            blendingGroup->renderInSoftware = myUsingSoftwareDeviceFlag ||
                              (info.numberOfBones > maxHardwareBlendMatrixIndex + 1) ||
                              (influenceGroupInfo.numberOfVertexInfluences > maxHardwareVertexInfluences);
            //DEBUG_INFO("Rendering mesh %lu / %lu   >>  texture group %lu / %lu  >> in %s", index, count, i, info.numberOfTextureGroups, blendingGroup->renderInSoftware ? "software" : "hardware");

            // Fill in the rendering parameters
            blendingGroup->fvf = influenceGroupInfo.d3dVertexFormat;
            blendingGroup->vertexStride = influenceGroupInfo.vertexSizeInBytes;
            switch(influenceGroupInfo.numberOfVertexInfluences)
            {
              case 0:   blendingGroup->blendFlags = D3DVBF_DISABLE; break;
              case 1:   blendingGroup->blendFlags = D3DVBF_0WEIGHTS; break;
              default:  blendingGroup->blendFlags = (D3DVERTEXBLENDFLAGS)(influenceGroupInfo.numberOfVertexInfluences-1); break;
            }

            // Render this whole group
            // TODO: remove the rendering info from the UnseenSkinnedMesh format, since it's entirely useless
            UnseenSkinnedMeshBoneInfluenceInfo renderingInfo;
            CONFIRM(file_reader.read(&renderingInfo, sizeof(UnseenSkinnedMeshBoneInfluenceInfo), 1)) else return false;
            //CONFIRM(0 == fseek(file_reader, sizeof(UnseenSkinnedMeshBoneInfluenceInfo) * (info.numberOfBones-1), SEEK_CUR)) else return false;
            CONFIRM(file_reader.move(sizeof(UnseenSkinnedMeshBoneInfluenceInfo) * (info.numberOfBones-1))) else return false;

            blendingGroup->triangleStartIndex = renderingInfo.thisAndHigher.triangleStartIndex;
            blendingGroup->numberOfTriangles = renderingInfo.thisAndHigher.numberOfTriangles;
            blendingGroup->lowestReferencedVertexIndex = renderingInfo.thisAndHigher.lowestReferencedVertexIndex;
            blendingGroup->numberOfVertices = renderingInfo.thisAndHigher.numberOfVertices;
            //DEBUG_INFO("%s: %lu %lu %u %lu",blendingGroup->renderInSoftware?"true":"false",blendingGroup->triangleStartIndex,blendingGroup->numberOfTriangles,blendingGroup->lowestReferencedVertexIndex,blendingGroup->numberOfVertices);

            // Create the vertex buffer
            if (APP_ERROR(FAILED(myD3DDevice->CreateVertexBuffer(influenceGroupInfo.vertexSizeInBytes * influenceGroupInfo.numberOfVertices,
              D3DUSAGE_WRITEONLY|(blendingGroup->renderInSoftware  ? D3DUSAGE_SOFTWAREPROCESSING : 0), influenceGroupInfo.d3dVertexFormat, D3DPOOL_DEFAULT, &blendingGroup->vertexBuffer, NULL)))("Couldn't create vertex buffer"))
              return false;

            // Add to cleanup
            cleanup.addInterface((IUnknown**)&blendingGroup->vertexBuffer);

            // Read the vertices
            {
              void* vertices;
              if (APP_ERROR(FAILED(blendingGroup->vertexBuffer->Lock(0, 0, &vertices, 0)))("Couldn't lock blending group VB"))
                return false;
              CONFIRM(file_reader.read(vertices, influenceGroupInfo.vertexSizeInBytes, influenceGroupInfo.numberOfVertices))
                else return false;
              blendingGroup->vertexBuffer->Unlock();
            }

            // Create the index buffer
            DWORD numberOfIndices = influenceGroupInfo.numberOfTriangles * 3;
            if (APP_ERROR(FAILED(myD3DDevice->CreateIndexBuffer(influenceGroupInfo.indexSizeInBytes * numberOfIndices,
              D3DUSAGE_WRITEONLY, (D3DFORMAT)influenceGroupInfo.d3dIndexFormat, D3DPOOL_DEFAULT, &blendingGroup->indexBuffer, NULL)))("Couldn't create index buffer"))
              return false;

            // Read the indices
            {
              void* indices;
              if (APP_ERROR(FAILED(blendingGroup->indexBuffer->Lock(0, 0, &indices, 0)))("Couldn't lock IB"))
                return false;
              CONFIRM(file_reader.read(indices, influenceGroupInfo.indexSizeInBytes, numberOfIndices))
                else return false;
              blendingGroup->indexBuffer->Unlock();
            }
          }

          textureGroup->numberOfBlendingGroups = textureGroupInfo.numberOfVertexInfluenceGroups;
        }
      }
    }

    // Save the array size
    myResources.animatedMeshes.numberOfSkinnedMeshes = count;
  }



  //=======================================================================================
  // Skinned Mesh Animations
  //=======================================================================================
  {
    size_t count;
    myResources.animatedMeshes.animations =
      readArraySizeAndAllocate<UnseenSkeletalAnimation>(file_reader, &count, RESOURCEFILE_MAX_SKINNEDMESHANIMATIONS);

    if (count > 0)
    {
      // Make sure the array was allocated
      if (APP_ERROR(!myResources.animatedMeshes.animations)("Couldn't allocate skinned mesh animation array"))
        return false;

      // Add to the cleanup variable
      cleanup.addArray((void**)&myResources.animatedMeshes.animations);

      // Load each animation
      for (size_t i = 0; i < count; ++i)
      {
        UnseenSkeletalAnimation* animation = &myResources.animatedMeshes.animations[i];

        file_reader.read(&animation->info, sizeof(UnseenSkeletalAnimationInfo), 1);
        animation->frameIndexToAnimatedFrameTransformTable = new DWORD[animation->info.numberOfSkeletalFrames];
        animation->keyframes = new UnseenSkeletalAnimation::Keyframe[animation->info.numberOfKeyframes];

        // Add to the cleanup variable
        cleanup.addArray((void**)&animation->frameIndexToAnimatedFrameTransformTable);
        cleanup.addArray((void**)&animation->keyframes);

        file_reader.read(animation->frameIndexToAnimatedFrameTransformTable, sizeof(DWORD), animation->info.numberOfSkeletalFrames);

        for (DWORD keyframeIndex = 0; keyframeIndex < animation->info.numberOfKeyframes; ++keyframeIndex)
        {
          UnseenSkeletalAnimation::Keyframe* keyframe = &animation->keyframes[keyframeIndex];
          keyframe->animatedFrameTransforms = new D3DXMATRIX[animation->info.numberOfAnimatedFrames];

          cleanup.addArray((void**)&keyframe->animatedFrameTransforms);

          file_reader.read(&keyframe->tick, sizeof(DWORD), 1);
          file_reader.read(keyframe->animatedFrameTransforms, sizeof(D3DXMATRIX), animation->info.numberOfAnimatedFrames);
        }
      }
    }

    // Save the array size
    myResources.animatedMeshes.numberOfAnimations = count;
  }



  //=======================================================================================
  // Actor Types
  //=======================================================================================
  {
    size_t types;
    file_reader.read(&types, sizeof(types), 1);

    global_actor_manager_.create(&mySceneryRenderer, &global_sound_manager_, &special_fx_manager_);

    for (size_t type=0; type<types; ++type) {

      // Get the description stored in the source file
      ClientActorTypeDescription game_file_description;
      file_reader.read(&game_file_description, sizeof(game_file_description), 1);

      // Build a description in the game
      ActorTypeDescription description;

      { // Find the actor's mesh type
        size_t skinned_mesh_index = game_file_description.skinnedMeshIndex;
        description.mesh =
        skinned_mesh_index < myResources.animatedMeshes.numberOfSkinnedMeshes ?
          &myResources.animatedMeshes.skinnedMeshes[skinned_mesh_index] : NULL;
      }
      { // Load all animations
        for (int i = 0; i < (int)(ACTORANIMATION_COUNT); ++i) {
          size_t animation_index = game_file_description.actionAnimationIndices[i];
          description.animations[i].animation =
            animation_index < myResources.animatedMeshes.numberOfAnimations ?
            &myResources.animatedMeshes.animations[animation_index] : NULL;
          description.animations[i].speed_factor =
            game_file_description.actionAnimationSpeedFactors[i];
          description.animations[i].speed_offset =
            game_file_description.actionAnimationSpeedOffsets[i];
        }
      }
      { // set attachment points
        for (int i = 0; i < (int)(ACTORATTACHMENTPOINT_COUNT); ++i) {
          memcpy(&description.attachment_points[i].offset_matrix,
                 &game_file_description.attachmentPointTransforms[i],
                 sizeof(D3DXMATRIX));
          description.attachment_points[i].source_bone =
              game_file_description.attachmentPointBoneMatrices[i];
        }
      }
      { // Copy other data
        memcpy(&description.root_transform,
           &game_file_description.rootTransform,
            sizeof(D3DXMATRIX));
        description.texture_id = game_file_description.skinTexture;
      }
      { // copy sounds
        memcpy(description.sounds,
               game_file_description.soundIndices,
               sizeof(unsigned int) * Evidyon::ACTORSOUND_COUNT);
      }

      // Add this actor type
      global_actor_manager_.addActorType(type, description);
    }
  }


  //=======================================================================================
  // Items
  //=======================================================================================
  {
    size_t numItems;
    myResources.items.itemDescriptionArray =
      readArraySizeAndAllocate<ClientItemDescription>(file_reader, &numItems, RESOURCEFILE_MAX_ITEMS);

    if (numItems > 0) {
      // Make sure the array was allocated
      if (APP_ERROR(!myResources.items.itemDescriptionArray)("Couldn't allocate items array"))
        return false;

      // Add to the cleanup variable
      cleanup.addArray((void**)&myResources.items.itemDescriptionArray);

      // Read the elements
      if (APP_ERROR(!file_reader.read(myResources.items.itemDescriptionArray,
              sizeof(ClientItemDescription),
              numItems))
          ("Couldn't read items"))
        return false;
    }

    // Save the array size
    myResources.items.itemDescriptionArraySize = numItems;
  }

  //=======================================================================================
  // Actor Description Template Types
  //=======================================================================================
  {
    size_t number;
    file_reader.read(&number, sizeof(number), 1);
    for (size_t i = 0; i < number; ++i) {
      ClientActorDescriptionTemplate description_template;
      file_reader.read(&description_template,
            sizeof(description_template),
            1);
      CONFIRM(global_actor_manager_.addActorDescriptionTemplate(i, description_template)) else return false;
    }
  }
  


  //=======================================================================================
  // Maps
  //=======================================================================================
  if (!global_map_manager_.createFromGameFile(myD3DDevice, &myResources, file_reader.stream(), &global_rendering_manager_)) {
    global_map_manager_.destroy();
    return false;
  }

  { // Init the main font from the bitmap font
    DWORD character_indices[128-32];
    character_indices[0] = 8*11+6; // ' '
    for (int i = 33; i < 128; ++i) {
    character_indices[i-32] = i-33;
    }
    if (!myWindowFont.create(myResources.images.d3dTextureFont, 16, 16, character_indices, 1.2f, 0))
    return false;
    gui_primary_layer_.setTexture(myWindowFont.getTextureID(), myWindowFont.getTexture());
  }

  // The primary GUI rendering layer draws to the fullscreen viewport
  if (APP_ERROR(!gui_primary_layer_.create(myD3DDevice, 3000, 1000))("Couldn't create primary GUI renderer")) {
    myWindowFont.destroy();
    return false;
  }
  gui_primary_layer_.setFullscreenViewport(0.0f, 1.0f);

  gui_primary_layer_.setTexture(VolucrisGUIDefs::GUI_TEXTURE_INDEX, myResources.images.d3dTextureGUI);

  { // Set up the mouse cursor
    mouse_position_canvas_.absoluteDepth(0.0f);
    mouse_position_canvas_.setSourceCanvas(gui_primary_layer_.getPrimaryCanvas());
    mouse_cursor_canvas_.setSourceCanvas(&mouse_position_canvas_);
    mouse_cursor_canvas_.absoluteWidth(VolucrisGUIDefs::MOUSE_CURSOR_NORMAL.calculateSize().width);
    mouse_cursor_canvas_.absoluteHeight(VolucrisGUIDefs::MOUSE_CURSOR_NORMAL.calculateSize().height);
    mouse_cursor_canvas_.absoluteDepth(0.0f);
    mouse_cursor_canvas_.alignXLeft(-2);
    mouse_cursor_canvas_.alignYTop(-3);
    mouse_cursor_.setSourceCanvas(&mouse_cursor_canvas_);
    mouse_cursor_.setTexture(VolucrisGUIDefs::GUI_TEXTURE_INDEX);
    setMouseCursorNormal();
    gui_primary_layer_.registerElement(&mouse_cursor_);

    SetCursor(NULL);
    ShowCursor(FALSE);
  }



  // Get rid of everything in the cleanup structure
  cleanup.clear();

  // Close the file
  //fclose(pf);
  file_reader.close();

  // Success
  return true;
}



//------------------------------------------------------------------------------------------------
// Name: acquireInput
// Desc: Unloads everything in the resources structure
//------------------------------------------------------------------------------------------------
void VolucrisClient::freeResources()
{
  special_fx_manager_.destroy();
  global_rendering_manager_.destroy();
  global_map_manager_.destroy();

  gui_primary_layer_.clearAllTextures();
  gui_primary_layer_.destroy();
  myWindowFont.destroy();

  SAFE_DELETE_ARRAY(myResources.spells.spell_descriptions_);
  myResources.spells.number_of_spell_descriptions = 0;

  ////// Free sounds
  ////for (size_t i = 0; i < myResources.sounds.soundArraySize; ++i)
  ////  SAFE_DELETE_ARRAY(myResources.sounds.soundArray[i].waveformData);
  ////SAFE_DELETE_ARRAY(myResources.sounds.soundArray);
  ////myResources.sounds.soundArraySize = 0;


  // Free animations
  for (size_t i = 0; i < myResources.animatedMeshes.numberOfAnimations; ++i)
  {
    UnseenSkeletalAnimation* animation = &myResources.animatedMeshes.animations[i];
    for (int j = 0; j < animation->info.numberOfKeyframes; ++j) {
      SAFE_DELETE_ARRAY(animation->keyframes[j].animatedFrameTransforms);
    }
    SAFE_DELETE_ARRAY(animation->frameIndexToAnimatedFrameTransformTable);
    SAFE_DELETE_ARRAY(animation->keyframes);
  }
  SAFE_DELETE_ARRAY(myResources.animatedMeshes.animations);
  myResources.animatedMeshes.numberOfAnimations = 0;

  // Erase animated meshes
  for (size_t i = 0; i < myResources.animatedMeshes.numberOfSkinnedMeshes; ++i)
  {
    GameFileDataSkinnedMesh* skinnedMesh = &myResources.animatedMeshes.skinnedMeshes[i];
    SAFE_DELETE_ARRAY(skinnedMesh->frameHierarchy);
    SAFE_DELETE_ARRAY(skinnedMesh->skeleton);
    for (size_t j = 0; j < skinnedMesh->numberOfTextureGroups; ++j)
    {
      GameFileDataSkinnedMesh::TextureGroup* group = &skinnedMesh->textureGroups[j];
      for (size_t b = 0; b < group->numberOfBlendingGroups; ++b)
      {
        GameFileDataSkinnedMesh::TextureGroup::BlendingGroup* blending = &group->blendingGroups[b];
        SAFE_RELEASE(blending->vertexBuffer);
        SAFE_RELEASE(blending->indexBuffer);
      }
        SAFE_DELETE_ARRAY(group->blendingGroups);
    }
    SAFE_DELETE_ARRAY(skinnedMesh->textureGroups);
  }
  SAFE_DELETE_ARRAY(myResources.animatedMeshes.skinnedMeshes);
  myResources.animatedMeshes.numberOfSkinnedMeshes = 0;

  SAFE_DELETE_ARRAY(myResources.items.itemDescriptionArray);
  myResources.items.itemDescriptionArraySize = 0;

  // Erase avatar information
  SAFE_DELETE_ARRAY(myResources.avatars.classArray);
  myResources.avatars.classArraySize = 0;
  SAFE_DELETE_ARRAY(myResources.avatars.raceArray);
  myResources.avatars.raceArraySize = 0;

  global_map_manager_.destroy();

  // Free scenery
  SAFE_DELETE_ARRAY(myResources.scenery.sceneryArray);
  myResources.scenery.sceneryArraySize = 0;

  // Free meshes
  if (myResources.meshes.meshArray)
  {
    // Free each element in the array
    for (size_t i = 0; i < myResources.meshes.meshArraySize; ++i)
    {
      // Free each subset in this element
      GameFileData_Mesh::iterator end = myResources.meshes.meshArray[i]->end();
      for (GameFileData_Mesh::iterator j = myResources.meshes.meshArray[i]->begin(); j != end; ++j)
        DeallocateGeometry(j->second);

      // Get rid of this element
      SAFE_DELETE(myResources.meshes.meshArray[i]);
    }

    // Free the array itself
    SAFE_DELETE_ARRAY(myResources.meshes.meshArray);
  }
  myResources.meshes.meshArraySize = 0;

  // Free skins
  SAFE_DELETE_ARRAY(myResources.skins.skinArray);
  myResources.skins.skinArraySize = 0;

  // Get rid of the D3D textures in the images member
  SAFE_RELEASE_ELEMENTS(myResources.images.d3dTextureArray, myResources.images.d3dTextureArraySize);
  SAFE_DELETE_ARRAY(myResources.images.d3dTextureArray);
  myResources.images.d3dTextureArraySize = 0;

  // Free all of the non-array texture references
  SAFE_RELEASE(myResources.images.d3dTextureFont);
  SAFE_RELEASE(myResources.images.d3dTextureTitle);
  SAFE_RELEASE(myResources.images.d3dTextureTitleBackground);
  SAFE_RELEASE(myResources.images.d3dTextureGUI);
  SAFE_RELEASE(myResources.images.d3dTextureEvidyonSymbol);
  SAFE_RELEASE(myResources.images.d3dTextureDayNightLightParticle);
}
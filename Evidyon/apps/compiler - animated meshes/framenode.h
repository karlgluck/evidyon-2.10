//------------------------------------------------------------------------------------------------
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __FRAMENODE_H__
#define __FRAMENODE_H__


/**
 * Used to hold data from a frame loaded from a .X file
 */
class FrameNode
{
    public:

        /**
         * This is used to store a mesh object
         */
        struct Mesh
        {
            DWORD numberOfMaterials;
            ID3DXBuffer* adjacencyBuffer;
            ID3DXBuffer* materialsBuffer;
            ID3DXBuffer* effectInstancesBuffer;
            ID3DXSkinInfo* skinInfo;
            ID3DXMesh* mesh;
        };

        struct ScaleAnimationKey
        {
            DWORD tick;
            D3DXVECTOR3 vector;
        };

        struct TranslateAnimationKey
        {
            DWORD tick;
            D3DXVECTOR3 vector;
        };

        struct RotateAnimationKey
        {
            DWORD tick;
            D3DXQUATERNION quaternion;
        };

        struct TransformAnimationKey
        {
            DWORD tick;
            D3DXMATRIX matrix;
        };

        struct AnimationOptions
        {
            /// Set to true if this animation options structure has been initialized
            bool valid;

            /// If true, this animation loops.  If false, it is "closed" and should
            /// only be played once.
            bool loops;

            /// If set to true, the animation should be spline-interpolated.  If
            /// false, it should be linearly interpolated.
            bool useSplineInterpolation;
        };

        struct FrameAnimation
        {
            /// The name of the frame that this animation applies to.  There can be multiple
            /// names, but only one of them should reference a valid frame.
            std::list<std::string> frameNames;

            ScaleAnimationKey* scalingKeys;
            size_t numberOfScalingKeys;

            TranslateAnimationKey* translationKeys;
            size_t numberOfTranslationKeys;

            RotateAnimationKey* rotationKeys;
            size_t numberOfRotationKeys;

            TransformAnimationKey* transformationKeys;
            size_t numberOfTransformationKeys;

            /// Animation options specified for this bone
            AnimationOptions animationOptions;
        };

        struct Animation
        {
            /// What this animation is called
            std::string name;

            /// Transformation sequences for each of the frames involved in this animation
            std::vector<FrameAnimation*> frameAnimations;

            /// Animation options specified for this animation
            AnimationOptions animationOptions;
        };

    public:

        typedef std::list<std::pair<std::string,UnseenSkeletalAnimation*>> UnseenAnimationSet;

    public:

        /**
         * Initializes this class
         */
        FrameNode();

        /**
         * Backup method to clean up leftover memory
         */
        ~FrameNode();

        /**
         * Loads data into this node
         */
        bool load( ID3DXFileData* dxFileData, IDirect3DDevice9* d3dDevice );

        /**
         * Frees data from this node and sub-nodes
         */
        void destroy();

        /**
         * Gets this frame node's path in the hierarchy
         */
        const std::string& path() const;

        /**
         * "Bakes" this mesh into the output mesh/animation structures
         */
        bool bake( LPDIRECT3DDEVICE9 d3dDevice, UnseenSkinnedMesh* mesh, UnseenAnimationSet* animationSet, float minimumBoneInfluenceWeight = 0.01f );

        /**
         * Frees the memory in an animation set that was allocated by the bake method
         */
        void deallocate( UnseenAnimationSet* animationSet );

    public:

        /**
         * Gets the parent of this node
         */
        FrameNode* getParentNode() { return myParentNode; }

        /**
         * Gets the names for this node
         */
        const std::list<std::string>& getNames() { return myNames; }

        /**
         * Gets this frame's hierarchial offset
         */
        const D3DXMATRIX* getTransform() { return &myTransform; }


    protected:

        /**
         * Loads a mesh encountered while parsing this frame node
         */
        bool loadMesh( const std::string& name, ID3DXFileData* dxFileData, IDirect3DDevice9* d3dDevice );

        /**
         * Loads this frame's transformation matrix
         */
        bool loadFrameTransformMatrix( const std::string& name, ID3DXFileData* dxFileData );

        /**
         * Loads a child frame for this mesh
         */
        bool loadChildFrame( const std::string& name, ID3DXFileData* dxFileData, IDirect3DDevice9* d3dDevice );

        /**
         * Loads an animation encountered while parsing this frame node
         */
        bool loadAnimationSet( const std::string& name, ID3DXFileData* dxFileData );

        /**
         * Loads an animation sub-object encountered while parsing this frame node.  This method is
         * called exclusively by loadAnimationSet.
         */
        bool loadAnimation( const std::string& name, ID3DXFileData* dxFileData, Animation* animation );

        /**
         * Loads animation options into the given object
         */
        bool loadAnimationOptions( const std::string& name, ID3DXFileData* dxFileData, AnimationOptions* animationOptions );

        /**
         * Loads a key that is used to animate the given bone
         */
        bool loadAnimationKey( const std::string& name, ID3DXFileData* dxFileData, FrameAnimation* frameAnimation );

        /**
         * Loads keys from the source data pointer into the 
         */
        bool loadRotationKeys( DWORD* dataPointer, SIZE_T dataSize, DWORD numberOfEntries, FrameAnimation* frameAnimation );

        /**
         * Loads keys from the source data pointer into the 
         */
        bool loadScalingKeys( DWORD* dataPointer, SIZE_T dataSize, DWORD numberOfEntries, FrameAnimation* frameAnimation );

        /**
         * Loads keys from the source data pointer into the 
         */
        bool loadTranslationKeys( DWORD* dataPointer, SIZE_T dataSize, DWORD numberOfEntries, FrameAnimation* frameAnimation );

        /**
         * Loads keys from the source data pointer into the 
         */
        bool loadTransformationKeys( DWORD* dataPointer, SIZE_T dataSize, DWORD numberOfEntries, FrameAnimation* frameAnimation );

    protected:

        /**
         * Turns scale/rotate/translate keys into transform keys
         */
        void bakeIntoTransformKeys( const D3DXMATRIX* frameTransform, FrameAnimation* animation );

        /**
         * Recursively searches the frame node hierarchy and builds a table of all of the meshes
         */
        void findAllMeshes( std::multimap<FrameNode*,Mesh*>* meshes );

        /**
         * Recursively searches the frame node hierarchy for all of the frames
         */
        void findAllFrames( std::vector<FrameNode*>* frameNodes );


    protected:

        /**
         * Gets the name of a file data element
         */
        static void getFileDataName( ID3DXFileData* dxFileData, std::string* name );


    protected:

        /// The owner frame node
        FrameNode* myParentNode;

        /// This frame's transformation matrix
        D3DXMATRIX myTransform;

        /// The list of frame children in the hierarchy
        std::vector<FrameNode*> myChildren;

        /// Meshes encountered in this frame node
        std::vector<Mesh*> myMeshes;

        /// Animations encountered in this frame node
        std::vector<Animation*> myAnimations;

        /// The path to this frame node in the hierarchy; returned by path()
        std::string myPath;

        /// This frame's name(s)
        std::list<std::string> myNames;

        /// The number of animation ticks-per-second
        DWORD myAnimationTicksPerSecond;
};



#endif
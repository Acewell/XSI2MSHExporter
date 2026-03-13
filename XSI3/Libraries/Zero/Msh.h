#ifndef __MSH_H__
#define __MSH_H__

class ZeroObject;
class ZeroMeshContext;

enum ZeroMshModelTypes
{
    Null_Node,
    Skin_Node,
    Cloth_Node,
    Envelop_Node,
    Static_Node,
    Child_Skin_Node,
	Shadow_Volume,
	Destructible_Node,

    Forcedword = 0x7fffffff,
};

struct ZeroMshShadowEdge
{
	unsigned short index[4];
};


struct ZeroMatrixWeightPair
{
    int boneID;
    float weight;

    ZeroMatrixWeightPair (unsigned bone, float vertexWeight)
    {
        boneID = bone;
        weight = vertexWeight;
    }
};



namespace MSH
{
    ZeroObject * __cdecl Load (const char *name, const ZeroMeshContext *context);
    bool __cdecl Save (const char *name, ZeroObject *mesh); 
}

#endif

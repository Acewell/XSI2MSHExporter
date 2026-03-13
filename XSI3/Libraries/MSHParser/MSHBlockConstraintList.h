/*
	CMSHBlockConstraintList
	
	Class for .msh file data blocks with 'CLST' header
*/

#ifndef MSHBLOCK_CONSTRAINTLIST_H
#define MSHBLOCK_CONSTRAINTLIST_H


#include "MSHBlock.h"

/*
class CMSHBlockConstraintList: public CMSHBlock
{

protected:
	//	type of constraint 'POSITION', 'INTEREST', 'UP_VECTOR', etc
	char *m_sType;
	//	name of object to constrain
	char *m_sObjectName;

	//	names of objects to constrain to
	unsigned int m_nConstraintNames;
	char **m_ppConstraintNames;

public:
	CMSHBlockConstraintList(BlockHeader *pHdr,CZeroData *pData);
	virtual ~CMSHBlockConstraintList();

	//--------------------
	//	File read/write methods
	virtual void Read(FILE *file);
	virtual void Write(FILE *file);
	virtual void WriteText(FILE *);

	//--------------------
	//	Get methods
	char *GetType();
	char *GetObjectName();
	char **GetConstraintNames(unsigned int &nNames);

	//--------------------
	//	Set methods
	void SetType(char *sType);
	void SetObjectName(char *sObjectName);
	void AddConstraint(char *sCnsName);
};
*/
#endif

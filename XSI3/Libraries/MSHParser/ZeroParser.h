#include <stdio.h>


class CZeroData;

void CreateZeroFileStructure(FILE* fp, CZeroData *pData);

class CZeroParser
{
protected:
	void ParseMSH1(FILE *fp, unsigned int size, CZeroData *pData);
	

public:
	CZeroParser();
	virtual ~CZeroParser();

	void ParseFile(FILE *fp, CZeroData *pData);
};

#ifndef _XMaterial_H_
#define _XMaterial_H_

#include "XGlobal.h"

namespace Xge
{
	class Material
	{
	public:
		Material();

		D3DMATERIAL9 *GetMaterial();

		BOOL SetDiffuseColor(unsigned char Red, unsigned char Green, unsigned char Blue);
		BOOL GetDiffuseColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue);

		BOOL SetAmbientColor(unsigned char Red, unsigned char Green, unsigned char Blue);
		BOOL GetAmbientColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue);
    
		BOOL SetSpecularColor(unsigned char Red, unsigned char Green, unsigned char Blue);
		BOOL GetSpecularColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue);
    
		BOOL SetEmissiveColor(unsigned char Red, unsigned char Green, unsigned char Blue);
		BOOL GetEmissiveColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue);

		BOOL  SetPower(float Power);
		float GetPower(float Power);
	
	protected:
		D3DMATERIAL9  m_Material;
	};
}

#endif

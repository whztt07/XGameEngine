#ifndef _XLight_H_
#define _XLight_H_

#include "XGlobal.h"

namespace Xge
{
	class Light
	{
	public:
		Light();

		D3DLIGHT9 *GetLight();

		BOOL SetType(D3DLIGHTTYPE Type);

		BOOL Move(float XPos, float YPos, float ZPos);
		BOOL MoveRel(float XPos, float YPos, float ZPos);
		BOOL GetPos(float *XPos, float *YPos, float *ZPos);

		BOOL Point(float XFrom, float YFrom, float ZFrom,
					float XAt,   float YAt,   float ZAt);
		BOOL GetDirection(float *XDir, float *YDir, float *ZDir);

		BOOL SetDiffuseColor(unsigned char Red, unsigned char Green, unsigned char Blue);
		BOOL GetDiffuseColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue);

		BOOL SetSpecularColor(unsigned char Red, unsigned char Green, unsigned char Blue);
		BOOL GetSpecularColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue);
    
		BOOL SetAmbientColor(unsigned char Red, unsigned char Green, unsigned char Blue);
		BOOL GetAmbientColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue);

		BOOL SetRange(float Range);
		float GetRange();
    
		BOOL SetFalloff(float Falloff);
		float GetFalloff();
    
		BOOL SetAttenuation0(float Attenuation);
		float GetAttenuation0();

		BOOL SetAttenuation1(float Attenuation);
		float GetAttenuation1();

		BOOL SetAttenuation2(float Attenuation);
		float GetAttenuation2();
    
		BOOL SetTheta(float Theta);
		float GetTheta();
    
		BOOL SetPhi(float Phi);
		float GetPhi();

	protected:
		D3DLIGHT9 m_Light;
	};
}

#endif

#include "Uniforms.hlsl"
#include "Samplers.hlsl"
#include "Transform.hlsl"

void VS(float4 iPos : POSITION,
		float2 iTexCoord : TEXCOORD0,  
		float4 iColor : COLOR0, 
		out float4 oPos : POSITION, 
		out float4 oColor : COLOR0)
{

	float4x3 modelMatrix = iModelMatrix;
    float3 worldPos = GetWorldPos(modelMatrix);
    oPos = GetClipPos(worldPos);
    oColor = iColor * iTexCoord.x;
}

void PS(float4 iColor : COLOR0, out float4 oColor : COLOR0)
{
	oColor = iColor;
}


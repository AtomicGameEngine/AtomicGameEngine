
#include "Uniforms.hlsl"
#include "Samplers.hlsl"
#include "Transform.hlsl"
#include "ScreenPos.hlsl"

#ifdef COMPILEPS
uniform float4 cShadowAmbient;
#endif

void VS(float4 iPos : POSITION, out float4 oPos : POSITION, out float2 oScreenPos : TEXCOORD0)
{
    float4x3 modelMatrix = iModelMatrix;
    float3 worldPos = GetWorldPos(modelMatrix);
    oPos = GetClipPos(worldPos);
    oScreenPos = GetScreenPosPreDiv(oPos);
}

void PS(float2 iScreenPos : TEXCOORD0, out float4 oColor : COLOR0)
{
	float4 diffInput = tex2D(sDiffMap, iScreenPos);
    float4 lightInput = tex2D(sEmissiveMap, iScreenPos);
	oColor.rgb = (diffInput.rgb * (lightInput.rgb + cShadowAmbient.rgb) * (lightInput.a + cShadowAmbient.a));
    oColor.a = 1.0;
}




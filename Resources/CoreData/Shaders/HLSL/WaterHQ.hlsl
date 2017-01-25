#include "Uniforms.hlsl"
#include "Samplers.hlsl"
#include "Transform.hlsl"
#include "ScreenPos.hlsl"
#include "Lighting.hlsl"
#include "Fog.hlsl"

#ifndef D3D11

// D3D9 uniforms
uniform float2 cNoiseSpeed;
uniform float cNoiseTiling;
uniform float cRefractNoiseStrength;
uniform float cReflectNoiseStrength;
uniform float cFresnelPower;
uniform float cFresnelBias;
uniform float3 cFoamColor;
uniform float cFoamTreshold;
uniform float3 cShallowColor;
uniform float3 cDeepColor;
uniform float cDepthScale;

#else

// D3D11 constant buffers
#ifdef COMPILEVS
cbuffer CustomVS : register(b6)
{
    float2 cNoiseSpeed;
    float cNoiseTiling;
}
#else
cbuffer CustomPS : register(b6)
{
    float cRefractNoiseStrength;
    float cReflectNoiseStrength;
    float cFresnelPower;
    float cFresnelBias;
    float3 cFoamColor;
    float cFoamTreshold;
    float3 cShallowColor;
    float3 cDeepColor;
    float cDepthScale;
}
#endif

#endif

void VS(float4 iPos : POSITION,
    float3 iNormal: NORMAL,
    #if LIGHTING
        float4 iTangent : TANGENT,
    #endif
    float2 iTexCoord : TEXCOORD0,
    #ifdef INSTANCED
        float4x3 iModelInstance : TEXCOORD4,
    #endif

    #if LIGHTING
        out float4 oWorldPos :TEXCOORD4,

        #ifdef PERPIXEL
            #ifdef SPOTLIGHT
                out float4 oSpotPos : TEXCOORD5,
            #endif
            #ifdef POINTLIGHT
                out float3 oCubeMaskVec : TEXCOORD5,
            #endif
        
            out float4 oTangent : TEXCOORD0,
            out float4 oTexCoord : TEXCOORD1,
            out float4 oTexCoord2 : TEXCOORD2,
        #endif
    #else
        out float4 oScreenPos : TEXCOORD0,
        out float2 oReflectUV : TEXCOORD1,
        out float2 oWaterUV : TEXCOORD2,
        out float4 oEyeVec : TEXCOORD4,
    #endif
    out float3 oNormal : TEXCOORD3,
    #if defined(D3D11) && defined(CLIPPLANE)
        out float oClip : SV_CLIPDISTANCE0,
    #endif
    out float4 oPos : OUTPOSITION)
{
    float4x3 modelMatrix = iModelMatrix;
    float3 worldPos = GetWorldPos(modelMatrix);
    oPos = GetClipPos(worldPos);

    oNormal = GetWorldNormal(modelMatrix);

    #if LIGHTING
        oWorldPos = float4(worldPos, GetDepth(oPos));

        #ifdef PERPIXEL
            // Per-pixel forward lighting
            float4 projWorldPos = float4(worldPos, 1.0);
                
            float3 tangent = GetWorldTangent(modelMatrix);
            float3 bitangent = cross(tangent, oNormal) * iTangent.w;
            oTexCoord = float4(GetTexCoord(iTexCoord * cNoiseTiling + cElapsedTime * cNoiseSpeed), bitangent.xy);
            oTexCoord2 = float4(GetTexCoord(iTexCoord.yx * cNoiseTiling - cElapsedTime * cNoiseSpeed), bitangent.xy);
            oTangent = float4(tangent, bitangent.z);
        
            #ifdef SPOTLIGHT
                // Spotlight projection: transform from world space to projector texture coordinates
                oSpotPos = mul(projWorldPos, cLightMatrices[0]);
            #endif
        
            #ifdef POINTLIGHT
                oCubeMaskVec = mul(worldPos - cLightPos.xyz, (float3x3)cLightMatrices[0]);
            #endif
        #endif
    #else
        oScreenPos = GetScreenPos(oPos);
        oEyeVec = float4(cCameraPos - worldPos, GetDepth(oPos));
        // GetQuadTexCoord() returns a float2 that is OK for quad rendering; multiply it with output W
        // coordinate to make it work with arbitrary meshes such as the water plane (perform divide in pixel shader)
        oReflectUV = GetQuadTexCoord(oPos) * oPos.w;
        oWaterUV = iTexCoord * cNoiseTiling + cElapsedTime * cNoiseSpeed;
    #endif

    #if defined(D3D11) && defined(CLIPPLANE)
        oClip = dot(oPos, cClipPlane);
    #endif
}

void PS(
    #if LIGHTING
        float4 iWorldPos : TEXCOORD4,

        #ifdef PERPIXEL
            #ifdef SPOTLIGHT
                float4 iSpotPos : TEXCOORD5,
            #endif
            #ifdef POINTLIGHT
                float3 iCubeMaskVec : TEXCOORD5,
            #endif
        
            float4 iTangent : TEXCOORD0,
            float4 iTexCoord : TEXCOORD1,
            float4 iTexCoord2 : TEXCOORD2,
        #endif
    #else
        float4 iScreenPos : TEXCOORD0,
        float2 iReflectUV : TEXCOORD1,
        float2 iWaterUV : TEXCOORD2,
        float4 iEyeVec : TEXCOORD4,
    #endif
    float3 iNormal : TEXCOORD3,
    #if defined(D3D11) && defined(CLIPPLANE)
        float iClip : SV_CLIPDISTANCE0,
    #endif
    out float4 oColor : OUTCOLOR0)
{
#if LIGHTING
    #ifdef PERPIXEL
        #if defined(SPOTLIGHT)
            float3 lightColor = iSpotPos.w > 0.0 ? Sample2DProj(LightSpotMap, iSpotPos).rgb * cLightColor.rgb : 0.0;
        #elif defined(CUBEMASK)
            float3 lightColor = SampleCube(LightCubeMap, iCubeMaskVec).rgb * cLightColor.rgb;
        #else
            float3 lightColor = cLightColor.rgb;
        #endif

        #ifdef DIRLIGHT
            float3 lightDir = cLightDirPS;
        #else
            float3 lightVec = (cLightPosPS.xyz - iWorldPos.xyz) * cLightPosPS.w;
            float3 lightDir = normalize(lightVec);
        #endif
        
        float3x3 tbn = float3x3(iTangent.xyz, float3(iTexCoord.zw, iTangent.w), iNormal);
        float3 normal = normalize(mul(DecodeNormal(Sample2D(SpecMap, iTexCoord.xy)), tbn)); //Those are normals but I had to use the unit3 to store the normals
        float3 normal2 = normalize(mul(DecodeNormal(Sample2D(SpecMap, iTexCoord2.xy)), tbn));
        normal = normalize(normal + normal2);
        
        #ifdef HEIGHTFOG
            float fogFactor = GetHeightFogFactor(iWorldPos.w, iWorldPos.y);
        #else
            float fogFactor = GetFogFactor(iWorldPos.w);
        #endif
        
        //Not sure about that
        float3 spec = GetSpecular(normal, cCameraPosPS - iWorldPos.xyz, lightDir, 200.0) * lightColor * cLightColor.a;
        
        oColor = float4(GetLitFog(spec, fogFactor), 1.0);
    #endif
#else
        //Get reflect and refract UV
        float2 refractUV = iScreenPos.xy / iScreenPos.w;
        //float2 reflectUV = iReflectUV.xy / iScreenPos.w;
    
        //Store refract UV before applying noise
        float2 noiseLessRefractUV = refractUV;
    
        float3 noise = (Sample2D(SpecMap, iWaterUV).rgb - 0.5);
        refractUV += noise.rg * cRefractNoiseStrength;
    
        //Do not shift reflect UV coordinate upward, because it will reveal the clipping of geometry below water
        if (noise.y < 0.0)
            noise.y = 0.0;
        //reflectUV += noise;
        float3 reflectDir = reflect(-iEyeVec.xyz, iNormal);
        reflectDir += noise * cReflectNoiseStrength;
    
        //float2 reflectDirUV = ;
    
        //Get depth and original depth
        float depth = Sample2DLod0(DepthBuffer, refractUV).r;
        float depthOriginal = Sample2DLod0(DepthBuffer, noiseLessRefractUV).r;
    
        #ifdef HWDEPTH
        depth = ReconstructDepth(depth);
        depthOriginal = ReconstructDepth(depthOriginal);
        #endif
    
        //Object above the water reset refracted UV and depth value
        refractUV = lerp(refractUV , noiseLessRefractUV, depth < depthOriginal);
        depth = lerp(depth , depthOriginal, depth < depthOriginal);
    
        //Calculate water depth
        float3 waterDepth = (depth - iEyeVec.w) * (cFarClipPS - cNearClipPS);
    
        //Calculate fresnel component
        half facing = (1.0 - dot(normalize(iEyeVec.xyz), iNormal));
        float fresnel = max(cFresnelBias + (1.0 - cFresnelBias) * pow(facing, cFresnelPower), 0.0);
    
        float steepness = 1.0 - dot(float3(0.0,1.0,0.0), normalize(DecodeNormal(Sample2D(NormalMap, noiseLessRefractUV))));
    
        float3 refractColor = Sample2D(EnvMap, refractUV).rgb;
        //float3 reflectColor = Sample2D(EnvMap, reflectUV).rgb;   
        float3 reflectColor = SampleCube(DiffCubeMap, reflectDir);   
    
        float3 waterColor = lerp(cShallowColor, cDeepColor, clamp(waterDepth * cDepthScale, 0.0, 1.0));
    
        refractColor *= waterColor;
    
        float3 finalColor = lerp(refractColor, reflectColor, fresnel);
    
        finalColor = lerp(cFoamColor, finalColor, clamp(saturate((waterDepth + (noise.x * 0.1)) / cFoamTreshold) + steepness, 0.5, 1.0));
    
        oColor = float4(GetFog(finalColor, GetFogFactor(iEyeVec.w)), 1.0 );
#endif
}
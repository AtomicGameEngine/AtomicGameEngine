

namespace AtomicEngine
{

    // These must be kept in sync with GraphicsDef.cpp
    // These could also be convertied to StringHash, would need override for setting on Graphics subsystem

    public static class ShaderParams
    {
        // Vertex
        public const string VSP_AMBIENTSTARTCOLOR = "AmbientStartColor";
        public const string VSP_AMBIENTENDCOLOR = "AmbientEndColor";
        public const string VSP_BILLBOARDROT = "BillboardRot";
        public const string VSP_CAMERAPOS = "CameraPos";
        public const string VSP_CAMERAROT = "CameraRot";
        public const string VSP_CLIPPLANE = "ClipPlane";
        public const string VSP_NEARCLIP = "NearClip";
        public const string VSP_FARCLIP = "FarClip";
        public const string VSP_DEPTHMODE = "DepthMode";
        public const string VSP_DELTATIME = "DeltaTime";
        public const string VSP_ELAPSEDTIME = "ElapsedTime";
        public const string VSP_FRUSTUMSIZE = "FrustumSize";
        public const string VSP_GBUFFEROFFSETS = "GBufferOffsets";
        public const string VSP_LIGHTDIR = "LightDir";
        public const string VSP_LIGHTPOS = "LightPos";
        public const string VSP_MODEL = "Model";
        public const string VSP_VIEWPROJ = "ViewProj";
        public const string VSP_UOFFSET = "UOffset";
        public const string VSP_VOFFSET = "VOffset";
        public const string VSP_ZONE = "Zone";
        public const string VSP_LIGHTMATRICES = "LightMatrices";
        public const string VSP_SKINMATRICES = "SkinMatrices";
        public const string VSP_VERTEXLIGHTS = "VertexLights";

        // Pixel
        public const string PSP_AMBIENTCOLOR = "AmbientColor";
        public const string PSP_CAMERAPOS = "CameraPosPS";
        public const string PSP_DELTATIME = "DeltaTimePS";
        public const string PSP_DEPTHRECONSTRUCT = "DepthReconstruct";
        public const string PSP_ELAPSEDTIME = "ElapsedTimePS";
        public const string PSP_FOGCOLOR = "FogColor";
        public const string PSP_FOGPARAMS = "FogParams";
        public const string PSP_GBUFFERINVSIZE = "GBufferInvSize";
        public const string PSP_LIGHTCOLOR = "LightColor";
        public const string PSP_LIGHTDIR = "LightDirPS";
        public const string PSP_LIGHTPOS = "LightPosPS";
        public const string PSP_MATDIFFCOLOR = "MatDiffColor";
        public const string PSP_MATEMISSIVECOLOR = "MatEmissiveColor";
        public const string PSP_MATENVMAPCOLOR = "MatEnvMapColor";
        public const string PSP_MATSPECCOLOR = "MatSpecColor";
        public const string PSP_NEARCLIP = "NearClipPS";
        public const string PSP_FARCLIP = "FarClipPS";
        public const string PSP_SHADOWCUBEADJUST = "ShadowCubeAdjust";
        public const string PSP_SHADOWDEPTHFADE = "ShadowDepthFade";
        public const string PSP_SHADOWINTENSITY = "ShadowIntensity";
        public const string PSP_SHADOWMAPINVSIZE = "ShadowMapInvSize";
        public const string PSP_SHADOWSPLITS = "ShadowSplits";
        public const string PSP_LIGHTMATRICES = "LightMatricesPS";


    }


}


using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
    [StructLayout(LayoutKind.Sequential)]
    public struct CrowdObstacleAvoidanceParams
    {
        public float VelBias;
        public float WeightDesVel;
        public float WeightCurVel;
        public float WeightSide;
        public float WeightToi;
        public float HorizTime;
        public byte GridSize;
        public byte AdaptiveDivs;
        public byte AdaptiveRings;
        public byte AdaptiveDepth;
    };

    public partial class CrowdManager : Component
    {
        public CrowdObstacleAvoidanceParams GetObstacleAvoidanceParams(uint obstacleAvoidanceType)
        {
            CrowdObstacleAvoidanceParams parms = new CrowdObstacleAvoidanceParams();
            csi_Atomic_CrowdManager_GetObstacleAvoidanceParams(nativeInstance, obstacleAvoidanceType, ref parms);
            return parms;
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void csi_Atomic_CrowdManager_GetObstacleAvoidanceParams(IntPtr self,  uint obstacleAvoidanceType, ref CrowdObstacleAvoidanceParams parms);

        public void SetObstacleAvoidanceParams(uint obstacleAvoidanceType, CrowdObstacleAvoidanceParams parms)
        {
            csi_Atomic_CrowdManager_SetObstacleAvoidanceParams(nativeInstance, obstacleAvoidanceType, ref parms);
        }

        [DllImport(Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void csi_Atomic_CrowdManager_SetObstacleAvoidanceParams(IntPtr self, uint obstacleAvoidanceType, ref CrowdObstacleAvoidanceParams parms);


    }

}

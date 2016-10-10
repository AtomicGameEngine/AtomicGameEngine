
using System;
using System.Runtime.InteropServices;

namespace AtomicEngine
{
    public partial class Log : AObject
    {

        public static void Warn(string message)
        {
            Log.Write(Constants.LOG_WARNING, message);
        }

        public static void Error(string message)
        {
            Log.Write(Constants.LOG_ERROR, message);
        }

        public static void Info(string message)
        {
            Log.Write(Constants.LOG_INFO, message);
        }

    };

}


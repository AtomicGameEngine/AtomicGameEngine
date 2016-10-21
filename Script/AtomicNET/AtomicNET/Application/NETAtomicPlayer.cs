namespace AtomicEngine
{
    public partial class NETAtomicPlayer : PlayerApp
    {

        /// <summary>
        ///  AtomicPlayer for NET deployments
        /// </summary>
        /// <param name="args"></param>
        /// <returns></returns>
        public static NETAtomicPlayer Create(string[] args)
        {
            // Initialize AtomicNET
            AtomicNET.Initialize();

            var app = CreateInternal();

            app.Initialize();

            return app;
        }


    }

}

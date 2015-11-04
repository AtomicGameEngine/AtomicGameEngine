

namespace AtomicEngine
{

	public partial class CSComponent : ScriptComponent
	{
		public virtual void Start()
		{

		}


		public virtual void Update(float timeStep)
		{

		}

		internal void StartInternal()
		{
			ApplyFieldValues();
			Start();
		}
			
	}

}
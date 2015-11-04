using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace AtomicEngine
{

	public class VariantMap
	{		
		public VariantMap(IntPtr native)
		{
			this.native = native;
		}

		public void Invalidate()
		{
			native = IntPtr.Zero;
		}

		void checkValid()
		{
			if (native == IntPtr.Zero)
				throw new System.AccessViolationException("Event data only valid during event");

		}

		public T Get<T>(string key) where T:RefCounted
		{
			checkValid ();

			// TODO: safe case

			IntPtr r = csb_Atomic_VariantMap_GetInstance (native, key);
							
			return r == IntPtr.Zero ? null :  NativeCore.WrapNative<T> (r);
		}

		public int GetInt(string key)
		{
			checkValid ();

			return 0;
		}

		~VariantMap()
		{

		}

		IntPtr native = default(IntPtr);

		[DllImport (Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
		private static extern IntPtr csb_Atomic_VariantMap_GetInstance(IntPtr native, string key);


	}

	public static class EventCore
	{
		class Subscription
		{
			public uint SenderRefID;
			public AtomicEventDelegate Handler;			
		}

		// event type -> subscribers
		static Dictionary<uint, List<uint> > eventSubscribers = new Dictionary<uint, List<uint>>();

		static Dictionary<uint, Dictionary<uint, Subscription> > subscriptions = new Dictionary<uint, Dictionary<uint, Subscription> >();


		public static void SendEvent(AObject sender, string eventType)
		{
			csb_Atomic_AObject_SendEvent (sender.nativeInstance, eventType);	
		}

		public static void SubscribeToEvent(AObject subscriber, AObject sender, string eventType, AtomicEventDelegate function)
		{
			var eventTypeID = Atomic.StringToStringHash (eventType);			

			Dictionary<uint, Subscription> subs;

			if (!subscriptions.TryGetValue (subscriber.RefID, out subs)) {

				subs = new Dictionary<uint, Subscription> ();
				subscriptions [subscriber.RefID] = subs;

			}

			Subscription sub;

			if (!subs.TryGetValue (eventTypeID, out sub)) {

				sub = new Subscription ();
				subs [eventTypeID] = sub;				
			}

			sub.SenderRefID = sender == null ? 0 : sender.RefID;
			sub.Handler = function;

			List<uint> subscribers;

			if (!eventSubscribers.TryGetValue (eventTypeID, out subscribers)) {

				subscribers = new List<uint> ();
				eventSubscribers [eventTypeID] = subscribers;
			}

			if (!subscribers.Contains (subscriber.RefID)) {

				subscribers.Add (subscriber.RefID);
			}
									
		}

		public static void BeginSendEvent(uint senderRefId, uint eventType, IntPtr eventData)
		{
			List<uint> subscribers;

			if (!eventSubscribers.TryGetValue (eventType, out subscribers)) {				
				return;			
			}

			if (subscribers.Count == 0) {
				return;
			}

			VariantMap vmap = new VariantMap (eventData);

			foreach (var id in subscribers) {

				Dictionary<uint, Subscription> subs;

				if (subscriptions.TryGetValue (id, out subs)) {

					Subscription sub;

					if (subs.TryGetValue (eventType, out sub)) {

						if (sub.SenderRefID == 0 || sub.SenderRefID == senderRefId) {

							sub.Handler (vmap);
							
						}

					}

				}

			}

			vmap.Invalidate ();

		}

		[DllImport (Constants.LIBNAME, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
		private static extern void csb_Atomic_AObject_SendEvent(IntPtr self, string eventType);

	}
}


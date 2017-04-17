using System;
using System.Linq;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using static System.Reflection.IntrospectionExtensions;

namespace AtomicEngine
{
    public partial class Scene : Node
    {

        /// <summary>
        /// Explicitly destroys scene and disposes of children and components
        /// </summary>       
        public override void Destroy()
        {
            if (Destroyed)
                return;

            cscomponents.Clear();
            cscomponentStart.Clear();

            base.Destroy();
        }

        internal override void PostNativeUpdate()
        {    
            SubscribeToEvent<NodeAddedEvent>(this, e =>
            {
                //Log.Info($"Node ADDED: {e.Node.Name}");

                // The NodeAdded event is generated when adding a node as a child

                e.Node.GetComponents<CSComponent>(csComponentVector);

                for (uint i = 0; i < csComponentVector.Size; i++)
                {
                    AddCSComponent(csComponentVector[i]);
                }

                csComponentVector.Clear();

            });

            SubscribeToEvent<NodeRemovedEvent>(this, e =>
            {
                //Log.Info($"Node REMOVED: {e.Node.Name}");

                // The NodeRemoved event is generated when explicitly removing nodes from a scene
                // For general cleanup, it will not be generated
                
                e.Node.GetComponents<CSComponent>(csComponentVector);

                for (uint i = 0; i < csComponentVector.Size; i++)
                {
                    HandleComponentRemoved(csComponentVector[i]);
                }

                csComponentVector.Clear();

            });

            SubscribeToEvent<CSComponentLoadEvent>(this, HandleCSComponentLoad);

            SubscribeToEvent<ComponentAddedEvent>(this, e =>
            {
                Component component = null;

                try
                {
                    // will throw if component isn't a known native
                    component = e.Component;
                }
                catch
                {
                    return;
                }

                HandleComponentAdded(component);

            });


            SubscribeToEvent<ComponentRemovedEvent>(this, e =>
            {
                Component component = null;

                try
                {
                    // will throw if component isn't a known native
                    component = e.Component;
                }
                catch
                {
                    return;
                }

                HandleComponentRemoved(component);

            });

            // Update variable timestep logic
            SubscribeToEvent<SceneUpdateEvent>(this, HandleSceneUpdate);

            // Update scene subsystems. If a physics world is present, it will be updated, triggering fixed timestep logic updates
            SubscribeToEvent<SceneSubsystemUpdateEvent>(this, HandleSceneSubsystemUpdate);

            // Update transform smoothing
            SubscribeToEvent<UpdateSmoothingEvent>(this, HandleUpdateSmoothing);

            // Post-update variable timestep logic
            SubscribeToEvent<ScenePostUpdateEvent>(this, HandleScenePostUpdate);

        }

        // Update variable timestep logic
        void HandleSceneUpdate(SceneUpdateEvent e)
        {            
            // Handle Start
            if (cscomponentStart.Count > 0)
            {
                var started = new List<CSComponent>();

                foreach (var csc in cscomponentStart.ToList())
                {
                    if (!csc.IsEnabled())
                    {
                        continue;
                    }

                    // mark as started whether or not a Start method exists
                    csc.started = true;
                    started.Add(csc);

                    CSComponentInfo info;
                    if (CSComponentCore.csinfoLookup.TryGetValue(csc.GetType(), out info))
                    {
                        if (info.StartMethod != null)
                        {
                            info.StartMethod.Invoke(csc, null);
                        }
                    }

                }

                foreach (var csc in started)
                {
                    cscomponentStart.Remove(csc);
                }
            }

            // Handle Scene Update

            Object[] args = new Object[1] { e.TimeStep };

            foreach (var item in cscomponents.ToList())
            {
                var info = item.Key;

                var UpdateMethod = info.UpdateMethod;

                if (UpdateMethod == null)
                    continue;

                foreach (var csc in item.Value.ToList())
                {
                    if (!csc.Started || !csc.IsEnabled())
                        continue;

                    UpdateMethod.Invoke(csc, args);
                }
                
            }

        }

        // Update scene subsystems. If a physics world is present, it will be updated, triggering fixed timestep logic updates
        void HandleSceneSubsystemUpdate(SceneSubsystemUpdateEvent e)
        {
        }

        // Update transform smoothing
        void HandleUpdateSmoothing(UpdateSmoothingEvent e)
        {
        }

        // Post-update variable timestep logic
        void HandleScenePostUpdate(ScenePostUpdateEvent e)
        {
            Object[] args = new Object[1] { e.TimeStep };

            foreach (var item in cscomponents)
            {
                var info = item.Key;

                var PostUpdateMethod = info.PostUpdateMethod;

                if (PostUpdateMethod == null)
                    continue;

                foreach (var csc in item.Value)
                {
                    if (!csc.Started || !csc.IsEnabled())
                        continue;

                    PostUpdateMethod.Invoke(csc, args);
                }

            }

        }

        void HandlePhysicsPreStep(PhysicsPreStepEvent e)
        {
            Object[] args = new Object[1] { e.TimeStep };

            foreach (var item in cscomponents)
            {
                var info = item.Key;

                var PhysicsPreStepMethod = info.PhysicsPreStepMethod;

                if (PhysicsPreStepMethod == null)
                    continue;

                foreach (var csc in item.Value)
                {
                    if (!csc.Started || !csc.IsEnabled())
                        continue;

                    PhysicsPreStepMethod.Invoke(csc, args);
                }

            }
        }

        void HandlePhysicsPostStep(PhysicsPostStepEvent e)
        {
            Object[] args = new Object[1] { e.TimeStep };

            foreach (var item in cscomponents)
            {
                var info = item.Key;

                var PhysicsPostStepMethod = info.PhysicsPostStepMethod;

                if (PhysicsPostStepMethod == null)
                    continue;

                foreach (var csc in item.Value)
                {
                    if (!csc.Started || !csc.IsEnabled())
                        continue;

                    PhysicsPostStepMethod.Invoke(csc, args);
                }

            }
        }

        void HandleCSComponentLoad(CSComponentLoadEvent e)
        {
            var scriptMap = e.scriptMap;

            // Get the NativeInstance as an IntPtr, otherwise we would be wrapping as a CSComponent
            IntPtr csnative = scriptMap.GetVoidPtr("NativeInstance");

            IntPtr fieldValues = IntPtr.Zero;
            if (scriptMap.Contains("FieldValues"))
                fieldValues = scriptMap.GetVoidPtr("FieldValues");

            CSComponentInfo csinfo;

            if (!CSComponentCore.componentCache.TryGetValue(e.ClassName, out csinfo))
            {
                Log.Error("Scene.HandleCSComponentLoad - unable to find CSComponent in cache for classname: " + e.ClassName );
                return;
            }

            var component = CSComponent.LoadCreateInstance(csinfo.Type, csnative);

            if (component == null)
            {
                Log.Error("Scene.HandleCSComponentLoad - unable to create CSComponent for classname: " + e.ClassName);
                return;
            }

            if (fieldValues != IntPtr.Zero)
                csinfo.ApplyFieldValues(component, fieldValues);

            AddCSComponent(component);

        }

        void AddCSComponent(CSComponent csc)
        {
            CSComponentInfo info;

            if (!CSComponentCore.csinfoLookup.TryGetValue(csc.GetType(), out info))
            {
                Log.Error("Scene.HandleComponentAdded - unable to get CSComponentInfo");
                return;
            }

            List<CSComponent> cslist;

            if (!cscomponents.TryGetValue(info, out cslist))
            {
                cslist = cscomponents[info] = new List<CSComponent>();
            }

            if (cslist.Contains(csc))
            {
                throw new InvalidOperationException("Scene.HandleComponentAdded - CSComponent already added to component list");
            }

            cslist.Add(csc);

            if (!csc.started)
            {
                if (cscomponentStart.Contains(csc))
                {
                    throw new InvalidOperationException("Scene.HandleComponentAdded CSComponent already added to start list");
                }


                cscomponentStart.Add(csc);
            }

        }

        void HandleComponentAdded(Component component)
        {

            // Check null (CSComponent) or other abstract component
            if (component == null)
            {                
                return;
            }

            if (component.GetType() == typeof(PhysicsWorld) || component.GetType() == typeof(PhysicsWorld2D))
            {
                SubscribeToEvent<PhysicsPreStepEvent>(component, HandlePhysicsPreStep);
                SubscribeToEvent<PhysicsPostStepEvent>(component, HandlePhysicsPostStep);
            }

            // CSComponent
            if (component.GetType().GetTypeInfo().IsSubclassOf(typeof(CSComponent)))
            {
                var csc = (CSComponent)component;
                AddCSComponent(csc);
            }

        }

        void HandleComponentRemoved(Component component)
        {
            if (component.GetType() == typeof(PhysicsWorld) || component.GetType() == typeof(PhysicsWorld2D))
            {
                UnsubscribeFromEvent<PhysicsPreStepEvent>();
                UnsubscribeFromEvent<PhysicsPostStepEvent>();
            }

            if (component.GetType().GetTypeInfo().IsSubclassOf(typeof(CSComponent)))
            {
                var csc = (CSComponent)component;

                CSComponentInfo info;

                if (!CSComponentCore.csinfoLookup.TryGetValue(csc.GetType(), out info))
                {
                    return;
                }

                cscomponentStart.Remove(csc);

                List<CSComponent> cslist;

                if (!cscomponents.TryGetValue(info, out cslist))
                {
                    return;
                }

                cslist.Remove(csc);

            }

        }

        Vector<CSComponent> csComponentVector = new Vector<CSComponent>();

        Dictionary<CSComponentInfo, List<CSComponent>> cscomponents = new Dictionary<CSComponentInfo, List<CSComponent>>();
        List<CSComponent> cscomponentStart = new List<CSComponent>();        

    }

}

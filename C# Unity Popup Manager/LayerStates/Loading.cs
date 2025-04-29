using System.Collections.Generic;
using UnityEngine;

namespace PopupManager.States
{
    public class Loading : LayerState
    {
        public Loading(PopupLayerController newParentStateMachine) : base(newParentStateMachine)
        {
        }

        public override void OnEnter()
        {
            ParentStateMachine.PopupManager.View.EnableBlocker(ParentStateMachine.PopupRequest.DisplayBlockerTint);
        }

        public override void OnExit() 
        {
            ParentStateMachine.PopupManager.View.SetPopUpDownloaderActive(false);
        }
        
        public override void ManagedUpdate()
        {
            switch (ParentStateMachine.CurrentPopupOperation.Update())
            {
                case waiting:
                    // Don't need to do anything here
                    break;
                case running:
                    if (ParentStateMachine.CurrentPopupOperation.IsLoaded()) 
                    {
                        ParentStateMachine.LoadFinished();
                    }

                    break;
                case done:
                    var result = ParentStateMachine.CurrentPopupOperation.GetResult();

                    switch (result.GetState())
                    {
                        case uninitialized:
                        case error:
                            ParentStateMachine.PopupManager.View.SetPopUpDownloaderActive(false);
                            
                            if (ParentStateMachine.PopupManager.UnloadLayer(result.GetState() == error ?
                                PopupUnloadType.Unload : PopupUnloadType.DoNotUnload, ParentStateMachine))
                            {//We will return if the result is an error and the layer unloads
                                return;
                            }

                            break;
                        case valid:
                            // Don't need to do anything here
                            break;
                    }

                    CheckNextPopupOrClose();
                    break;
            }
        }
    }
}

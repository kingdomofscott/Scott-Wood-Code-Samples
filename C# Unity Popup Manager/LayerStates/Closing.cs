using UnityEngine;

namespace PopupManager.States
{
    public class Closing : LayerState
    {
        public PopupUnloadType PopupUnload { private get; set; }

        public Closing(PopupLayerController newParentStateMachine) : base(newParentStateMachine)
        {
            PopupUnload = PopupUnloadType.DoNotUnload;
        }

        public override void OnEnter()
        {
            switch (ParentStateMachine.CurrentPopupOperation.GetState())
            {
                case done:
                    // Don't need to do anything here
                    break;
                case running:
                case waiting:
                    ParentStateMachine.CurrentPopupOperation.Abort();
                    break;
            }

            ParentStateMachine.PopupManager.LayerStartingToClose(ParentStateMachine);
        }

        public override void ManagedUpdate()
        {
            if (!ParentStateMachine.PopupManager.UnloadLayer(PopupUnload, ParentStateMachine))//Immediate unload will kill this layer, if it's deferred then try to load the next
            {
                CheckNextPopupOrClose();
            }
        }

        public override void ManagedOnDestroy()
        {
            ParentStateMachine.ControllerList.ManagedOnDestroy();
        }
    }
}
